#!/usr/bin/env python3
"""Audit requirement designators, attribution footers, and rationale provenance."""

from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path

CLAUSE_RE = re.compile(r"^λ\s+(REQ-[A-Z]+-\d+[A-Z]?)\(x\)\.$", re.MULTILINE)
FOOTER_RE = re.compile(r"^\s*\{([^\n]+)\}\s*$", re.MULTILINE)
RATIONALE_RE = re.compile(r"^\s*rationale:", re.MULTILINE)
VALID_SOURCES = {"stakeholder_decided", "AI_researched_fact"}
VALID_RATIONALE_SOURCES = {"origin_artifact", "AI_inferred"}


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "paths",
        nargs="*",
        type=Path,
        help="requirements module files (default: requirements/requirements-module-*.md)",
    )
    parser.add_argument("--self-test", action="store_true", help="run parser regression checks without reading repository files")
    parser.add_argument(
        "--index",
        type=Path,
        default=Path("requirements/requirements-index.md"),
        help="index declaring allowed domain prefixes (default: %(default)s)",
    )
    return parser.parse_args()


def declared_domains(index_path: Path) -> set[str]:
    text = index_path.read_text(encoding="utf-8")
    match = re.search(r"^## Domain Prefixes \(Closed Set\)\s*$([\s\S]*?)(?=^## |\Z)", text, re.MULTILINE)
    if match is None:
        raise ValueError(f"{index_path}: missing Domain Prefixes (Closed Set) section")
    return set(re.findall(r"REQ-([A-Z]+)", match.group(1)))


def validate_module(text: str, path: str, allowed_domains: set[str]) -> tuple[list[tuple[str, str]], list[str]]:
    clauses = list(CLAUSE_RE.finditer(text))
    locations: list[tuple[str, str]] = []
    failures: list[str] = []

    for index, clause in enumerate(clauses):
        designator = clause.group(1)
        end = clauses[index + 1].start() if index + 1 < len(clauses) else len(text)
        body = text[clause.end() : end]
        locations.append((designator, path))

        domain = designator.split("-")[1]
        if domain not in allowed_domains:
            failures.append(f"{path}: {designator}: domain prefix is not declared")

        footer = FOOTER_RE.search(body)
        if footer is None:
            failures.append(f"{path}: {designator}: missing attribution footer")
            continue

        fields = dict(
            (key.strip(), value.strip())
            for key, value in re.findall(r"([\w-]+)\s*:\s*([^,}]+)", footer.group(1))
        )
        if fields.get("source") not in VALID_SOURCES:
            failures.append(f"{path}: {designator}: invalid or missing source attribution")
        if not fields.get("decided_by"):
            failures.append(f"{path}: {designator}: missing decided_by attribution")
        if RATIONALE_RE.search(body):
            if fields.get("rationale_source") not in VALID_RATIONALE_SOURCES:
                failures.append(f"{path}: {designator}: rationale lacks valid rationale_source")

    return locations, failures


def run_self_test() -> list[str]:
    sample = "λ REQ-TEST-001(x).\n  ∀ sample: true\n  {source: stakeholder_decided, decided_by: tester}\n"
    locations, failures = validate_module(sample, "sample.md", {"TEST"})
    problems = []
    if failures or locations != [("REQ-TEST-001", "sample.md")]:
        problems.append("valid sample requirement was not parsed")

    if not any("domain prefix" in item for item in validate_module(sample, "sample.md", {"OTHER"})[1]):
        problems.append("undeclared domain was not rejected")

    missing_footer = "λ REQ-TEST-001(x).\n  ∀ sample: true\n"
    if not any("missing attribution footer" in item for item in validate_module(missing_footer, "sample.md", {"TEST"})[1]):
        problems.append("missing footer was not rejected")

    bad_rationale = sample.replace("∀ sample: true", "∀ sample: true\n  rationale: test")
    if not any("rationale lacks" in item for item in validate_module(bad_rationale, "sample.md", {"TEST"})[1]):
        problems.append("rationale without provenance was not rejected")
    return problems


def main() -> int:
    args = parse_args()
    if args.self_test:
        failures = run_self_test()
        if failures:
            print("requirements-structure:self-test-fail")
            print("\n".join(f"  {failure}" for failure in failures))
            return 1
        print("requirements-structure:self-test:ok")
        return 0
    paths = args.paths or sorted(Path("requirements").glob("requirements-module-*.md"))
    if not paths or any(not path.is_file() for path in paths):
        print("requirements-structure: input file not found", file=sys.stderr)
        return 2
    if not args.index.is_file():
        print(f"requirements-structure: index file not found: {args.index}", file=sys.stderr)
        return 2
    try:
        allowed_domains = declared_domains(args.index)
    except ValueError as error:
        print(f"requirements-structure: {error}", file=sys.stderr)
        return 2

    locations: dict[str, list[str]] = {}
    failures: list[str] = []
    clause_count = 0

    for path in paths:
        module_locations, module_failures = validate_module(
            path.read_text(encoding="utf-8"), str(path), allowed_domains
        )
        clause_count += len(module_locations)
        failures.extend(module_failures)
        for designator, location in module_locations:
            locations.setdefault(designator, []).append(location)

    for designator, files in sorted(locations.items()):
        if len(files) > 1:
            failures.append(f"{designator}: duplicate designator in {', '.join(files)}")

    if failures:
        print("requirements-structure:fail")
        print("\n".join(f"  {failure}" for failure in failures))
        return 1

    print(f"requirements-structure:ok clauses={clause_count} unique={len(locations)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
