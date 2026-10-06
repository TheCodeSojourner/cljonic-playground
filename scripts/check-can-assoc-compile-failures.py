#!/usr/bin/env python3
"""Verify that `can_assoc` rejects out-of-domain calls at compile time with a
targeted diagnostic (REQ-DIAG-009) and admits the supported domain, for both
modular and single-header builds.

Covers the CanAssoc spec obligations and
RejectionDiagnostic.AppliesToCollectionPrimitives (REQ-DIAG-009).
"""

from __future__ import annotations

import os
import re
import subprocess

PREAMBLE = """
struct CanAssocNotConvertible {};
"""

# name -> (body, case-specific required message substrings)
DIAGNOSTIC_CASES = {
    "CanAssoc-non-associative-collection": (
        "(void)cljonic::can_assoc(cljonic::Set<int, 4>{}, 1);",
        ("must be an associative collection", "Set and Queue are not associative"),
    ),
    "CanAssoc-inadmissible-key-vector": (
        "(void)cljonic::can_assoc(cljonic::Vector<int, 4>{}, CanAssocNotConvertible{});",
        ("outside this collection's association domain", "integer index"),
    ),
    "CanAssoc-inadmissible-key-map": (
        "(void)cljonic::can_assoc(cljonic::Map<int, int, 4>{}, CanAssocNotConvertible{});",
        ("outside this collection's association domain", "Map takes a key"),
    ),
}

PASS_CASES = {
    "CanAssoc-map": "(void)cljonic::can_assoc(cljonic::Map<int, int, 4>{}, 1);",
    "CanAssoc-vector": "(void)cljonic::can_assoc(cljonic::Vector<int, 4>{10, 20}, 0U);",
    "CanAssoc-vector-negative-index": "(void)cljonic::can_assoc(cljonic::Vector<int, 4>{10, 20}, -1);",
    "CanAssoc-string": "(void)cljonic::can_assoc(cljonic::String<8>{\"ab\"}, 2U);",
}

DIAGNOSTIC_ANCHOR = "cljonic::can_assoc:"


def build_source(header: str, body: str) -> str:
    return "\n".join(
        (
            f'#include "{header}"',
            PREAMBLE,
            "int main() {",
            f"  {body}",
            "  return 0;",
            "}",
        )
    )


def compile_source(compiler: list[str], include_dir: str, source: str) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        [*compiler, "-std=c++23", "-fsyntax-only", "-I", include_dir, "-x", "c++", "-"],
        input=source,
        text=True,
        capture_output=True,
        check=False,
    )


def _normalized(text: str) -> str:
    return re.sub(r"\s+", " ", text)


def main() -> int:
    compiler = os.environ.get("CXX", "c++").split()
    root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    configurations = (
        ("modular", os.path.join(root, "src"), "cljonic-core.hpp"),
        ("single-header", root, "cljonic.hpp"),
    )
    failures: list[str] = []

    for configuration, include_dir, header in configurations:
        for name, (body, required) in DIAGNOSTIC_CASES.items():
            result = compile_source(compiler, include_dir, build_source(header, body))
            if result.returncode == 0:
                failures.append(f"{configuration}/{name}: expected compile failure, but compiled")
                continue
            output = _normalized(result.stdout + result.stderr)
            if not all(_normalized(needle) in output for needle in (DIAGNOSTIC_ANCHOR, *required)):
                failures.append(f"{configuration}/{name}: targeted rejection diagnostic content not reported")

        for name, body in PASS_CASES.items():
            result = compile_source(compiler, include_dir, build_source(header, body))
            if result.returncode != 0:
                failures.append(f"{configuration}/{name}: expected to compile, but failed")

    if failures:
        print("can-assoc-compile-fail:failures:")
        print("\n".join(f"  {failure}" for failure in failures))
        return 1

    print("can-assoc-compile-fail:ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
