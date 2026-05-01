cmake_minimum_required(VERSION 3.13.4)

# ---------- CMake policies ----------
if(POLICY CMP0068)
  cmake_policy(SET CMP0068 NEW)
  set(CMAKE_BUILD_WITH_INSTALL_NAME_DIR ON)
endif()

if(POLICY CMP0075)
  cmake_policy(SET CMP0075 NEW)
endif()

if(POLICY CMP0077)
  cmake_policy(SET CMP0077 NEW)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

# ===================================================================
# Out-of-tree vs in-tree (LLVM external project) build
# ===================================================================
if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  # -----------------------------------------------------------------
  # Standalone / out-of-tree build: use an installed LLVM+MLIR
  # -----------------------------------------------------------------
  project(ScaleChiplet LANGUAGES CXX C)

  find_package(MLIR REQUIRED CONFIG)
  find_package(LLVM REQUIRED CONFIG)

  message(STATUS "Using MLIRConfig.cmake in: ${MLIR_DIR}")
  message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

  list(APPEND CMAKE_MODULE_PATH "${MLIR_CMAKE_DIR}")
  list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")

  include(TableGen)
  include(AddLLVM)
  include(AddMLIR)
  include(HandleLLVMOptions)

else()
  # -----------------------------------------------------------------
  # In-tree / external-project build (e.g. via llvm -DLLVM_EXTERNAL_PROJECTS)
  # -----------------------------------------------------------------
  set(LLVM_SOURCE_DIR ${LLVM_MAIN_SRC_DIR})
  set(MLIR_MAIN_SRC_DIR ${LLVM_MAIN_SRC_DIR}/../mlir)
  set(MLIR_INCLUDE_DIRS ${MLIR_MAIN_SRC_DIR}/include)
  set(MLIR_CMAKE_DIR ${MLIR_MAIN_SRC_DIR}/cmake/modules)
  set(MLIR_TABLEGEN_EXE $<TARGET_FILE:mlir-tblgen>)
  set(MLIR_TABLEGEN_OUTPUT_DIR ${LLVM_BINARY_DIR}/tools/mlir/include)

  include_directories(SYSTEM ${MLIR_INCLUDE_DIRS})
  include_directories(SYSTEM ${MLIR_TABLEGEN_OUTPUT_DIR})

  list(APPEND CMAKE_MODULE_PATH "${MLIR_MAIN_SRC_DIR}/cmake/modules")

  include(TableGen)
  include(AddLLVM)
  include(AddMLIR)
  include(HandleLLVMOptions)
endif()

# ===================================================================
# Output directories
# ===================================================================
set(LLVM_RUNTIME_OUTPUT_INTDIR ${CMAKE_BINARY_DIR}/bin)
set(LLVM_LIBRARY_OUTPUT_INTDIR ${CMAKE_BINARY_DIR}/lib)
set(MLIR_BINARY_DIR ${CMAKE_BINARY_DIR})

# ===================================================================
# Project-specific paths
# ===================================================================
set(SCALECHIPLET_MAIN_SRC_DIR      ${CMAKE_CURRENT_SOURCE_DIR})
set(SCALECHIPLET_MAIN_INCLUDE_DIR  ${SCALECHIPLET_MAIN_SRC_DIR}/include)

set(SCALECHIPLET_SOURCE_DIR        ${CMAKE_CURRENT_SOURCE_DIR})
set(SCALECHIPLET_BINARY_DIR        ${CMAKE_CURRENT_BINARY_DIR})
set(SCALECHIPLET_INCLUDE_DIR       ${SCALECHIPLET_BINARY_DIR}/include)
set(SCALECHIPLET_TOOLS_DIR         ${CMAKE_BINARY_DIR}/bin)

# ===================================================================
# Include / link config
# ===================================================================
include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${MLIR_INCLUDE_DIRS})
include_directories(${SCALECHIPLET_MAIN_INCLUDE_DIR})
include_directories(${SCALECHIPLET_INCLUDE_DIR})

if(DEFINED LLVM_BUILD_LIBRARY_DIR)
  link_directories(${LLVM_BUILD_LIBRARY_DIR})
elseif(DEFINED LLVM_LIBRARY_DIR)
  link_directories(${LLVM_LIBRARY_DIR})
endif()

add_definitions(${LLVM_DEFINITIONS})

# ===================================================================
# Subdirectories
# ===================================================================
add_subdirectory(include/scale-chiplet)
add_subdirectory(lib)
add_subdirectory(tools)

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/test/CMakeLists.txt")
  add_subdirectory(test)
endif()
