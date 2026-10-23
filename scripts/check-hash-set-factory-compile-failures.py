#!/usr/bin/env python3
"""Verify hash_set factory admission and rejection for modular and single headers."""

from __future__ import annotations

import os
import re
import subprocess

FAILURE_CASES = {
    "untyped-empty": "  (void)cljonic::hash_set();",
    "typed-empty-without-capacity": "  (void)cljonic::hash_set<int>();",
    "unsafe-deduced-conversion": "  (void)cljonic::hash_set(1, 3LL);",
    "unsafe-explicit-conversion": "  (void)cljonic::hash_set<int>(2LL);",
    "oversized-argument-count": "  (void)cljonic::hash_set(1, 10, 100);",
    "explicit-capacity-too-small": "  (void)cljonic::hash_set<int, 1>(1, 10);",
    "explicit-capacity-over-maximum": "  (void)cljonic::hash_set<int, 3>();",
}

PASS_CASES = {
    "deduced-elements": (
        "  constexpr auto value = cljonic::hash_set(1, short{2}); "
        "static_assert(value.capacity() == 2U && value.count() == 2U && value(2) == 2);"
    ),
    "explicit-type": (
        "  constexpr auto value = cljonic::hash_set<long>(short{1}, short{2}); "
        "static_assert(value.capacity() == 2U && value(1L) == 1L);"
    ),
    "explicit-capacity": (
        "  constexpr auto value = cljonic::hash_set<int, 2>(1, 2); "
        "static_assert(value.count() == 2U && value.capacity() == 2U && value(2) == 2);"
    ),
    "typed-empty-zero-capacity": (
        "  constexpr auto value = cljonic::hash_set<int, 0>(); "
        "static_assert(value.count() == 0U && value.capacity() == 0U);"
    ),
    "runtime-deduplication": (
        "  auto value = cljonic::hash_set(1, 1); "
        "if (value.capacity() != 2U || value.count() != 1U) { return 1; }"
    ),
}

DIAGNOSTIC_CASES = {
    "untyped-empty": (
        FAILURE_CASES["untyped-empty"],
        "empty calls require explicit element and capacity types",
    ),
    "typed-empty-without-capacity": (
        FAILURE_CASES["typed-empty-without-capacity"],
        "empty calls require explicit element and capacity types",
    ),
    "unsafe-deduced-conversion": (
        FAILURE_CASES["unsafe-deduced-conversion"],
        "every argument must safely construct the element type without throwing",
    ),
    "unsafe-explicit-conversion": (
        FAILURE_CASES["unsafe-explicit-conversion"],
        "every argument must safely construct the element type without throwing",
    ),
    "oversized-argument-count": (
        FAILURE_CASES["oversized-argument-count"],
        "argument count exceeds CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT",
    ),
    "explicit-capacity-too-small": (
        FAILURE_CASES["explicit-capacity-too-small"],
        "arguments must fit within the selected capacity",
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
                expected not in output for expected in ("cljonic::hash_set:", normalized(message))
            ):
                failures.append(f"{configuration}/{name}: targeted hash_set diagnostic not reported")

    if failures:
        print("hash-set-factory-compile-fail:failures:")
        print("\n".join(f"  {failure}" for failure in failures))
        return 1

    print("hash-set-factory-compile-fail:ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
