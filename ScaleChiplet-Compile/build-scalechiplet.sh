#!/usr/bin/env bash

set -o errexit
set -o pipefail
set -o nounset

JOBS=""

while getopts 'j:' opt; do
  case $opt in
    j) JOBS="${OPTARG}";;
  esac
done

CMAKE_GENERATOR="Unix Makefiles"

PROJECT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
LOCAL_LLVM_PROJECT_DIR="${PROJECT_DIR}/polygeist/llvm-project/llvm"
BUILD_DIR="${PROJECT_DIR}/build"

if [[ -n "${VIRTUAL_ENV:-}" && -x "${VIRTUAL_ENV}/bin/python3" ]]; then
  PYTHON_EXE="${VIRTUAL_ENV}/bin/python3"
elif [[ -x "${PROJECT_DIR}/mlir_venv/bin/python3" ]]; then
  PYTHON_EXE="${PROJECT_DIR}/mlir_venv/bin/python3"
else
  PYTHON_EXE="$(command -v python3)"
fi

if [[ -n "${LLVM_PROJECT_DIR:-}" ]]; then
  LLVM_SRC_DIR="${LLVM_PROJECT_DIR}"
elif [[ -d "${LOCAL_LLVM_PROJECT_DIR}" ]]; then
  LLVM_SRC_DIR="${LOCAL_LLVM_PROJECT_DIR}"
else
  echo "ERROR: could not find the required LLVM source tree." >&2
  echo "Set LLVM_PROJECT_DIR to an llvm-project/llvm checkout, or place one at:" >&2
  echo "  ${LOCAL_LLVM_PROJECT_DIR}" >&2
  exit 1
fi

configure() {
  cmake --fresh -G "${CMAKE_GENERATOR}" \
    -S "${LLVM_SRC_DIR}" \
    -B "${BUILD_DIR}" \
    -DLLVM_ENABLE_PROJECTS="mlir;clang" \
    -DLLVM_EXTERNAL_PROJECTS="scalechiplet" \
    -DLLVM_EXTERNAL_SCALECHIPLET_SOURCE_DIR="${PROJECT_DIR}" \
    -DLLVM_TARGETS_TO_BUILD="host" \
    -DLLVM_ENABLE_ASSERTIONS=ON \
    -DCMAKE_BUILD_TYPE=DEBUG \
    -DLLVM_PARALLEL_LINK_JOBS="${JOBS:=}" \
    -DLLVM_USE_LINKER=lld \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_CXX_COMPILER=clang++ \
    -DPython3_EXECUTABLE="${PYTHON_EXE}"
}

echo ""
echo ">>> Unified LLVM + MLIR + Clang + ScaleChiplet-compatible build..."
echo ">>> LLVM source dir: ${LLVM_SRC_DIR}"
echo ">>> CMake generator: ${CMAKE_GENERATOR}"
echo ""

mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

if [[ ! -f "CMakeCache.txt" ]]; then
  configure
else
  CACHED_PYTHON="$(sed -n 's/^_Python3_EXECUTABLE:INTERNAL=//p' CMakeCache.txt)"
  CACHED_HOME_DIR="$(sed -n 's/^CMAKE_HOME_DIRECTORY:INTERNAL=//p' CMakeCache.txt)"
  CACHED_GENERATOR="$(sed -n 's/^CMAKE_GENERATOR:INTERNAL=//p' CMakeCache.txt)"
  PROJECT_NAME="$(sed -n 's/^CMAKE_PROJECT_NAME:STATIC=//p' CMakeCache.txt)"
  DUP_LINK_POOL_COUNT="$(
    if [[ -f "CMakeFiles/rules.ninja" ]]; then
      grep -c '^pool link_job_pool$' CMakeFiles/rules.ninja || true
    else
      echo 0
    fi
  )"

  if [[ "${CACHED_HOME_DIR}" != "${LLVM_SRC_DIR}" ]]; then
    echo ">>> Reconfiguring CMake because the LLVM source directory changed."
    configure
  elif [[ "${CACHED_GENERATOR}" != "${CMAKE_GENERATOR}" ]]; then
    echo ">>> Reconfiguring CMake because the generator changed to: ${CMAKE_GENERATOR}"
    configure
  elif [[ "${PROJECT_NAME}" != "LLVM" ]]; then
    echo ">>> Reconfiguring CMake because the cached project is invalid: ${PROJECT_NAME}"
    configure
  elif [[ "${DUP_LINK_POOL_COUNT}" -gt 1 ]]; then
    echo ">>> Reconfiguring CMake because the Ninja build graph is stale."
    configure
  elif [[ -z "${CACHED_PYTHON}" || ! -x "${CACHED_PYTHON}" || "${CACHED_PYTHON}" != "${PYTHON_EXE}" ]]; then
    echo ">>> Reconfiguring CMake to use Python: ${PYTHON_EXE}"
    configure
  fi
fi

if [[ "${CMAKE_GENERATOR}" == "Ninja" ]]; then
  if [[ -n "${JOBS}" ]]; then
    ninja -j "${JOBS}"
  else
    ninja
  fi
else
  make -j "${JOBS:-$(nproc)}"
fi

echo ""
echo ">>> Build complete."
echo "ScaleChiplet tools should be in: ${PROJECT_DIR}/build/bin"
echo "For example:"
echo "  ${PROJECT_DIR}/build/bin/scale-chiplet-opt --help"
echo "  LLVM_PROJECT_DIR=${LLVM_SRC_DIR} ${PROJECT_DIR}/build-scalechiplet.sh"
echo ""
