#!/usr/bin/env bash
set -euo pipefail

if [[ $# -lt 2 || $# -gt 4 ]]; then
  echo "usage: run_scalechiplet_flow.sh INPUT_MLIR OUT_DIR [NUM_CHIPLETS=8] [--input-is-ready]" >&2
  exit 1
fi

INPUT_MLIR="$(realpath "$1")"
OUT_DIR="$(realpath -m "$2")"
NUM_CHIPLETS="${3:-8}"
READY_FLAG="${4:-}"

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
ROOT_DIR="$( cd "${SCRIPT_DIR}/.." &> /dev/null && pwd )"

LOCAL_SCALECHIPLET_OPT="${ROOT_DIR}/build/bin/scale-chiplet-opt"
LOCAL_TOSAFLOW_OPT="$(realpath -m "${ROOT_DIR}/../../TosaFlow/build/bin/tosa-flow-opt")"
OPT_BIN="${SCALECHIPLET_OPT_BIN:-}"
NORMALIZER="${ROOT_DIR}/scripts/torchmlir_to_scalechiplet_ready.py"
SPLITTER="${ROOT_DIR}/scripts/split_chiplets_mlir.sh"
VERIFY="${ROOT_DIR}/scripts/verify_chiplets_split.py"

if [[ -z "${OPT_BIN}" ]]; then
  if [[ -x "${LOCAL_SCALECHIPLET_OPT}" ]]; then
    OPT_BIN="${LOCAL_SCALECHIPLET_OPT}"
  elif [[ -x "${LOCAL_TOSAFLOW_OPT}" ]]; then
    OPT_BIN="${LOCAL_TOSAFLOW_OPT}"
  else
    OPT_BIN="${LOCAL_SCALECHIPLET_OPT}"
  fi
fi

if [[ ! -x "${OPT_BIN}" ]]; then
  echo "ERROR: compiler binary not found at ${OPT_BIN}." >&2
  echo "Build ScaleChiplet locally with ./build-scalechiplet.sh, or point SCALECHIPLET_OPT_BIN at an existing binary." >&2
  exit 1
fi

PIPELINE_FLAG="--scale-chiplet-pipeline=num-chiplets=${NUM_CHIPLETS}"
if [[ "$(basename "${OPT_BIN}")" == "tosa-flow-opt" ]]; then
  PIPELINE_FLAG="--tosa-flow-pipeline=num-chiplets=${NUM_CHIPLETS}"
  echo "Using TosaFlow compatibility binary: ${OPT_BIN}"
fi

mkdir -p "${OUT_DIR}"
READY_INPUT="${OUT_DIR}/$(basename "${INPUT_MLIR%.mlir}")_scalechiplet_ready.mlir"
COMBINED_OUTPUT="${OUT_DIR}/chiplets.mlir"
SPLIT_OUTPUT_DIR="${OUT_DIR}/chiplets_split"

if [[ "${READY_FLAG}" == "--input-is-ready" ]]; then
  cp "${INPUT_MLIR}" "${READY_INPUT}"
else
  python3 "${NORMALIZER}" "${INPUT_MLIR}" -o "${READY_INPUT}"
fi

"${OPT_BIN}" "${READY_INPUT}" \
  "${PIPELINE_FLAG}" \
  -o "${COMBINED_OUTPUT}"

"${SPLITTER}" "${COMBINED_OUTPUT}" "${SPLIT_OUTPUT_DIR}"
python3 "${VERIFY}" "${COMBINED_OUTPUT}" "${SPLIT_OUTPUT_DIR}" --expect-count "${NUM_CHIPLETS}"

echo "ready_input=${READY_INPUT}"
echo "combined_output=${COMBINED_OUTPUT}"
echo "split_output_dir=${SPLIT_OUTPUT_DIR}"
