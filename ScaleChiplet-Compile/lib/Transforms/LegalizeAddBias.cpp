//===----------------------------------------------------------------------===//
// Normalize chip7 classifier tail without changing semantics:
//   return tosa.add(tosa.reshape(tosa.matmul(...)), bias)
// to:
//   return tosa.add(tosa.matmul(...), tosa.reshape(bias))
//
// This preserves the bias addition while avoiding the problematic final
// rank-reducing tail seen in downstream ScaleHLS dataflow lowering.
//===----------------------------------------------------------------------===//

#include "scale-chiplet/Transforms/Passes.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/Tosa/IR/TosaOps.h"
#include "mlir/IR/BuiltinTypes.h"
#include "mlir/IR/Builders.h"
#include "llvm/ADT/StringRef.h"
#include <optional>

using namespace mlir;

namespace mlir::scale_chiplet {
#define GEN_PASS_DEF_LEGALIZEADDBIASPASS
#include "scale-chiplet/Transforms/Passes.h.inc"
} // namespace mlir::scale_chiplet

namespace {
struct LegalizeAddBiasPass
    : public mlir::scale_chiplet::impl::LegalizeAddBiasPassBase<
          LegalizeAddBiasPass> {
  static bool isLikelyChip7(func::FuncOp func) {
    StringRef name = func.getName();
    return name.endswith("chip7");
  }

  struct TailMatch {
    tosa::AddOp add;
    tosa::ReshapeOp reshapeFromMatmul;
    Value matmulVal;
    Value biasVal;
  };

  static std::optional<TailMatch> matchClassifierTail(Value retVal) {
    auto add = retVal.getDefiningOp<tosa::AddOp>();
    if (!add)
      return std::nullopt;

    auto lhsReshape = add.getInput1().getDefiningOp<tosa::ReshapeOp>();
    auto rhsReshape = add.getInput2().getDefiningOp<tosa::ReshapeOp>();
    tosa::ReshapeOp reshape = lhsReshape ? lhsReshape : rhsReshape;
    if (!reshape)
      return std::nullopt;

    auto reshapeOutTy = dyn_cast<RankedTensorType>(reshape.getResult().getType());
    auto reshapeInTy = dyn_cast<RankedTensorType>(reshape.getInput1().getType());
    if (!reshapeOutTy || !reshapeInTy)
      return std::nullopt;
    if (reshapeOutTy.getRank() != 2 || reshapeInTy.getRank() != 3)
      return std::nullopt;
    if (reshapeOutTy.getDimSize(0) != 1 || reshapeInTy.getDimSize(0) != 1 ||
        reshapeInTy.getDimSize(1) != 1)
      return std::nullopt;

    Value matmulVal = reshape.getInput1();
    if (!matmulVal.getDefiningOp<tosa::MatMulOp>())
      return std::nullopt;

    Value biasVal = (reshape == lhsReshape) ? add.getInput2() : add.getInput1();
    auto biasTy = dyn_cast<RankedTensorType>(biasVal.getType());
    if (!biasTy)
      return std::nullopt;

    const int64_t n = reshapeInTy.getDimSize(2);
    bool biasTypeSupported =
        (biasTy.getRank() == 1 && biasTy.getDimSize(0) == n) ||
        (biasTy.getRank() == 2 && biasTy.getDimSize(0) == 1 &&
         biasTy.getDimSize(1) == n) ||
        (biasTy.getRank() == 3 && biasTy.getDimSize(0) == 1 &&
         biasTy.getDimSize(1) == 1 && biasTy.getDimSize(2) == n);
    if (!biasTypeSupported)
      return std::nullopt;

    return TailMatch{add, reshape, matmulVal, biasVal};
  }

  void runOnOperation() override {
    ModuleOp module = getOperation();
    bool changed = false;

    module.walk([&](func::FuncOp func) {
      if (!isLikelyChip7(func))
        return;
      if (func.getFunctionType().getNumResults() != 1)
        return;

      auto ret = dyn_cast<func::ReturnOp>(func.front().getTerminator());
      if (!ret || ret.getNumOperands() != 1)
        return;

      auto match = matchClassifierTail(ret.getOperand(0));
      if (!match)
        return;

      auto matmulTy = dyn_cast<RankedTensorType>(match->matmulVal.getType());
      if (!matmulTy)
        return;

      Value bias3D = match->biasVal;
      auto biasTy = cast<RankedTensorType>(bias3D.getType());
      if (biasTy.getRank() != 3) {
        auto bias3DTy =
            RankedTensorType::get({1, 1, matmulTy.getDimSize(2)},
                                  matmulTy.getElementType());
        OpBuilder b(match->add);
        bias3D =
            b.create<tosa::ReshapeOp>(match->add.getLoc(), bias3DTy, bias3D,
                                      b.getI64ArrayAttr({1, 1, matmulTy.getDimSize(2)}))
                .getResult();
      }

      OpBuilder b(match->add);
      Value newAdd = b
                         .create<tosa::AddOp>(match->add.getLoc(), matmulTy,
                                              match->matmulVal, bias3D)
                         .getResult();
      ret.setOperand(0, newAdd);

      auto oldType = func.getFunctionType();
      auto newType = FunctionType::get(func.getContext(), oldType.getInputs(),
                                       TypeRange{newAdd.getType()});
      func.setType(newType);

      if (auto oldAdd = dyn_cast_or_null<tosa::AddOp>(match->add.getOperation()))
        if (oldAdd->use_empty())
          oldAdd->erase();
      if (auto oldReshape =
              dyn_cast_or_null<tosa::ReshapeOp>(match->reshapeFromMatmul.getOperation()))
        if (oldReshape->use_empty())
          oldReshape->erase();

      changed = true;
    });

    if (!changed)
      return;
  }
};
} // namespace

std::unique_ptr<Pass>
mlir::scale_chiplet::createLegalizeAddBiasPass() {
  return std::make_unique<LegalizeAddBiasPass>();
}
