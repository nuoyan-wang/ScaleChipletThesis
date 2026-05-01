add_mlir_library(ScaleChipletChipletDialect
  
Chiplet.cpp
Utils.cpp
  
DEPENDS
    ScaleChipletChipletOpsIncGen

  LINK_LIBS PUBLIC
    MLIRIR
)
