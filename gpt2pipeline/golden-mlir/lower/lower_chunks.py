#!/usr/bin/env python3
"""
lower_chunks.py — lower local chunk0/chunk1/chunk2 MLIR files to raw HLS C++.
"""
from __future__ import annotations
import argparse
import re
import shutil
import subprocess
from pathlib import Path

SCRIPT_DIR = Path(__file__).resolve().parent
GOLDEN_DIR = SCRIPT_DIR.parent


def _candidate_tool_paths(start: Path, tool_name: str) -> list[Path]:
    candidates: list[Path] = []
    for base in [start, *start.parents]:
        candidates.append(base / "build/bin" / tool_name)
        candidates.append(base / "tensor-mlir-fxp8/build/bin" / tool_name)
    return candidates


def resolve_tool(tool_name: str) -> Path:
    which_path = shutil.which(tool_name)
    if which_path:
        return Path(which_path)
    for candidate in _candidate_tool_paths(SCRIPT_DIR, tool_name):
        if candidate.exists():
            return candidate
    raise FileNotFoundError(
        f"Could not find `{tool_name}`. Put it on PATH or place it under a nearby "
        f"`build/bin/` or `tensor-mlir-fxp8/build/bin/` directory."
    )


OPT = resolve_tool("tensor-mlir-opt")
XLATE = resolve_tool("tensor-mlir-translate")
VITIS = Path(shutil.which("vitis-run") or "vitis-run")

# ── lowering pipeline ─────────────────────────────────────────────────────────
# Identical to FIT_MONOLITHIC_PIPELINE in gpt2_block.py EXCEPT:
#   - memref-const-to-argument removed  (weights stay local)
PIPELINE = [
    "--tosa-dense-to-dense-resource=threshold=129",
    "--func-set-top-func",
    "--tosa-legalize-integer-types",
    "--tosa-to-scf",
    "--tosa-to-linalg-pipeline",
    "--tosa-to-arith",
    "--tosa-to-tensor",
    "--linalg-fold-unit-extent-dims",
    "--linalg-generalize-named-ops",
    "--linalg-fold-into-elementwise",
    "--canonicalize",
    "--one-shot-bufferize=bufferize-function-boundaries "
        "function-boundary-type-conversion=identity-layout-map buffer-alignment=0",
    "--memref-reorder-global-ops",
    "--func-results-to-params",
    "--memref-const-to-argument=threshold=129",
    "--func-legalize-params",
    "--dataflow-generate-graph",
    "--fold-memref-alias-ops",
    "--linalg-tile-ops=tile-sizes=1,1,16,4",
    "--convert-linalg-to-affine-loops",
    "--convert-memref-copy-to-affine",
    "--memref-bf16-to-fp8",
    "--scf-rewrite-reduce-loops=num-partial-results=2",
    "--lower-affine",
    "--fold-memref-alias-ops",
    "--lower-affine",
    "--arith-expand",
    "--canonicalize",
    "--memref-reuse-buffers",
    "--canonicalize",
    "--hls-apply-unroll-to-kernel",
    "--hls-estimate-cycles",
    "--loop-invariant-code-motion",
    "--dataflow-node-to-func",
    "--canonicalize",
]


# ── helpers ───────────────────────────────────────────────────────────────────
def run(cmd: list) -> None:
    print("+", " ".join(str(c) for c in cmd), flush=True)
    subprocess.run([str(c) for c in cmd], check=True)


def rewrite_negf(text: str) -> str:
    """Replace arith.negf (unsupported in HLSCPP) with mulf -1."""
    ctr = 0
    def _sub(m: re.Match) -> str:
        nonlocal ctr; ctr += 1
        i, d, s, t = m["i"], m["d"], m["s"], m["t"]
        c = f"%_neg1_{ctr}"
        return f"{i}{c} = arith.constant -1.000000e+00 : {t}\n{i}{d} = arith.mulf {s}, {c} : {t}"
    return re.sub(
        r"(?P<i>[ \t]*)(?P<d>%\w+) = arith\.negf (?P<s>%\w+) : (?P<t>[\w<>x]+)",
        _sub, text,
    )


def normalize_tosa_attrs(text: str) -> str:
    # tensor-mlir-opt expects reduce axes as i32 here, while the TosaFlow-ready
    # manual chiplets carry i64.
    text = re.sub(r"axis = (\d+) : i64", r"axis = \1 : i32", text)
    # tensor-mlir-opt expects tosa.const to use `values`, while the
    # TosaFlow-normalized form uses `value`.
    text = re.sub(r'("tosa\.const"\(\)\s*)\{value\s*=', r"\1{values =", text)
    # tensor-mlir-opt expects the older 3-operand tosa.mul form with an i8
    # shift/zero-point operand, and the older 4-operand tosa.matmul form with
    # two bf16 zero-point operands. Some golden-mlir chunks do not already
    # contain those constants, so materialize stable helper SSA values.
    zero_i8 = "%_tf_zero_i8"
    zero_bf16 = "%_tf_zero_bf16"
    out_lines = []
    seen_helper_defs: set[str] = set()
    inserted_legacy_consts = zero_i8 in text or zero_bf16 in text
    for line in text.splitlines():
        if re.match(rf"\s*{re.escape(zero_i8)}\s*=", line):
            if zero_i8 in seen_helper_defs:
                continue
            seen_helper_defs.add(zero_i8)
        if re.match(rf"\s*{re.escape(zero_bf16)}\s*=", line):
            if zero_bf16 in seen_helper_defs:
                continue
            seen_helper_defs.add(zero_bf16)
        out_lines.append(line)
        if not inserted_legacy_consts and re.match(r"\s*func\.func\b", line):
            indent = re.match(r"(\s*)", line).group(1) + "  "
            out_lines.append(
                f'{indent}{zero_i8} = "tosa.const"() {{values = dense<0> : tensor<1xi8>}} : () -> tensor<1xi8>'
            )
            out_lines.append(
                f'{indent}{zero_bf16} = "tosa.const"() {{values = dense<0.000000e+00> : tensor<1xbf16>}} : () -> tensor<1xbf16>'
            )
            inserted_legacy_consts = True
    text = "\n".join(out_lines) + ("\n" if text.endswith("\n") else "")

    text = re.sub(
        r'"tosa\.mul"\s*\((%[\w.]+),\s*(%[\w.]+)\)\s*\{shift = 0 : i32\}\s*:\s*\(([^,]+),\s*([^)]+)\)\s*->\s*([^\n]+)',
        rf'"tosa.mul" (\1, \2, {zero_i8}) : (\3, \4, tensor<1xi8>) -> \5',
        text,
    )
    text = re.sub(
        r'"tosa\.matmul"\s*\((%[\w.]+),\s*(%[\w.]+)\)\s*:\s*\(([^,]+),\s*([^)]+)\)\s*->\s*([^\n]+)',
        rf'"tosa.matmul" (\1, \2, {zero_bf16}, {zero_bf16}) : (\3, \4, tensor<1xbf16>, tensor<1xbf16>) -> \5',
        text,
    )
    # tensor-mlir-opt expects old-style shape operands instead of embedded attrs.
    tile_ctr = 0
    reshape_ctr = 0
    slice_ctr = 0
    out_lines = []
    for line in text.splitlines():
        m = re.match(
            r'(\s*)(%\w+)\s*=\s*"tosa\.transpose"\s*\((%\w+),\s*(%\w+)\)\s*:\s*\(([^,]+),\s*tensor<([0-9]+)xi32>\)\s*->\s*(.+)',
            line,
        )
        if m:
            indent, dst, src, perm_ssa, in_ty, rank, out_ty = m.groups()
            perm_line = None
            for candidate in text.splitlines():
                if re.match(rf'\s*{re.escape(perm_ssa)}\s*=\s*"tosa\.const"\(\)\s*\{{values = dense<\[[0-9,\s]+\]> : tensor<{rank}xi32>\}} : \(\) -> tensor<{rank}xi32>', candidate):
                    perm_line = candidate.strip()
                    break
            if perm_line:
                perm_vals = re.search(r'dense<\[([0-9,\s]+)\]>', perm_line).group(1)
                perm_attr = ", ".join(v.strip() for v in perm_vals.split(","))
                out_lines.append(
                    f'{indent}{dst} = tosa.transpose {src} {{perms = array<i32: {perm_attr}>}} : ({in_ty}) -> {out_ty}'
                )
                continue
        m = re.match(
            r'(\s*)(%\w+)\s*=\s*"tosa\.tile"\s*\((%\w+)\)\s*\{multiples = \[([0-9,\s]+)\]\}\s*:\s*\(([^)]+)\)\s*->\s*(.+)',
            line,
        )
        if m:
            indent, dst, src, multiples, in_ty, out_ty = m.groups()
            vals = ", ".join(v.strip() for v in multiples.split(","))
            rank = len([v for v in multiples.split(",") if v.strip()])
            shape = f"%_tf_tile_shape_{tile_ctr}"
            tile_ctr += 1
            out_lines.append(
                f'{indent}{shape} = tosa.const_shape  {{values = dense<[{vals}]> : tensor<{rank}xindex>}} : () -> !tosa.shape<{rank}>'
            )
            out_lines.append(
                f'{indent}{dst} = "tosa.tile" ({src}, {shape}) : ({in_ty}, !tosa.shape<{rank}>) -> {out_ty}'
            )
            continue
        m = re.match(
            r'(\s*)(%\w+)\s*=\s*"tosa\.reshape"\s*\((%\w+)\)\s*\{new_shape = \[([0-9,\s]+)\]\}\s*:\s*\(([^)]+)\)\s*->\s*(.+)',
            line,
        )
        if m:
            indent, dst, src, new_shape, in_ty, out_ty = m.groups()
            vals = ", ".join(v.strip() for v in new_shape.split(","))
            rank = len([v for v in new_shape.split(",") if v.strip()])
            shape = f"%_tf_reshape_shape_{reshape_ctr}"
            reshape_ctr += 1
            out_lines.append(
                f'{indent}{shape} = tosa.const_shape  {{values = dense<[{vals}]> : tensor<{rank}xindex>}} : () -> !tosa.shape<{rank}>'
            )
            out_lines.append(
                f'{indent}{dst} = "tosa.reshape" ({src}, {shape}) : ({in_ty}, !tosa.shape<{rank}>) -> {out_ty}'
            )
            continue
        m = re.match(
            r'(\s*)(%\w+)\s*=\s*"tosa\.slice"\s*\((%\w+)\)\s*\{([^}]*)\}\s*:\s*\(([^)]+)\)\s*->\s*(.+)',
            line,
        )
        if m:
            indent, dst, src, attrs, in_ty, out_ty = m.groups()
            start_match = re.search(r"start = \[([0-9,\s]+)\]", attrs)
            size_match = re.search(r"size = \[([0-9,\s]+)\]", attrs)
            if not start_match or not size_match:
                out_lines.append(line)
                continue
            start_vals = start_match.group(1)
            size_vals = size_match.group(1)
            start_list = ", ".join(v.strip() for v in start_vals.split(","))
            size_list = ", ".join(v.strip() for v in size_vals.split(","))
            rank = len([v for v in start_vals.split(",") if v.strip()])
            start_shape = f"%_tf_slice_start_{slice_ctr}"
            size_shape = f"%_tf_slice_size_{slice_ctr}"
            slice_ctr += 1
            out_lines.append(
                f'{indent}{start_shape} = tosa.const_shape  {{values = dense<[{start_list}]> : tensor<{rank}xindex>}} : () -> !tosa.shape<{rank}>'
            )
            out_lines.append(
                f'{indent}{size_shape} = tosa.const_shape  {{values = dense<[{size_list}]> : tensor<{rank}xindex>}} : () -> !tosa.shape<{rank}>'
            )
            out_lines.append(
                f'{indent}{dst} = "tosa.slice" ({src}, {start_shape}, {size_shape}) : ({in_ty}, !tosa.shape<{rank}>, !tosa.shape<{rank}>) -> {out_ty}'
            )
            continue
        out_lines.append(line)
    text = "\n".join(out_lines) + ("\n" if text.endswith("\n") else "")
    return text


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Lower local chunk MLIR files in golden-mlir to HLS C++."
    )
    parser.add_argument(
        "--input-dir",
        type=Path,
        default=GOLDEN_DIR,
        help="Directory containing chunk0.mlir, chunk1.mlir, and chunk2.mlir.",
    )
    parser.add_argument(
        "--output-dir",
        type=Path,
        default=GOLDEN_DIR,
        help="Directory for lowered MLIR and generated C++ outputs.",
    )
    return parser.parse_args()


def load_input_chunks(input_dir: Path) -> dict[str, str] | None:
    paths = {
        "chunk0": input_dir / "chunk0.mlir",
        "chunk1": input_dir / "chunk1.mlir",
        "chunk2": input_dir / "chunk2.mlir",
    }
    if not all(path.exists() for path in paths.values()):
        return None
    return {
        name: normalize_tosa_attrs(path.read_text())
        for name, path in paths.items()
    }


# ── main ──────────────────────────────────────────────────────────────────────
def main() -> None:
    args = parse_args()
    out_dir = args.output_dir.resolve()
    out_dir.mkdir(parents=True, exist_ok=True)

    chunks = load_input_chunks(args.input_dir.resolve())
    if chunks is None:
        raise FileNotFoundError(
            f"Expected chunk0.mlir, chunk1.mlir, and chunk2.mlir in {args.input_dir.resolve()}"
        )
    print(f"Using local chunk MLIR files from {args.input_dir.resolve()}", flush=True)

    for name, mlir_text in chunks.items():
        print(f"\n{'='*64}\n  {name}\n{'='*64}", flush=True)

        src_mlir = out_dir / f"{name}.mlir"
        src_mlir.write_text(mlir_text)
        print(f"  wrote {src_mlir.name}")

        lowered = out_dir / f"{name}_lowered.mlir"
        run([OPT, src_mlir, *PIPELINE, "-o", lowered])

        hls_ready = out_dir / f"{name}_hls_ready.mlir"
        hls_ready.write_text(rewrite_negf(lowered.read_text()))

        chunk_dir = out_dir / name
        chunk_dir.mkdir(exist_ok=True)
        raw_cpp = chunk_dir / "model_raw.cpp"
        run([XLATE, "--mlir-to-hlscpp", hls_ready, "-o", raw_cpp])
        print(f"  raw C++ → {raw_cpp.name}")

    print("\n" + "="*64)
    print("Generated raw HLS C++ files:")
    for name in chunks:
        print(f"  {out_dir/name/'model_raw.cpp'}")


if __name__ == "__main__":
    main()
