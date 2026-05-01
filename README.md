## ScaleChipletThesis
This a repository of M.S Thesis project of Nuoyan Wang

# Backend: ScaleHLS / Tensor-MLIR
For backend lowering, we reused and adapted existing HLS-oriented infrastructure rather than inventing a new codegen stack from scratch. The downstream path borrows from both ScaleHLS-style MLIR lowering and Paul’s Tensor-MLIR flow, using tensor-mlir to lower split chiplets into HLS-ready C++ and then wrapping them for export as hardware IPs. Our main case study was a manually optimized 3-chiplet GPT-2 pipeline: chunk0 handled LN1, QKV, KV-cache update, attention, and output projection; chunk1 handled LN2, FC1, and GELU; chunk2 handled FC2 and the final residual. We then hand-tuned the generated kernels with reproducible patch scripts, added stream-oriented wrappers and routers, and assembled a 5-kernel streaming design c0_router -> chunk0 -> chunk1 -> chunk2 -> c2_router, which we validated against the monolithic baseline before driving from the XRT host runtime.

## Steps for Verification

# Frontend: ScaleChiplet
We built ScaleChiplet as the front end of a multi-chiplet compilation flow: starting from PyTorch, we lower into TOSA/MLIR, outline the graph into a custom chiplet dialect, annotate task costs and dependencies, assign work across an arbitrary number of chiplets, insert explicit inter-chiplet send/receive edges, and finally materialize per-chiplet functions for downstream lowering. In practice, that gave us a flexible partitioner that could split models either by a fixed chiplet count or by more guided placement choices, while preserving a clean compiler representation of communication and placement


1. set `LLVM_PROJECT_DIR` to an `llvm-project/llvm` checkout, or
2. place that checkout at `polygeist/llvm-project/llvm` inside this repo.

To fetch the exact `llvm-project` revision used by the matching Polygeist tree:

```bash
cd ScaleChiplet-Compile
mkdir -p polygeist
git clone https://github.com/hanchenye/llvm-project.git polygeist/llvm-project
git -C polygeist/llvm-project checkout cf0ff67a7b051ed44b7d689eec73b12990676013
```

Then build:

```bash
cd ScaleChiplet-Compile
./build-scalechiplet.sh -j 16
```

## GPT2 Small (Change the number 3 in any of these to get any number of chiplets)
cd /home/nu/ChenLab/ScaleChipletThesis/ScaleChiplet-Compile

SCALECHIPLET_OPT_BIN=build/bin/scale-chiplet-opt \
./scripts/run_scalechiplet_flow.sh \
  samples/mlir/tensor-mlir/gpt2_small_bf16.mlir \
  output/gpt2_small_split3_scalechiplet \
  3

## GPT2 Medium
cd /home/nu/ChenLab/ScaleChipletThesis/ScaleChiplet-Compile

SCALECHIPLET_OPT_BIN=build/bin/scale-chiplet-opt \
./scripts/run_scalechiplet_flow.sh \
  samples/mlir/tensor-mlir/gpt2_medium_fxp8.mlir \
  output/gpt2_medium_fxp8_split3_scalechiplet \
  3

## MobileNet
cd /home/nu/ChenLab/ScaleChipletThesis/ScaleChiplet-Compile

SCALECHIPLET_OPT_BIN=build/bin/scale-chiplet-opt \
./scripts/run_scalechiplet_flow.sh \
  samples/mlir/scalehls/mobilenet.mlir \
  output/mobilenet_split3_scalechiplet \
  3

## ResNet18
cd /home/nu/ChenLab/ScaleChipletThesis/ScaleChiplet-Compile

SCALECHIPLET_OPT_BIN=build/bin/scale-chiplet-opt \
./scripts/run_scalechiplet_flow.sh \
  samples/mlir/scalehls/resnet18.mlir \
  output/resnet18_split3_scalechiplet \
  3

## VGG16
cd /home/nu/ChenLab/ScaleChipletThesis/ScaleChiplet-Compile

SCALECHIPLET_OPT_BIN=build/bin/scale-chiplet-opt \
./scripts/run_scalechiplet_flow.sh \
  samples/mlir/scalehls/vgg16.mlir \
  output/vgg16_split3_scalechiplet \
  3

## To ensure correctness of flow, all outputs should work with downstream emitter, and output of GPT2 Medium should match optimal model

# GPT-2 Medium Proof Bundle, Start with MLIR to CPP
cd /ScaleChipletThesis/gpt2pipeline/golden-mlir
python3 lower/lower_chunks.py


To validate that the routed `3 chunks + 2 routers` GPT-2 medium design matches
the monolithic `/ScaleChipletThesis/gpt2pipeline/golden/newest_gpt2_medium.cpp` reference provided by Paul 

First copy golden_io folder into /ScaleChipletThesis/gpt2pipeline/golden/gpt2_medium_3chunk_proof_bundle_20260430/,

then run:

```bash
cd gpt2pipeline/golden/gpt2_medium_3chunk_proof_bundle_20260430
./run_compare.sh
```

Expected output:

```text
position=0 router_match
checked=1 mismatching_rounds=0
all_router_rounds_match=True
```

What this does:

- runs the bundled routed design harnesses for `router0 -> chunk0 -> chunk1 -> chunk2 -> router2`
- runs the bundled monolithic `newest_gpt2_medium.cpp` boundary reference
- compares the routed outputs against the monolithic reference using the bundled
  golden IO tree
- reports whether all checked routed rounds match exactly at the boundary level

If the prebuilt binaries are missing or incompatible, rebuild and rerun:

```bash
cd gpt2pipeline/golden/gpt2_medium_3chunk_proof_bundle_20260430
./build_binaries.sh
./run_compare.sh
```

# GPT-2 Synth & Link
Code is ready in /ScaleChipletThesis/gpt2pipeline/3prompt to synth and link:
cd gpt2pipeline/3prompt
export PLATFORM=/path/to/your/platform.xpfm
./run_all_csynth.sh && ./build_all_xo.sh && ./link_300mhz.sh

# GPT-2 Onboard Results
/ScaleChipletThesis/gpt2pipeline/onboards holds onboard for my design
You can run with 2 prompt design or 3 prompt design on device.

# BestOf3 Voting Scheme 
Run the GPT-2 Medium Best-of-3 voting simulation from the bundled `gpt2-bo3` folder:


```bash
cd gpt2pipeline/gpt2-bo3
python3 scripts/compare_mlx_single_vs_best_of_3.py \
 --model models/gpt2-medium-mlx-8bit \
 --prompts data/prompts_100_len2.txt \
 --max-new-tokens 16 \
 --temperature 2.0 \
 --top-k 25 \
 --seed 7 \
 --degradation softened-logits \
 --output-json results/mlx_softened_temp20_top25_len2_comparison.json \
 --output-csv results/mlx_softened_temp20_top25_len2_comparison.csv \
 --output-list-csv results/mlx_softened_temp20_top25_len2_perplexity_list.csv
```


# Conclusions
With full MLIR to Chiplet-MLIR workflow, usage of Paul's flow, real onboard deployment, csim tests,
reviewed host code, and additional simulations with matching results, the codebase is complete.


