#!/usr/bin/env python3
"""Verify that `assoc` rejects out-of-domain calls at compile time with a
targeted diagnostic (REQ-DIAG-009) and admits the supported domain, for both
modular and single-header builds.

Covers the Assoc / RejectionDiagnostic spec obligations:
- Assoc.SupportsVariadicAssoc and the variadic fold (REQ-FN-002U)
- RejectionDiagnostic.AppliesToCollectionPrimitives (REQ-DIAG-009)
"""

from __future__ import annotations

import os
import re
import subprocess

PREAMBLE = """
struct AssocNotConvertible {};
"""

# name -> (body, case-specific required message substrings)
DIAGNOSTIC_CASES = {
    "Assoc-non-associative-collection": (
        "(void)cljonic::assoc(cljonic::Set<int, 4>{}, 1, 1);",
        ("must be a Map, a Vector, or a String", "is not supported"),
    ),
    "Assoc-inadmissible-pair": (
        "(void)cljonic::assoc(cljonic::Vector<int, 4>{}, AssocNotConvertible{}, 1);",
        ("both have a type this collection cannot associate",),
    ),
    "Assoc-key-without-value": (
        "(void)cljonic::assoc(cljonic::Map<int, int, 4>{}, 1, 100, 2);",
        ("key with no following value", "two or more pairs"),
    ),
    "Assoc-odd-variadic-arity": (
        "(void)cljonic::assoc(cljonic::Map<int, int, 4>{}, 1, 100, 2, 200, 3);",
        ("complete key/value pairs",),
    ),
    "Assoc-bad-variadic-pair": (
        "(void)cljonic::assoc(cljonic::Map<int, int, 4>{}, 1, 100, 2, AssocNotConvertible{}, 3, 3);",
        ("complete key/value pairs", "has a type the collection cannot associate"),
    ),
}

PASS_CASES = {
    "Assoc-single-pair": "(void)cljonic::assoc(cljonic::Map<int, int, 4>{}, 1, 100);",
    "Assoc-variadic-map": "(void)cljonic::assoc(cljonic::Map<int, int, 4>{}, 1, 100, 2, 200);",
    "Assoc-variadic-vector": "(void)cljonic::assoc(cljonic::Vector<int, 4>{10, 20}, 1, 200, 2, 300);",
    "Assoc-variadic-string": "(void)cljonic::assoc(cljonic::String<8>{\"ab\"}, 2U, 'c', 3U, 'd');",
    "Assoc-over-capacity-no-op": "(void)cljonic::assoc(cljonic::Map<int, int, 2>{}, 1, 100, 2, 200, 3, 300);",
}

DIAGNOSTIC_ANCHOR = "cljonic::assoc:"


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
        print("assoc-compile-fail:failures:")
        print("\n".join(f"  {failure}" for failure in failures))
        return 1

    print("assoc-compile-fail:ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
