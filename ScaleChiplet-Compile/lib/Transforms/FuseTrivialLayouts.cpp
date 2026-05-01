//===----------------------------------------------------------------------===//
// Inline trivial layout tasks (kind="layout", single-op) into their consumers.
//===----------------------------------------------------------------------===//

#include "scale-chiplet/Transforms/Passes.h"
#include "scale-chiplet/Dialect/Chiplet/Chiplet.h"
#include "llvm/ADT/STLExtras.h"

using namespace mlir;
using namespace mlir::scale_chiplet;
using namespace mlir::scale_chiplet::chiplet;

namespace mlir::scale_chiplet {
#define GEN_PASS_DEF_FUSETRIVIALLAYOUTSPASS
#include "scale-chiplet/Transforms/Passes.h.inc"
} // namespace mlir::scale_chiplet

namespace {
struct FuseTrivialLayoutsPass
    : public mlir::scale_chiplet::impl::FuseTrivialLayoutsPassBase<
          FuseTrivialLayoutsPass> {
  static bool isLayoutTask(TaskOp task) {
    auto attr = task->getAttrOfType<StringAttr>("kind");
    if (!attr || attr.getValue() != "layout")
      return false;
    auto &ops = task.getBody().front().getOperations();
    return ops.size() == 2; // single op + yield
  }

  static bool hasSingleUsePerResult(TaskOp producer) {
    for (Value res : producer.getResults())
      if (!res.hasOneUse())
        return false;
    return true;
  }

  static bool allUsesInsideConsumer(TaskOp producer, TaskOp consumer) {
    for (Value res : producer.getResults()) {
      for (OpOperand &use : res.getUses()) {
        Operation *owner = use.getOwner();
        if (owner == consumer.getOperation())
          continue;
        if (!consumer.getOperation()->isProperAncestor(owner))
          return false;
      }
    }
    return true;
  }

  static Operation *getSingleInnerOp(TaskOp t) {
    Block &b = t.getBody().front();
    Operation &first = b.front();
    return isa<YieldOp>(first) ? nullptr : &first;
  }

  static bool pullLayoutsInto(TaskOp consumer) {
    bool changed = false;
    bool local = true;
    while (local) {
      local = false;
      for (Operation &innerOp :
           llvm::make_early_inc_range(consumer.getBody().front())) {
        if (isa<YieldOp>(&innerOp))
          continue;
        for (Value operand : innerOp.getOperands()) {
          auto producer = operand.getDefiningOp<TaskOp>();
          if (!producer || producer == consumer)
            continue;
          if (!isLayoutTask(producer))
            continue;
          if (!hasSingleUsePerResult(producer))
            continue;
          if (!allUsesInsideConsumer(producer, consumer))
            continue;
          Operation *inner = getSingleInnerOp(producer);
          if (!inner)
            continue;

          inner->moveBefore(&innerOp);

          auto producerYield =
              cast<YieldOp>(producer.getBody().front().getTerminator());
          SmallVector<Value> yielded(producerYield.getOperands().begin(),
                                     producerYield.getOperands().end());
          for (auto it : llvm::enumerate(producer.getResults()))
            it.value().replaceAllUsesWith(yielded[it.index()]);

          producer.erase();
          local = true;
          changed = true;
          break;
        }
        if (local)
          break;
      }
    }
    return changed;
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
          if (pullLayoutsInto(t)) {
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

std::unique_ptr<mlir::Pass> mlir::scale_chiplet::createFuseTrivialLayoutsPass() {
  return std::make_unique<FuseTrivialLayoutsPass>();
}
