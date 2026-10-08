#!/usr/bin/env python3
"""Audit vocabulary term metadata, duplicate names, and Related references."""

from __future__ import annotations

import argparse
import re
import sys
from collections import Counter
from pathlib import Path

TERM_RE = re.compile(r"^### (.+)$", re.MULTILINE)
FIELDS = ("Definition", "Deprecated Synonyms", "Related", "Usage", "Examples")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("path", nargs="?", type=Path, default=Path("vocabulary.md"))
    parser.add_argument("--self-test", action="store_true", help="run parser regression checks without reading repository files")
    return parser.parse_args()


def validate_vocabulary(text: str) -> tuple[int, list[str]]:
    headers = list(TERM_RE.finditer(text))
    names = [match.group(1).strip() for match in headers]
    known_names = set(names)
    failures: list[str] = []

    for name, count in sorted(Counter(names).items()):
        if count > 1:
            failures.append(f"duplicate term heading: {name}")

    for index, header in enumerate(headers):
        name = header.group(1).strip()
        end = headers[index + 1].start() if index + 1 < len(headers) else len(text)
        block = text[header.end() : end]
        fields: dict[str, str] = {}
        for field in FIELDS:
            match = re.search(rf"^- \*\*{re.escape(field)}:\*\*\s*(.*)$", block, re.MULTILINE)
            if match is None or not match.group(1).strip():
                failures.append(f"{name}: missing or empty {field} field")
            else:
                fields[field] = match.group(1).strip()

        for related in fields.get("Related", "").split(","):
            related = related.strip()
            if related == name:
                failures.append(f"{name}: Related contains a self-reference")
            elif related and related not in known_names:
                failures.append(f"{name}: Related references unknown term {related!r}")

    return len(names), failures


def run_self_test() -> list[str]:
    valid = "### Alpha\n- **Definition:** A term.\n- **Deprecated Synonyms:** none\n- **Related:** Beta\n- **Usage:** docs\n- **Examples:** example\n\n### Beta\n- **Definition:** Another term.\n- **Deprecated Synonyms:** none\n- **Related:** Alpha\n- **Usage:** docs\n- **Examples:** example\n"
    count, errors = validate_vocabulary(valid)
    problems = []
    if count != 2 or errors:
        problems.append("valid vocabulary sample was rejected")

    duplicate_count, duplicate_errors = validate_vocabulary(valid + "\n### Alpha\n")
    if duplicate_count != 3 or not any("duplicate term heading" in error for error in duplicate_errors):
        problems.append("duplicate term heading was not rejected")

    dangling = valid.replace("Related:** Beta", "Related:** Missing")
    if not any("unknown term" in error for error in validate_vocabulary(dangling)[1]):
        problems.append("dangling Related reference was not rejected")

    self_related = valid.replace("Related:** Beta", "Related:** Alpha")
    if not any("self-reference" in error for error in validate_vocabulary(self_related)[1]):
        problems.append("self Related reference was not rejected")

    missing = valid.replace("- **Usage:** docs\n", "")
    if not any("Usage" in error for error in validate_vocabulary(missing)[1]):
        problems.append("missing metadata field was not rejected")
    return problems


def main() -> int:
    args = parse_args()
    if args.self_test:
        failures = run_self_test()
        if failures:
            print("vocabulary-structure:self-test-fail")
            print("\n".join(f"  {failure}" for failure in failures))
            return 1
        print("vocabulary-structure:self-test:ok")
        return 0

    path = args.path
    if not path.is_file():
        print(f"vocabulary-structure: file not found: {path}", file=sys.stderr)
        return 2

    term_count, failures = validate_vocabulary(path.read_text(encoding="utf-8"))
    if failures:
        print("vocabulary-structure:fail")
        print("\n".join(f"  {failure}" for failure in failures))
        return 1

    print(f"vocabulary-structure:ok terms={term_count}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
