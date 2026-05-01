#include "scale-chiplet/Transforms/Passes.h"
#include "mlir/Pass/PassManager.h"
#include "mlir/Transforms/Passes.h"

namespace mlir::scale_chiplet {
#define GEN_PASS_DEF_TOSACONVPOOLFUSIONPASS
#define GEN_PASS_DEF_CREATECHIPLETFROMTOSA
#define GEN_PASS_DEF_FUSELAYOUTCHAINSPASS
#define GEN_PASS_DEF_FUSETRIVIALLAYOUTSPASS
#define GEN_PASS_DEF_FUSETRIVIALCONSTSPASS
#define GEN_PASS_DEF_PRUNEDEADTASKYIELDSPASS
#define GEN_PASS_DEF_NORMALIZETASKOPERANDSPASS
#define GEN_PASS_DEF_COSTANNOTATEPASS
#define GEN_PASS_DEF_ANNOTATETASKDAGPASS
#define GEN_PASS_DEF_EVENCLUSTERTASKSPASS
#define GEN_PASS_DEF_MERGECLUSTERTASKSPASS
#define GEN_PASS_DEF_INSERTCHIPLETSENDRECVPASS
#define GEN_PASS_DEF_MATERIALIZECHIPLETSPASS
#define GEN_PASS_DEF_LEGALIZEADDBIASPASS
#define GEN_PASS_DEF_LEGALIZEDEPTHWISETAILPASS
#define GEN_PASS_DEF_LEGALIZETRANSFORMERTAILPASS
#include "scale-chiplet/Transforms/Passes.h.inc"

struct ScaleChipletPipelineOptions
    : public PassPipelineOptions<ScaleChipletPipelineOptions> {
  Option<unsigned> debugPoint{
      *this, "debug-point",
      ::llvm::cl::desc("Stop the pipeline at the given debug point"),
      ::llvm::cl::init(0)};
  Option<unsigned> numChiplets{
      *this, "num-chiplets",
      ::llvm::cl::desc("Number of chiplets to emit"),
      ::llvm::cl::init(8)};
};

void registerScaleChipletPasses() {
  PassPipelineRegistration<ScaleChipletPipelineOptions>(
      "scale-chiplet-pipeline", "Run canonicalization + conv/pool fusion",
      [](OpPassManager &pm, const ScaleChipletPipelineOptions &opts) {

        // Pre-cleanup and setup
        pm.addPass(createCanonicalizerPass());
        pm.addPass(createCSEPass());
        if (opts.debugPoint == 1) return;

        pm.nest<func::FuncOp>().addPass(createCreateChipletFromTosaPass());
        if (opts.debugPoint == 2) return;

        pm.nest<func::FuncOp>().addPass(createFuseLayoutChainsPass());
        if (opts.debugPoint == 3) return;

        pm.nest<func::FuncOp>().addPass(createFuseTrivialLayoutsPass());
        if (opts.debugPoint == 4) return;

        pm.nest<func::FuncOp>().addPass(createFuseTrivialConstsPass());
        if (opts.debugPoint == 5) return;

        pm.nest<func::FuncOp>().addPass(createPruneDeadTaskYieldsPass());
        if (opts.debugPoint == 6) return;

        pm.nest<func::FuncOp>().addPass(createNormalizeTaskOperandsPass());
        if (opts.debugPoint == 7) return;

        pm.nest<func::FuncOp>().addPass(createCostAnnotatePass());
        if (opts.debugPoint == 8) return;

        pm.nest<func::FuncOp>().addPass(createAnnotateTaskDAGPass());
        if (opts.debugPoint == 9) return;

        pm.nest<func::FuncOp>().addPass(
            createEvenClusterTasksPass(opts.numChiplets));
        if (opts.debugPoint == 10) return;

        pm.nest<func::FuncOp>().addPass(createMergeClusterTasksPass());
        if (opts.debugPoint == 11) return;

        pm.nest<func::FuncOp>().addPass(createInsertChipletSendRecvPass());
        // Send/recv insertion can make old cross-chip task results dead.
        pm.nest<func::FuncOp>().addPass(createPruneDeadTaskYieldsPass());
        if (opts.debugPoint == 12) return;

        // Outline each chiplet_id into its own func for downstream pipelines.
        pm.addPass(createMaterializeChipletsPass());
        if (opts.debugPoint == 13) return;

        // Canonicalize chip7 classifier tail into a ScaleHLS-friendly form.
        pm.addPass(createLegalizeAddBiasPass());
        if (opts.debugPoint == 14) return;

        // Avoid chiplet tails that lower to illegal dispatch-yield patterns.
        pm.addPass(createLegalizeDepthwiseTailPass());
        if (opts.debugPoint == 15) return;

        // Post-cleanup
        pm.addPass(createCanonicalizerPass());
        if (opts.debugPoint == 16) return;

        // Keep this after final cleanup so the identity tail does not fold away.
        pm.addPass(createLegalizeTransformerTailPass());
      });
}
} // namespace mlir::scale_chiplet
