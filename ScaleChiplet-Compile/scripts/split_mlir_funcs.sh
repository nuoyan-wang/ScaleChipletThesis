#!/usr/bin/env bash
set -euo pipefail

INPUT_MLIR="${1:?usage: split_mlir_funcs.sh INPUT_MLIR OUT_DIR}"
OUT_DIR="${2:?usage: split_mlir_funcs.sh INPUT_MLIR OUT_DIR}"

mkdir -p "${OUT_DIR}"

awk -v outdir="${OUT_DIR}" '
function brace_delta(s, t, opens, closes) {
  t = s
  opens = gsub(/\{/, "{", t)
  t = s
  closes = gsub(/\}/, "}", t)
  return opens - closes
}

BEGIN {
  header = "module {"
  inFunc = 0
  depth = 0
  file = ""
}

/^module[[:space:]]/ {
  header = $0
  next
}

{
  if (!inFunc) {
    if ($0 ~ /^[[:space:]]*func\.func @[A-Za-z0-9_.$-]+/) {
      inFunc = 1
      depth = 0
      fname = $0
      sub(/^[[:space:]]*func\.func @/, "", fname)
      sub(/\(.*/, "", fname)
      file = outdir "/" fname ".mlir"
      print header > file
      print $0 >> file
      depth += brace_delta($0)
      if (depth == 0) {
        print "}" >> file
        close(file)
        inFunc = 0
        file = ""
      }
    }
    next
  }

  print $0 >> file
  depth += brace_delta($0)
  if (depth == 0) {
    print "}" >> file
    close(file)
    inFunc = 0
    file = ""
  }
}
' "${INPUT_MLIR}"

echo "Wrote split MLIR files to: ${OUT_DIR}"
find "${OUT_DIR}" -maxdepth 1 -type f -name "*.mlir" | sort
