add_mlir_dialect(Chiplet chiplet)

set(LLVM_TARGET_DEFINITIONS Ops.td)
mlir_tablegen(ChipletOps.h.inc -gen-op-decls)
mlir_tablegen(ChipletOps.cpp.inc -gen-op-defs)
add_public_tablegen_target(ScaleChipletChipletOpsIncGen)

set(LLVM_TARGET_DEFINITIONS Chiplet.td)
mlir_tablegen(ChipletDialect.h.inc -gen-dialect-decls -dialect=chiplet)
mlir_tablegen(ChipletDialect.cpp.inc -gen-dialect-defs -dialect=chiplet)
add_public_tablegen_target(ScaleChipletChipletDialectIncGen)
