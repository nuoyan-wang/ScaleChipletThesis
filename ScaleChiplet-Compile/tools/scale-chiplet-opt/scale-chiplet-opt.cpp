//===----------------------------------------------------------------------===//
//
// ScaleChiplet Optimization Tool
//
//===----------------------------------------------------------------------===//

#include "mlir/Tools/mlir-opt/MlirOptMain.h"
#include "scale-chiplet/InitAllDialects.h"
#include "scale-chiplet/InitAllPasses.h"

int main(int argc, char **argv) {
  mlir::DialectRegistry registry;

  mlir::scale_chiplet::registerAllDialects(registry);
  mlir::scale_chiplet::registerAllPasses();

  return mlir::failed(mlir::MlirOptMain(
      argc, argv, "ScaleChiplet Optimization Tool", registry, /*allowUnregisteredDialects=*/true));
}
