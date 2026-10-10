#!/usr/bin/env python3
"""Verify conj rejection diagnostics for both public header forms."""

from __future__ import annotations

import os
import re
import subprocess

PREAMBLE = """
struct ConjNotConvertible {};
struct ConjNonConjableCollection {};

namespace cljonic::concepts_detail {
template <>
struct collection_traits<ConjNonConjableCollection> {
    static constexpr bool is_cljonic_collection = true;
    static constexpr collection_kind kind = collection_kind::none;
};
} // namespace cljonic::concepts_detail
"""

DIAGNOSTIC_CASES = {
    "non-conjable-cljonic-collection": (
        "(void)cljonic::conj(ConjNonConjableCollection{}, 1);",
        "must be a Vector, a Set, a Map, a Queue, or a String",
    ),
    "inadmissible-vector-value": (
        "(void)cljonic::conj(cljonic::Vector<int, 4>{}, ConjNotConvertible{});",
        "cannot be added to the collection with conj",
    ),
    "inadmissible-set-value": (
        "(void)cljonic::conj(cljonic::Set<int, 4>{}, ConjNotConvertible{});",
        "cannot be added to the collection with conj",
    ),
    "inadmissible-map-value": (
        "(void)cljonic::conj(cljonic::Map<int, int, 4>{}, 1);",
        "A Map takes a MapEntry",
    ),
    "inadmissible-string-value": (
        "(void)cljonic::conj(cljonic::String<4>{}, ConjNotConvertible{});",
        "cannot be added to the collection with conj",
    ),
    "non-conjable-cljonic-collection-zero-value": (
        "(void)cljonic::conj(ConjNonConjableCollection{});",
        "must be a Vector, a Set, a Map, a Queue, or a String",
    ),
    "non-conjable-cljonic-collection-variadic": (
        "(void)cljonic::conj(ConjNonConjableCollection{}, 1, 2);",
        "must be a Vector, a Set, a Map, a Queue, or a String",
    ),
    "inadmissible-variadic-vector-value": (
        "(void)cljonic::conj(cljonic::Vector<int, 4>{}, 1, ConjNotConvertible{});",
        "every value after the collection must be one this collection can add",
    ),
    "inadmissible-variadic-map-value": (
        "(void)cljonic::conj(cljonic::Map<int, int, 4>{}, cljonic::MapEntry<int, int>{1, 2}, 3);",
        "every value after the collection must be one this collection can add",
    ),
}

PASS_CASES = {
    "vector": "(void)cljonic::conj(cljonic::Vector<int, 4>{}, 1);",
    "set": "(void)cljonic::conj(cljonic::Set<int, 4>{}, 1);",
    "map-entry": "(void)cljonic::conj(cljonic::Map<int, int, 4>{}, cljonic::MapEntry<int, int>{1, 2});",
    "queue": "(void)cljonic::conj(cljonic::Queue<int, 4>{}, 1);",
    "string": "(void)cljonic::conj(cljonic::String<4>{}, 'A');",
    "zero-value-vector": "(void)cljonic::conj(cljonic::Vector<int, 4>{});",
    "zero-value-map": "(void)cljonic::conj(cljonic::Map<int, int, 4>{});",
    "variadic-vector": "(void)cljonic::conj(cljonic::Vector<int, 4>{}, 1, 2, 3);",
    "variadic-set": "(void)cljonic::conj(cljonic::Set<int, 4>{}, 1, 2);",
    "variadic-queue": "(void)cljonic::conj(cljonic::Queue<int, 4>{}, 1, 2);",
    "variadic-string": "(void)cljonic::conj(cljonic::String<4>{}, 'A', 'B');",
    "variadic-map-entry": (
        "(void)cljonic::conj(cljonic::Map<int, int, 4>{}, cljonic::MapEntry<int, int>{1, 2}, "
        "cljonic::MapEntry<int, int>{3, 4});"
    ),
}

DIAGNOSTIC_ANCHOR = "cljonic::conj:"


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


def normalized(text: str) -> str:
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
        for name, (body, message) in DIAGNOSTIC_CASES.items():
            result = compile_source(compiler, include_dir, build_source(header, body))
            if result.returncode == 0:
                failures.append(f"{configuration}/{name}: expected compile failure, but compiled")
                continue
            output = normalized(result.stdout + result.stderr)
            if DIAGNOSTIC_ANCHOR not in output or normalized(message) not in output:
                failures.append(f"{configuration}/{name}: targeted rejection diagnostic content not reported")

        for name, body in PASS_CASES.items():
            result = compile_source(compiler, include_dir, build_source(header, body))
            if result.returncode != 0:
                failures.append(f"{configuration}/{name}: expected compilation, but failed")

    if failures:
        print("conj-compile-fail:failures:")
        print("\n".join(f"  {failure}" for failure in failures))
        return 1

    print("conj-compile-fail:ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())