//===----------------------------------------------------------------------===//
// Make chiplet.task operands explicit: add block args and set task operands.
//===----------------------------------------------------------------------===//

#include "scale-chiplet/Transforms/Passes.h"
#include "scale-chiplet/Dialect/Chiplet/Chiplet.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/SmallVector.h"

using namespace mlir;
using namespace mlir::scale_chiplet;
using namespace mlir::scale_chiplet::chiplet;

namespace mlir::scale_chiplet {
#define GEN_PASS_DEF_NORMALIZETASKOPERANDSPASS
#include "scale-chiplet/Transforms/Passes.h.inc"
} // namespace mlir::scale_chiplet

namespace {
struct NormalizeTaskOperandsPass
    : public mlir::scale_chiplet::impl::NormalizeTaskOperandsPassBase<
          NormalizeTaskOperandsPass> {
  static bool normalizeTask(TaskOp task) {
    Block &body = task.getBody().front();
    DenseMap<Value, BlockArgument> mapping;
    SmallVector<Value> operands;

    // Collect external captures in first-use order and create block args.
    for (Operation &op : body) {
      if (isa<YieldOp>(&op))
        continue;
      for (Value v : op.getOperands()) {
        if (task->isAncestor(v.getParentBlock()->getParentOp()))
          continue; // defined inside task
        if (mapping.count(v))
          continue;
        BlockArgument arg = body.addArgument(v.getType(), task.getLoc());
        mapping[v] = arg;
        operands.push_back(v);
      }
    }

    if (operands.empty())
      return false;

    // Replace uses of captured values inside the task with the new block args.
    for (auto &entry : mapping) {
      Value captured = entry.first;
      BlockArgument arg = entry.second;
      captured.replaceUsesWithIf(arg, [&](OpOperand &use) {
        return task->isAncestor(use.getOwner());
      });
    }

    task->setOperands(operands);
    return true;
  }

  void runOnOperation() override {
    func::FuncOp func = getOperation();
    bool changed = false;

    func.walk([&](GraphOp graph) {
      for (Operation &op : graph.getBody().front()) {
        if (auto task = dyn_cast<TaskOp>(&op))
          changed |= normalizeTask(task);
      }
    });

    if (!changed)
      return;
  }
};
} // namespace

std::unique_ptr<mlir::Pass> mlir::scale_chiplet::createNormalizeTaskOperandsPass() {
  return std::make_unique<NormalizeTaskOperandsPass>();
}
