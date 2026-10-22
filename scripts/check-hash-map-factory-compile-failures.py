#!/usr/bin/env python3
"""Verify hash_map factory admission and rejection for modular and single headers."""

from __future__ import annotations

import os
import re
import subprocess

FAILURE_CASES = {
    "untyped-empty": "  (void)cljonic::hash_map();",
    "typed-empty-without-capacity": "  (void)cljonic::hash_map<int, int>();",
    "odd-deduced-arguments": "  (void)cljonic::hash_map(1, 10, 2);",
    "odd-explicit-arguments": "  (void)cljonic::hash_map<int, int>(1, 10, 2);",
    "unsafe-deduced-conversion": "  (void)cljonic::hash_map(1, 10, 3LL, 20);",
    "unsafe-explicit-conversion": "  (void)cljonic::hash_map<int, int>(1LL, 10);",
    "oversized-pair-count": "  (void)cljonic::hash_map(1, 10, 2, 20, 3, 30);",
    "explicit-capacity-too-small": "  (void)cljonic::hash_map<int, int, 1>(1, 10, 2, 20);",
    "explicit-capacity-over-maximum": "  (void)cljonic::hash_map<int, int, 3>();",
}

PASS_CASES = {
    "deduced-pairs": (
        "  constexpr auto value = cljonic::hash_map(1, 10L, short{2}, int{20}); "
        "static_assert(value.capacity() == 2U && value.count() == 2U && value(2) == 20L);"
    ),
    "explicit-types": (
        "  constexpr auto value = cljonic::hash_map<long, long>(short{1}, short{10}); "
        "static_assert(value.capacity() == 1U && value(1L) == 10L);"
    ),
    "explicit-capacity": (
        "  constexpr auto value = cljonic::hash_map<int, int, 2>(1, 10); "
        "static_assert(value.count() == 1U && value.capacity() == 2U && value(1) == 10);"
    ),
    "typed-empty-zero-capacity": (
        "  constexpr auto value = cljonic::hash_map<int, int, 0>(); "
        "static_assert(value.count() == 0U && value.capacity() == 0U);"
    ),
}

DIAGNOSTIC_CASES = {
    "untyped-empty": (
        FAILURE_CASES["untyped-empty"],
        "empty calls require explicit key, value, and capacity types",
    ),
    "typed-empty-without-capacity": (
        FAILURE_CASES["typed-empty-without-capacity"],
        "empty calls require explicit key, value, and capacity types",
    ),
    "odd-deduced-arguments": (
        FAILURE_CASES["odd-deduced-arguments"],
        "nonempty calls require complete alternating key/value pairs",
    ),
    "odd-explicit-arguments": (
        FAILURE_CASES["odd-explicit-arguments"],
        "nonempty calls require complete alternating key/value pairs",
    ),
    "unsafe-deduced-conversion": (
        FAILURE_CASES["unsafe-deduced-conversion"],
        "every key and value must safely construct its declared type without throwing",
    ),
    "unsafe-explicit-conversion": (
        FAILURE_CASES["unsafe-explicit-conversion"],
        "every key and value must safely construct its declared type without throwing",
    ),
    "oversized-pair-count": (
        FAILURE_CASES["oversized-pair-count"],
        "pair count exceeds CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT",
    ),
    "explicit-capacity-too-small": (
        FAILURE_CASES["explicit-capacity-too-small"],
        "arguments must be complete key/value pairs and fit within the selected capacity",
    ),
    "explicit-capacity-over-maximum": (
        FAILURE_CASES["explicit-capacity-over-maximum"],
        "capacity exceeds CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT",
    ),
}


def compile_case(compiler: list[str], include_dir: str, header: str, body: str) -> subprocess.CompletedProcess[str]:
    source = "\n".join(
        (
            "#define CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT 2",
            f'#include "{header}"',
            "int main() {",
            body,
            "  return 0;",
            "}",
        )
    )
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
        for name, body in FAILURE_CASES.items():
            result = compile_case(compiler, include_dir, header, body)
            if result.returncode == 0:
                failures.append(f"{configuration}/{name}: expected compile failure, but compiled")

        for name, body in PASS_CASES.items():
            result = compile_case(compiler, include_dir, header, body)
            if result.returncode != 0:
                failures.append(f"{configuration}/{name}: expected to compile, but failed")

        for name, (body, message) in DIAGNOSTIC_CASES.items():
            result = compile_case(compiler, include_dir, header, body)
            output = normalized(result.stdout + result.stderr)
            if result.returncode == 0 or any(
                expected not in output for expected in ("cljonic::hash_map:", normalized(message))
            ):
                failures.append(f"{configuration}/{name}: targeted hash_map diagnostic not reported")

    if failures:
        print("hash-map-factory-compile-fail:failures:")
        print("\n".join(f"  {failure}" for failure in failures))
        return 1

    print("hash-map-factory-compile-fail:ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
