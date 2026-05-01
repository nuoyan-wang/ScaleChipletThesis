//===----------------------------------------------------------------------===//
// Outline each chiplet_id into its own func for downstream pipelines.
// This flattens chiplet.task bodies into the new function, wiring operands and
// live-outs as function arguments/results.
//===----------------------------------------------------------------------===//

#include "scale-chiplet/Transforms/Passes.h"
#include "scale-chiplet/Dialect/Chiplet/Chiplet.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/IR/BlockAndValueMapping.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/MLIRContext.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/SmallVector.h"

using namespace mlir;
using namespace mlir::scale_chiplet;
using namespace mlir::scale_chiplet::chiplet;

namespace mlir::scale_chiplet {
#define GEN_PASS_DEF_MATERIALIZECHIPLETSPASS
#include "scale-chiplet/Transforms/Passes.h.inc"
} // namespace mlir::scale_chiplet

namespace {
struct MaterializeChipletsPass
    : public mlir::scale_chiplet::impl::MaterializeChipletsPassBase<
          MaterializeChipletsPass> {
  static void cloneExternalDefTree(Value v, BlockAndValueMapping &mapping,
                                   OpBuilder &builder,
                                   function_ref<bool(Operation *)> isInCluster,
                                   DenseSet<Operation *> &inProgress) {
    if (mapping.contains(v))
      return;
    if (isa<BlockArgument>(v))
      return;

    Operation *def = v.getDefiningOp();
    if (!def || isInCluster(def) || isa<TaskOp>(def))
      return;
    if (!inProgress.insert(def).second)
      return;

    for (Value operand : def->getOperands())
      cloneExternalDefTree(operand, mapping, builder, isInCluster, inProgress);

    Operation *cloned = builder.clone(*def, mapping);
    for (auto it : llvm::zip(def->getResults(), cloned->getResults()))
      mapping.map(std::get<0>(it), std::get<1>(it));
    inProgress.erase(def);
  }

  static SmallVector<Value> getClusterLiveOuts(MutableArrayRef<TaskOp> tasks) {
    SmallPtrSet<Operation *, 32> clusterOps;
    for (TaskOp t : tasks)
      clusterOps.insert(t.getOperation());

    auto isInCluster = [&](Operation *op) {
      while (op) {
        if (clusterOps.count(op))
          return true;
        op = op->getParentOp();
      }
      return false;
    };

    SmallVector<Value> liveOuts;
    for (TaskOp t : tasks) {
      for (Value res : t.getResults()) {
        bool escapes = llvm::any_of(res.getUses(), [&](OpOperand &use) {
          return !isInCluster(use.getOwner());
        });
        if (escapes)
          liveOuts.push_back(res);
      }
    }
    return liveOuts;
  }

  void runOnOperation() override {
    ModuleOp module = getOperation();
    SmallVector<func::FuncOp> newFuncs;
    SmallPtrSet<Operation *, 8> keepFuncs;

    module.walk([&](GraphOp graph) {
      func::FuncOp parentFunc = graph->getParentOfType<func::FuncOp>();
      if (!parentFunc)
        return;

      SmallVector<TaskOp> orderedTasks;
      for (Operation &op : graph.getBody().front()) {
        if (auto t = dyn_cast<TaskOp>(&op))
          orderedTasks.push_back(t);
      }

      if (orderedTasks.empty())
        return;

      struct Cluster {
        int64_t cid;
        SmallVector<TaskOp> tasks;
      };

      SmallVector<Cluster> clusters;
      for (TaskOp t : orderedTasks) {
        auto cidAttr = t->getAttrOfType<IntegerAttr>("chiplet_id");
        int64_t cid = cidAttr ? cidAttr.getInt() : -1;
        if (clusters.empty() || clusters.back().cid != cid) {
          clusters.push_back(Cluster{cid, SmallVector<TaskOp>()});
        }
        clusters.back().tasks.push_back(t);
      }

      for (auto &cluster : clusters) {
        MutableArrayRef<TaskOp> tasks = cluster.tasks;
        int64_t cid = cluster.cid;

        struct RecvPort {
          int64_t senderId;
          int64_t bytes;
          StringAttr commKind;
          Type type;
          RecvOp op;
        };
        struct SendPort {
          int64_t receiverId;
          int64_t bytes;
          StringAttr commKind;
          Type type;
          SendOp op;
        };

        SmallPtrSet<Operation *, 32> clusterOps;
        for (TaskOp t : tasks)
          clusterOps.insert(t.getOperation());

        auto isInCluster = [&](Operation *op) {
          while (op) {
            if (clusterOps.count(op))
              return true;
            op = op->getParentOp();
          }
          return false;
        };

        SmallVector<RecvPort> recvPorts;
        DenseMap<Operation *, unsigned> recvPortIndex;
        SmallVector<SendPort> sendPorts;
        DenseMap<Operation *, unsigned> sendPortIndex;
        for (TaskOp t : tasks) {
          for (Operation &op : t.getBody().front()) {
            if (auto recv = dyn_cast<RecvOp>(&op)) {
              if (recvPortIndex.count(recv.getOperation()))
                continue;
              recvPortIndex[recv.getOperation()] = recvPorts.size();
              recvPorts.push_back(RecvPort{
                  recv.getSenderIdAttr().getInt(), recv.getBytesAttr().getInt(),
                  recv.getCommKindAttr(), recv.getResult().getType(), recv});
            } else if (auto send = dyn_cast<SendOp>(&op)) {
              if (sendPortIndex.count(send.getOperation()))
                continue;
              sendPortIndex[send.getOperation()] = sendPorts.size();
              sendPorts.push_back(SendPort{
                  send.getReceiverIdAttr().getInt(), send.getBytesAttr().getInt(),
                  send.getCommKindAttr(), send.getValue().getType(), send});
            }
          }
        }

        SmallVector<Value> extOperands;
        DenseMap<Value, unsigned> extIndex;
        SmallVector<Value> materializeValues;
        DenseSet<Value> seenMaterializeValues;
        for (TaskOp t : tasks) {
          for (Value v : t.getOperands()) {
            Operation *def = v.getDefiningOp();
            if (def && isInCluster(def))
              continue;
            if (def && !isa<TaskOp>(def) &&
                seenMaterializeValues.insert(v).second) {
              materializeValues.push_back(v);
              continue;
            }
            if (extIndex.count(v))
              continue;
            extIndex[v] = extOperands.size();
            extOperands.push_back(v);
          }
        }

        SmallVector<Value> liveOuts = getClusterLiveOuts(tasks);

        SmallVector<Type> argTypes, resultTypes;
        for (Value v : extOperands)
          argTypes.push_back(v.getType());
        for (const RecvPort &p : recvPorts)
          argTypes.push_back(p.type);
        for (Value v : liveOuts)
          resultTypes.push_back(v.getType());
        for (const SendPort &p : sendPorts)
          resultTypes.push_back(p.type);

        std::string fnName = (parentFunc.getName() + "_chip" +
                              Twine(cid).str())
                                 .str();
        OpBuilder moduleBuilder(module.getBodyRegion());
        auto fnType = moduleBuilder.getFunctionType(argTypes, resultTypes);
        auto newFunc =
            moduleBuilder.create<func::FuncOp>(graph.getLoc(), fnName, fnType);
        newFuncs.push_back(newFunc);
        keepFuncs.insert(newFunc.getOperation());

        Block *entry = newFunc.addEntryBlock();
        BlockAndValueMapping mapping;
        for (auto it : llvm::enumerate(extOperands))
          mapping.map(it.value(), entry->getArgument(it.index()));

        Builder attrBuilder(module.getContext());
        for (auto it : llvm::enumerate(recvPorts)) {
          unsigned argIndex = extOperands.size() + it.index();
          SmallVector<NamedAttribute> attrs;
          attrs.push_back(attrBuilder.getNamedAttr(
              "chiplet.sender_id",
              IntegerAttr::get(IntegerType::get(module.getContext(), 64),
                               it.value().senderId)));
          attrs.push_back(attrBuilder.getNamedAttr(
              "chiplet.bytes",
              IntegerAttr::get(IntegerType::get(module.getContext(), 64),
                               it.value().bytes)));
          attrs.push_back(attrBuilder.getNamedAttr("chiplet.comm_kind",
                                                   it.value().commKind));
          attrs.push_back(attrBuilder.getNamedAttr(
              "chiplet.comm_dir", StringAttr::get(module.getContext(), "recv")));
          newFunc.setArgAttrs(argIndex,
                              DictionaryAttr::get(module.getContext(), attrs));
        }
        for (auto it : llvm::enumerate(sendPorts)) {
          unsigned resIndex = liveOuts.size() + it.index();
          SmallVector<NamedAttribute> attrs;
          attrs.push_back(attrBuilder.getNamedAttr(
              "chiplet.receiver_id",
              IntegerAttr::get(IntegerType::get(module.getContext(), 64),
                               it.value().receiverId)));
          attrs.push_back(attrBuilder.getNamedAttr(
              "chiplet.bytes",
              IntegerAttr::get(IntegerType::get(module.getContext(), 64),
                               it.value().bytes)));
          attrs.push_back(attrBuilder.getNamedAttr("chiplet.comm_kind",
                                                   it.value().commKind));
          attrs.push_back(attrBuilder.getNamedAttr(
              "chiplet.comm_dir", StringAttr::get(module.getContext(), "send")));
          newFunc.setResultAttrs(resIndex,
                                 DictionaryAttr::get(module.getContext(), attrs));
        }

        OpBuilder defBuilder(entry, entry->begin());
        DenseSet<Operation *> inProgress;
        for (Value v : materializeValues)
          cloneExternalDefTree(v, mapping, defBuilder, isInCluster, inProgress);

        OpBuilder b(entry, entry->end());
        SmallVector<Value> sendReturnVals(sendPorts.size());
        for (TaskOp t : tasks) {
          Block &taskBody = t.getBody().front();
          for (auto it : llvm::enumerate(taskBody.getArguments())) {
            Value src = t.getOperand(it.index());
            Value mappedSrc = mapping.lookupOrDefault(src);
            mapping.map(it.value(), mappedSrc);
          }
          for (Operation &op : taskBody) {
            if (isa<YieldOp>(&op))
              continue;
            if (auto recv = dyn_cast<RecvOp>(&op)) {
              auto recvIt = recvPortIndex.find(recv.getOperation());
              if (recvIt == recvPortIndex.end()) {
                parentFunc.emitError("missing recv port mapping");
                signalPassFailure();
                return;
              }
              unsigned argIndex = extOperands.size() + recvIt->second;
              mapping.map(recv.getResult(), entry->getArgument(argIndex));
              continue;
            }
            if (auto send = dyn_cast<SendOp>(&op)) {
              auto sendIt = sendPortIndex.find(send.getOperation());
              if (sendIt == sendPortIndex.end()) {
                parentFunc.emitError("missing send port mapping");
                signalPassFailure();
                return;
              }
              sendReturnVals[sendIt->second] = mapping.lookupOrDefault(send.getValue());
              continue;
            }
            Operation *cloned = b.clone(op, mapping);
            for (auto it : llvm::zip(op.getResults(), cloned->getResults()))
              mapping.map(std::get<0>(it), std::get<1>(it));
          }
          auto yield = cast<YieldOp>(taskBody.getTerminator());
          for (auto it : llvm::enumerate(yield.getOperands())) {
            Value mappedVal = mapping.lookupOrDefault(it.value());
            mapping.map(t.getResult(it.index()), mappedVal);
          }
        }

        SmallVector<Value> retVals;
        for (Value v : liveOuts)
          retVals.push_back(mapping.lookupOrDefault(v));
        for (auto it : llvm::enumerate(sendPorts)) {
          Value retV = sendReturnVals[it.index()];
          if (!retV)
            retV = mapping.lookupOrDefault(it.value().op.getValue());
          if (!retV) {
            parentFunc.emitError("missing send value for receiver_id ")
                << it.value().receiverId;
            signalPassFailure();
            return;
          }
          retVals.push_back(retV);
        }
        b.create<func::ReturnOp>(graph.getLoc(), retVals);
      }

    });

    if (newFuncs.empty())
      return;

    for (Operation &op :
         llvm::make_early_inc_range(module.getBody()->getOperations())) {
      if (!keepFuncs.count(&op))
        op.erase();
    }
  }
};
} // namespace

std::unique_ptr<mlir::Pass> mlir::scale_chiplet::createMaterializeChipletsPass() {
  return std::make_unique<MaterializeChipletsPass>();
}
