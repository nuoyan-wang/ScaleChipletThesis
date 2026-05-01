#!/usr/bin/env python3
"""Convert fresh torch-mlir TOSA text into the older ScaleChiplet input style.

The local ScaleChiplet build predates newer TOSA custom assembly for shape values.
This script keeps the tensor computation unchanged, but rewrites:

* op property wrappers: ``<{...}>`` -> ``{...}``
* ``tosa.const_shape`` SSA values into op attributes
* ``tosa.reshape/tile/slice`` shape operands into attributes
* ``tosa.matmul/mul`` zero-point/shift operands into the older forms
* ``tosa.negate`` zero-point operands into the older single-operand form
* one-operand transpose with ``perms`` attr into two-operand transpose
"""

from __future__ import annotations

import argparse
import pathlib
import re
import sys

from fix_torchmlir_format import normalize_mlir_text


SSA = r"%[-A-Za-z0-9_.$]+"
LHS = rf"(\s*{SSA}\s*=\s*)"
TYPE = r"([^,()]+(?:<[^>]+>)?)"

CONST_SHAPE_RE = re.compile(
    rf"^\s*({SSA})\s*=\s*\"tosa\.const_shape\"\s*\(\)\s*"
    r"\{values?\s*=\s*dense<([^>]+)>\s*:\s*tensor<(\d+)xindex>\}\s*"
    r":\s*\(\)\s*->\s*!tosa\.shape<\d+>\s*$"
)
CONST_VALUES_RE = re.compile(r'("tosa\.const"\(\)\s*)\{values\s*=')
RESHAPE_RE = re.compile(
    rf"^{LHS}\"tosa\.reshape\"\s*\(({SSA}),\s*({SSA})\)\s*"
    r":\s*\(([^,]+),\s*!tosa\.shape<\d+>\)\s*->\s*(.+)$"
)
TILE_RE = re.compile(
    rf"^{LHS}\"tosa\.tile\"\s*\(({SSA}),\s*({SSA})\)\s*"
    r":\s*\(([^,]+),\s*!tosa\.shape<\d+>\)\s*->\s*(.+)$"
)
SLICE_RE = re.compile(
    rf"^{LHS}\"tosa\.slice\"\s*\(({SSA}),\s*({SSA}),\s*({SSA})\)\s*"
    r":\s*\(([^,]+),\s*!tosa\.shape<\d+>,\s*!tosa\.shape<\d+>\)\s*->\s*(.+)$"
)
MATMUL_RE = re.compile(
    rf"^{LHS}\"tosa\.matmul\"\s*\(({SSA}),\s*({SSA}),\s*{SSA},\s*{SSA}\)\s*"
    r":\s*\(([^,]+),\s*([^,]+),\s*[^,]+,\s*[^)]+\)\s*->\s*(.+)$"
)
MUL_RE = re.compile(
    rf"^{LHS}\"tosa\.mul\"\s*\(({SSA}),\s*({SSA}),\s*{SSA}\)\s*"
    r":\s*\(([^,]+),\s*([^,]+),\s*[^)]+\)\s*->\s*(.+)$"
)
NEGATE_RE = re.compile(
    rf"^{LHS}\"tosa\.negate\"\s*\(({SSA}),\s*{SSA},\s*{SSA}\)\s*"
    r":\s*\(([^,]+),\s*[^,]+,\s*[^)]+\)\s*->\s*(.+)$"
)
TRANSPOSE_ATTR_RE = re.compile(
    rf"^{LHS}\"tosa\.transpose\"\s*\(({SSA})\)\s*"
    r"\{perms\s*=\s*\[([^\]]+)\]\}\s*:\s*\(([^)]+)\)\s*->\s*(.+)$"
)


def _parse_shape_values(raw: str, rank: int) -> list[int]:
    raw = raw.strip()
    if raw.startswith("[") and raw.endswith("]"):
        body = raw[1:-1].strip()
        if not body:
            return []
        values = [int(part.strip()) for part in body.split(",")]
        return values
    return [int(raw)] * rank


def _shape_attr(values: list[int]) -> str:
    return "[" + ", ".join(str(v) for v in values) + "]"


def convert_mlir_text(text: str) -> tuple[str, dict[str, int]]:
    text, open_count, close_count, tosa_rewrites, array_count = normalize_mlir_text(text)
    text = CONST_VALUES_RE.sub(r"\1{value =", text)

    shape_values: dict[str, list[int]] = {}
    out_lines: list[str] = []
    counts = {
        "attr_wrappers": open_count + close_count,
        "custom_ops": tosa_rewrites,
        "array_attrs": array_count,
        "const_shapes": 0,
        "const_values": 0,
        "reshape": 0,
        "tile": 0,
        "slice": 0,
        "matmul": 0,
        "mul": 0,
        "negate": 0,
        "transpose": 0,
    }

    transpose_id = 0

    for line in text.splitlines():
        const_shape = CONST_SHAPE_RE.match(line)
        if const_shape:
            name, raw_values, rank = const_shape.groups()
            shape_values[name] = _parse_shape_values(raw_values, int(rank))
            counts["const_shapes"] += 1
            continue

        m = RESHAPE_RE.match(line)
        if m:
            lhs, tensor, shape, input_type, output_type = m.groups()
            values = shape_values[shape]
            line = (
                f'{lhs}"tosa.reshape" ({tensor}) '
                f"{{new_shape = {_shape_attr(values)}}} : ({input_type}) -> {output_type}"
            )
            counts["reshape"] += 1
            out_lines.append(line)
            continue

        m = TILE_RE.match(line)
        if m:
            lhs, tensor, shape, input_type, output_type = m.groups()
            values = shape_values[shape]
            line = (
                f'{lhs}"tosa.tile" ({tensor}) '
                f"{{multiples = {_shape_attr(values)}}} : ({input_type}) -> {output_type}"
            )
            counts["tile"] += 1
            out_lines.append(line)
            continue

        m = SLICE_RE.match(line)
        if m:
            lhs, tensor, start_shape, size_shape, input_type, output_type = m.groups()
            start = shape_values[start_shape]
            size = shape_values[size_shape]
            line = (
                f'{lhs}"tosa.slice" ({tensor}) '
                f"{{start = {_shape_attr(start)}, size = {_shape_attr(size)}}} : "
                f"({input_type}) -> {output_type}"
            )
            counts["slice"] += 1
            out_lines.append(line)
            continue

        m = MATMUL_RE.match(line)
        if m:
            lhs, lhs_operand, rhs_operand, lhs_type, rhs_type, output_type = m.groups()
            line = (
                f'{lhs}"tosa.matmul" ({lhs_operand}, {rhs_operand}) : '
                f"({lhs_type}, {rhs_type}) -> {output_type}"
            )
            counts["matmul"] += 1
            out_lines.append(line)
            continue

        m = MUL_RE.match(line)
        if m:
            lhs, lhs_operand, rhs_operand, lhs_type, rhs_type, output_type = m.groups()
            line = (
                f'{lhs}"tosa.mul" ({lhs_operand}, {rhs_operand}) '
                f"{{shift = 0 : i32}} : ({lhs_type}, {rhs_type}) -> {output_type}"
            )
            counts["mul"] += 1
            out_lines.append(line)
            continue

        m = NEGATE_RE.match(line)
        if m:
            lhs, operand, input_type, output_type = m.groups()
            line = f'{lhs}"tosa.negate" ({operand}) : ({input_type}) -> {output_type}'
            counts["negate"] += 1
            out_lines.append(line)
            continue

        m = TRANSPOSE_ATTR_RE.match(line)
        if m:
            lhs, tensor, perms_raw, input_type, output_type = m.groups()
            perms = [int(part.strip()) for part in perms_raw.split(",")]
            indent = re.match(r"^(\s*)", line).group(1)
            perm_name = f"%tf_perm_{transpose_id}"
            transpose_id += 1
            out_lines.append(
                f'{indent}{perm_name} = "tosa.const"() '
                f"{{value = dense<{_shape_attr(perms)}> : tensor<{len(perms)}xi32>}} : "
                f"() -> tensor<{len(perms)}xi32>"
            )
            line = (
                f'{lhs}"tosa.transpose" ({tensor}, {perm_name}) : '
                f"({input_type}, tensor<{len(perms)}xi32>) -> {output_type}"
            )
            counts["transpose"] += 1
            out_lines.append(line)
            continue

        out_lines.append(line)

    converted = "\n".join(out_lines) + ("\n" if text.endswith("\n") else "")
    counts["const_values"] = len(CONST_VALUES_RE.findall(text))
    return converted, counts


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("input", type=pathlib.Path)
    parser.add_argument("-o", "--output", type=pathlib.Path, required=True)
    args = parser.parse_args()

    converted, counts = convert_mlir_text(args.input.read_text(encoding="utf-8"))
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(converted, encoding="utf-8")
    summary = ", ".join(f"{key}={value}" for key, value in counts.items())
    print(f"converted={args.output} {summary}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
