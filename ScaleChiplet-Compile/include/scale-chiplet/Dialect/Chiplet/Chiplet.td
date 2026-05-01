//===----------------------------------------------------------------------===//
//
// Chiplet dialect for ScaleChiplet.
//
//===----------------------------------------------------------------------===//

#ifndef TOSA_FLOW_DIALECT_CHIPLET_TD
#define TOSA_FLOW_DIALECT_CHIPLET_TD

include "mlir/IR/AttrTypeBase.td"
include "mlir/IR/OpBase.td"

def ChipletDialect : Dialect {
  let name = "chiplet";
  let summary = "Chiplet grouping dialect for ScaleChiplet";
  let description = [{
    This dialect introduces chiplet-level grouping operations that can wrap
    parts of a graph to represent placement onto individual chiplets.
  }];
  let useDefaultTypePrinterParser = 1;
  let useDefaultAttributePrinterParser = 1;
  let cppNamespace = "::mlir::scale_chiplet::chiplet";
}

class ChipletOp<string mnemonic, list<Trait> traits = []> :
    Op<ChipletDialect, mnemonic, traits>;

include "scale-chiplet/Dialect/Chiplet/Ops.td"

#endif // TOSA_FLOW_DIALECT_CHIPLET_TD
