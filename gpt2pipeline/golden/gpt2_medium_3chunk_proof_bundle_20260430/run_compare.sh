#!/usr/bin/env bash
set -euo pipefail

here="$(cd "$(dirname "$0")" && pwd)"

if [[ "${REBUILD_BINARIES:-0}" == "1" || ! -x "$here/bin/chunk0_harness" || ! -x "$here/bin/tb_newest_layer_boundary" ]]; then
  "$here/build_binaries.sh"
fi

if [[ -z "${PYTHON:-}" ]]; then
  if [[ -x /home/nu/ChenLab/TosaFlow/mlir_venv/bin/python3 ]]; then
    PYTHON=/home/nu/ChenLab/TosaFlow/mlir_venv/bin/python3
  else
    PYTHON=python3
  fi
fi
"$PYTHON" "$here/compare_routered_vs_newest.py" "$@"
