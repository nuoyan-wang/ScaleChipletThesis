#include "scale-chiplet/Transforms/Passes.h"

#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/Tosa/IR/TosaOps.h"
#include "mlir/IR/PatternMatch.h"
#include "mlir/Pass/Pass.h"
#include "mlir/Transforms/GreedyPatternRewriteDriver.h"

using namespace mlir;
namespace tosa = mlir::tosa;

namespace mlir::scale_chiplet {
namespace {

///   %conv  = tosa.conv2d(%input, %weight, %bias, ...)
///   %t1    = tosa.transpose(%conv, ...)
///   %clamp = tosa.clamp(%t1, ...)
///   %t2    = tosa.transpose(%clamp, ...)
///   %pool  = tosa.max_pool2d(%t2, ...)
///
/// Replace this whole chain with:
///
///   %fused = "scale_chiplet.fused_conv2d_maxpool"(%input, %weight, %bias)
///              : (...) -> type(%pool)

struct ConvClampPoolFusionPattern
    : public OpRewritePattern<tosa::MaxPool2dOp> {
  using OpRewritePattern::OpRewritePattern;

  LogicalResult matchAndRewrite(tosa::MaxPool2dOp poolOp,
                                PatternRewriter &rewriter) const override {
    // pool input should come from the *second* transpose.
    Value poolInput = poolOp.getInput();
    auto t2 = poolInput.getDefiningOp<tosa::TransposeOp>();
    if (!t2)
      return failure();

    // t2 input should come from clamp.
    // NOTE: older TOSA API uses getInput1() for transpose input.
    auto clampOp = t2.getInput1().getDefiningOp<tosa::ClampOp>();
    if (!clampOp)
      return failure();

    // clamp input should come from the *first* transpose.
    auto t1 = clampOp.getInput().getDefiningOp<tosa::TransposeOp>();
    if (!t1)
      return failure();

    // t1 input should come from conv2d.
    auto convOp = t1.getInput1().getDefiningOp<tosa::Conv2DOp>();
    if (!convOp)
      return failure();

    // Now we have: conv2d -> t1 -> clamp -> t2 -> pool
    Location loc = poolOp.getLoc();

    // Use conv operands as fused operands.
    SmallVector<Value, 3> fusedOperands;
    fusedOperands.push_back(convOp.getInput());
    fusedOperands.push_back(convOp.getWeight());
    fusedOperands.push_back(convOp.getBias());

    // Result type is the same as max_pool2d result.
    Type fusedResultType = poolOp.getType();

    OperationState state(loc, "scale_chiplet.fused_conv2d_maxpool");
    state.addOperands(fusedOperands);
    state.addTypes(fusedResultType);

    // Preserve conv + pool attrs
    if (auto dil = convOp.getDilationAttr())
      state.addAttribute("conv_dilation", dil);
    if (auto pad = convOp.getPadAttr())
      state.addAttribute("conv_pad", pad);
    if (auto stride = convOp.getStrideAttr())
      state.addAttribute("conv_stride", stride);

    if (auto k = poolOp.getKernelAttr())
      state.addAttribute("pool_kernel", k);
    if (auto p = poolOp.getPadAttr())
      state.addAttribute("pool_pad", p);
    if (auto s = poolOp.getStrideAttr())
      state.addAttribute("pool_stride", s);

    Operation *fusedOp = rewriter.create(state);

    rewriter.replaceOp(poolOp, fusedOp->getResults());

    // Clean up the old ops if they’re now dead.
    if (t2->use_empty())
      rewriter.eraseOp(t2);
    if (clampOp->use_empty())
      rewriter.eraseOp(clampOp);
    if (t1->use_empty())
      rewriter.eraseOp(t1);
    if (convOp->use_empty())
      rewriter.eraseOp(convOp);

    return success();
  }
};

struct TosaConvPoolFusionPass
    : public PassWrapper<TosaConvPoolFusionPass,
                         OperationPass<func::FuncOp>> {
  MLIR_DEFINE_EXPLICIT_INTERNAL_INLINE_TYPE_ID(TosaConvPoolFusionPass)

  void getDependentDialects(DialectRegistry &registry) const override {
    registry.insert<tosa::TosaDialect>();
  }

  void runOnOperation() override {
    func::FuncOp func = getOperation();

    // Debug mark to prove the pass ran.
    func->setAttr("scale_chiplet.debug",
                  StringAttr::get(func.getContext(), "was_here"));

    RewritePatternSet patterns(&getContext());
    patterns.add<ConvClampPoolFusionPattern>(&getContext());

    if (failed(applyPatternsAndFoldGreedily(func, std::move(patterns))))
      signalPassFailure();
  }
};

} // end anonymous namespace

std::unique_ptr<Pass> createTosaConvPoolFusionPass() {
  return std::make_unique<TosaConvPoolFusionPass>();
}

} // namespace mlir::scale_chiplet
