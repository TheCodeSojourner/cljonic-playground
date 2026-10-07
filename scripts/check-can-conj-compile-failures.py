#!/usr/bin/env python3
"""Verify can_conj rejection diagnostics for both public header forms."""

from __future__ import annotations

import os
import re
import subprocess

PREAMBLE = """
struct CanConjNotConvertible {};
struct CanConjNonConjableCollection {};

namespace cljonic::concepts_detail {
template <>
struct collection_traits<CanConjNonConjableCollection> {
    static constexpr bool is_cljonic_collection = true;
    static constexpr collection_kind kind = collection_kind::none;
};
} // namespace cljonic::concepts_detail
"""

DIAGNOSTIC_CASES = {
    "non-conjable-cljonic-collection": (
        "(void)cljonic::can_conj(CanConjNonConjableCollection{}, 1);",
        "first argument must be a Conjable collection",
    ),
    "inadmissible-vector-value": (
        "(void)cljonic::can_conj(cljonic::Vector<int, 4>{}, CanConjNotConvertible{});",
        "value must be admissible for the collection's conj operation",
    ),
    "inadmissible-map-value": (
        "(void)cljonic::can_conj(cljonic::Map<int, int, 4>{}, 1);",
        "value must be admissible for the collection's conj operation",
    ),
    "inadmissible-string-value": (
        "(void)cljonic::can_conj(cljonic::String<4>{}, CanConjNotConvertible{});",
        "value must be admissible for the collection's conj operation",
    ),
}

PASS_CASES = {
    "vector": "(void)cljonic::can_conj(cljonic::Vector<int, 4>{}, 1);",
    "map-entry": "(void)cljonic::can_conj(cljonic::Map<int, int, 4>{}, cljonic::MapEntry<int, int>{1, 2});",
    "string": "(void)cljonic::can_conj(cljonic::String<4>{}, 'A');",
}

DIAGNOSTIC_ANCHOR = "cljonic::can_conj:"


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
        print("can-conj-compile-fail:failures:")
        print("\n".join(f"  {failure}" for failure in failures))
        return 1

    print("can-conj-compile-fail:ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())