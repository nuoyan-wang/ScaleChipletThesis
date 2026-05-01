add_mlir_library(ScaleChipletTransforms
  Passes.cpp
  TosaConvPoolFusion.cpp
  CreateChipletFromTosa.cpp
  FuseLayoutChains.cpp
  FuseTrivialLayouts.cpp
  FuseTrivialConsts.cpp
  PruneDeadTaskYields.cpp
  NormalizeTaskOperands.cpp
  CostAnnotate.cpp
  AnnotateTaskDAG.cpp
  EvenClusterTasks.cpp
  MergeClusterTasks.cpp
  InsertChipletSendRecv.cpp
  MaterializeChiplets.cpp
  LegalizeAddBias.cpp
  LegalizeDepthwiseTail.cpp
  LegalizeTransformerTail.cpp

  DEPENDS
    ScaleChipletPassIncGen
    ScaleChipletChipletOpsIncGen

  LINK_LIBS PUBLIC
    MLIRIR
    MLIRFuncDialect
    MLIRPass
    MLIRTransforms
    MLIRRewrite
    ScaleChipletChipletDialect
)
