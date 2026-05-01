//===----------------------------------------------------------------------===//
// Merge contiguous tasks with the same chiplet_id into a single task.
//===----------------------------------------------------------------------===//

#include "scale-chiplet/Transforms/Passes.h"
#include "scale-chiplet/Dialect/Chiplet/Chiplet.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/SmallVector.h"
#include "mlir/IR/BlockAndValueMapping.h"

using namespace mlir;
using namespace mlir::scale_chiplet;
using namespace mlir::scale_chiplet::chiplet;

namespace mlir::scale_chiplet {
#define GEN_PASS_DEF_MERGECLUSTERTASKSPASS
#include "scale-chiplet/Transforms/Passes.h.inc"
} // namespace mlir::scale_chiplet

namespace {
struct MergeClusterTasksPass
    : public mlir::scale_chiplet::impl::MergeClusterTasksPassBase<
          MergeClusterTasksPass> {
  void runOnOperation() override {
    func::FuncOp func = getOperation();
    func.walk([&](GraphOp graph) {
      SmallVector<TaskOp> tasks;
      for (Operation &op : graph.getBody().front())
        if (auto t = dyn_cast<TaskOp>(&op))
          tasks.push_back(t);

      for (size_t i = 0; i < tasks.size();) {
        auto cidAttr = tasks[i]->getAttrOfType<IntegerAttr>("chiplet_id");
        if (!cidAttr) {
          ++i;
          continue;
        }
        int64_t cid = cidAttr.getInt();
        size_t j = i + 1;
        while (j < tasks.size()) {
          auto nextAttr = tasks[j]->getAttrOfType<IntegerAttr>("chiplet_id");
          if (!nextAttr || nextAttr.getInt() != cid)
            break;
          ++j;
        }
        // tasks[i..j) share the same chiplet_id.
        if (j - i == 1) {
          i = j;
          continue;
        }
        SmallVector<TaskOp> cluster(tasks.begin() + i, tasks.begin() + j);
        TaskOp merged = mergeCluster(cluster, cid, func.getContext());
        tasks.erase(tasks.begin() + i, tasks.begin() + j);
        tasks.insert(tasks.begin() + i, merged);
        ++i;
      }
    });
  }

  TaskOp mergeCluster(SmallVector<TaskOp> &cluster, int64_t cid,
                      MLIRContext *ctx) {
    if (cluster.empty())
      return nullptr;

    // Collect ops in cluster.
    SmallPtrSet<Operation *, 32> clusterOps;
    for (TaskOp t : cluster)
      clusterOps.insert(t.getOperation());

    auto isInCluster = [&](Operation *op) {
      while (op) {
        if (clusterOps.count(op))
          return true;
        op = op->getParentOp();
      }
      return false;
    };

    // Collect external operands (task boundary inputs) in first-use order.
    SmallVector<Value> extOperands;
    DenseMap<Value, unsigned> extIndex;
    for (TaskOp t : cluster) {
      for (Value v : t.getOperands()) {
        Operation *def = v.getDefiningOp();
        if (def && isInCluster(def))
          continue;
        if (extIndex.count(v))
          continue;
        extIndex[v] = extOperands.size();
        extOperands.push_back(v);
      }
    }

    // Collect live-out values (task results) used outside the cluster.
    SmallVector<Value> liveOuts;
    for (TaskOp t : cluster) {
      for (Value res : t.getResults()) {
        bool escapes = llvm::any_of(res.getUses(), [&](OpOperand &use) {
          return !isInCluster(use.getOwner());
        });
        if (escapes)
          liveOuts.push_back(res);
      }
    }

    // Create merged task.
    SmallVector<Type> resultTypes;
    for (Value v : liveOuts)
      resultTypes.push_back(v.getType());

    OpBuilder builder(cluster.front());
    auto merged =
        builder.create<TaskOp>(cluster.front().getLoc(), resultTypes, extOperands);
    merged->setAttrs(cluster.front()->getAttrs());
    Block *body = builder.createBlock(&merged.getBody());
    for (Value opnd : extOperands)
      body->addArgument(opnd.getType(), merged.getLoc());

    BlockAndValueMapping mapping;
    for (auto it : llvm::enumerate(extOperands))
      mapping.map(it.value(), body->getArgument(it.index()));

    // Map each original task block argument to the value it represents (either
    // an external operand or an internal task result).
    for (TaskOp t : cluster) {
      Block &oldBody = t.getBody().front();
      for (auto it : llvm::enumerate(oldBody.getArguments())) {
        Value incoming = t.getOperand(it.index());
        mapping.map(it.value(), mapping.lookupOrDefault(incoming));
      }
    }

    builder.setInsertionPointToStart(body);
    for (TaskOp t : cluster) {
      // Clone body ops.
      for (Operation &op : t.getBody().front()) {
        if (isa<YieldOp>(&op))
          continue;
        builder.clone(op, mapping);
        // Results of ops are recorded in the mapping automatically.
      }
      // Map original task results to the yielded values for downstream users.
      auto yield = cast<YieldOp>(t.getBody().front().getTerminator());
      for (auto it : llvm::enumerate(yield.getOperands())) {
        Value mappedVal = mapping.lookupOrDefault(it.value());
        mapping.map(t.getResult(it.index()), mappedVal);
      }
    }

    // Yield live-out values.
    SmallVector<Value> yieldVals;
    for (Value v : liveOuts)
      yieldVals.push_back(mapping.lookupOrDefault(v));
    builder.create<YieldOp>(merged.getLoc(), yieldVals);

    // Redirect live-out uses to merged task results (only outside the merged op).
    for (auto it : llvm::enumerate(liveOuts)) {
      Value newRes = merged.getResult(it.index());
      it.value().replaceUsesWithIf(newRes, [&](OpOperand &use) {
        return !merged->isProperAncestor(use.getOwner());
      });
    }

    // If any task results still have users (e.g., missed liveOut), rewrite them
    // to the cloned value inside the merged body.
    for (TaskOp t : cluster) {
      auto yield = cast<YieldOp>(t.getBody().front().getTerminator());
      for (auto it : llvm::enumerate(t.getResults())) {
        Value res = it.value();
        if (res.use_empty())
          continue;
        Value replacement = mapping.lookupOrDefault(yield.getOperand(it.index()));
        res.replaceAllUsesWith(replacement);
      }
    }

    // Erase old tasks (back-to-front to drop intra-cluster uses first).
    for (auto tIt = cluster.rbegin(); tIt != cluster.rend(); ++tIt)
      (*tIt).erase();
    // Ensure merged has correct chiplet_id.
    merged->setAttr("chiplet_id",
                    IntegerAttr::get(IntegerType::get(ctx, 64), cid));
    return merged;
  }
};
} // namespace

std::unique_ptr<mlir::Pass> mlir::scale_chiplet::createMergeClusterTasksPass() {
  return std::make_unique<MergeClusterTasksPass>();
}
