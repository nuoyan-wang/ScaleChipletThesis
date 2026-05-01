//===----------------------------------------------------------------------===//
// Insert chiplet.send/chiplet.recv on cross-chip task edges.
//===----------------------------------------------------------------------===//

#include "scale-chiplet/Transforms/Passes.h"
#include "scale-chiplet/Dialect/Chiplet/Chiplet.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/SmallVector.h"
#include "mlir/IR/BuiltinAttributes.h"

using namespace mlir;
using namespace mlir::scale_chiplet;
using namespace mlir::scale_chiplet::chiplet;

namespace mlir::scale_chiplet {
#define GEN_PASS_DEF_INSERTCHIPLETSENDRECVPASS
#include "scale-chiplet/Transforms/Passes.h.inc"
} // namespace mlir::scale_chiplet

namespace {
struct InsertChipletSendRecvPass
    : public mlir::scale_chiplet::impl::InsertChipletSendRecvPassBase<
          InsertChipletSendRecvPass> {
  static TaskOp rebuildTaskDroppingOperands(TaskOp task,
                                            ArrayRef<unsigned> dropArgIdx) {
    if (dropArgIdx.empty())
      return task;

    llvm::SmallDenseSet<unsigned, 8> dropSet(dropArgIdx.begin(),
                                             dropArgIdx.end());
    SmallVector<Value> newOperands;
    SmallVector<unsigned> eraseArgIdx;
    newOperands.reserve(task.getNumOperands() - dropArgIdx.size());
    eraseArgIdx.reserve(dropArgIdx.size());

    for (unsigned i = 0, e = task.getNumOperands(); i < e; ++i) {
      if (dropSet.count(i)) {
        eraseArgIdx.push_back(i);
        continue;
      }
      newOperands.push_back(task.getOperand(i));
    }

    SmallVector<Type> resultTypes;
    resultTypes.reserve(task.getNumResults());
    for (Value r : task.getResults())
      resultTypes.push_back(r.getType());

    OpBuilder b(task);
    auto newTask = b.create<TaskOp>(task.getLoc(), resultTypes, newOperands);
    newTask->setAttrs(task->getAttrs());
    newTask.getBody().takeBody(task.getBody());

    Block &newBody = newTask.getBody().front();
    llvm::sort(eraseArgIdx, std::greater<unsigned>());
    for (unsigned idx : eraseArgIdx)
      newBody.eraseArgument(idx);

    for (auto it : llvm::enumerate(task.getResults()))
      it.value().replaceAllUsesWith(newTask.getResult(it.index()));
    task.erase();
    return newTask;
  }

  static int64_t getTensorBytes(Type type) {
    auto ranked = dyn_cast<RankedTensorType>(type);
    if (!ranked || !ranked.hasStaticShape())
      return -1;
    Type elemTy = ranked.getElementType();
    if (!elemTy.isIntOrFloat())
      return -1;
    int64_t elemBits = elemTy.getIntOrFloatBitWidth();
    if (elemBits <= 0)
      return -1;
    int64_t numElems = ranked.getNumElements();
    return numElems * ((elemBits + 7) / 8);
  }

  static StringAttr getCommKindAttr(MLIRContext *ctx, Value producerRes) {
    int64_t crossChipUsers = 0;
    auto producerTask = producerRes.getDefiningOp<TaskOp>();
    if (!producerTask)
      return StringAttr::get(ctx, "axi_mm");
    auto producerCidAttr = producerTask->getAttrOfType<IntegerAttr>("chiplet_id");
    int64_t producerCid = producerCidAttr ? producerCidAttr.getInt() : -1;
    for (OpOperand &use : producerRes.getUses()) {
      auto userTask = use.getOwner()->getParentOfType<TaskOp>();
      if (!userTask)
        continue;
      auto userCidAttr = userTask->getAttrOfType<IntegerAttr>("chiplet_id");
      int64_t userCid = userCidAttr ? userCidAttr.getInt() : -1;
      if (userCid != producerCid)
        ++crossChipUsers;
    }
    return StringAttr::get(ctx, crossChipUsers <= 1 ? "axis" : "axi_mm");
  }

  void runOnOperation() override {
    func::FuncOp func = getOperation();
    MLIRContext *ctx = func.getContext();
    func.walk([&](GraphOp graph) {
      SmallVector<TaskOp> tasks;
      for (Operation &op : graph.getBody().front())
        if (auto t = dyn_cast<TaskOp>(&op))
          tasks.push_back(t);

      for (TaskOp consumerTask : tasks) {

        auto consumerCidAttr =
            consumerTask->getAttrOfType<IntegerAttr>("chiplet_id");
        int64_t consumerCid = consumerCidAttr ? consumerCidAttr.getInt() : -1;
        Block &body = consumerTask.getBody().front();
        SmallVector<unsigned> dropArgIdx;

        for (auto it : llvm::enumerate(consumerTask.getOperands())) {
          Value incoming = it.value();
          auto producerTask = incoming.getDefiningOp<TaskOp>();
          if (!producerTask)
            continue;
          auto producerCidAttr =
              producerTask->getAttrOfType<IntegerAttr>("chiplet_id");
          int64_t producerCid = producerCidAttr ? producerCidAttr.getInt() : -1;
          if (producerCid == consumerCid)
            continue;

          int64_t bytes = getTensorBytes(incoming.getType());
          StringAttr commKind = getCommKindAttr(ctx, incoming);

          // Producer side: send the exact yielded value for this result.
          unsigned resIdx = incoming.cast<OpResult>().getResultNumber();
          auto producerYield =
              cast<YieldOp>(producerTask.getBody().front().getTerminator());
          Value producerValue = producerYield.getOperand(resIdx);
          OpBuilder prodBuilder(producerYield);
          prodBuilder.create<SendOp>(
              producerTask.getLoc(), producerValue,
              IntegerAttr::get(IntegerType::get(ctx, 64), consumerCid),
              IntegerAttr::get(IntegerType::get(ctx, 64), bytes), commKind);

          // Consumer side: receive and replace this boundary arg's uses.
          BlockArgument boundaryArg = body.getArgument(it.index());
          OpBuilder consBuilder(&body, body.begin());
          auto recv = consBuilder.create<RecvOp>(
              consumerTask.getLoc(), boundaryArg.getType(),
              IntegerAttr::get(IntegerType::get(ctx, 64), producerCid),
              IntegerAttr::get(IntegerType::get(ctx, 64), bytes), commKind);
          boundaryArg.replaceUsesWithIf(recv.getResult(), [&](OpOperand &use) {
            return use.getOwner() != recv.getOperation();
          });
          if (boundaryArg.use_empty())
            dropArgIdx.push_back(it.index());
        }

        if (!dropArgIdx.empty())
          consumerTask = rebuildTaskDroppingOperands(consumerTask, dropArgIdx);
      }
    });
  }
};
} // namespace

std::unique_ptr<mlir::Pass> mlir::scale_chiplet::createInsertChipletSendRecvPass() {
  return std::make_unique<InsertChipletSendRecvPass>();
}
