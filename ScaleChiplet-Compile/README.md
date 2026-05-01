# ScaleChiplet-Compile

This directory packages the ScaleChiplet chiplet partitioner source and the
helper scripts needed to turn input MLIR into `N` chiplet output MLIR files.

This folder is the frontend entrypoint for the compiler flow. After cloning the
artifact, all compile-side commands should be run from here.

## External LLVM Source Dependency

The large LLVM source tree is intentionally not vendored here.

The build script looks for LLVM in one of two places:

```bash
$LLVM_PROJECT_DIR
polygeist/llvm-project/llvm
```

You can either:

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

Or, if LLVM lives elsewhere:

```bash
cd ScaleChiplet-Compile
LLVM_PROJECT_DIR=../llvm-project/llvm ./build-scalechiplet.sh -j 16
```

## Quick Start

From the repository root:

```bash
cd ScaleChiplet-Compile
```

Build the compiler:

```bash
./build-scalechiplet.sh -j 16
```

## LocalBuild
## GPT2 Small
cd /home/nu/ChenLab/ScaleChipletThesis/ScaleChiplet-Compile

SCALECHIPLET_OPT_BIN=/home/nu/ChenLab/TosaFlow/build/bin/tosa-flow-opt \
./scripts/run_scalechiplet_flow.sh \
  /home/nu/ChenLab/ScaleChipletThesis/ScaleChiplet-Compile/samples/mlir/tensor-mlir/gpt2_small_bf16.mlir \
  /home/nu/ChenLab/ScaleChipletThesis/ScaleChiplet-Compile/output/gpt2_small_split3_tosaflow_semantic \
  3

## GPT2 Medium
cd /home/nu/ChenLab/ScaleChipletThesis/ScaleChiplet-Compile

SCALECHIPLET_OPT_BIN=/home/nu/ChenLab/TosaFlow/build/bin/tosa-flow-opt \
./scripts/run_scalechiplet_flow.sh \
  /home/nu/ChenLab/ScaleChipletThesis/ScaleChiplet-Compile/samples/mlir/tensor-mlir/gpt2_medium_fxp8.mlir \
  /home/nu/ChenLab/ScaleChipletThesis/ScaleChiplet-Compile/output/gpt2_medium_fxp8_split3_tosaflow_semantic \
  3

## MobileNet
cd /home/nu/ChenLab/ScaleChipletThesis/ScaleChiplet-Compile

SCALECHIPLET_OPT_BIN=/home/nu/ChenLab/TosaFlow/build/bin/tosa-flow-opt \
./scripts/run_scalechiplet_flow.sh \
  /home/nu/ChenLab/ScaleChipletThesis/ScaleChiplet-Compile/samples/mlir/scalehls/mobilenet.mlir \
  /home/nu/ChenLab/ScaleChipletThesis/ScaleChiplet-Compile/output/mobilenet_split3_tosaflow_semantic \
  3

## ResNet18
cd /home/nu/ChenLab/ScaleChipletThesis/ScaleChiplet-Compile

SCALECHIPLET_OPT_BIN=/home/nu/ChenLab/TosaFlow/build/bin/tosa-flow-opt \
./scripts/run_scalechiplet_flow.sh \
  /home/nu/ChenLab/ScaleChipletThesis/ScaleChiplet-Compile/samples/mlir/scalehls/resnet18.mlir \
  /home/nu/ChenLab/ScaleChipletThesis/ScaleChiplet-Compile/output/resnet18_split3_tosaflow_semantic \
  3

## VGG16
cd /home/nu/ChenLab/ScaleChipletThesis/ScaleChiplet-Compile

SCALECHIPLET_OPT_BIN=/home/nu/ChenLab/TosaFlow/build/bin/tosa-flow-opt \
./scripts/run_scalechiplet_flow.sh \
  /home/nu/ChenLab/ScaleChipletThesis/ScaleChiplet-Compile/samples/mlir/scalehls/vgg16.mlir \
  /home/nu/ChenLab/ScaleChipletThesis/ScaleChiplet-Compile/output/vgg16_split3_tosaflow_semantic \
  3

## Online Build Commands
## GPT2 Small
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

## Current Sample Inputs

Current MLIR sample inputs live under:

```bash
samples/mlir/scalehls/
samples/mlir/tensor-mlir/
```
