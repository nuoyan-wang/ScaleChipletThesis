#!/usr/bin/env bash
set -euo pipefail

if ! command -v vitis-run >/dev/null 2>&1; then
  echo "vitis-run is not on PATH. Source your Vitis settings script first."
  exit 1
fi

here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

for k in router0 chip0 chip1 chip2 router2; do
  echo "=== Exporting $k XO ==="
  (
    cd "$here/$k"
    vitis-run --mode hls --tcl run_export_xo.tcl
  )
done

echo "XO export complete:"
echo "  $here/router0/c0_router.xo"
echo "  $here/chip0/c0.xo"
echo "  $here/chip1/c1.xo"
echo "  $here/chip2/c2.xo"
echo "  $here/router2/c2_router.xo"
