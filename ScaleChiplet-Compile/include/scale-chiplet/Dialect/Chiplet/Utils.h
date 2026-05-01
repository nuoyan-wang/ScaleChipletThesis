//===----------------------------------------------------------------------===//
// Chiplet dialect utilities.
//===----------------------------------------------------------------------===//
#pragma once

#include "mlir/IR/BuiltinTypes.h"
#include "mlir/IR/PatternMatch.h"
#include "scale-chiplet/Dialect/Chiplet/Chiplet.h"

namespace mlir::scale_chiplet::chiplet {

TaskOp fuseOpsIntoTask(ArrayRef<Operation *> ops, PatternRewriter &rewriter,
                       int64_t chipletId = 0, bool insertToLastOp = false);

} // namespace mlir::scale_chiplet::chiplet
