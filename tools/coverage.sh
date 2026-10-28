#!/usr/bin/env bash
# Mide la cobertura de código de las pruebas sobre el dominio y la capa de
# presentación (Clang + llvm-cov). Uso: tools/coverage.sh
# Genera un resumen en consola y un reporte HTML en build/coverage/html.
set -euo pipefail

cd "$(dirname "$0")/.."
build_dir="build/coverage"

llvm_tool() {
    if command -v xcrun >/dev/null 2>&1; then xcrun "$@"; else "$@"; fi
}

cmake --preset coverage >/dev/null
cmake --build --preset coverage
rm -rf "$build_dir/profiles" && mkdir -p "$build_dir/profiles"
ctest --preset coverage

llvm_tool llvm-profdata merge -sparse "$build_dir"/profiles/*.profraw -o "$build_dir/tests.profdata"

# llvm-cov recibe el primer ejecutable como argumento y el resto con -object.
binaries=()
for test_binary in "$build_dir"/tests/tst_*; do
    # Sólo la suite del repositorio; las suites de IA se miden con compare_suites.py.
    [[ "$(basename "$test_binary")" == tst_ia_* ]] && continue
    [[ -f "$test_binary" && -x "$test_binary" ]] && binaries+=("$test_binary")
done
objects=("${binaries[0]}")
for test_binary in "${binaries[@]:1}"; do
    objects+=(-object "$test_binary")
done

sources=(src/core src/presentation)
ignore='(tests/|_autogen/|qmltyperegistrations|qmlcache|\.rcc/)'
common=(-instr-profile "$build_dir/tests.profdata" -ignore-filename-regex "$ignore")

llvm_tool llvm-cov report "${objects[@]}" "${common[@]}" "${sources[@]}"
llvm_tool llvm-cov show "${objects[@]}" "${common[@]}" -format html \
    -output-dir "$build_dir/html" "${sources[@]}"

echo "Reporte HTML: $build_dir/html/index.html"
