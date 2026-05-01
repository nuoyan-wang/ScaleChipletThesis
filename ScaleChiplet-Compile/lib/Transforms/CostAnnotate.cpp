//===----------------------------------------------------------------------===//
// Compute coarse cost annotations (flops/bytes) for chiplet tasks.
//===----------------------------------------------------------------------===//

#include "scale-chiplet/Transforms/Passes.h"
#include "scale-chiplet/Dialect/Chiplet/Chiplet.h"
#include "mlir/Dialect/Tosa/IR/TosaOps.h"
#include "llvm/ADT/TypeSwitch.h"
#include <algorithm>
#include <limits>

using namespace mlir;
using namespace mlir::scale_chiplet;
using namespace mlir::scale_chiplet::chiplet;

namespace mlir::scale_chiplet {
#define GEN_PASS_DEF_COSTANNOTATEPASS
#include "scale-chiplet/Transforms/Passes.h.inc"
} // namespace mlir::scale_chiplet

namespace {
static Optional<int64_t> getNumElements(Value v) {
  auto t = dyn_cast<ShapedType>(v.getType());
  if (!t || !t.hasStaticShape())
    return llvm::None;
  return t.getNumElements();
}

static Optional<int64_t> getElemBits(Value v) {
  auto t = dyn_cast<ShapedType>(v.getType());
  if (!t)
    return llvm::None;
  if (auto it = t.getElementType().dyn_cast<IntegerType>())
    return it.getWidth();
  if (auto ft = t.getElementType().dyn_cast<FloatType>())
    return ft.getWidth();
  return llvm::None;
}

static Optional<int64_t> getTensorBytes(Value v) {
  auto elems = getNumElements(v);
  auto bits = getElemBits(v);
  if (!elems || !bits)
    return llvm::None;
  return (*elems * *bits + 7) / 8;
}

static int64_t saturatingMul(int64_t lhs, int64_t rhs) {
  if (lhs <= 0 || rhs <= 0)
    return 0;
  if (lhs > std::numeric_limits<int64_t>::max() / rhs)
    return std::numeric_limits<int64_t>::max();
  return lhs * rhs;
}

static int64_t saturatingAdd(int64_t lhs, int64_t rhs) {
  if (rhs > 0 && lhs > std::numeric_limits<int64_t>::max() - rhs)
    return std::numeric_limits<int64_t>::max();
  return lhs + rhs;
}

static int64_t getElementWorkScale(Type elemType) {
  if (auto ft = dyn_cast<FloatType>(elemType)) {
    unsigned bits = ft.getWidth();
    if (bits <= 16)
      return 2;
    if (bits <= 32)
      return 4;
    return 8;
  }
  if (auto it = dyn_cast<IntegerType>(elemType)) {
    unsigned bits = it.getWidth();
    if (bits <= 1)
      return 1;
    if (bits <= 8)
      return 1;
    if (bits <= 16)
      return 2;
    if (bits <= 32)
      return 3;
    return 4;
  }
  return 2;
}

static int64_t getValueWorkScale(Value value) {
  auto shaped = dyn_cast<ShapedType>(value.getType());
  if (!shaped)
    return 2;
  return getElementWorkScale(shaped.getElementType());
}

static int64_t getOpWorkScale(Operation *op) {
  int64_t scale = 1;
  for (Value operand : op->getOperands())
    scale = std::max(scale, getValueWorkScale(operand));
  for (Value result : op->getResults())
    scale = std::max(scale, getValueWorkScale(result));
  return scale;
}

static Optional<int64_t> estimateComputeUnits(Operation *op) {
  return TypeSwitch<Operation *, Optional<int64_t>>(op)
      .Case<tosa::Conv2DOp>([&](tosa::Conv2DOp cv) {
        auto outElems = getNumElements(cv.getOutput());
        if (!outElems)
          return Optional<int64_t>();
        auto inTy = dyn_cast<ShapedType>(cv.getInput().getType());
        auto filtTy = dyn_cast<ShapedType>(cv.getWeight().getType());
        if (!inTy || !filtTy || !inTy.hasRank() || !filtTy.hasRank())
          return Optional<int64_t>();
        // Kernel height/width * input channels.
        int64_t kh = filtTy.getDimSize(filtTy.getRank() - 2);
        int64_t kw = filtTy.getDimSize(filtTy.getRank() - 1);
        int64_t ic = inTy.getDimSize(inTy.getRank() - 1);
        if (kh <= 0 || kw <= 0 || ic <= 0)
          return Optional<int64_t>();
        int64_t macs = saturatingMul(*outElems, saturatingMul(kh * kw, ic));
        return Optional<int64_t>(saturatingMul(2 * getOpWorkScale(op), macs));
      })
      .Case<tosa::MatMulOp>([&](tosa::MatMulOp mm) {
        auto outElems = getNumElements(mm.getResult());
        auto lhsTy = dyn_cast<ShapedType>(mm.getA().getType());
        auto rhsTy = dyn_cast<ShapedType>(mm.getB().getType());
        if (!outElems || !lhsTy || !rhsTy || !lhsTy.hasRank() ||
            !rhsTy.hasRank())
          return Optional<int64_t>();
        int64_t k = lhsTy.getDimSize(lhsTy.getRank() - 1);
        if (k <= 0)
          return Optional<int64_t>();
        int64_t macs = saturatingMul(*outElems, k);
        return Optional<int64_t>(saturatingMul(2 * getOpWorkScale(op), macs));
      })
      .Case<tosa::MulOp>([&](Operation *elem) {
        auto elems = getNumElements(elem->getResult(0));
        if (!elems)
          return Optional<int64_t>();
        return Optional<int64_t>(
            saturatingMul(*elems, 2 * getOpWorkScale(elem)));
      })
      .Case<tosa::AddOp, tosa::SubOp, tosa::AbsOp, tosa::NegateOp,
            tosa::GreaterEqualOp, tosa::EqualOp, tosa::SelectOp>(
          [&](Operation *elem) {
            auto elems = getNumElements(elem->getResult(0));
            if (!elems)
              return Optional<int64_t>();
            return Optional<int64_t>(
                saturatingMul(*elems, getOpWorkScale(elem)));
          })
      .Case<tosa::DivOp>([&](Operation *elem) {
        auto elems = getNumElements(elem->getResult(0));
        if (!elems)
          return Optional<int64_t>();
        return Optional<int64_t>(
            saturatingMul(*elems, 8 * getOpWorkScale(elem)));
      })
      .Case<tosa::ExpOp, tosa::SigmoidOp, tosa::TanhOp>([&](Operation *elem) {
        auto elems = getNumElements(elem->getResult(0));
        if (!elems)
          return Optional<int64_t>();
        return Optional<int64_t>(
            saturatingMul(*elems, 12 * getOpWorkScale(elem)));
      })
      .Case<tosa::ReciprocalOp, tosa::RsqrtOp>([&](Operation *elem) {
        auto elems = getNumElements(elem->getResult(0));
        if (!elems)
          return Optional<int64_t>();
        return Optional<int64_t>(
            saturatingMul(*elems, 10 * getOpWorkScale(elem)));
      })
      .Case<tosa::PowOp>([&](Operation *elem) {
        auto elems = getNumElements(elem->getResult(0));
        if (!elems)
          return Optional<int64_t>();
        return Optional<int64_t>(
            saturatingMul(*elems, 16 * getOpWorkScale(elem)));
      })
      .Case<tosa::ReduceMaxOp, tosa::ReduceSumOp>([&](Operation *elem) {
        auto inputElems = getNumElements(elem->getOperand(0));
        if (!inputElems)
          return Optional<int64_t>();
        return Optional<int64_t>(
            saturatingMul(*inputElems, 2 * getOpWorkScale(elem)));
      })
      .Default([&](Operation *) { return Optional<int64_t>(); });
}

// Heuristic data-movement work units for ops where tensor size dominates.
static Optional<int64_t> estimateDataMovementUnits(Operation *op) {
  auto sumBytes = [&](Operation *elem, int64_t divisor) -> Optional<int64_t> {
    if (divisor <= 0)
      divisor = 1;
    int64_t totalBytes = 0;
    bool known = false;
    for (Value operand : elem->getOperands()) {
      if (auto bytes = getTensorBytes(operand)) {
        totalBytes = saturatingAdd(totalBytes, *bytes);
        known = true;
      }
    }
    for (Value result : elem->getResults()) {
      if (auto bytes = getTensorBytes(result)) {
        totalBytes = saturatingAdd(totalBytes, *bytes);
        known = true;
      }
    }
    if (!known)
      return Optional<int64_t>();
    return Optional<int64_t>(std::max<int64_t>(1, totalBytes / divisor));
  };

  return TypeSwitch<Operation *, Optional<int64_t>>(op)
      .Case<tosa::TransposeOp>([&](Operation *elem) {
        return sumBytes(elem, 2);
      })
      .Case<tosa::TileOp, tosa::SliceOp, tosa::CastOp>([&](Operation *elem) {
        return sumBytes(elem, 1);
      })
      .Case<tosa::ClampOp>(
          [&](Operation *elem) {
            auto elems = getNumElements(elem->getResult(0));
            if (!elems)
              return Optional<int64_t>();
            return Optional<int64_t>(*elems);
          })
      .Case<tosa::ReshapeOp>([&](Operation *) {
        return Optional<int64_t>(1);
      })
      .Default([&](Operation *) { return Optional<int64_t>(); });
}

// Additional partitioning bias for ops that tend to dominate the attention
// front-half after lowering even when simple FLOP counts look moderate.
static Optional<int64_t> estimatePartitionBiasUnits(Operation *op) {
  auto resultElems = [&](Operation *elem) -> Optional<int64_t> {
    if (elem->getNumResults() == 0)
      return Optional<int64_t>();
    return getNumElements(elem->getResult(0));
  };
  auto scaledElems = [&](Operation *elem, int64_t factor) -> Optional<int64_t> {
    auto elems = resultElems(elem);
    if (!elems)
      return Optional<int64_t>();
    return Optional<int64_t>(
        saturatingMul(*elems, factor * getOpWorkScale(elem)));
  };
  auto scaledInputElems =
      [&](Operation *elem, int64_t factor) -> Optional<int64_t> {
    if (elem->getNumOperands() == 0)
      return Optional<int64_t>();
    auto elems = getNumElements(elem->getOperand(0));
    if (!elems)
      return Optional<int64_t>();
    return Optional<int64_t>(
        saturatingMul(*elems, factor * getOpWorkScale(elem)));
  };
  auto scaledBytes = [&](Operation *elem, int64_t factor) -> Optional<int64_t> {
    int64_t totalBytes = 0;
    bool known = false;
    for (Value operand : elem->getOperands()) {
      if (auto bytes = getTensorBytes(operand)) {
        totalBytes = saturatingAdd(totalBytes, *bytes);
        known = true;
      }
    }
    for (Value result : elem->getResults()) {
      if (auto bytes = getTensorBytes(result)) {
        totalBytes = saturatingAdd(totalBytes, *bytes);
        known = true;
      }
    }
    if (!known)
      return Optional<int64_t>();
    return Optional<int64_t>(saturatingMul(totalBytes, factor));
  };

  return TypeSwitch<Operation *, Optional<int64_t>>(op)
      .Case<tosa::TileOp>([&](Operation *elem) { return scaledBytes(elem, 6); })
      .Case<tosa::SliceOp>([&](Operation *elem) { return scaledBytes(elem, 4); })
      .Case<tosa::TransposeOp>([&](Operation *elem) {
        return scaledBytes(elem, 3);
      })
      .Case<tosa::CastOp>([&](Operation *elem) { return scaledBytes(elem, 2); })
      .Case<tosa::SelectOp>([&](Operation *elem) { return scaledElems(elem, 8); })
      .Case<tosa::EqualOp>([&](Operation *elem) { return scaledElems(elem, 8); })
      .Case<tosa::ReduceMaxOp>([&](Operation *elem) {
        return scaledInputElems(elem, 6);
      })
      .Case<tosa::ExpOp>([&](Operation *elem) { return scaledElems(elem, 20); })
      .Case<tosa::ReciprocalOp>([&](Operation *elem) {
        return scaledElems(elem, 18);
      })
      .Case<tosa::RsqrtOp>([&](Operation *elem) { return scaledElems(elem, 12); })
      .Default([&](Operation *) { return Optional<int64_t>(); });
}

struct CostAnnotatePass
    : public mlir::scale_chiplet::impl::CostAnnotatePassBase<CostAnnotatePass> {
  void runOnOperation() override {
    func::FuncOp func = getOperation();
    func.walk([&](chiplet::TaskOp task) {
      int64_t compute = 0;
      int64_t movement = 0;
      int64_t partitionBias = 0;
      int64_t bytesIn = 0;
      int64_t bytesOut = 0;
      bool anyUnknown = false;

      for (Value opnd : task.getOperands()) {
        if (auto b = getTensorBytes(opnd))
          bytesIn += *b;
        else
          anyUnknown = true;
      }
      for (Value res : task.getResults()) {
        if (auto b = getTensorBytes(res))
          bytesOut += *b;
        else
          anyUnknown = true;
      }

      task.walk([&](Operation *inner) {
        if (isa<chiplet::YieldOp, chiplet::TaskOp>(inner))
          return;
        if (auto f = estimateComputeUnits(inner)) {
          compute = saturatingAdd(compute, *f);
        } else if (auto m = estimateDataMovementUnits(inner)) {
          movement = saturatingAdd(movement, *m);
        } else {
          anyUnknown = true;
        }
        if (auto bias = estimatePartitionBiasUnits(inner))
          partitionBias = saturatingAdd(partitionBias, *bias);
      });

      OpBuilder b(task);
      int64_t total =
          saturatingAdd(saturatingAdd(compute, movement), partitionBias);
      task->setAttr("cost_flops", b.getI64IntegerAttr(compute));
      task->setAttr("cost_misc", b.getI64IntegerAttr(movement));
      task->setAttr("cost_ops_total", b.getI64IntegerAttr(total));
      task->setAttr("cost_balance_total", b.getI64IntegerAttr(total));
      task->setAttr("cost_partition_bias", b.getI64IntegerAttr(partitionBias));
      task->setAttr("cost_bytes_in", b.getI64IntegerAttr(bytesIn));
      task->setAttr("cost_bytes_out", b.getI64IntegerAttr(bytesOut));
      task->setAttr("cost_bytes_total",
                    b.getI64IntegerAttr(bytesIn + bytesOut));
      if (anyUnknown)
        task->setAttr("cost_partial", b.getUnitAttr());
    });
  }
};
} // namespace

std::unique_ptr<mlir::Pass> mlir::scale_chiplet::createCostAnnotatePass() {
  return std::make_unique<CostAnnotatePass>();
}
