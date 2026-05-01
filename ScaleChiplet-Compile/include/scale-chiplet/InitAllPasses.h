//===----------------------------------------------------------------------===//
//
// ScaleChiplet InitAllPasses
//
//===----------------------------------------------------------------------===//

#ifndef SCALECHIPLET_INITALLPASSES_H
#define SCALECHIPLET_INITALLPASSES_H

#include "mlir/InitAllPasses.h"
#include "scale-chiplet/Transforms/Passes.h"

namespace mlir {
namespace scale_chiplet {


inline void registerAllPasses() {
  mlir::scale_chiplet::registerScaleChipletPasses();
  mlir::registerAllPasses();
}

} // namespace scale_chiplet
} // namespace mlir

#endif // SCALECHIPLET_INITALLPASSES_H
