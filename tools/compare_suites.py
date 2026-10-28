#!/usr/bin/env python3
"""Compara las suites de pruebas del repositorio con las generadas por IA.

Para cada suite mide:
  - casos ejecutados, aserciones y líneas de código de prueba;
  - cobertura de líneas, ramas y regiones del dominio (src/core) con llvm-cov;
  - puntaje de mutación: se introducen errores deliberados (mutantes) en el dominio
    y se cuenta cuántos detecta cada suite (un mutante "muere" si alguna prueba que
    pasaba empieza a fallar o si la suite deja de compilar).

El trabajo se hace en una copia temporal del proyecto: el árbol de trabajo no se toca.
Requiere Clang con llvm-profdata y llvm-cov (en macOS vienen con Xcode).

Uso:  python3 tools/compare_suites.py
Salida: docs/resultados/{resultados.json, resumen.md, resumen.tex, mutantes.tex}
"""

from __future__ import annotations

import json
import os
import re
import shutil
import subprocess
import sys
import tempfile
from dataclasses import dataclass, field
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
OUTPUT_DIR = ROOT / "docs" / "resultados"


@dataclass(frozen=True)
class Suite:
    key: str
    name: str
    targets: tuple[str, ...]
    sources: tuple[str, ...]


SUITES = (
    Suite(
        "repositorio",
        "Repositorio (TDD)",
        ("tst_board", "tst_board_placement", "tst_turns", "tst_winner", "tst_gamecontroller"),
        (
            "tests/tictactoe/tst_board.cpp",
            "tests/tictactoe/tst_board_placement.cpp",
            "tests/tictactoe/tst_turns.cpp",
            "tests/tictactoe/tst_winner.cpp",
            "tests/presentation/tst_gamecontroller.cpp",
        ),
    ),
    Suite("ia_basico", "IA: prompt básico", ("tst_ia_basico",), ("tests/ia/basico/tst_tictactoe.cpp",)),
    Suite(
        "ia_estructurado",
        "IA: prompt estructurado",
        ("tst_ia_estructurado",),
        ("tests/ia/estructurado/tst_ia_estructurado.cpp",),
    ),
)


@dataclass(frozen=True)
class Mutant:
    key: str
    requirement: str
    description: str
    file: str
    original: str
    mutated: str


BOARD = "src/core/src/tictactoe/Board.cpp"
GAME = "src/core/src/tictactoe/TicTacToe.cpp"
GAME_H = "src/core/include/tresenraya/tictactoe/TicTacToe.h"

MUTANTS = (
    Mutant("M1", "R1", "Rechaza la coordenada 3 (límite superior excluido)", BOARD,
           "coordinate <= kMaxCoordinate", "coordinate < kMaxCoordinate"),
    Mutant("M2", "R1", "Acepta la coordenada 0", BOARD,
           "coordinate >= kMinCoordinate", "coordinate >= kMinCoordinate - 1"),
    Mutant("M3", "R1", "Acepta la coordenada 4", BOARD,
           "coordinate <= kMaxCoordinate", "coordinate <= kMaxCoordinate + 1"),
    Mutant("M4", "R1", "Informa el eje equivocado al salir del eje X", BOARD,
           "throw OutOfBoardException(Axis::X, position.x);",
           "throw OutOfBoardException(Axis::Y, position.x);"),
    Mutant("M5", "R1", "Permite jugar sobre una casilla ocupada", BOARD,
           "throw OccupiedCellException(position, *occupant);", "(void)occupant;"),
    Mutant("M6", "R1", "El tablero nunca se considera lleno", BOARD,
           "return std::ranges::all_of(m_cells, [](const auto &cell) { return cell.has_value(); });",
           "return false;"),
    Mutant("M7", "R2", "El primer turno es de O", GAME_H,
           "static constexpr Player kFirstPlayer = Player::X;",
           "static constexpr Player kFirstPlayer = Player::O;"),
    Mutant("M8", "R2", "Los turnos no alternan", GAME,
           "return m_lastPlayer ? opponentOf(*m_lastPlayer) : kFirstPlayer;", "return kFirstPlayer;"),
    Mutant("M9", "R2", "Una jugada inválida consume el turno", GAME,
           "    m_board.place(position, player);\n    m_lastPlayer = player;\n",
           "    m_lastPlayer = player;\n    m_board.place(position, player);\n"),
    Mutant("M10", "R3", "No reconoce la diagonal secundaria", GAME,
           "Line{{{3, 1}, {2, 2}, {1, 3}}},", "Line{{{1, 1}, {2, 2}, {3, 3}}},"),
    Mutant("M11", "R3", "No reconoce la tercera columna", GAME,
           "Line{{{3, 1}, {3, 2}, {3, 3}}},", "Line{{{1, 1}, {1, 2}, {1, 3}}},"),
    Mutant("M12", "R3", "No reconoce la fila inferior", GAME,
           "Line{{{1, 3}, {2, 3}, {3, 3}}},", "Line{{{1, 1}, {2, 1}, {3, 1}}},"),
    Mutant("M13", "R3", "Un tablero lleno es empate aunque haya línea", GAME,
           "    if (m_winningLine)\n        m_status = GameStatus::Won;\n"
           "    else if (m_board.isFull())\n        m_status = GameStatus::Draw;\n",
           "    if (m_board.isFull())\n        m_status = GameStatus::Draw;\n"
           "    else if (m_winningLine)\n        m_status = GameStatus::Won;\n"),
    Mutant("M14", "R3", "Se puede seguir jugando después de ganar", GAME,
           "    if (isOver())\n        throw GameOverException();\n", ""),
    Mutant("M15", "R3", "winner() devuelve al último jugador aunque no haya ganado", GAME,
           "return m_status == GameStatus::Won ? m_lastPlayer : std::nullopt;", "return m_lastPlayer;"),
    Mutant("M16", "R3", "reset() no reinicia el estado de la partida", GAME,
           "    m_winningLine.reset();\n    m_status = GameStatus::InProgress;\n",
           "    m_winningLine.reset();\n"),
)

ASSERTION = re.compile(r"\b(QCOMPARE|QVERIFY2?|QVERIFY_THROWS_EXCEPTION|QFAIL|static_assert)\b")
RESULT_LINE = re.compile(r"^(PASS|FAIL!|XFAIL|XPASS)\s*:\s*\S+::(\S+?)\((.*?)\)", re.M)


@dataclass
class SuiteRun:
    compiled: bool
    passed: set[str] = field(default_factory=set)
    failed: set[str] = field(default_factory=set)


def llvm(tool: str) -> list[str]:
    return ["xcrun", tool] if shutil.which("xcrun") else [tool]


def run(command: list[str], cwd: Path, env: dict[str, str] | None = None) -> subprocess.CompletedProcess:
    return subprocess.run(command, cwd=cwd, env=env, capture_output=True, text=True)


def copy_project(destination: Path) -> None:
    ignore = shutil.ignore_patterns("build", ".git", "docs", "*.png", "*.icns", "*.ico")
    shutil.copytree(ROOT, destination, ignore=ignore, dirs_exist_ok=True)


def configure(project: Path) -> None:
    result = run(["cmake", "-S", ".", "-B", "build", "-G", "Ninja", "-DCMAKE_BUILD_TYPE=Debug",
                  "-DTRESENRAYA_BUILD_APP=OFF", "-DTRESENRAYA_ENABLE_COVERAGE=ON"], project)
    if result.returncode != 0:
        sys.exit(f"Error al configurar:\n{result.stdout}\n{result.stderr}")


def build(project: Path, targets: tuple[str, ...]) -> bool:
    return run(["cmake", "--build", "build", "--target", *targets], project).returncode == 0


def test_functions(binary: Path, project: Path) -> list[str]:
    listing = run([str(binary), "-functions"], project).stdout
    return [line.strip().removesuffix("()") for line in listing.splitlines() if line.strip()]


def run_suite(project: Path, suite: Suite, profile_dir: Path | None = None) -> SuiteRun:
    """Ejecuta cada función de prueba en su propio proceso.

    Así, si una prueba aborta el ejecutable (por ejemplo, por una excepción no
    capturada), no oculta los resultados ni la cobertura de las demás.
    """
    if not build(project, suite.targets):
        return SuiteRun(compiled=False)
    outcome = SuiteRun(compiled=True)
    for target in suite.targets:
        binary = project / "build" / "tests" / target
        for function in test_functions(binary, project):
            env = dict(os.environ)
            if profile_dir is not None:
                env["LLVM_PROFILE_FILE"] = str(profile_dir / f"{target}-{function}-%p.profraw")
            result = run([str(binary), function], project, env)
            reported = RESULT_LINE.findall(result.stdout)
            for status, name, tag in reported:
                if name in ("initTestCase", "cleanupTestCase"):
                    continue
                case = f"{target}::{name}({tag})"
                (outcome.passed if status in ("PASS", "XFAIL") else outcome.failed).add(case)
            if result.returncode != 0 and not any(status == "FAIL!" for status, _, _ in reported):
                outcome.failed.add(f"{target}::{function}(<terminó de forma anormal>)")
    return outcome


def coverage(project: Path, suite: Suite, profile_dir: Path) -> dict[str, float]:
    merged = profile_dir / f"{suite.key}.profdata"
    profiles = sorted(str(p) for p in profile_dir.glob("*.profraw")
                      if p.name.split("-")[0] in suite.targets and p.stat().st_size > 0)
    run([*llvm("llvm-profdata"), "merge", "-sparse", *profiles, "-o", str(merged)], project)
    binaries = [str(project / "build" / "tests" / t) for t in suite.targets]
    objects = [binaries[0]] + [arg for b in binaries[1:] for arg in ("-object", b)]
    result = run([*llvm("llvm-cov"), "export", *objects, "-instr-profile", str(merged),
                  "-summary-only", "src/core"], project)
    totals = json.loads(result.stdout)["data"][0]["totals"]
    return {kind: round(totals[kind]["percent"], 1) for kind in ("lines", "branches", "regions")}


def static_metrics(suite: Suite) -> dict[str, int]:
    text = "\n".join((ROOT / source).read_text(encoding="utf-8") for source in suite.sources)
    code_lines = [line for line in text.splitlines()
                  if line.strip() and not line.strip().startswith("//")]
    return {"assertions": len(ASSERTION.findall(text)), "lines": len(code_lines)}


def apply(project: Path, mutant: Mutant) -> str:
    path = project / mutant.file
    original = path.read_text(encoding="utf-8")
    if original.count(mutant.original) != 1:
        sys.exit(f"{mutant.key}: el fragmento original no aparece exactamente una vez")
    path.write_text(original.replace(mutant.original, mutant.mutated), encoding="utf-8")
    return original


def latex_escape(text: str) -> str:
    return text.replace("&", r"\&").replace("%", r"\%").replace("_", r"\_").replace("#", r"\#")


def write_reports(results: dict) -> None:
    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)
    (OUTPUT_DIR / "resultados.json").write_text(
        json.dumps(results, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")

    suites = results["suites"]
    total = len(results["mutants"])

    rows = [
        ("Casos ejecutados", lambda s: str(s["cases"])),
        ("Casos que fallan sin mutantes", lambda s: str(s["baseline_failures"])),
        ("Aserciones", lambda s: str(s["assertions"])),
        ("Líneas de código de prueba", lambda s: str(s["test_lines"])),
        ("Cobertura de líneas (dominio)", lambda s: f'{s["coverage"]["lines"]:.1f}\\,\\%'),
        ("Cobertura de ramas (dominio)", lambda s: f'{s["coverage"]["branches"]:.1f}\\,\\%'),
        ("Mutantes detectados", lambda s: f'{s["killed"]} de {total}'),
        ("Puntaje de mutación", lambda s: f'{s["mutation_score"]:.0f}\\,\\%'),
    ]
    tex = ["% Archivo generado por tools/compare_suites.py: no editar a mano.",
           r"\begin{tabular}{l" + "c" * len(suites) + "}", r"\toprule",
           "Métrica & " + " & ".join(r"\textbf{" + s["name"] + "}" for s in suites) + r" \\",
           r"\midrule"]
    tex += [f"{label} & " + " & ".join(value(s) for s in suites) + r" \\" for label, value in rows]
    tex += [r"\bottomrule", r"\end{tabular}", ""]
    (OUTPUT_DIR / "resumen.tex").write_text("\n".join(tex), encoding="utf-8")

    tex = ["% Archivo generado por tools/compare_suites.py: no editar a mano.",
           r"\begin{tabular}{llp{5.6cm}" + "c" * len(suites) + "}", r"\toprule",
           "& Req. & Error introducido & " + " & ".join(r"\textbf{" + s["short"] + "}" for s in suites)
           + r" \\", r"\midrule"]
    for mutant in results["mutants"]:
        marks = [r"\killed" if mutant["killed_by"][s["key"]] else r"\survived" for s in suites]
        tex.append(f'{mutant["key"]} & {mutant["requirement"]} & {latex_escape(mutant["description"])} & '
                   + " & ".join(marks) + r" \\")
    tex += [r"\bottomrule", r"\end{tabular}", ""]
    (OUTPUT_DIR / "mutantes.tex").write_text("\n".join(tex), encoding="utf-8")

    md = ["# Resultados de la comparación", "",
          "Generado por `tools/compare_suites.py`. Cobertura medida sobre `src/core`.", "",
          "| Métrica | " + " | ".join(s["name"] for s in suites) + " |",
          "| --- |" + " --- |" * len(suites)]
    for label, value in rows:
        md.append(f"| {label} | " + " | ".join(value(s).replace("\\,\\%", " %") for s in suites) + " |")
    md += ["", "## Mutantes", "",
           "| Mutante | Req. | Error introducido | " + " | ".join(s["name"] for s in suites) + " |",
           "| --- | --- | --- |" + " --- |" * len(suites)]
    for mutant in results["mutants"]:
        marks = ["detectado" if mutant["killed_by"][s["key"]] else "**sobrevive**" for s in suites]
        md.append(f'| {mutant["key"]} | {mutant["requirement"]} | {mutant["description"]} | '
                  + " | ".join(marks) + " |")
    md += ["", "## Pruebas que fallan sin mutantes", ""]
    for suite in suites:
        for name in suite["baseline_failed_tests"]:
            md.append(f"- {suite['name']}: `{name}`")
    (OUTPUT_DIR / "resumen.md").write_text("\n".join(md) + "\n", encoding="utf-8")


def main() -> None:
    short_names = {"repositorio": "Repo", "ia_basico": "IA básica", "ia_estructurado": "IA estruct."}
    with tempfile.TemporaryDirectory(prefix="tresenraya-mutacion-") as temp:
        project = Path(temp) / "proyecto"
        copy_project(project)
        print("Configurando copia temporal…", flush=True)
        configure(project)

        profile_dir = Path(temp) / "perfiles"
        profile_dir.mkdir()
        baseline: dict[str, SuiteRun] = {}
        suite_results = []
        for suite in SUITES:
            outcome = run_suite(project, suite, profile_dir)
            if not outcome.compiled:
                sys.exit(f"La suite {suite.name} no compila sobre el código original")
            baseline[suite.key] = outcome
            suite_results.append({
                "key": suite.key, "name": suite.name, "short": short_names[suite.key],
                "cases": len(outcome.passed) + len(outcome.failed),
                "baseline_failures": len(outcome.failed),
                "baseline_failed_tests": sorted(outcome.failed),
                "coverage": coverage(project, suite, profile_dir),
                **{("test_lines" if k == "lines" else k): v for k, v in static_metrics(suite).items()},
            })
            print(f"  {suite.name}: {suite_results[-1]['cases']} casos, "
                  f"{len(outcome.failed)} fallan, cobertura {suite_results[-1]['coverage']}", flush=True)

        mutant_results = []
        for mutant in MUTANTS:
            original = apply(project, mutant)
            killed_by = {}
            for suite in SUITES:
                outcome = run_suite(project, suite)
                newly_failing = outcome.failed - baseline[suite.key].failed
                killed_by[suite.key] = (not outcome.compiled) or bool(newly_failing)
            (project / mutant.file).write_text(original, encoding="utf-8")
            mutant_results.append({**mutant.__dict__, "killed_by": killed_by})
            marks = " ".join("✔" if killed_by[s.key] else "✘" for s in SUITES)
            print(f"  {mutant.key:>3} {marks}  {mutant.description}", flush=True)

        for suite in suite_results:
            killed = sum(m["killed_by"][suite["key"]] for m in mutant_results)
            suite["killed"] = killed
            suite["mutation_score"] = 100.0 * killed / len(mutant_results)

        write_reports({"suites": suite_results, "mutants": mutant_results})
        print(f"Reportes escritos en {OUTPUT_DIR.relative_to(ROOT)}/")


if __name__ == "__main__":
    main()
