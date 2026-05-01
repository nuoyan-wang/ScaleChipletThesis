//===----------------------------------------------------------------------===//
// Chiplet dialect entry
//===----------------------------------------------------------------------===//
#pragma once

#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/IR/BuiltinTypes.h"
#include "mlir/IR/Dialect.h"
#include "mlir/IR/OpDefinition.h"
#include "mlir/IR/OperationSupport.h"
#include "scale-chiplet/Dialect/Chiplet/ChipletDialect.h.inc"

#define GET_OP_CLASSES
#include "scale-chiplet/Dialect/Chiplet/Chiplet.h.inc"
