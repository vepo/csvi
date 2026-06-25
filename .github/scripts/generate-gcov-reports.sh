#!/usr/bin/env bash
# Generate .gcov reports from .gcda files produced by `make check`.
set -euo pipefail

gcda_files=()
while IFS= read -r -d '' gcda; do
  gcda_files+=("$gcda")
done < <(find src tests -name '*.gcda' -print0)

if [ "${#gcda_files[@]}" -eq 0 ]; then
  echo "error: no .gcda files found (build with --enable-coverage and run make check)" >&2
  exit 1
fi

for gcda in "${gcda_files[@]}"; do
  objdir=$(dirname "$gcda")
  objbase=$(basename "$gcda" .gcda)
  (cd "$objdir" && gcov -b -c "$objbase") || true
done

report_count=$(find src tests -name '*.gcov' | wc -l)
echo "generated ${report_count} gcov report(s) under src/ and tests/"
