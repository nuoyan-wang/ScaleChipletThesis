//===----------------------------------------------------------------------===//
//
// Chiplet dialect operations for ScaleChiplet.
//
//===----------------------------------------------------------------------===//

#ifndef TOSA_FLOW_DIALECT_CHIPLET_OPS_TD
#define TOSA_FLOW_DIALECT_CHIPLET_OPS_TD

include "mlir/Interfaces/ControlFlowInterfaces.td"
include "mlir/Interfaces/SideEffectInterfaces.td"
include "mlir/IR/BuiltinAttributeInterfaces.td"
include "scale-chiplet/Dialect/Chiplet/Chiplet.td"

//===----------------------------------------------------------------------===//
// Chiplet Ops
//===----------------------------------------------------------------------===//

// Container for tasks within a function.
def GraphOp : ChipletOp<"graph",
    [SingleBlockImplicitTerminator<"YieldOp">,
     ParentOneOf<["::mlir::func::FuncOp"]>]> {
  let summary = "Container for chiplet tasks";
  let description = [{
    Groups a sequence of chiplet.task operations in program order. The body is
    a single block terminated by chiplet.yield and the results mirror any
    values needed outside the container.
  }];

  let results = (outs Variadic<AnyType>:$results);
  let regions = (region SizedRegion<1>:$body);
  let assemblyFormat = "attr-dict-with-keyword ( `:` type($results)^ )? $body";
}

// Group tasks assigned to one chiplet.
def ChipOp : ChipletOp<"chip",
    [SingleBlockImplicitTerminator<"YieldOp">,
     ParentOneOf<["GraphOp", "::mlir::func::FuncOp"]>]> {
  let summary = "Chiplet chip grouping tasks";
  let description = [{
    Groups one or more chiplet.task ops intended to execute on the same chiplet.
    The body is a single block terminated by chiplet.yield.
  }];

  let arguments = (ins I32Attr:$id); // numeric chiplet ID
  let results = (outs Variadic<AnyType>:$results);
  let regions   = (region SizedRegion<1>:$body);
  let assemblyFormat = "$id attr-dict-with-keyword ( `:` type($results)^ )? $body";
}

// Task within a chiplet chip.
def TaskOp : ChipletOp<"task",
    [SingleBlockImplicitTerminator<"YieldOp">,
     ParentOneOf<["GraphOp", "ChipOp", "::mlir::func::FuncOp"]>]> {
  let summary = "Task inside a chiplet chip";
  let description = [{
    Contains a sequence of operations to run on a chiplet; single-block region
    terminated by chiplet.yield.
  }];

  let arguments = (ins Variadic<AnyType>:$operands);
  let results = (outs Variadic<AnyType>:$results);
  let regions = (region SizedRegion<1>:$body);
  let assemblyFormat = "attr-dict-with-keyword `(` $operands `:` type($operands) `)` ( `:` type($results)^ )? $body";
}

// Explicit communication marker for producer->consumer cross-chip edges.
def SendOp : ChipletOp<"send", [ParentOneOf<["TaskOp"]>]> {
  let summary = "Send a value across chiplets";
  let description = [{
    Marks a cross-chip outgoing edge from a task with channel metadata.
  }];

  let arguments = (ins AnyType:$value, I64Attr:$receiver_id, I64Attr:$bytes,
                   StrAttr:$comm_kind);
  let assemblyFormat = "$value attr-dict `:` type($value)";
}

// Explicit communication marker for producer->consumer cross-chip edges.
def RecvOp : ChipletOp<"recv", [ParentOneOf<["TaskOp"]>]> {
  let summary = "Receive a value from another chiplet";
  let description = [{
    Marks a cross-chip incoming edge into a task with channel metadata.
  }];

  let arguments = (ins I64Attr:$sender_id, I64Attr:$bytes, StrAttr:$comm_kind);
  let results = (outs AnyType:$result);
  let assemblyFormat = "attr-dict `:` type($result)";
}

// Terminator for chip/task regions.
def YieldOp : ChipletOp<"yield",
    [NoMemoryEffect, ReturnLike, Terminator,
     ParentOneOf<["GraphOp", "ChipOp", "TaskOp"]>]> {
  let summary = "Terminate and yield results of a chip or task";

  let arguments = (ins Variadic<AnyType>:$results);
  let assemblyFormat = "$results attr-dict `:` type($results)";
  
  let builders = [OpBuilder<(ins), "build($_builder, $_state, llvm::None);">];
}

#endif // TOSA_FLOW_DIALECT_CHIPLET_OPS_TD
