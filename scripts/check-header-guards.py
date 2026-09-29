#!/usr/bin/env python3
"""Verify that every public and test header uses `#pragma once`.

The repository standard is `#pragma once` for all `src/**/*.hpp` and
`tests/**/*.hpp` headers (the generated single header `cljonic.hpp` is produced
from these and is not scanned directly). Include guards (`#ifndef CLJONIC_*_HPP`
/ `#define` / `#endif // CLJONIC_*_HPP`) are drift: they are a class of bug
(mismatched or missing `#endif`) that `#pragma once` cannot have, and they make
the public surface inconsistent.
"""

from __future__ import annotations

import pathlib
import re
import sys

ROOTS = ("src", "tests")

GUARD_IFNDEF_RE = re.compile(r"^#ifndef\s+CLJONIC_[A-Z0-9_]*_HPP\s*$")
GUARD_DEFINE_RE = re.compile(r"^#define\s+CLJONIC_[A-Z0-9_]*_HPP\s*$")
GUARD_ENDIF_RE = re.compile(r"^#endif\s+//\s*CLJONIC_[A-Z0-9_]*_HPP\s*$")


def header_files() -> list[pathlib.Path]:
    files: list[pathlib.Path] = []
    for root in ROOTS:
        base = pathlib.Path(root)
        if base.is_dir():
            files.extend(sorted(base.rglob("*.hpp")))
    return files


def main() -> int:
    failures: list[str] = []
    for path in header_files():
        lines = path.read_text(encoding="utf-8").splitlines()

        first_directive = next((line for line in lines if line.strip()), "")
        if first_directive != "#pragma once":
            failures.append(f"{path}: first line must be '#pragma once' (found: {first_directive!r})")

        for line_number, line in enumerate(lines, start=1):
            if GUARD_IFNDEF_RE.match(line):
                failures.append(f"{path}:{line_number}: include-guard '#ifndef' found; use '#pragma once'")
            elif GUARD_DEFINE_RE.match(line):
                failures.append(f"{path}:{line_number}: include-guard '#define' found; use '#pragma once'")
            elif GUARD_ENDIF_RE.match(line):
                failures.append(f"{path}:{line_number}: include-guard '#endif // ...' found; use '#pragma once'")

    if failures:
        print("header-guards:failures:", file=sys.stderr)
        print("\n".join(f"  {failure}" for failure in failures), file=sys.stderr)
        return 1

    print("header-guards:ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
