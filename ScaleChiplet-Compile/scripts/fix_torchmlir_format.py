#!/usr/bin/env python3
"""Normalize torch-mlir/TOSA text formatting to generic-op style.

Transforms:
1) Generic op-attr wrappers:
   "op"() <{...}> : (...)  ->  "op"() {...} : (...)
2) TOSA custom assembly op lines:
   %x = tosa.foo %a, %b {k = array<i64: 1, 2>} : (...) -> ...
   ->
   %x = "tosa.foo" (%a, %b) {k = [1, 2]} : (...) -> ...
"""

from __future__ import annotations

import argparse
import pathlib
import re
import sys


OPEN_ATTR_RE = re.compile(r"\)\s*<\{")
CLOSE_ATTR_RE = re.compile(r"\}>\s*:")
TOSA_CUSTOM_RE = re.compile(
    r"^(\s*%[^\s=]+)\s*=\s*tosa\.([A-Za-z0-9_]+)\s+(.*?)\s*:\s*(\(.*\)\s*->\s*.*)$"
)
ARRAY_ATTR_RE = re.compile(r"array<[^:>]+:\s*([^>]+)>")
SHIFT_ATTR_RE = re.compile(r"shift\s*=\s*(-?\d+)\s*:\s*i8\b")
AXIS_ATTR_RE = re.compile(r"axis\s*=\s*(-?\d+)\s*:\s*i32\b")


def _normalize_array_attrs(attr_dict: str) -> tuple[str, int]:
    count = 0

    def repl(match: re.Match[str]) -> str:
        nonlocal count
        count += 1
        elems = ", ".join(part.strip() for part in match.group(1).split(","))
        return f"[{elems}]"

    return ARRAY_ATTR_RE.sub(repl, attr_dict), count


def _rewrite_tosa_custom_line(line: str) -> tuple[str, int, int]:
    m = TOSA_CUSTOM_RE.match(line)
    if not m:
        return line, 0, 0

    lhs, op_name, body, type_sig = m.groups()
    body = body.strip()
    attrs = ""
    operands = body
    if "{" in body and body.endswith("}"):
        left, right = body.split("{", 1)
        operands = left.strip()
        attrs = "{" + right
    attrs, array_count = _normalize_array_attrs(attrs)

    # Build generic op form expected by parsers without TOSA custom assembly.
    if attrs:
        rewritten = f'{lhs} = "tosa.{op_name}" ({operands}) {attrs} : {type_sig}'
    else:
        rewritten = f'{lhs} = "tosa.{op_name}" ({operands}) : {type_sig}'
    return rewritten, 1, array_count


def normalize_mlir_text(text: str) -> tuple[str, int, int, int, int]:
    """Return normalized text plus replacement counts."""
    text, open_count = OPEN_ATTR_RE.subn(r") {", text)
    text, close_count = CLOSE_ATTR_RE.subn(r"} :", text)
    text, shift_count = SHIFT_ATTR_RE.subn(r"shift = \1 : i32", text)
    text, axis_count = AXIS_ATTR_RE.subn(r"axis = \1 : i64", text)
    tosa_rewrites = 0
    array_count = 0
    out_lines = []
    for line in text.splitlines():
        new_line, rewrites, arrays = _rewrite_tosa_custom_line(line)
        out_lines.append(new_line)
        tosa_rewrites += rewrites
        array_count += arrays
    # Preserve a trailing newline for file stability.
    text = "\n".join(out_lines) + ("\n" if text.endswith("\n") else "")
    return (
        text,
        open_count,
        close_count + shift_count + axis_count,
        tosa_rewrites,
        array_count,
    )


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("input", type=pathlib.Path, help="Input MLIR file")
    parser.add_argument(
        "-o",
        "--output",
        type=pathlib.Path,
        help="Output MLIR file (required unless --inplace)",
    )
    parser.add_argument(
        "--inplace",
        action="store_true",
        help="Overwrite input file in-place",
    )
    args = parser.parse_args()

    if args.inplace and args.output is not None:
        parser.error("Use either --inplace or --output, not both.")
    if not args.inplace and args.output is None:
        parser.error("Either --inplace or --output is required.")

    input_path = args.input
    output_path = input_path if args.inplace else args.output

    text = input_path.read_text(encoding="utf-8")
    fixed, open_count, close_count, tosa_rewrites, array_count = normalize_mlir_text(
        text
    )
    output_path.write_text(fixed, encoding="utf-8")

    print(
        f"Replacements: open={open_count}, close={close_count}, "
        f"tosa_custom={tosa_rewrites}, array_attrs={array_count}, "
        f"written={output_path}"
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
