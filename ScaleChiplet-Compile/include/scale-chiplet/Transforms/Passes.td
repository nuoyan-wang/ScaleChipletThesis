include "mlir/Pass/PassBase.td"

def TosaConvPoolFusionPass : Pass<"tosa-fuse-conv-pool"> {
  let summary = "Fuse TOSA conv2d + clamp + pool patterns";
  let constructor = "mlir::scale_chiplet::createTosaConvPoolFusionPass()";
}

def CreateChipletFromTosa :
      Pass<"tosa-create-chiplet-from-tosa", "func::FuncOp"> {
  let summary = "Outline selected TOSA ops into chiplet.task/chip";
  let constructor = "mlir::scale_chiplet::createCreateChipletFromTosaPass()";
}

def FuseLayoutChainsPass :
      Pass<"tosa-fuse-layout-chains", "func::FuncOp"> {
  let summary = "Fuse layout-nonlayout-layout chains into a single task";
  let constructor = "mlir::scale_chiplet::createFuseLayoutChainsPass()";
}

def FuseTrivialLayoutsPass :
      Pass<"tosa-fuse-trivial-layouts", "func::FuncOp"> {
  let summary = "Inline trivial layout tasks into their consumers";
  let constructor = "mlir::scale_chiplet::createFuseTrivialLayoutsPass()";
}

def FuseTrivialConstsPass :
      Pass<"tosa-fuse-trivial-consts", "func::FuncOp"> {
  let summary = "Inline trivial const tasks into their consumers";
  let constructor = "mlir::scale_chiplet::createFuseTrivialConstsPass()";
}

def PruneDeadTaskYieldsPass :
      Pass<"tosa-prune-dead-task-yields", "func::FuncOp"> {
  let summary = "Remove unused chiplet.task results/yields";
  let constructor = "mlir::scale_chiplet::createPruneDeadTaskYieldsPass()";
}

def NormalizeTaskOperandsPass :
      Pass<"tosa-normalize-task-operands", "func::FuncOp"> {
  let summary = "Make chiplet.task operands explicit via block arguments";
  let constructor = "mlir::scale_chiplet::createNormalizeTaskOperandsPass()";
}

def CostAnnotatePass :
      Pass<"tosa-cost-annotate", "func::FuncOp"> {
  let summary = "Annotate chiplet tasks with coarse flops/bytes costs";
  let constructor = "mlir::scale_chiplet::createCostAnnotatePass()";
}

def AnnotateTaskDAGPass :
      Pass<"tosa-annotate-task-dag", "func::FuncOp"> {
  let summary = "Annotate chiplet tasks with DAG predecessor/successor info";
  let constructor = "mlir::scale_chiplet::createAnnotateTaskDAGPass()";
}

def EvenClusterTasksPass :
      Pass<"tosa-even-cluster-tasks", "func::FuncOp"> {
  let summary = "Evenly assign chiplet_id across tasks for a fixed chiplet count";
  let constructor = "mlir::scale_chiplet::createEvenClusterTasksPass()";
  let options = [
    Option<"numChiplets", "num-chiplets", "int", "8",
           "Number of chiplets to evenly assign tasks to">
  ];
}

def MergeClusterTasksPass :
      Pass<"tosa-merge-cluster-tasks", "func::FuncOp"> {
  let summary = "Merge contiguous tasks with the same chiplet_id into one task";
  let constructor = "mlir::scale_chiplet::createMergeClusterTasksPass()";
}

def InsertChipletSendRecvPass :
      Pass<"tosa-insert-chiplet-send-recv", "func::FuncOp"> {
  let summary = "Insert chiplet.send/chiplet.recv on cross-chip task edges";
  let constructor = "mlir::scale_chiplet::createInsertChipletSendRecvPass()";
}

def MaterializeChipletsPass :
      Pass<"tosa-materialize-chiplets", "ModuleOp"> {
  let summary = "Outline each chiplet_id into its own func for downstream pipelines";
  let constructor = "mlir::scale_chiplet::createMaterializeChipletsPass()";
}

def LegalizeAddBiasPass :
      Pass<"tosa-legalize-add-bias", "ModuleOp"> {
  let summary = "Normalize chip7 classifier tail while preserving bias addition";
  let constructor = "mlir::scale_chiplet::createLegalizeAddBiasPass()";
}

def LegalizeDepthwiseTailPass :
      Pass<"tosa-legalize-depthwise-tail", "ModuleOp"> {
  let summary = "Avoid depthwise-conv terminal chiplet tails that fail in ScaleHLS";
  let constructor = "mlir::scale_chiplet::createLegalizeDepthwiseTailPass()";
}

def LegalizeTransformerTailPass :
      Pass<"tosa-legalize-transformer-tail", "ModuleOp"> {
  let summary = "Avoid transformer chiplet tails that fail in ScaleHLS";
  let constructor = "mlir::scale_chiplet::createLegalizeTransformerTailPass()";
}
