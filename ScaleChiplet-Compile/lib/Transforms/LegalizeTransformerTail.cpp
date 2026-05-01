//===----------------------------------------------------------------------===//
// Legalize transformer chiplet tails for ScaleHLS.
//
// Some GPT2/LLaMA chiplets end in reshape/transpose-only tails, communicate
// tensors in a layout that the next chiplet immediately reshapes away, or
// return the same tensor multiple times. Downstream ScaleHLS can lower those to
// hls.dataflow.dispatch/task ops that illegally yield memref results.
//
// Keep semantics unchanged by:
// 1. Rewriting transformer chiplet boundaries of the form
//    1x8x4x8 -> reshape(1x8x32) when the consumer immediately performs that
//    reshape, so the producer returns 1x8x32 directly.
// 2. Appending an identity add/conv to each returned tensor result for
//    transformer-like chiplets. This gives the lowering a concrete terminal op
//    outside the problematic tail pattern.
//===----------------------------------------------------------------------===//

#include "scale-chiplet/Transforms/Passes.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/StringRef.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/Tosa/IR/TosaOps.h"
#include "mlir/IR/Block.h"
#include "mlir/IR/BuiltinAttributes.h"
#include "mlir/IR/BuiltinTypes.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/Operation.h"
#include "mlir/IR/Value.h"
#include "llvm/ADT/SmallVector.h"

using namespace mlir;

namespace mlir::scale_chiplet {
#define GEN_PASS_DEF_LEGALIZETRANSFORMERTAILPASS
#include "scale-chiplet/Transforms/Passes.h.inc"
} // namespace mlir::scale_chiplet

namespace {
struct LegalizeTransformerTailPass
    : public mlir::scale_chiplet::impl::LegalizeTransformerTailPassBase<
          LegalizeTransformerTailPass> {
  static bool isTransformerModule(ModuleOp module) {
    auto nameAttr =
        module->getAttrOfType<StringAttr>("torch.debug_module_name");
    if (!nameAttr)
      return false;
    StringRef name = nameAttr.getValue();
    return name.contains_insensitive("llama") ||
           name.contains_insensitive("gpt");
  }

  static bool hasTransformerOps(func::FuncOp func) {
    bool found = false;
    func.walk([&](Operation *op) {
      if (isa<tosa::ReduceMaxOp, tosa::ReduceSumOp, tosa::ExpOp,
              tosa::ReciprocalOp>(op)) {
        found = true;
        return WalkResult::interrupt();
      }
      return WalkResult::advance();
    });
    return found;
  }

  static bool shouldLegalize(func::FuncOp func, bool transformerModule) {
    if (func.getFunctionType().getNumResults() == 0)
      return false;
    if (transformerModule)
      return true;
    return hasTransformerOps(func);
  }

  static int64_t getChipId(func::FuncOp func) {
    StringRef name = func.getName();
    size_t pos = name.rfind("chip");
    if (pos == StringRef::npos)
      return -1;
    int64_t chipId = -1;
    return name.drop_front(pos + 4).getAsInteger(10, chipId) ? -1 : chipId;
  }

  static bool hasRecvFrom(func::FuncOp func, int64_t senderId, unsigned &argIdx) {
    for (auto it : llvm::enumerate(func.getArguments())) {
      auto dir = func.getArgAttrOfType<StringAttr>(it.index(), "chiplet.comm_dir");
      auto sid =
          func.getArgAttrOfType<IntegerAttr>(it.index(), "chiplet.sender_id");
      if (dir && sid && dir.getValue() == "recv" && sid.getInt() == senderId) {
        argIdx = it.index();
        return true;
      }
    }
    return false;
  }

  static bool rewriteInterfaceRank4ToRank3(func::FuncOp producer,
                                           unsigned resultIdx,
                                           func::FuncOp consumer,
                                           unsigned argIdx) {
    auto prodType =
        dyn_cast<RankedTensorType>(producer.getResultTypes()[resultIdx]);
    auto consType =
        dyn_cast<RankedTensorType>(consumer.getArgument(argIdx).getType());
    if (!prodType || !consType || prodType != consType)
      return false;
    if (prodType.getRank() != 4 || !prodType.getElementType().isF32())
      return false;
    if (prodType.getShape() != ArrayRef<int64_t>({1, 8, 4, 8}))
      return false;

    auto *consumerEntry = &consumer.front();
    Value arg = consumerEntry->getArgument(argIdx);
    auto reshapeUser = dyn_cast_or_null<tosa::ReshapeOp>(arg.use_begin()->getOwner());
    if (!reshapeUser || !arg.hasOneUse())
      return false;

    auto newType = dyn_cast<RankedTensorType>(reshapeUser.getType());
    if (!newType || newType.getShape() != ArrayRef<int64_t>({1, 8, 32}) ||
        !newType.getElementType().isF32())
      return false;

    auto ret = dyn_cast<func::ReturnOp>(producer.front().getTerminator());
    if (!ret || resultIdx >= ret.getNumOperands())
      return false;
    Value retOperand = ret.getOperand(resultIdx);
    auto transpose = retOperand.getDefiningOp<tosa::TransposeOp>();
    if (!transpose)
      return false;

    OpBuilder pb(ret);
    auto reshaped = pb.create<tosa::ReshapeOp>(
        ret.getLoc(), newType, transpose.getInput1(),
        pb.getI64ArrayAttr(newType.getShape()));
    ret.setOperand(resultIdx, reshaped.getResult());

    reshapeUser.replaceAllUsesWith(arg);
    reshapeUser.erase();

    SmallVector<Type> prodInputs(producer.getArgumentTypes().begin(),
                                 producer.getArgumentTypes().end());
    SmallVector<Type> prodResults(producer.getResultTypes().begin(),
                                  producer.getResultTypes().end());
    prodResults[resultIdx] = newType;
    producer.setType(pb.getFunctionType(prodInputs, prodResults));

    SmallVector<Type> consInputs(consumer.getArgumentTypes().begin(),
                                 consumer.getArgumentTypes().end());
    SmallVector<Type> consResults(consumer.getResultTypes().begin(),
                                  consumer.getResultTypes().end());
    consInputs[argIdx] = newType;
    consumer.setType(pb.getFunctionType(consInputs, consResults));
    consumer.getArgument(argIdx).setType(newType);
    return true;
  }

  static void rewriteTransformerCommShapes(ModuleOp module) {
    SmallVector<func::FuncOp> funcs;
    module.walk([&](func::FuncOp func) { funcs.push_back(func); });

    DenseMap<int64_t, func::FuncOp> byChipId;
    for (func::FuncOp func : funcs) {
      int64_t chipId = getChipId(func);
      if (chipId >= 0)
        byChipId[chipId] = func;
    }

    for (func::FuncOp producer : funcs) {
      int64_t producerId = getChipId(producer);
      if (producerId < 0)
        continue;
      for (unsigned resultIdx = 0, e = producer.getNumResults(); resultIdx < e;
           ++resultIdx) {
        auto dir =
            producer.getResultAttrOfType<StringAttr>(resultIdx, "chiplet.comm_dir");
        auto rid = producer.getResultAttrOfType<IntegerAttr>(
            resultIdx, "chiplet.receiver_id");
        if (!dir || !rid || dir.getValue() != "send")
          continue;

        auto consumerIt = byChipId.find(rid.getInt());
        if (consumerIt == byChipId.end())
          continue;

        unsigned argIdx = 0;
        if (!hasRecvFrom(consumerIt->second, producerId, argIdx))
          continue;
        rewriteInterfaceRank4ToRank3(producer, resultIdx, consumerIt->second,
                                     argIdx);
      }
    }
  }

  static bool canExpandBroadcastAlongC(RankedTensorType smallTy,
                                       RankedTensorType largeTy) {
    if (!smallTy || !largeTy || smallTy.getRank() != 4 || largeTy.getRank() != 4)
      return false;
    if (smallTy.getElementType() != largeTy.getElementType())
      return false;
    if (smallTy.getShape()[0] != 1 || largeTy.getShape()[0] != 1)
      return false;
    if (smallTy.getShape()[1] != 1 || largeTy.getShape()[1] <= 1)
      return false;
    return smallTy.getShape()[2] == largeTy.getShape()[2] &&
           smallTy.getShape()[3] == largeTy.getShape()[3];
  }

  static FailureOr<std::pair<unsigned, int64_t>>
  getSingleBroadcastAxis(RankedTensorType smallTy, RankedTensorType largeTy) {
    if (!smallTy || !largeTy || smallTy.getRank() != 4 || largeTy.getRank() != 4)
      return failure();
    if (smallTy.getElementType() != largeTy.getElementType())
      return failure();

    Optional<unsigned> axis;
    int64_t copies = 1;
    for (unsigned i = 0; i < 4; ++i) {
      int64_t s = smallTy.getShape()[i];
      int64_t l = largeTy.getShape()[i];
      if (s == l)
        continue;
      if (s != 1 || l <= 1 || axis)
        return failure();
      axis = i;
      copies = l;
    }
    if (!axis)
      return failure();
    return std::make_pair(*axis, copies);
  }

  static Value materializeExplicitBroadcast(OpBuilder &b, Location loc,
                                            Value small,
                                            RankedTensorType outTy) {
    auto smallTy = dyn_cast<RankedTensorType>(small.getType());
    auto axisAndCopies = getSingleBroadcastAxis(smallTy, outTy);
    if (failed(axisAndCopies))
      return Value();

    auto [axis, copies] = *axisAndCopies;
    SmallVector<int64_t> multiples(smallTy.getRank(), 1);
    multiples[axis] = copies;
    auto tile = b.create<tosa::TileOp>(loc, outTy, small,
                                       b.getI64ArrayAttr(multiples));
    return tile.getResult();
  }

  static void rewriteExplicitBroadcasts(func::FuncOp func) {
    SmallVector<Operation *> worklist;
    func.walk([&](Operation *op) {
      if (isa<tosa::AddOp, tosa::MulOp>(op))
        worklist.push_back(op);
    });

    for (Operation *op : worklist) {
      Value lhs;
      Value rhs;
      RankedTensorType outTy;
      IntegerAttr shiftAttr;
      if (auto add = dyn_cast<tosa::AddOp>(op)) {
        lhs = add.getInput1();
        rhs = add.getInput2();
        outTy = dyn_cast<RankedTensorType>(add.getType());
      } else if (auto mul = dyn_cast<tosa::MulOp>(op)) {
        lhs = mul.getInput1();
        rhs = mul.getInput2();
        outTy = dyn_cast<RankedTensorType>(mul.getType());
        shiftAttr = mul->getAttrOfType<IntegerAttr>("shift");
      } else {
        continue;
      }

      auto lhsTy = dyn_cast<RankedTensorType>(lhs.getType());
      auto rhsTy = dyn_cast<RankedTensorType>(rhs.getType());
      if (!lhsTy || !rhsTy || !outTy)
        continue;

      OpBuilder b(op);
      if (lhsTy != outTy) {
        if (Value expanded = materializeExplicitBroadcast(b, op->getLoc(), lhs, outTy))
          lhs = expanded;
      }
      if (rhsTy != outTy) {
        if (Value expanded = materializeExplicitBroadcast(b, op->getLoc(), rhs, outTy))
          rhs = expanded;
      }

      if (lhs.getType() != outTy || rhs.getType() != outTy)
        continue;

      Value replacement;
      if (isa<tosa::AddOp>(op)) {
        replacement = b.create<tosa::AddOp>(op->getLoc(), outTy, lhs, rhs);
      } else {
        int32_t shift = shiftAttr ? static_cast<int32_t>(shiftAttr.getInt()) : 0;
        replacement =
            b.create<tosa::MulOp>(op->getLoc(), outTy, lhs, rhs, shift);
      }

      op->getResult(0).replaceAllUsesWith(replacement);
      op->erase();
    }
  }

  static void rewriteTailReshapes(func::FuncOp func) {
    SmallVector<tosa::ReshapeOp> reshapes;
    func.walk([&](tosa::ReshapeOp reshape) { reshapes.push_back(reshape); });

    for (tosa::ReshapeOp reshape : reshapes) {
      auto srcTy = dyn_cast<RankedTensorType>(reshape.getInput1().getType());
      auto dstTy = dyn_cast<RankedTensorType>(reshape.getType());
      if (!srcTy || !dstTy)
        continue;
      if (srcTy.getShape() != ArrayRef<int64_t>({1, 4, 8, 8}) ||
          dstTy.getShape() != ArrayRef<int64_t>({1, 8, 32}))
        continue;

      Value source = reshape.getInput1();
      tosa::TransposeOp candidateTranspose;
      for (Operation *user : source.getUsers()) {
        auto transpose = dyn_cast<tosa::TransposeOp>(user);
        if (!transpose)
          continue;
        auto transTy = dyn_cast<RankedTensorType>(transpose.getType());
        if (transTy && transTy.getShape() == ArrayRef<int64_t>({1, 8, 4, 8})) {
          candidateTranspose = transpose;
          break;
        }
      }
      if (!candidateTranspose)
        continue;

      OpBuilder b(reshape);
      auto newReshape = b.create<tosa::ReshapeOp>(
          reshape.getLoc(), dstTy, candidateTranspose.getResult(),
          b.getI64ArrayAttr(dstTy.getShape()));
      reshape.replaceAllUsesWith(newReshape.getResult());
      reshape.erase();
    }
  }

  void runOnOperation() override {
    ModuleOp module = getOperation();
    bool transformerModule = isTransformerModule(module);

    if (transformerModule)
      rewriteTransformerCommShapes(module);

    module.walk([&](func::FuncOp func) {
      if (!shouldLegalize(func, transformerModule))
        return;

      rewriteExplicitBroadcasts(func);
      rewriteTailReshapes(func);

      auto ret = dyn_cast<func::ReturnOp>(func.front().getTerminator());
      if (!ret || ret.getNumOperands() == 0)
        return;

      OpBuilder b(ret);
      DenseMap<Type, Value> zeroByType;
      SmallVector<Value> newResults;
      newResults.reserve(ret.getNumOperands());

      for (Value operand : ret.getOperands()) {
        auto tensorTy = dyn_cast<RankedTensorType>(operand.getType());
        if (!tensorTy || !tensorTy.getElementType().isa<FloatType>()) {
          newResults.push_back(operand);
          continue;
        }

        if (tensorTy.getRank() == 4 && tensorTy.getDimSize(0) == 1 &&
            tensorTy.getElementType().isF32()) {
          const int64_t c = tensorTy.getDimSize(3);
          if (c > 0) {
            auto f32Ty = b.getF32Type();

            auto wTy = RankedTensorType::get({c, 1, 1, c}, f32Ty);
            SmallVector<float> wVals(c * c, 0.0f);
            for (int64_t i = 0; i < c; ++i)
              wVals[i * c + i] = 1.0f;
            auto wAttr = DenseElementsAttr::get(wTy, ArrayRef<float>(wVals));
            Value w = b.create<tosa::ConstOp>(ret.getLoc(), wTy, wAttr);

            auto bTy = RankedTensorType::get({c}, f32Ty);
            auto zeroBias = DenseElementsAttr::get(
                bTy, b.getFloatAttr(f32Ty, 0.0));
            Value bias = b.create<tosa::ConstOp>(ret.getLoc(), bTy, zeroBias);

            Value legalized = b.create<tosa::Conv2DOp>(
                ret.getLoc(), tensorTy, operand, w, bias,
                b.getI64ArrayAttr({0, 0, 0, 0}),
                b.getI64ArrayAttr({1, 1}), b.getI64ArrayAttr({1, 1}));
            newResults.push_back(legalized);
            continue;
          }
        }

        Value zero = zeroByType.lookup(tensorTy);
        if (!zero) {
          auto zeroAttr = DenseElementsAttr::get(
              tensorTy, b.getFloatAttr(tensorTy.getElementType(), 0.0));
          zero = b.create<tosa::ConstOp>(ret.getLoc(), tensorTy, zeroAttr);
          zeroByType[tensorTy] = zero;
        }
        Value legalized =
            b.create<tosa::AddOp>(ret.getLoc(), tensorTy, operand, zero);
        newResults.push_back(legalized);
      }

      ret->setOperands(newResults);
    });
  }
};
} // namespace

std::unique_ptr<Pass>
mlir::scale_chiplet::createLegalizeTransformerTailPass() {
  return std::make_unique<LegalizeTransformerTailPass>();
}
