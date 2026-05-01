//===----------------------------------------------------------------------===//
// Chiplet dialect utilities
//===----------------------------------------------------------------------===//

#include "scale-chiplet/Dialect/Chiplet/Utils.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SetVector.h"
#include "mlir/IR/BuiltinTypes.h"

using namespace mlir;
using namespace mlir::scale_chiplet::chiplet;

TaskOp mlir::scale_chiplet::chiplet::fuseOpsIntoTask(
    ArrayRef<Operation *> ops, PatternRewriter &rewriter, int64_t chipletId,
    bool insertToLastOp) {
  assert(!ops.empty() && "must fuse at least one op");

  // Collect output values that have users outside the selected set.
  llvm::SmallDenseSet<Operation *, 4> opSet(ops.begin(), ops.end());
  llvm::SetVector<Value> outputValues;
  for (Operation *op : ops)
    for (Value result : op->getResults())
      if (llvm::any_of(result.getUsers(),
                       [&](Operation *user) { return !opSet.count(user); }))
        outputValues.insert(result);

  // Create the chip/task scaffold.
  Location loc = rewriter.getUnknownLoc();
  if (!insertToLastOp)
    rewriter.setInsertionPoint(ops.front());
  else
    rewriter.setInsertionPointAfter(ops.back());

  auto chip = rewriter.create<ChipOp>(
      loc, TypeRange(outputValues.getArrayRef()),
      rewriter.getI32IntegerAttr(chipletId));
  Block *chipBlock = rewriter.createBlock(&chip.getBody());

  rewriter.setInsertionPointToEnd(chipBlock);
  auto task =
      rewriter.create<TaskOp>(loc, TypeRange(outputValues.getArrayRef()),
                              ValueRange());
  Block *taskBlock = rewriter.createBlock(&task.getBody());

  // Move ops into the task region and yield their forwarded results.
  rewriter.setInsertionPointToEnd(taskBlock);
  auto taskYield = rewriter.create<YieldOp>(loc, outputValues.getArrayRef());
  for (Operation *op : ops)
    op->moveBefore(taskYield);

  rewriter.setInsertionPointToEnd(chipBlock);
  rewriter.create<YieldOp>(loc, task.getResults());

  // Replace external uses with chip results.
  unsigned idx = 0;
  for (Value output : outputValues)
    output.replaceUsesWithIf(chip.getResult(idx++), [&](OpOperand &use) {
      return !chip->isProperAncestor(use.getOwner());
    });

  // Inline nested tasks to avoid deep nesting when fusing existing tasks.
  for (auto subTask : llvm::make_early_inc_range(task.getOps<TaskOp>())) {
    auto &subOps = subTask.getBody().front().getOperations();
    auto &taskOps = task.getBody().front().getOperations();
    taskOps.splice(subTask->getIterator(), subOps, subOps.begin(),
                   std::prev(subOps.end()));
    auto *term = subTask.getBody().front().getTerminator();
    auto yield = cast<YieldOp>(term);
    rewriter.replaceOp(subTask, yield.getOperands());
  }

  return task;
}
