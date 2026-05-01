//===----------------------------------------------------------------------===//
// Build simple DAG annotations for chiplet.task ops.
// Attaches:
//   dag_in  = [task_id,...]  (predecessors)
//   dag_out = [task_id,...]  (successors)
//   topo_index = integer topological order
//===----------------------------------------------------------------------===//

#include "scale-chiplet/Transforms/Passes.h"
#include "scale-chiplet/Dialect/Chiplet/Chiplet.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/SmallSet.h"

using namespace mlir;
using namespace mlir::scale_chiplet;
using namespace mlir::scale_chiplet::chiplet;

namespace mlir::scale_chiplet {
#define GEN_PASS_DEF_ANNOTATETASKDAGPASS
#include "scale-chiplet/Transforms/Passes.h.inc"
} // namespace mlir::scale_chiplet

namespace {
struct AnnotateTaskDAGPass
    : public mlir::scale_chiplet::impl::AnnotateTaskDAGPassBase<
          AnnotateTaskDAGPass> {
  void runOnOperation() override {
    func::FuncOp func = getOperation();

    func.walk([&](GraphOp graph) {
      SmallVector<TaskOp> tasks;
      DenseMap<TaskOp, int64_t> taskIdMap;
      int64_t nextId = 0;
      for (Operation &op : graph.getBody().front())
        if (auto t = dyn_cast<TaskOp>(&op)) {
          tasks.push_back(t);
          int64_t tid = nextId++;
          t->setAttr("task_id", IntegerAttr::get(IntegerType::get(func.getContext(), 64), tid));
          taskIdMap[t] = tid;
        }

      // Build preds/succs maps by inspecting operand defs.
      DenseMap<TaskOp, SmallVector<int64_t>> preds, succs;
      for (TaskOp consumer : tasks) {
        for (Value opnd : consumer.getOperands()) {
          if (auto producer = opnd.getDefiningOp<TaskOp>()) {
            if (producer == consumer)
              continue;
            auto pid = taskIdMap.lookup(producer);
            auto cid = taskIdMap.lookup(consumer);
            preds[consumer].push_back(pid);
            succs[producer].push_back(cid);
          }
        }
      }

      // Simple topo order using Kahn's algorithm.
      DenseMap<int64_t, int64_t> indegree;
      for (TaskOp t : tasks) {
        int64_t tid = taskIdMap.lookup(t);
        indegree[tid] = preds[t].size();
      }
      SmallVector<int64_t> queue;
      for (auto &kv : indegree)
        if (kv.second == 0)
          queue.push_back(kv.first);

      DenseMap<int64_t, int64_t> topoIndex;
      int64_t idx = 0;
      while (!queue.empty()) {
        int64_t tid = queue.pop_back_val();
        topoIndex[tid] = idx++;
        // successors of tid
        for (int64_t succId : succs.lookup(getTaskById(tasks, taskIdMap, tid)))
          if (--indegree[succId] == 0)
            queue.push_back(succId);
      }

      OpBuilder b(func.getContext());
      for (TaskOp t : tasks) {
        auto tid = taskIdMap.lookup(t);
        auto toI64Array = [&](ArrayRef<int64_t> v) {
          return b.getI64ArrayAttr(SmallVector<int64_t>(v.begin(), v.end()));
        };
        t->setAttr("dag_in", toI64Array(preds.lookup(t)));
        t->setAttr("dag_out", toI64Array(succs.lookup(t)));
        if (topoIndex.count(tid))
          t->setAttr("topo_index", b.getI64IntegerAttr(topoIndex[tid]));
      }
    });
  }

  static TaskOp getTaskById(const SmallVector<TaskOp> &tasks,
                            const DenseMap<TaskOp, int64_t> &map, int64_t id) {
    for (TaskOp t : tasks)
      if (map.lookup(t) == id)
        return t;
    return nullptr;
  }
};
} // namespace

std::unique_ptr<mlir::Pass> mlir::scale_chiplet::createAnnotateTaskDAGPass() {
  return std::make_unique<AnnotateTaskDAGPass>();
}
