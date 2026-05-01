#!/usr/bin/env bash
set -euo pipefail

if ! command -v vitis-run >/dev/null 2>&1; then
  echo "vitis-run is not on PATH. Source your Vitis settings script first."
  exit 1
fi

here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

for k in router0 chip0 chip1 chip2 router2; do
  echo "=== C-synthesizing $k ==="
  (
    cd "$here/$k"
    vitis-run --mode hls --tcl run_csynth.tcl
  )
done

echo "C synthesis complete."
