#!/usr/bin/env bash
set -euo pipefail

here="$(cd "$(dirname "$0")" && pwd)"
detect_xilinx_hls() {
  local candidates=()
  if [[ -n "${XILINX_HLS:-}" ]]; then
    candidates+=("$XILINX_HLS")
  fi
  if [[ -n "${XILINX_VITIS:-}" ]]; then
    candidates+=("$XILINX_VITIS")
  fi
  candidates+=(
    "/tools/Xilinx/2025.2/Vitis"
    "/tools/Xilinx/Vitis/2025.2"
    "/opt/Xilinx/Vitis/2025.2"
    "/opt/xilinx/Vitis/2025.2"
  )
  local root
  for root in "${candidates[@]}"; do
    if [[ -f "$root/include/ap_int.h" ]]; then
      printf '%s\n' "$root"
      return 0
    fi
  done
  return 1
}

if ! XILINX_HLS_ROOT="$(detect_xilinx_hls)"; then
  echo "Could not find Vitis/HLS headers containing ap_int.h."
  echo "Set XILINX_HLS to your Vitis root, for example:"
  echo "  export XILINX_HLS=/tools/Xilinx/2025.2/Vitis"
  echo "Then rerun ./build_binaries.sh"
  exit 1
fi

inc="$XILINX_HLS_ROOT/include"
bin="$here/bin"
design="$here/design"
ref="$here/reference"

mkdir -p "$bin"

compile() {
  local out="$1"
  shift
  g++ -std=c++17 -O2 -fno-stack-protector -Wno-unknown-pragmas -DAP_INT_MAX_W=4096 -I"$inc" "$@" -o "$bin/$out"
}

compile router0_harness \
  "$design/stage_harnesses/router0_harness.cpp" \
  "$design/routers/router_c0_kernel/kernel.cpp"

compile router2_harness \
  "$design/stage_harnesses/router2_harness.cpp" \
  "$design/routers/router_c2_kernel/kernel.cpp"

compile chunk0_harness \
  "$design/stage_harnesses/chunk0_harness.cpp" \
  "$design/chunk0_attn/stream_ip.cpp" \
  "$design/stage_harnesses/hls_math_shim.cpp"

compile chunk1_harness \
  "$design/stage_harnesses/chunk1_harness.cpp" \
  "$design/chunk1_ffn_up/stream_ip.cpp" \
  "$design/stage_harnesses/hls_math_shim.cpp"

compile chunk2_harness \
  "$design/stage_harnesses/chunk2_harness.cpp" \
  "$design/chunk2_ffn_down/stream_ip.cpp" \
  "$design/stage_harnesses/hls_math_shim.cpp"

g++ -std=c++17 -O1 -DNDEBUG -I"$inc" -DHLS_NO_XIL_FPO_LIB -fno-stack-protector -c "$ref/newest_gpt2_medium.cpp" -o "$ref/newest_gpt2_boundary.o"
g++ -std=c++17 -O1 -DNDEBUG -I"$inc" -DHLS_NO_XIL_FPO_LIB -fno-stack-protector -c "$ref/tb_newest_layer_boundary.cpp" -o "$ref/tb_newest_layer_boundary.o"
g++ -std=c++17 -O1 -DNDEBUG -I"$inc" -DHLS_NO_XIL_FPO_LIB -c "$ref/hls_math_stub.cpp" -o "$ref/stub_newest_boundary.o"
g++ -std=c++17 -O1 -DNDEBUG -I"$inc" -DHLS_NO_XIL_FPO_LIB \
  -o "$bin/tb_newest_layer_boundary" \
  "$ref/tb_newest_layer_boundary.o" "$ref/newest_gpt2_boundary.o" "$ref/stub_newest_boundary.o" \
  -pthread

echo "Built bundle binaries in $bin"
