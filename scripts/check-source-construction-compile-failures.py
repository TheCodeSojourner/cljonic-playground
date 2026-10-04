#!/usr/bin/env python3
"""Verify the REQ-FN-027A source-construction admission contract.

A cljonic collection or producer is NOT a direct source constructor argument:
only a non-cljonic C++ range/view imports into a collection's source
constructor. A cljonic value that is not exactly the destination's element type
must fail to compile (materialization is the role of into/fits_into). A sole
argument in a class-template-argument-deduction form is enclosed as one element.

Positive controls guard against a trivially-passing harness.
"""

from __future__ import annotations

import os
import subprocess

# Every rejected form must fail with the REQ-DIAG-010 targeted diagnostic
# (content-asserted, whitespace-normalized) rather than a generic element
# conversion error.
REQUIRED_MESSAGE_ANCHOR = "it is never a source to materialize"

# Each entry must FAIL to compile (rejected: treated as a non-convertible
# element, not a source to materialize).
FAILURE_CASES = {
    "vector-from-vector": "  cljonic::Vector<int, 4> value{cljonic::Vector<int, 2>{1, 2}}; (void)value;",
    "vector-from-producer": "  cljonic::Vector<int, 4> value{cljonic::Range<int>{0, 10, 1}}; (void)value;",
    "set-from-set": "  cljonic::Set<int, 4> value{cljonic::Set<int, 2>{1, 2}}; (void)value;",
    "set-from-producer": "  cljonic::Set<int, 4> value{cljonic::Range<int>{0, 10, 1}}; (void)value;",
    "queue-from-queue": "  cljonic::Queue<int, 4> value{cljonic::Queue<int, 2>{1, 2}}; (void)value;",
    "queue-from-producer": "  cljonic::Queue<int, 4> value{cljonic::Repeat<int>{7, 2U}}; (void)value;",
    "map-from-map": ("  cljonic::Map<int, int, 4> value{cljonic::Map<int, int, 2>"
                     "{cljonic::MapEntry<int, int>{1, 2}}}; (void)value;"),
    "map-from-entry-vector": ("  cljonic::Map<int, int, 4> value{cljonic::Vector<cljonic::MapEntry<int, int>, 2>"
                              "{cljonic::MapEntry<int, int>{1, 2}}}; (void)value;"),
    "string-from-string": "  cljonic::String<4> value{cljonic::String<2>{\"ab\"}}; (void)value;",
    "string-from-producer": "  cljonic::String<4> value{cljonic::Repeat<char>{'a', 2U}}; (void)value;",
}

# Each entry must COMPILE (enclosure and non-cljonic C++ interop sources).
POSITIVE_CASES = {
    "enclosure-vector": "  static_assert(cljonic::Vector{cljonic::Vector<int, 2>{1, 2}}.count() == 1U);",
    "enclosure-set": "  static_assert(cljonic::Set{cljonic::Set<int, 2>{1, 2}}.count() == 1U);",
    "enclosure-queue": "  static_assert(cljonic::Queue{cljonic::Queue<int, 2>{1, 2}}.count() == 1U);",
    "enclosure-cross-class": "  static_assert(cljonic::Set{cljonic::Range<int>{0, 5, 1}}.count() == 1U);",
    "interop-span": ("  static constexpr int values[2] = {1, 2};\n"
                     "  cljonic::Vector<int, 4> value{std::span<const int>{values}}; (void)value;"),
}


def compile_snippet(compiler: list[str], include_dir: str, header: str, body: str) -> tuple[bool, str]:
    source = "\n".join(
        (
            "#include <span>",
            f'#include "{header}"',
            "int main() {",
            body,
            "  return 0;",
            "}",
        )
    )
    result = subprocess.run(
        [*compiler, "-std=c++23", "-fsyntax-only", "-I", include_dir, "-x", "c++", "-"],
        input=source,
        text=True,
        capture_output=True,
        check=False,
    )
    return result.returncode == 0, result.stderr


def main() -> int:
    compiler = os.environ.get("CXX", "c++").split()
    root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    configurations = (
        ("modular", os.path.join(root, "src"), "cljonic-core.hpp"),
        ("single-header", root, "cljonic.hpp"),
    )
    failures: list[str] = []

    for configuration, include_dir, header in configurations:
        for name, body in FAILURE_CASES.items():
            compiled, stderr = compile_snippet(compiler, include_dir, header, body)
            if compiled:
                failures.append(f"{configuration}/{name} (expected rejection, compiled)")
            elif REQUIRED_MESSAGE_ANCHOR not in " ".join(stderr.split()):
                failures.append(f"{configuration}/{name} (rejected without the targeted diagnostic)")
        for name, body in POSITIVE_CASES.items():
            compiled, _ = compile_snippet(compiler, include_dir, header, body)
            if not compiled:
                failures.append(f"{configuration}/{name} (expected success, failed)")

    if failures:
        print("source-construction-compile-fail:missing expected behavior:")
        print("\n".join(f"  {failure}" for failure in failures))
        return 1

    print("source-construction-compile-fail:ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
