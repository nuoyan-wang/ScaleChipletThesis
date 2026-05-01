#pragma once
#include "mlir/Pass/Pass.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include <string>

namespace mlir {
namespace scale_chiplet {
#define GEN_PASS_DECL_TOSACONVPOOLFUSIONPASS
#define GEN_PASS_DECL_CREATECHIPLETFROMTOSA
#define GEN_PASS_DECL_FUSELAYOUTCHAINSPASS
#define GEN_PASS_DECL_FUSETRIVIALLAYOUTSPASS
#define GEN_PASS_DECL_FUSETRIVIALCONSTSPASS
#define GEN_PASS_DECL_PRUNEDEADTASKYIELDSPASS
#define GEN_PASS_DECL_NORMALIZETASKOPERANDSPASS
#define GEN_PASS_DECL_COSTANNOTATEPASS
#define GEN_PASS_DECL_ANNOTATETASKDAGPASS
#define GEN_PASS_DECL_EVENCLUSTERTASKSPASS
#define GEN_PASS_DECL_MERGECLUSTERTASKSPASS
#define GEN_PASS_DECL_INSERTCHIPLETSENDRECVPASS
#define GEN_PASS_DECL_MATERIALIZECHIPLETSPASS
#define GEN_PASS_DECL_LEGALIZEADDBIASPASS
#define GEN_PASS_DECL_LEGALIZEDEPTHWISETAILPASS
#define GEN_PASS_DECL_LEGALIZETRANSFORMERTAILPASS
#include "scale-chiplet/Transforms/Passes.h.inc"

std::unique_ptr<mlir::Pass> createTosaConvPoolFusionPass();
std::unique_ptr<mlir::Pass> createCreateChipletFromTosaPass();
std::unique_ptr<mlir::Pass> createFuseLayoutChainsPass();
std::unique_ptr<mlir::Pass> createFuseTrivialLayoutsPass();
std::unique_ptr<mlir::Pass> createFuseTrivialConstsPass();
std::unique_ptr<mlir::Pass> createPruneDeadTaskYieldsPass();
std::unique_ptr<mlir::Pass> createNormalizeTaskOperandsPass();
std::unique_ptr<mlir::Pass> createCostAnnotatePass();
std::unique_ptr<mlir::Pass> createAnnotateTaskDAGPass();
std::unique_ptr<mlir::Pass> createEvenClusterTasksPass();
std::unique_ptr<mlir::Pass> createEvenClusterTasksPass(int numChiplets);
std::unique_ptr<mlir::Pass> createMergeClusterTasksPass();
std::unique_ptr<mlir::Pass> createInsertChipletSendRecvPass();
std::unique_ptr<mlir::Pass> createMaterializeChipletsPass();
std::unique_ptr<mlir::Pass> createLegalizeAddBiasPass();
std::unique_ptr<mlir::Pass> createLegalizeDepthwiseTailPass();
std::unique_ptr<mlir::Pass> createLegalizeTransformerTailPass();
void registerScaleChipletPasses();
} // namespace scale_chiplet
} // namespace mlir
