#!/usr/bin/env python3
"""Verify that split chiplet files exactly match functions in chiplets.mlir."""

from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path


FUNC_RE = re.compile(r"^\s*func\.func @([^\s(]+)")


def extract_functions(text: str) -> dict[str, str]:
    lines = text.splitlines()
    functions: dict[str, str] = {}
    i = 0
    while i < len(lines):
        line = lines[i]
        match = FUNC_RE.match(line)
        if match is None:
            i += 1
            continue

        name = match.group(1)
        chunk = [line]
        depth = line.count("{") - line.count("}")
        i += 1
        while depth > 0 and i < len(lines):
            line = lines[i]
            chunk.append(line)
            depth += line.count("{") - line.count("}")
            i += 1
        functions[name] = "\n".join(chunk).rstrip() + "\n"
    return functions


def split_file_map(split_dir: Path) -> dict[str, Path]:
    return {path.stem: path for path in sorted(split_dir.glob("*.mlir"))}


def verify_chiplets(chiplets_mlir: Path, split_dir: Path, expect_count: int) -> list[str]:
    errors: list[str] = []
    combined_functions = extract_functions(chiplets_mlir.read_text(encoding="utf-8"))
    split_files = split_file_map(split_dir)

    combined_names = sorted(combined_functions)
    split_names = sorted(split_files)

    if len(combined_functions) != expect_count:
        errors.append(
            f"{chiplets_mlir}: expected {expect_count} chiplet funcs, found "
            f"{len(combined_functions)} ({combined_names})"
        )
    if len(split_files) != expect_count:
        errors.append(
            f"{split_dir}: expected {expect_count} split files, found "
            f"{len(split_files)} ({split_names})"
        )
    if set(combined_functions) != set(split_files):
        errors.append(
            f"name mismatch between {chiplets_mlir} ({combined_names}) and "
            f"{split_dir} ({split_names})"
        )

    for split_name, split_path in sorted(split_files.items()):
        split_functions = extract_functions(split_path.read_text(encoding="utf-8"))
        if len(split_functions) != 1:
            errors.append(
                f"{split_path}: expected exactly 1 func, found "
                f"{len(split_functions)} ({sorted(split_functions)})"
            )
            continue

        [(func_name, func_body)] = split_functions.items()
        if func_name != split_name:
            errors.append(
                f"{split_path}: file stem '{split_name}' does not match func "
                f"name '{func_name}'"
            )

        combined_body = combined_functions.get(func_name)
        if combined_body is None:
            errors.append(f"{split_path}: func '{func_name}' not found in {chiplets_mlir}")
            continue

        if func_body != combined_body:
            errors.append(
                f"{split_path}: function body does not exactly match {chiplets_mlir}"
            )

    return errors


def compare_split_dirs(first_dir: Path, second_dir: Path, expect_count: int) -> list[str]:
    errors: list[str] = []
    first_files = split_file_map(first_dir)
    second_files = split_file_map(second_dir)

    first_names = sorted(first_files)
    second_names = sorted(second_files)

    if len(first_files) != expect_count:
        errors.append(
            f"{first_dir}: expected {expect_count} split files, found "
            f"{len(first_files)} ({first_names})"
        )
    if len(second_files) != expect_count:
        errors.append(
            f"{second_dir}: expected {expect_count} split files, found "
            f"{len(second_files)} ({second_names})"
        )
    if set(first_files) != set(second_files):
        errors.append(
            f"name mismatch between {first_dir} ({first_names}) and "
            f"{second_dir} ({second_names})"
        )

    for name in sorted(set(first_files) & set(second_files)):
        first_text = first_files[name].read_text(encoding="utf-8")
        second_text = second_files[name].read_text(encoding="utf-8")
        if first_text != second_text:
            errors.append(
                f"{first_files[name]} and {second_files[name]} differ for '{name}'"
            )

    return errors


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("chiplets_mlir", type=Path, help="Combined chiplets.mlir file")
    parser.add_argument("split_dir", type=Path, help="Directory with per-chiplet .mlir files")
    parser.add_argument(
        "--compare-dir",
        type=Path,
        help="Optional second split directory to compare byte-for-byte",
    )
    parser.add_argument(
        "--expect-count",
        type=int,
        default=8,
        help="Expected number of chiplets/functions (default: 8)",
    )
    args = parser.parse_args()

    errors = verify_chiplets(args.chiplets_mlir, args.split_dir, args.expect_count)
    if args.compare_dir is not None:
        errors.extend(compare_split_dirs(args.split_dir, args.compare_dir, args.expect_count))

    if errors:
        for error in errors:
            print(f"FAIL: {error}")
        return 1

    print(
        f"PASS: {args.split_dir} exactly matches functions in {args.chiplets_mlir}"
    )
    if args.compare_dir is not None:
        print(
            f"PASS: {args.split_dir} and {args.compare_dir} are byte-for-byte identical"
        )
    return 0


if __name__ == "__main__":
    sys.exit(main())
