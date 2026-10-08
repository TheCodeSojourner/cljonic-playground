#!/usr/bin/env python3
"""Check that planned Allium obligation IDs have matching test TRACE_IDs."""

from __future__ import annotations

import argparse
import json
import re
import subprocess
import sys
from pathlib import Path

TRACE_RE = re.compile(r'TRACE_ID\((.*?)\)', re.DOTALL)
STRING_LITERAL_RE = re.compile(r'"([^"\\]*(?:\\.[^"\\]*)*)"')


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("spec", type=Path, nargs="?", help="Allium specification file")
    parser.add_argument(
        "tests",
        type=Path,
        nargs="*",
        help="test source files or directories containing TRACE_ID calls",
    )
    parser.add_argument("--exact", action="store_true", help="also reject trace IDs unrelated to this spec")
    parser.add_argument("--self-test", action="store_true", help="run parser regression checks without invoking Allium")
    return parser.parse_args()


def extract_trace_ids(source: str) -> set[str]:
    source = re.sub(r"^[ \t]*#\s*define\s+TRACE_ID\b[^\n]*", "", source, flags=re.MULTILINE)
    ids: set[str] = set()
    for call in TRACE_RE.findall(source):
        for literal in STRING_LITERAL_RE.findall(call):
            if re.search(r"\\(?:[0-7]{1,3}|x[0-9A-Fa-f]+|u[0-9A-Fa-f]{4}|U[0-9A-Fa-f]{8})", literal):
                continue
            literal = re.sub(r"\\\\([\\\"'])", r"\1", literal)
            ids.update(re.findall(r"(?:entity-fields\.[A-Za-z0-9_]+|invariant\.[A-Za-z0-9_]+\.[A-Za-z0-9_]+)", literal))
    return ids


def run_self_test() -> list[str]:
    problems = []
    if resolve_test_files([]) != []:
        problems.append("empty test input unexpectedly resolved files")
    source = '#define TRACE_ID(id) INFO("trace-id: " id)\nTRACE_ID("entity-fields.Example");\n'
    ids = extract_trace_ids(source)
    if ids != {"entity-fields.Example"}:
        problems.append("literal TRACE_ID was not extracted")
    concatenated = 'TRACE_ID("invariant.Example.One" "invariant.Example.Two");'
    if extract_trace_ids(concatenated) != {"invariant.Example.One", "invariant.Example.Two"}:
        problems.append("adjacent TRACE_ID string literals were not extracted")
    escaped = r'TRACE_ID("invariant.Example.Escaped\x2eName");'
    if extract_trace_ids(escaped) != set():
        problems.append("unsupported escaped C++ string literal was not rejected safely")
    if extract_trace_ids(r'TRACE_ID("invariant.Example.Not\x2eAnId");'):
        problems.append("hex escapes were incorrectly decoded into a trace ID")
    return problems


def resolve_test_files(paths: list[Path]) -> list[Path]:
    files: set[Path] = set()
    for path in paths:
        if path.is_dir():
            files.update(item for item in path.rglob("*") if item.suffix in {".cpp", ".cc"})
        elif path.is_file() and path.suffix in {".cpp", ".cc"}:
            files.add(path)
        else:
            raise ValueError(f"not a test source file or directory: {path}")
    return sorted(files)


def main() -> int:
    args = parse_args()
    if args.self_test:
        failures = run_self_test()
        if failures:
            print("spec-test-trace-ids:self-test-fail")
            print("\n".join(f"  {failure}" for failure in failures))
            return 1
        print("spec-test-trace-ids:self-test:ok")
        return 0

    if args.spec is None or not args.spec.is_file():
        print(f"spec-test-trace-ids: spec file not found: {args.spec}", file=sys.stderr)
        return 2
    try:
        test_files = resolve_test_files(args.tests)
    except ValueError as error:
        print(f"spec-test-trace-ids: {error}", file=sys.stderr)
        return 2
    if not test_files:
        print("spec-test-trace-ids: no test source files found", file=sys.stderr)
        return 2

    try:
        result = subprocess.run(
            ["allium", "plan", str(args.spec)],
            check=False,
            capture_output=True,
            text=True,
        )
    except OSError as error:
        print(f"spec-test-trace-ids: cannot run allium: {error}", file=sys.stderr)
        return 2
    if result.returncode:
        print(result.stderr or result.stdout, file=sys.stderr)
        return result.returncode

    try:
        obligations = json.loads(result.stdout).get("obligations", [])
    except (ValueError, TypeError) as error:
        print(f"spec-test-trace-ids: invalid Allium plan output: {error}", file=sys.stderr)
        return 2

    planned = {item["id"] for item in obligations if isinstance(item, dict) and "id" in item}
    traces: set[str] = set()
    for test_path in test_files:
        traces.update(extract_trace_ids(test_path.read_text(encoding="utf-8")))
    missing = sorted(planned - traces)
    extra = sorted(traces - planned)

    if missing:
        print("spec-test-trace-ids:missing=" + ", ".join(missing))
        return 1
    if args.exact and extra:
        print("spec-test-trace-ids:extra=" + ", ".join(extra))
        return 1

    print(f"spec-test-trace-ids:ok obligations={len(planned)} traces={len(traces)} tests={len(test_files)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
