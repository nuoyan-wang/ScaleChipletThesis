# GPT-2 Medium 3-Chunk Proof Bundle

This bundle packages the current `3 chunks + 2 routers` routed design, the monolithic `newest` reference boundary checker, and the bundled golden IO tree used to prove that the routed design matches the reference.

## Contents

- `design/`
  - `chunk0_attn/stream_ip.cpp`
  - `chunk1_ffn_up/stream_ip.cpp`
  - `chunk2_ffn_down/stream_ip.cpp`
  - `routers/router_c0_kernel/kernel.cpp`
  - `routers/router_c2_kernel/kernel.cpp`
  - stage harness source files
- `reference/`
  - `newest_gpt2_medium.cpp`
  - `tb_newest_layer_boundary.cpp`
  - `hls_math_stub.cpp`
- `golden_io/`
  - bundled 3-prompt GPT-2 medium golden files
- `bin/`
  - prebuilt stage harnesses and `tb_newest_layer_boundary`
- `reports/`
  - current local stream csynth reports for chunk0, chunk1, chunk2

## Fast path

If you are on a compatible Linux x86_64 machine, the prebuilt binaries should run directly:

```bash
cd gpt2_medium_3chunk_proof_bundle_20260430
./run_compare.sh
```

Expected output:

```text
position=0 router_match
checked=1 mismatching_rounds=0
all_router_rounds_match=True
```

## Rebuild path

If you want to rebuild the binaries from source:

Requirements:

- `g++`
- `python3`
- `numpy`
- Xilinx Vitis/HLS headers available in a Vitis install root containing `include/ap_int.h`
  - the build script auto-detects common roots such as `/tools/Xilinx/2025.2/Vitis`
  - or set `XILINX_HLS=/path/to/Vitis`

If you see:

```text
fatal error: ap_int.h: No such file or directory
```

then your machine either does not have Vitis installed in one of the expected
locations, or `sudo` cleared the environment you meant to use. In that case:

```bash
export XILINX_HLS=/path/to/your/Vitis
./build_binaries.sh
```

Avoid `sudo` unless you actually need it. If you must use `sudo`, preserve the
variable explicitly:

```bash
sudo env XILINX_HLS=/path/to/your/Vitis ./build_binaries.sh
```

Build and run:

```bash
cd gpt2_medium_3chunk_proof_bundle_20260430
./build_binaries.sh
./run_compare.sh
```

Or force rebuild in one step:

```bash
cd gpt2_medium_3chunk_proof_bundle_20260430
REBUILD_BINARIES=1 ./run_compare.sh
```

## Useful options

Run more than one round:

```bash
./run_compare.sh --rounds 2
```

Use a custom scratch directory:

```bash
./run_compare.sh --scratch-dir /tmp/gpt2_bundle_compare
```

Use a different bundled prompt-slot mapping:

```bash
./run_compare.sh --prompts 0 1 2
```

## Notes

- The bundled golden IO tree is large. If you put this bundle in git, use Git LFS.
- This bundle is self-contained for the routed compare. It does not depend on the original repo layout.
- The included csynth reports are reference artifacts only; the proof of correctness is `run_compare.sh`.
