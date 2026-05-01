get_property(dialect_libs GLOBAL PROPERTY MLIR_DIALECT_LIBS)
get_property(conversion_libs GLOBAL PROPERTY MLIR_CONVERSION_LIBS)

add_llvm_tool(scale-chiplet-opt
  scale-chiplet-opt.cpp
)

llvm_update_compile_flags(scale-chiplet-opt)

target_link_libraries(scale-chiplet-opt
  PRIVATE
    ${dialect_libs}
    ${conversion_libs}
    MLIROptLib

    ScaleChipletTransforms

    MLIRFuncDialect
)
