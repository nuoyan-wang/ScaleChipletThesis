#!/usr/bin/env bash
set -euo pipefail

if [[ -z "${PLATFORM:-}" ]]; then
  echo "Set PLATFORM to your target .xpfm path or platform name."
  exit 1
fi

if ! command -v v++ >/dev/null 2>&1; then
  echo "v++ is not on PATH. Source your Vitis settings script first."
  exit 1
fi

here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
output_dir="${OUTPUT_DIR:-$here/build_link}"
temp_dir="$output_dir/_x"
vivado_jobs="${VIVADO_JOBS:-1}"
out="${XCLBIN_OUT:-$output_dir/gpt2_medium_3prompt_300mhz.xclbin}"

xos=(
  "$here/router0/c0_router.xo"
  "$here/chip0/c0.xo"
  "$here/chip1/c1.xo"
  "$here/chip2/c2.xo"
  "$here/router2/c2_router.xo"
)

for xo in "${xos[@]}"; do
  if [[ ! -f "$xo" ]]; then
    echo "Missing XO: $xo"
    echo "Run ./build_all_xo.sh first."
    exit 1
  fi
done

mkdir -p "$output_dir"

if [[ "${CLEAN_LINK:-0}" == "1" ]]; then
  stamp="$(date +%Y%m%d_%H%M%S)"
  for path in "$temp_dir" "$out" "$out.link_summary"; do
    if [[ -e "$path" ]]; then
      mv "$path" "$path.$stamp"
    fi
  done
fi

common_args=(
  -t hw
  --platform "$PLATFORM"
  --link
  --freqhz=300MHz:c0_router_1.ap_clk
  --freqhz=300MHz:chunk0_stream_ip_1.ap_clk
  --freqhz=300MHz:chunk1_stream_ip_1.ap_clk
  --freqhz=300MHz:chunk2_stream_ip_1.ap_clk
  --freqhz=300MHz:c2_router_1.ap_clk
  --save-temps
  --temp_dir "$temp_dir"
  --config "$here/system.cfg"
  --vivado.synth.jobs "$vivado_jobs"
  --vivado.impl.jobs "$vivado_jobs"
  "${xos[@]}"
  -o "$out"
)

echo "Using Vivado synth/impl jobs: $vivado_jobs"
echo "Running link to vpl.config_hw_runs..."
v++ "${common_args[@]}" --to_step vpl.config_hw_runs

vpl_dir="$temp_dir/link/vivado/vpl"
good="$vpl_dir/.local/hw_platform/iprepo/ipdefs/other_ipdefs/clk_metadata_adapter_v1_0/hdl/clk_metadata_adapter_v1_0_vl_rfs.v"

if [[ -f "$good" ]]; then
  mapfile -t targets < <(
    find "$vpl_dir" -type f \( \
      -path "*/pxi_ii_infra/clk_metadata_adapter_v1_0/hdl/clk_metadata_adapter_v1_0_vl_rfs.v" -o \
      -path "*/prj.gen/*/ipshared/*/hdl/clk_metadata_adapter_v1_0_vl_rfs.v" \
    \) | sort
  )

  if [[ "${#targets[@]}" -gt 0 ]]; then
    echo "Patching ${#targets[@]} generated clk metadata adapter HDL file(s)..."
    for target in "${targets[@]}"; do
      chmod u+w "$target"
      cp "$good" "$target"
    done
  else
    echo "No generated clk metadata adapter HDL targets needed patching."
  fi
else
  echo "No clk metadata adapter workaround source found under:"
  echo "  $good"
  echo "Continuing without the patch workaround."
fi

echo "Resuming link from vpl.synth..."
v++ "${common_args[@]}" --from_step vpl.synth

echo "Link complete:"
echo "  $out"
