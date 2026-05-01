//===----------------------------------------------------------------------===//
// Legalize depthwise-conv chiplet tails for ScaleHLS.
//
// Some chiplets ending in depthwise-conv patterns lower to a top-level
// hls.dataflow.dispatch that yields a memref result, which fails legality in
// downstream ScaleHLS. Keep semantics unchanged by appending an identity 1x1
// conv2d at the return value for those tails.
//===----------------------------------------------------------------------===//

#include "scale-chiplet/Transforms/Passes.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/Tosa/IR/TosaOps.h"
#include "mlir/IR/BuiltinAttributes.h"
#include "mlir/IR/BuiltinTypes.h"
#include "mlir/IR/Builders.h"

using namespace mlir;

namespace mlir::scale_chiplet {
#define GEN_PASS_DEF_LEGALIZEDEPTHWISETAILPASS
#include "scale-chiplet/Transforms/Passes.h.inc"
} // namespace mlir::scale_chiplet

namespace {
struct LegalizeDepthwiseTailPass
    : public mlir::scale_chiplet::impl::LegalizeDepthwiseTailPassBase<
          LegalizeDepthwiseTailPass> {
  static tosa::DepthwiseConv2DOp findDepthwiseAncestor(Value v) {
    Value cur = v;
    while (Operation *def = cur.getDefiningOp()) {
      if (auto dw = dyn_cast<tosa::DepthwiseConv2DOp>(def))
        return dw;
      if (auto t = dyn_cast<tosa::TransposeOp>(def)) {
        cur = t.getInput1();
        continue;
      }
      if (auto c = dyn_cast<tosa::ClampOp>(def)) {
        cur = c.getInput();
        continue;
      }
      if (auto r = dyn_cast<tosa::ReshapeOp>(def)) {
        cur = r.getInput1();
        continue;
      }
      break;
    }
    return {};
  }

  void runOnOperation() override {
    ModuleOp module = getOperation();
    bool changed = false;

    module.walk([&](func::FuncOp func) {
      if (func.getFunctionType().getNumResults() != 1)
        return;

      auto ret = dyn_cast<func::ReturnOp>(func.front().getTerminator());
      if (!ret || ret.getNumOperands() != 1)
        return;

      Value retVal = ret.getOperand(0);
      auto outTy = dyn_cast<RankedTensorType>(retVal.getType());
      if (!outTy || outTy.getRank() != 4 || !outTy.getElementType().isF32())
        return;

      if (outTy.getDimSize(0) != 1)
        return;

      if (!findDepthwiseAncestor(retVal))
        return;

      const int64_t c = outTy.getDimSize(3);
      if (c <= 0)
        return;

      OpBuilder b(ret);
      auto f32Ty = b.getF32Type();

      auto wTy = RankedTensorType::get({c, 1, 1, c}, f32Ty);
      SmallVector<float> wVals(c * c, 0.0f);
      for (int64_t i = 0; i < c; ++i)
        wVals[i * c + i] = 1.0f;
      auto wAttr = DenseElementsAttr::get(wTy, ArrayRef<float>(wVals));
      Value w = b.create<tosa::ConstOp>(ret.getLoc(), wTy, wAttr);

      auto bTy = RankedTensorType::get({c}, f32Ty);
      SmallVector<float> zVals(c, 0.0f);
      auto bAttr = DenseElementsAttr::get(bTy, ArrayRef<float>(zVals));
      Value bias = b.create<tosa::ConstOp>(ret.getLoc(), bTy, bAttr);

      auto conv = b.create<tosa::Conv2DOp>(
          ret.getLoc(), outTy, retVal, w, bias,
          b.getI64ArrayAttr({0, 0, 0, 0}),
          b.getI64ArrayAttr({1, 1}),
          b.getI64ArrayAttr({1, 1}));

      ret.setOperand(0, conv.getResult());
      changed = true;
    });

    (void)changed;
  }
};
} // namespace

std::unique_ptr<Pass> mlir::scale_chiplet::createLegalizeDepthwiseTailPass() {
  return std::make_unique<LegalizeDepthwiseTailPass>();
}
