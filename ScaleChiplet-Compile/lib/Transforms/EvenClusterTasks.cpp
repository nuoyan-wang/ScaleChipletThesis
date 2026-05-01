//===----------------------------------------------------------------------===//
// Temporary pass: evenly assign tasks to a fixed number of chiplets.
//===----------------------------------------------------------------------===//

#include "scale-chiplet/Transforms/Passes.h"
#include "scale-chiplet/Dialect/Chiplet/Chiplet.h"
#include "mlir/Dialect/Tosa/IR/TosaOps.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include <cmath>
#include <cstdlib>
#include <limits>
#include <utility>

using namespace mlir;
using namespace mlir::scale_chiplet;
using namespace mlir::scale_chiplet::chiplet;

namespace mlir::scale_chiplet {
#define GEN_PASS_DEF_EVENCLUSTERTASKSPASS
#include "scale-chiplet/Transforms/Passes.h.inc"
} // namespace mlir::scale_chiplet

namespace {
struct EvenClusterTasksPass
    : public mlir::scale_chiplet::impl::EvenClusterTasksPassBase<
          EvenClusterTasksPass> {
  enum class PartitionMode {
    Balance,
    HardwareFirst,
  };

  EvenClusterTasksPass() = default;
  explicit EvenClusterTasksPass(int requestedChiplets) {
    numChiplets = requestedChiplets;
  }

  struct Score {
    int64_t maxBoundaryTensorBytes;
    int64_t maxSegCost;
    int64_t minSegCost;
    int64_t firstSegCost;
    int64_t totalBoundaryBytes;
    unsigned maxSegTasks;
    unsigned minSegTasks;
    long double balancePenalty;
    bool valid;
  };

  static PartitionMode getPartitionModeFromEnv() {
    const char *env = std::getenv("SCALECHIPLET_PARTITION_MODE");
    if (!env || !*env)
      return PartitionMode::Balance;
    return StringRef(env).equals_insensitive("hardware")
               ? PartitionMode::HardwareFirst
               : PartitionMode::Balance;
  }

  static int64_t getTaskCost(TaskOp task) {
    if (auto totalAttr =
            task->getAttrOfType<IntegerAttr>("cost_balance_total")) {
      return std::max<int64_t>(1, totalAttr.getInt());
    }
    if (auto totalAttr =
            task->getAttrOfType<IntegerAttr>("cost_ops_total")) {
      return std::max<int64_t>(1, totalAttr.getInt());
    }

    int64_t fallback = 0;
    if (auto flopsAttr = task->getAttrOfType<IntegerAttr>("cost_flops"))
      fallback += flopsAttr.getInt();
    if (auto miscAttr = task->getAttrOfType<IntegerAttr>("cost_misc"))
      fallback += miscAttr.getInt();
    return std::max<int64_t>(1, fallback);
  }

  static bool isBetterScore(const Score &lhs, const Score &rhs,
                            PartitionMode mode) {
    if (!lhs.valid)
      return false;
    if (!rhs.valid)
      return true;
    if (mode == PartitionMode::HardwareFirst) {
      if (lhs.maxBoundaryTensorBytes != rhs.maxBoundaryTensorBytes)
        return lhs.maxBoundaryTensorBytes < rhs.maxBoundaryTensorBytes;
      if (lhs.totalBoundaryBytes != rhs.totalBoundaryBytes)
        return lhs.totalBoundaryBytes < rhs.totalBoundaryBytes;
    }
    int64_t lhsSpread = lhs.maxSegCost - lhs.minSegCost;
    int64_t rhsSpread = rhs.maxSegCost - rhs.minSegCost;

    // HLS latency is not perfectly predicted by the coarse task cost model.
    // If two partitions are close on cost spread, prefer the one that is less
    // front-loaded and less lopsided in task count. This avoids packing the
    // entire transformer attention front-half into chip0 just to win a tiny
    // synthetic cost-spread improvement.
    int64_t spreadDiff = std::llabs(lhsSpread - rhsSpread);
    constexpr int64_t kNearTieCostSpread = 8000000;
    if (spreadDiff <= kNearTieCostSpread) {
      unsigned lhsTaskSpread = lhs.maxSegTasks - lhs.minSegTasks;
      unsigned rhsTaskSpread = rhs.maxSegTasks - rhs.minSegTasks;
      if (lhsTaskSpread != rhsTaskSpread)
        return lhsTaskSpread < rhsTaskSpread;
      if (lhs.firstSegCost != rhs.firstSegCost)
        return lhs.firstSegCost < rhs.firstSegCost;
    }

    if (lhsSpread != rhsSpread)
      return lhsSpread < rhsSpread;
    unsigned lhsTaskSpread = lhs.maxSegTasks - lhs.minSegTasks;
    unsigned rhsTaskSpread = rhs.maxSegTasks - rhs.minSegTasks;
    if (lhs.maxSegCost != rhs.maxSegCost)
      return lhs.maxSegCost < rhs.maxSegCost;
    if (lhs.totalBoundaryBytes != rhs.totalBoundaryBytes)
      return lhs.totalBoundaryBytes < rhs.totalBoundaryBytes;
    if (lhsTaskSpread != rhsTaskSpread)
      return lhsTaskSpread < rhsTaskSpread;
    if (lhs.minSegCost != rhs.minSegCost)
      return lhs.minSegCost > rhs.minSegCost;
    if (lhs.minSegTasks != rhs.minSegTasks)
      return lhs.minSegTasks > rhs.minSegTasks;
    if (lhs.maxSegTasks != rhs.maxSegTasks)
      return lhs.maxSegTasks < rhs.maxSegTasks;
    if (lhs.balancePenalty != rhs.balancePenalty)
      return lhs.balancePenalty < rhs.balancePenalty;
    return false;
  }

  static bool isI1Tensor(Value value) {
    auto shaped = dyn_cast<ShapedType>(value.getType());
    if (!shaped)
      return false;
    auto intType = dyn_cast<IntegerType>(shaped.getElementType());
    return intType && intType.getWidth() == 1;
  }

  static bool getTrailingDims(Value value, int64_t &dim0, int64_t &dim1) {
    auto shaped = dyn_cast<RankedTensorType>(value.getType());
    if (!shaped || !shaped.hasStaticShape() || shaped.getRank() < 2)
      return false;
    dim0 = shaped.getDimSize(shaped.getRank() - 2);
    dim1 = shaped.getDimSize(shaped.getRank() - 1);
    return dim0 > 0 && dim1 > 0;
  }

  static bool hasRankAndLastDim(Value value, int64_t rank, int64_t lastDim) {
    auto shaped = dyn_cast<RankedTensorType>(value.getType());
    if (!shaped || !shaped.hasStaticShape() || shaped.getRank() != rank)
      return false;
    return shaped.getDimSize(rank - 1) == lastDim;
  }

  static bool taskContainsMatmulRhsShape(TaskOp task, int64_t rows,
                                         int64_t cols) {
    bool found = false;
    task.walk([&](tosa::MatMulOp matmul) {
      int64_t rhsRows = 0;
      int64_t rhsCols = 0;
      if (!found && getTrailingDims(matmul.getB(), rhsRows, rhsCols) &&
          rhsRows == rows && rhsCols == cols)
        found = true;
    });
    return found;
  }

  static bool taskContainsReshapeToShape(TaskOp task, int64_t rows,
                                         int64_t cols) {
    bool found = false;
    task.walk([&](tosa::ReshapeOp reshape) {
      int64_t resRows = 0;
      int64_t resCols = 0;
      if (!found && getTrailingDims(reshape.getResult(), resRows, resCols) &&
          resRows == rows && resCols == cols)
        found = true;
    });
    return found;
  }

  static bool taskContainsRank3AddWithLastDim(TaskOp task, int64_t hiddenSize) {
    bool found = false;
    task.walk([&](tosa::AddOp add) {
      if (!found && hasRankAndLastDim(add.getResult(), /*rank=*/3, hiddenSize))
        found = true;
    });
    return found;
  }

  static bool inferTransformerBestOf3Cuts(MutableArrayRef<TaskOp> tasks,
                                          SmallVectorImpl<unsigned> &cuts) {
    struct ReciprocalMatMulPair {
      int64_t hiddenSize = 0;
      int64_t expandedSize = 0;
      int64_t area = 0;
      bool valid = false;
    };

    ReciprocalMatMulPair bestPair;
    const unsigned n = tasks.size();

    for (unsigned i = 0; i < n; ++i) {
      SmallVector<std::pair<int64_t, int64_t>> lhsPairs;
      tasks[i].walk([&](tosa::MatMulOp matmul) {
        int64_t rows = 0;
        int64_t cols = 0;
        if (getTrailingDims(matmul.getB(), rows, cols))
          lhsPairs.emplace_back(rows, cols);
      });

      for (const auto &lhsPair : lhsPairs) {
        int64_t rows = lhsPair.first;
        int64_t cols = lhsPair.second;
        if (rows <= 0 || cols <= 0 || rows == cols)
          continue;

        int64_t hiddenSize = std::min(rows, cols);
        int64_t expandedSize = std::max(rows, cols);
        int64_t area = hiddenSize * expandedSize;
        if (area <= bestPair.area)
          continue;

        bool foundReciprocal = false;
        for (unsigned j = 0; j < n && !foundReciprocal; ++j) {
          foundReciprocal =
              taskContainsMatmulRhsShape(tasks[j], expandedSize, hiddenSize);
        }
        if (!foundReciprocal)
          continue;

        bestPair.hiddenSize = hiddenSize;
        bestPair.expandedSize = expandedSize;
        bestPair.area = area;
        bestPair.valid = true;
      }
    }

    if (!bestPair.valid)
      return false;

    unsigned ffnUpStart = n;
    unsigned ffnDownStart = n;
    unsigned outputProjTask = n;

    for (unsigned i = 0; i < n; ++i) {
      if (ffnUpStart == n &&
          taskContainsMatmulRhsShape(tasks[i], bestPair.hiddenSize,
                                     bestPair.expandedSize))
        ffnUpStart = i;

      if (ffnDownStart == n &&
          (taskContainsReshapeToShape(tasks[i], bestPair.expandedSize,
                                      bestPair.hiddenSize) ||
           taskContainsMatmulRhsShape(tasks[i], bestPair.expandedSize,
                                      bestPair.hiddenSize)))
        ffnDownStart = i;
    }

    if (ffnUpStart == n || ffnDownStart == n || ffnUpStart >= ffnDownStart)
      return false;

    for (unsigned i = 0; i < ffnUpStart; ++i) {
      if (taskContainsMatmulRhsShape(tasks[i], bestPair.hiddenSize,
                                     bestPair.hiddenSize)) {
        outputProjTask = i;
        break;
      }
    }
    if (outputProjTask == n)
      return false;

    unsigned attnCut = n;
    for (unsigned i = outputProjTask; i < ffnUpStart; ++i) {
      if (taskContainsRank3AddWithLastDim(tasks[i], bestPair.hiddenSize)) {
        attnCut = i + 1;
        break;
      }
    }

    if (attnCut == n || attnCut == 0 || attnCut >= ffnDownStart ||
        ffnDownStart >= n)
      return false;

    cuts.assign({0, attnCut, ffnDownStart, n});
    return true;
  }

  static unsigned
  countExternalLiveOutsInRange(MutableArrayRef<TaskOp> tasks, unsigned begin,
                               unsigned end) {
    DenseSet<Operation *> clusterOps;
    for (unsigned i = begin; i < end; ++i)
      clusterOps.insert(tasks[i].getOperation());

    auto isInCluster = [&](Operation *op) {
      while (op) {
        if (clusterOps.count(op))
          return true;
        op = op->getParentOp();
      }
      return false;
    };

    unsigned liveOutCount = 0;
    for (unsigned i = begin; i < end; ++i) {
      for (Value res : tasks[i].getResults()) {
        bool escapes = llvm::any_of(res.getUses(), [&](OpOperand &use) {
          return !isInCluster(use.getOwner());
        });
        if (escapes)
          ++liveOutCount;
      }
    }
    return liveOutCount;
  }

  static bool hasExternalBoolLiveOutInRange(MutableArrayRef<TaskOp> tasks,
                                            unsigned begin, unsigned end) {
    DenseSet<Operation *> clusterOps;
    for (unsigned i = begin; i < end; ++i)
      clusterOps.insert(tasks[i].getOperation());

    auto isInCluster = [&](Operation *op) {
      while (op) {
        if (clusterOps.count(op))
          return true;
        op = op->getParentOp();
      }
      return false;
    };

    for (unsigned i = begin; i < end; ++i) {
      for (Value res : tasks[i].getResults()) {
        if (!isI1Tensor(res))
          continue;
        bool escapes = llvm::any_of(res.getUses(), [&](OpOperand &use) {
          return !isInCluster(use.getOwner());
        });
        if (escapes)
          return true;
      }
    }
    return false;
  }

  static Optional<int64_t> getTensorBytes(Value value) {
    auto shaped = dyn_cast<ShapedType>(value.getType());
    if (!shaped || !shaped.hasStaticShape())
      return llvm::None;

    Type elemType = shaped.getElementType();
    int64_t elemBits = 0;
    if (auto intType = dyn_cast<IntegerType>(elemType)) {
      elemBits = intType.getWidth();
    } else if (auto floatType = dyn_cast<FloatType>(elemType)) {
      elemBits = floatType.getWidth();
    } else {
      return llvm::None;
    }
    return (shaped.getNumElements() * elemBits + 7) / 8;
  }

  static int64_t getSegmentBoundaryBytes(MutableArrayRef<TaskOp> tasks,
                                         unsigned begin, unsigned end) {
    DenseSet<Operation *> clusterOps;
    for (unsigned i = begin; i < end; ++i)
      clusterOps.insert(tasks[i].getOperation());

    auto isInCluster = [&](Operation *op) {
      while (op) {
        if (clusterOps.count(op))
          return true;
        op = op->getParentOp();
      }
      return false;
    };

    DenseSet<Value> extOperands;
    DenseSet<Value> liveOuts;
    int64_t bytes = 0;

    for (unsigned i = begin; i < end; ++i) {
      for (Value operand : tasks[i].getOperands()) {
        Operation *def = operand.getDefiningOp();
        if (def && isInCluster(def))
          continue;
        if (!extOperands.insert(operand).second)
          continue;
        if (auto tensorBytes = getTensorBytes(operand))
          bytes += *tensorBytes;
      }
      for (Value result : tasks[i].getResults()) {
        bool escapes = llvm::any_of(result.getUses(), [&](OpOperand &use) {
          return !isInCluster(use.getOwner());
        });
        if (!escapes || !liveOuts.insert(result).second)
          continue;
        if (auto tensorBytes = getTensorBytes(result))
          bytes += *tensorBytes;
      }
    }

    return std::max<int64_t>(0, bytes);
  }

  static int64_t getSegmentMaxBoundaryTensorBytes(MutableArrayRef<TaskOp> tasks,
                                                  unsigned begin,
                                                  unsigned end) {
    DenseSet<Operation *> clusterOps;
    for (unsigned i = begin; i < end; ++i)
      clusterOps.insert(tasks[i].getOperation());

    auto isInCluster = [&](Operation *op) {
      while (op) {
        if (clusterOps.count(op))
          return true;
        op = op->getParentOp();
      }
      return false;
    };

    DenseSet<Value> extOperands;
    DenseSet<Value> liveOuts;
    int64_t maxBytes = 0;

    for (unsigned i = begin; i < end; ++i) {
      for (Value operand : tasks[i].getOperands()) {
        Operation *def = operand.getDefiningOp();
        if (def && isInCluster(def))
          continue;
        if (!extOperands.insert(operand).second)
          continue;
        if (auto tensorBytes = getTensorBytes(operand))
          maxBytes = std::max(maxBytes, *tensorBytes);
      }
      for (Value result : tasks[i].getResults()) {
        bool escapes = llvm::any_of(result.getUses(), [&](OpOperand &use) {
          return !isInCluster(use.getOwner());
        });
        if (!escapes || !liveOuts.insert(result).second)
          continue;
        if (auto tensorBytes = getTensorBytes(result))
          maxBytes = std::max(maxBytes, *tensorBytes);
      }
    }

    return std::max<int64_t>(0, maxBytes);
  }

  static int64_t getBoundarySemanticPenalty(MutableArrayRef<TaskOp> tasks,
                                            unsigned begin, unsigned end) {
    DenseSet<Operation *> clusterOps;
    for (unsigned i = begin; i < end; ++i)
      clusterOps.insert(tasks[i].getOperation());

    auto isInCluster = [&](Operation *op) {
      while (op) {
        if (clusterOps.count(op))
          return true;
        op = op->getParentOp();
      }
      return false;
    };

    DenseSet<Value> penalizedValues;
    int64_t penalty = 0;
    for (unsigned i = begin; i < end; ++i) {
      for (Value result : tasks[i].getResults()) {
        bool escapes = llvm::any_of(result.getUses(), [&](OpOperand &use) {
          return !isInCluster(use.getOwner());
        });
        if (!escapes || !penalizedValues.insert(result).second)
          continue;

        auto resultElems = [&]() -> int64_t {
          auto shaped = dyn_cast<ShapedType>(result.getType());
          if (!shaped || !shaped.hasStaticShape())
            return 1;
          return std::max<int64_t>(1, shaped.getNumElements());
        };

        if (isI1Tensor(result))
          penalty += 5000000 + 64 * resultElems();

        for (OpOperand &use : result.getUses()) {
          if (isInCluster(use.getOwner()))
            continue;
          if (isa<tosa::SelectOp>(use.getOwner()) && isI1Tensor(result))
            penalty += 10000000 + 128 * resultElems();
          if (isa<tosa::EqualOp, tosa::GreaterEqualOp>(result.getDefiningOp()) &&
              isa<tosa::SelectOp>(use.getOwner()))
            penalty += 6000000;
        }
      }
    }
    return penalty;
  }

  void runOnOperation() override {
    func::FuncOp func = getOperation();
    PartitionMode partitionMode = getPartitionModeFromEnv();
    if (numChiplets == 0) {
      func.emitError("num-chiplets must be > 0");
      signalPassFailure();
      return;
    }

    func.walk([&](GraphOp graph) {
      SmallVector<TaskOp> tasks;
      for (Operation &op : graph.getBody().front())
        if (auto task = dyn_cast<TaskOp>(&op))
          tasks.push_back(task);

      if (tasks.empty())
        return;

      const unsigned n = tasks.size();
      const unsigned k = numChiplets;
      if (k > n) {
        func.emitError("num-chiplets cannot exceed number of tasks");
        signalPassFailure();
        return;
      }

      SmallVector<int64_t> prefixCost(n + 1, 0);
      for (unsigned i = 0; i < n; ++i)
        prefixCost[i + 1] = prefixCost[i] + getTaskCost(tasks[i]);
      const int64_t totalCost = prefixCost[n];

      // Precompute validity and cross-segment traffic for each contiguous
      // segment. We allow multiple live-outs, but disallow segments that do
      // not contribute any externally visible value.
      SmallVector<SmallVector<char>> valid(n + 1, SmallVector<char>(n + 1, 0));
      SmallVector<SmallVector<int64_t>> boundaryBytes(
          n + 1, SmallVector<int64_t>(n + 1, 0));
      SmallVector<SmallVector<int64_t>> maxBoundaryTensorBytes(
          n + 1, SmallVector<int64_t>(n + 1, 0));
      SmallVector<SmallVector<int64_t>> boundaryPenalty(
          n + 1, SmallVector<int64_t>(n + 1, 0));
      for (unsigned j = 0; j < n; ++j) {
        for (unsigned i = j + 1; i <= n; ++i) {
          unsigned liveOutCount = countExternalLiveOutsInRange(tasks, j, i);
          bool hasBoolBoundary = hasExternalBoolLiveOutInRange(tasks, j, i);
          valid[j][i] = (liveOutCount > 0 && !hasBoolBoundary) ? 1 : 0;
          boundaryBytes[j][i] = getSegmentBoundaryBytes(tasks, j, i);
          maxBoundaryTensorBytes[j][i] =
              getSegmentMaxBoundaryTensorBytes(tasks, j, i);
          boundaryPenalty[j][i] = getBoundarySemanticPenalty(tasks, j, i);
        }
      }

      // DP for exact-k partition that:
      //   1) minimizes the cost spread across chiplets
      //   2) minimizes task-count spread across chiplets
      //   3) minimizes the maximum cluster cost
      //   4) maximizes the minimum cluster cost / task count
      //   5) breaks ties by minimizing squared deviation from the ideal cost
      // This preserves contiguous segments and the one-live-out constraint,
      // while strongly discouraging a single chiplet from absorbing an
      // outlier tail or collapsing a chiplet to near-zero work.
      const int64_t inf = std::numeric_limits<int64_t>::max() / 4;
      const int64_t targetTimesK = totalCost;
      SmallVector<SmallVector<Score>> dp(
          k + 1,
          SmallVector<Score>(n + 1,
                             Score{inf, inf, 0, inf, inf, n, 0, 0.0L, false}));
      SmallVector<SmallVector<int>> prev(k + 1, SmallVector<int>(n + 1, -1));
      dp[0][0] = Score{0, 0, inf, 0, 0, 0, n, 0.0L, true};

      SmallVector<unsigned> semanticCuts;
      if (partitionMode == PartitionMode::Balance && k == 3)
        inferTransformerBestOf3Cuts(tasks, semanticCuts);

      for (unsigned c = 1; c <= k; ++c) {
        for (unsigned i = c; i <= n; ++i) {
          for (unsigned j = c - 1; j < i; ++j) {
            if (!dp[c - 1][j].valid || !valid[j][i])
              continue;
            if (!semanticCuts.empty() && i != semanticCuts[c])
              continue;
            int64_t segTaskCost = prefixCost[i] - prefixCost[j];
            int64_t segBoundaryBytes = boundaryBytes[j][i];
            int64_t segMaxBoundaryTensorBytes = maxBoundaryTensorBytes[j][i];
            int64_t segBoundaryPenalty = boundaryPenalty[j][i];
            int64_t segCost =
                segTaskCost + segBoundaryBytes + segBoundaryPenalty;
            int64_t deltaTimesK =
                segCost * static_cast<int64_t>(k) - targetTimesK;
            long double penalty =
                static_cast<long double>(deltaTimesK) *
                static_cast<long double>(deltaTimesK);
            unsigned segTasks = i - j;
            int64_t nextMaxBoundaryTensorBytes =
                std::max(dp[c - 1][j].maxBoundaryTensorBytes,
                         segMaxBoundaryTensorBytes);
            int64_t nextMax = std::max(dp[c - 1][j].maxSegCost, segCost);
            int64_t nextMin =
                (c == 1) ? segCost : std::min(dp[c - 1][j].minSegCost, segCost);
            unsigned nextMaxTasks =
                (c == 1) ? segTasks
                         : std::max(dp[c - 1][j].maxSegTasks, segTasks);
            unsigned nextMinTasks =
                (c == 1) ? segTasks
                         : std::min(dp[c - 1][j].minSegTasks, segTasks);
            int64_t nextFirstSegCost =
                (c == 1) ? segCost : dp[c - 1][j].firstSegCost;
            Score cand{
                nextMaxBoundaryTensorBytes,
                nextMax,
                nextMin,
                nextFirstSegCost,
                dp[c - 1][j].totalBoundaryBytes + segBoundaryBytes,
                nextMaxTasks,
                nextMinTasks,
                dp[c - 1][j].balancePenalty + penalty,
                true,
            };
            if (isBetterScore(cand, dp[c][i], partitionMode)) {
              dp[c][i] = cand;
              prev[c][i] = static_cast<int>(j);
            }
          }
        }
      }

      if (!dp[k][n].valid) {
        func.emitError("failed to partition tasks into exactly ")
            << k << " contiguous clusters with at least one external live-out "
            << "per cluster";
        signalPassFailure();
        return;
      }

      SmallVector<unsigned> cuts(k + 1, 0);
      cuts[k] = n;
      unsigned curI = n;
      for (unsigned c = k; c > 0; --c) {
        int j = prev[c][curI];
        cuts[c - 1] = static_cast<unsigned>(j);
        curI = static_cast<unsigned>(j);
      }

      for (unsigned cid = 0; cid < k; ++cid) {
        for (unsigned i = cuts[cid]; i < cuts[cid + 1]; ++i) {
          tasks[i]->setAttr("chiplet_id", IntegerAttr::get(
                                              IntegerType::get(func.getContext(), 64),
                                              cid));
        }
      }
    });
  }
};
} // namespace

std::unique_ptr<mlir::Pass> mlir::scale_chiplet::createEvenClusterTasksPass() {
  return std::make_unique<EvenClusterTasksPass>();
}

std::unique_ptr<mlir::Pass>
mlir::scale_chiplet::createEvenClusterTasksPass(int numChiplets) {
  return std::make_unique<EvenClusterTasksPass>(numChiplets);
}
