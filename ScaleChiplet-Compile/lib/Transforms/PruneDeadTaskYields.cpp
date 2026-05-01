//===----------------------------------------------------------------------===//
// Remove unused chiplet.task results by shrinking chiplet.yield and task type.
//===----------------------------------------------------------------------===//

#include "scale-chiplet/Transforms/Passes.h"
#include "scale-chiplet/Dialect/Chiplet/Chiplet.h"

using namespace mlir;
using namespace mlir::scale_chiplet;
using namespace mlir::scale_chiplet::chiplet;

namespace mlir::scale_chiplet {
#define GEN_PASS_DEF_PRUNEDEADTASKYIELDSPASS
#include "scale-chiplet/Transforms/Passes.h.inc"
} // namespace mlir::scale_chiplet

namespace {
struct PruneDeadTaskYieldsPass
    : public mlir::scale_chiplet::impl::PruneDeadTaskYieldsPassBase<
          PruneDeadTaskYieldsPass> {
  static bool pruneTask(TaskOp task) {
    auto &body = task.getBody().front();
    auto yield = cast<YieldOp>(body.getTerminator());

    SmallVector<unsigned> liveIdx;
    SmallVector<Type> liveTypes;
    liveIdx.reserve(task.getNumResults());
    liveTypes.reserve(task.getNumResults());

    for (auto it : llvm::enumerate(task.getResults())) {
      Value res = it.value();
      bool live = false;
      for (OpOperand &use : res.getUses()) {
        Operation *owner = use.getOwner();
        if (!task->isProperAncestor(owner)) {
          live = true;
          break;
        }
        // Conservative: if used inside the task, keep it.
        if (task->isAncestor(owner)) {
          live = true;
          break;
        }
      }
      if (live) {
        liveIdx.push_back(it.index());
        liveTypes.push_back(res.getType());
      }
    }

    if (liveIdx.size() == task.getNumResults())
      return false; // nothing to prune

    // Build live yield operands.
    SmallVector<Value> liveYield;
    liveYield.reserve(liveIdx.size());
    for (unsigned idx : liveIdx)
      liveYield.push_back(yield.getOperand(idx));

    OpBuilder builder(task);
    auto newTask =
        builder.create<TaskOp>(task.getLoc(), liveTypes, task.getOperands());
    newTask->setAttrs(task->getAttrs());

    // Move body into the new task.
    newTask.getBody().takeBody(task.getBody());

    // Fix the yield inside the moved body.
    auto *term = newTask.getBody().front().getTerminator();
    auto oldYield = cast<YieldOp>(term);
    builder.setInsertionPoint(oldYield);
    builder.create<YieldOp>(oldYield.getLoc(), liveYield);
    oldYield.erase();

    // Rewire uses from old results to new ones.
    for (auto it : llvm::enumerate(liveIdx)) {
      task.getResult(it.value())
          .replaceAllUsesWith(newTask.getResult(it.index()));
    }

    task.erase();
    return true;
  }

  void runOnOperation() override {
    func::FuncOp func = getOperation();
    bool changed = false;

    func.walk([&](GraphOp graph) {
      bool again = true;
      while (again) {
        again = false;
        SmallVector<TaskOp> tasks;
        for (Operation &op : graph.getBody().front())
          if (auto t = dyn_cast<TaskOp>(&op))
            tasks.push_back(t);

        for (TaskOp t : tasks) {
          if (pruneTask(t)) {
            changed = true;
            again = true;
            break;
          }
        }
      }
    });

    if (!changed)
      return;
  }
};
} // namespace

std::unique_ptr<mlir::Pass> mlir::scale_chiplet::createPruneDeadTaskYieldsPass() {
  return std::make_unique<PruneDeadTaskYieldsPass>();
}
