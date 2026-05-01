#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
ROOT_DIR="$( cd "${SCRIPT_DIR}/.." &> /dev/null && pwd )"

INPUT_MLIR="${1:-${ROOT_DIR}/chiplets.mlir}"
OUT_DIR="${2:-${ROOT_DIR}/chiplets_split}"

mkdir -p "${OUT_DIR}"
rm -f "${OUT_DIR}"/*.mlir

awk -v outdir="${OUT_DIR}" '
function brace_delta(s, t, opens, closes) {
  t = s
  opens = gsub(/\{/, "{", t)
  t = s
  closes = gsub(/\}/, "}", t)
  return opens - closes
}

BEGIN {
  header = ""
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

echo "Wrote split chiplet files to: ${OUT_DIR}"
find "${OUT_DIR}" -maxdepth 1 -type f -name "*.mlir" | sort
