//===----------------------------------------------------------------------===//
// Fuse layout-nonlayout-layout task chains into a single task.
//===----------------------------------------------------------------------===//

#include "scale-chiplet/Transforms/Passes.h"
#include "scale-chiplet/Dialect/Chiplet/Chiplet.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/STLExtras.h"

using namespace mlir;
using namespace mlir::scale_chiplet;
using namespace mlir::scale_chiplet::chiplet;

namespace mlir::scale_chiplet {
#define GEN_PASS_DEF_FUSELAYOUTCHAINSPASS
#include "scale-chiplet/Transforms/Passes.h.inc"
} // namespace mlir::scale_chiplet

namespace {
struct FuseLayoutChainsPass
    : public mlir::scale_chiplet::impl::FuseLayoutChainsPassBase<
          FuseLayoutChainsPass> {
  static SmallVector<TaskOp> collectTasks(GraphOp graph) {
    SmallVector<TaskOp> tasks;
    for (Operation &op : graph.getBody().front())
      if (auto task = dyn_cast<TaskOp>(&op))
        tasks.push_back(task);
    return tasks;
  }

  static bool isKind(TaskOp task, StringRef kind) {
    auto attr = task->getAttrOfType<StringAttr>("kind");
    return attr && attr.getValue() == kind;
  }

  // Fuse layout-nonlayout-layout chains into a single task.
  static bool fuseLayoutSandwich(GraphOp graph, SmallVector<TaskOp> &tasks) {
    for (size_t i = 0; i + 2 < tasks.size(); ++i) {
      TaskOp pre = tasks[i];
      TaskOp mid = tasks[i + 1];
      TaskOp post = tasks[i + 2];

      if (!isKind(pre, "layout") || !isKind(post, "layout"))
        continue;

      // Require single-op layout tasks.
      auto &preOps = pre.getBody().front().getOperations();
      auto &postOps = post.getBody().front().getOperations();
      if (preOps.size() != 2 || postOps.size() != 2)
        continue;

      // Build chain set.
      SmallPtrSet<Operation *, 3> chainOps = {pre.getOperation(),
                                              mid.getOperation(),
                                              post.getOperation()};
      auto isOutsideChain = [&](Operation *op) {
        return !chainOps.count(op);
      };

      // Collect live-outs in program order (pre, mid, post).
      SmallVector<Value> liveOuts;
      auto collectLiveOuts = [&](TaskOp t) {
        for (Value res : t.getResults()) {
          bool escapes = llvm::any_of(res.getUses(), [&](OpOperand &use) {
            return isOutsideChain(use.getOwner());
          });
          if (escapes)
            liveOuts.push_back(res);
        }
      };
      collectLiveOuts(pre);
      collectLiveOuts(mid);
      collectLiveOuts(post);

      // Map each task result to the value it yielded (inside the task).
      DenseMap<Value, Value> taskResultToInner;
      auto recordMapping = [&](TaskOp t) {
        auto y = cast<YieldOp>(t.getBody().front().getTerminator());
        for (auto it : llvm::enumerate(t.getResults()))
          taskResultToInner[it.value()] = y.getOperand(it.index());
      };
      recordMapping(pre);
      recordMapping(mid);
      recordMapping(post);

      // Fuse into a new task by moving ops and retuning live-outs.
      SmallVector<Type> resultTypes;
      for (Value v : liveOuts)
        resultTypes.push_back(v.getType());
      DenseMap<Value, unsigned> liveOutIdx;
      for (auto it : llvm::enumerate(liveOuts))
        liveOutIdx[it.value()] = it.index();

      OpBuilder builder(graph);
      builder.setInsertionPoint(pre);
      auto fused =
          builder.create<TaskOp>(pre.getLoc(), resultTypes, ValueRange());
      fused->setAttrs(mid->getAttrs());
      fused->removeAttr("kind"); // merged task is general now
      Block *fusedBlock = builder.createBlock(&fused.getBody());

      auto moveOps = [&](TaskOp t) {
        for (Operation &op :
             llvm::make_early_inc_range(t.getBody().front().without_terminator()))
          op.moveBefore(fusedBlock, fusedBlock->end());
      };
      moveOps(pre);
      moveOps(mid);
      moveOps(post);

      builder.setInsertionPointToEnd(fusedBlock);
      SmallVector<Value> yieldVals;
      yieldVals.reserve(liveOuts.size());
      for (Value v : liveOuts)
        yieldVals.push_back(taskResultToInner.lookup(v));
      builder.create<YieldOp>(post.getLoc(), yieldVals);

      // Replace external uses of live-outs with fused results.
      for (auto it : llvm::enumerate(liveOuts)) {
        Value repl = fused.getResult(it.index());
        Value old = it.value();
        old.replaceUsesWithIf(repl, [&](OpOperand &use) {
          return !fused->isProperAncestor(use.getOwner());
        });
      }

      // Replace remaining uses of task results:
      //  - inside the fused task: use the inner value
      //  - outside the fused task: use the fused result (if exported)
      auto replaceResults = [&](TaskOp t) {
        for (Value res : t.getResults()) {
          Value fusedOut;
          if (auto it = liveOutIdx.find(res); it != liveOutIdx.end())
            fusedOut = fused.getResult(it->second);
          Value inner = taskResultToInner.lookup(res);
          if (inner) {
            res.replaceUsesWithIf(inner, [&](OpOperand &use) {
              return fused->isProperAncestor(use.getOwner());
            });
          }
          if (fusedOut) {
            res.replaceUsesWithIf(fusedOut, [&](OpOperand &use) {
              return !fused->isProperAncestor(use.getOwner());
            });
          }
        }
      };
      replaceResults(pre);
      replaceResults(mid);
      replaceResults(post);

      pre.erase();
      mid.erase();
      post.erase();
      tasks.erase(tasks.begin() + i, tasks.begin() + i + 3);
      tasks.insert(tasks.begin() + i, fused);
      return true;
    }
    return false;
  }

  void runOnOperation() override {
    func::FuncOp func = getOperation();
    bool changed = false;

    func.walk([&](GraphOp graph) {
      bool localChange = true;
      while (localChange) {
        localChange = false;
        auto tasks = collectTasks(graph);
        if (fuseLayoutSandwich(graph, tasks)) {
          localChange = true;
          changed = true;
          continue;
        }
      }
    });

    if (!changed)
      return;
  }
};
} // namespace

std::unique_ptr<mlir::Pass> mlir::scale_chiplet::createFuseLayoutChainsPass() {
  return std::make_unique<FuseLayoutChainsPass>();
}
