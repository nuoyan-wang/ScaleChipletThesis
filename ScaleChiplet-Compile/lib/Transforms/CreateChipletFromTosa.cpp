//===----------------------------------------------------------------------===//
// Outline TOSA ops into Chiplet tasks (formation) and assign to chiplets
// (placement).
//===----------------------------------------------------------------------===//

#include "scale-chiplet/Transforms/Passes.h"
#include "scale-chiplet/Dialect/Chiplet/Chiplet.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/SetVector.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "mlir/Dialect/Tosa/IR/TosaOps.h"
#include "mlir/IR/Dominance.h"
#include "mlir/IR/PatternMatch.h"

using namespace mlir;
using namespace mlir::scale_chiplet;
using namespace mlir::scale_chiplet::chiplet;

namespace {
/// Lightweight rewriter to access the protected PatternRewriter constructor.
struct ChipletRewriter : public PatternRewriter {
  explicit ChipletRewriter(MLIRContext *ctx) : PatternRewriter(ctx) {}
};

static bool isTargetOp(Operation *op) {
  return isa<tosa::Conv2DOp, tosa::DepthwiseConv2DOp, tosa::AvgPool2dOp,
             tosa::MaxPool2dOp, tosa::MatMulOp, tosa::MulOp, tosa::SubOp,
             tosa::AddOp, tosa::DivOp, tosa::TransposeOp, tosa::ClampOp,
             tosa::ReshapeOp, tosa::ReduceMaxOp, tosa::ReduceSumOp,
             tosa::ExpOp, tosa::ReciprocalOp, tosa::SigmoidOp,
             tosa::AbsOp, tosa::GreaterEqualOp, tosa::NegateOp,
             tosa::SelectOp, tosa::CastOp, tosa::SliceOp, tosa::TileOp,
             tosa::RsqrtOp, tosa::PowOp, tosa::TanhOp, tosa::EqualOp,
             tosa::ConcatOp>(
      op);
}

static bool isConstLikeOp(Operation *op) {
  if (!op)
    return false;
  return isa<tosa::ConstOp>(op);
}

/// Heuristic: task that materializes only constants (e.g., weight transpose).
static bool isConstTask(TaskOp task) {
  // Expect single non-terminator op.
  auto &ops = task.getBody().front().getOperations();
  if (ops.size() != 2) // one op + yield
    return false;
  Operation &inner = ops.front();
  return llvm::all_of(inner.getOperands(), [](Value v) {
    return isConstLikeOp(v.getDefiningOp());
  });
}

/// Heuristic: layout-only task (transpose/reshape/cast), single op.
static bool isLayoutTask(TaskOp task) {
  auto &ops = task.getBody().front().getOperations();
  if (ops.size() != 2) // one op + yield
    return false;
  Operation &inner = ops.front();
  return isa<tosa::TransposeOp, tosa::ReshapeOp, tosa::CastOp>(&inner);
}

/// Create a chiplet.task for a single op (no placement).
static TaskOp createTaskForOp(Operation *op, ChipletRewriter &rewriter) {
  SmallVector<Value> results(op->result_begin(), op->result_end());
  rewriter.setInsertionPoint(op);
  auto task =
      rewriter.create<TaskOp>(op->getLoc(), TypeRange(results), ValueRange());
  Block *body = rewriter.createBlock(&task.getBody());

  rewriter.setInsertionPointToEnd(body);
  auto yield = rewriter.create<YieldOp>(op->getLoc(), results);
  op->moveBefore(yield);

  for (auto [idx, value] : llvm::enumerate(results)) {
    value.replaceUsesWithIf(task.getResult(idx), [&](OpOperand &use) {
      return !task->isProperAncestor(use.getOwner());
    });
  }

  return task;
}

/// Collect tasks that are still unplaced (direct children of the func).
static SmallVector<TaskOp> collectUnplacedTasks(func::FuncOp func) {
  SmallVector<TaskOp> tasks;
  for (Operation &op : func.getBody().front()) {
    if (auto task = dyn_cast<TaskOp>(&op)) {
      if (!task->getParentOfType<ChipOp>())
        tasks.push_back(task);
    }
  }
  return tasks;
}

/// Group tasks into a single graph that only returns function outputs.
static GraphOp wrapTasksIntoGraph(ArrayRef<TaskOp> tasks,
                                  func::ReturnOp funcReturn,
                                  ChipletRewriter &rewriter) {
  assert(!tasks.empty() && "must wrap at least one task");

  llvm::SmallPtrSet<Operation *, 8> taskSet;
  for (TaskOp t : tasks)
    taskSet.insert(t.getOperation());

  // Collect the function return operands that come from the task set, in order
  // and uniqued.
  llvm::SetVector<Value> exportValues;
  for (Value v : funcReturn.getOperands())
    if (taskSet.count(v.getDefiningOp()))
      exportValues.insert(v);

  rewriter.setInsertionPoint(tasks.front());
  auto graph = rewriter.create<GraphOp>(tasks.front()->getLoc(),
                                        TypeRange(exportValues.getArrayRef()));
  Block *graphBlock = rewriter.createBlock(&graph.getBody());

  for (TaskOp t : tasks)
    t->moveBefore(graphBlock, graphBlock->end());

  rewriter.setInsertionPointToEnd(graphBlock);
  rewriter.create<YieldOp>(tasks.back()->getLoc(),
                           exportValues.getArrayRef());

  // Replace external uses of exported values with the graph results.
  unsigned idx = 0;
  for (Value exportVal : exportValues) {
    Value replacement = graph.getResult(idx++);
    exportVal.replaceUsesWithIf(replacement, [&](OpOperand &use) {
      return !graph->isProperAncestor(use.getOwner());
    });
  }

  return graph;
}
} // namespace

namespace mlir::scale_chiplet {
#define GEN_PASS_DEF_CREATECHIPLETFROMTOSA
#include "scale-chiplet/Transforms/Passes.h.inc"
} // namespace mlir::scale_chiplet

namespace {
struct CreateChipletFromTosaPass
    : public mlir::scale_chiplet::impl::CreateChipletFromTosaBase<
          CreateChipletFromTosaPass> {
  CreateChipletFromTosaPass() = default;

  void runOnOperation() override {
    auto func = getOperation();
    ChipletRewriter rewriter(func.getContext());

    auto ret = dyn_cast<func::ReturnOp>(func.getBody().back().getTerminator());
    if (!ret) {
      func.emitError("expected func.return terminator");
      signalPassFailure();
      return;
    }

    // If tasks are not already formed, create them now.
    auto tasks = collectUnplacedTasks(func);
    if (tasks.empty()) {
      SmallVector<Operation *> targets;
      func.walk([&](Operation *op) {
        if (op->getParentOfType<TaskOp>() || op->getParentOfType<ChipOp>())
          return;
        if (isTargetOp(op))
          targets.push_back(op);
      });
      for (Operation *op : targets)
        (void)createTaskForOp(op, rewriter);
      tasks = collectUnplacedTasks(func);
    }

    if (tasks.empty())
      return;

    // Annotate tasks with optional kind.
    for (TaskOp t : tasks) {
      if (isConstTask(t))
        t->setAttr("kind", rewriter.getStringAttr("const"));
      else if (isLayoutTask(t))
        t->setAttr("kind", rewriter.getStringAttr("layout"));
    }

    // Wrap all tasks into a single graph, preserving program order.
    auto graph = wrapTasksIntoGraph(tasks, ret, rewriter);

    // Repoint the function return to the graph results, preserving order.
    SmallVector<Value> newReturns;
    for (Value v : ret.getOperands()) {
      if (v.getDefiningOp() && graph->isProperAncestor(v.getDefiningOp())) {
        // v was inside the graph; find its corresponding result.
        // We aligned graph results in the order of unique return operands,
        // so look it up by matching.
        for (auto it : llvm::enumerate(graph.getResults())) {
          if (v == graph.getBody().front().getTerminator()->getOperand(it.index())) {
            newReturns.push_back(it.value());
            goto next;
          }
        }
      }
      newReturns.push_back(v);
    next:;
    }
    rewriter.setInsertionPoint(ret);
    rewriter.replaceOpWithNewOp<func::ReturnOp>(ret, newReturns);
  }
};
} // namespace

std::unique_ptr<mlir::Pass> mlir::scale_chiplet::createCreateChipletFromTosaPass(
    ) {
  return std::make_unique<CreateChipletFromTosaPass>();
}
