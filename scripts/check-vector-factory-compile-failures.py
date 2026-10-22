#!/usr/bin/env python3
"""Verify vector factory rejection and admission for modular and single headers."""

from __future__ import annotations

import os
import re
import subprocess

FAILURE_CASES = {
    "untyped-empty": "  (void)cljonic::vector();",
    "void-element-type": "  (void)cljonic::vector<void>();",
    "invalid-element-type": "  (void)cljonic::vector<NonStorable>();",
    "unsafe-deduced-conversion": "  (void)cljonic::vector(1, 2LL);",
    "unsafe-explicit-conversion": "  (void)cljonic::vector<int>(2LL);",
    "oversized-pack": "  (void)cljonic::vector(1, 2, 3);",
    "oversized-explicit-pack": "  (void)cljonic::vector<int>(1, 2, 3);",
    "explicit-capacity-too-small": "  (void)cljonic::vector<int, 1>(1, 2);",
    "explicit-capacity-over-maximum": "  (void)cljonic::vector<int, 3>();",
}

PASS_CASES = {
    "typed-empty": "  static_assert(cljonic::vector<int>().capacity() == 0U);",
    "deduced-pack": (
        "  constexpr auto value = cljonic::vector(1, short{2}); "
        "static_assert(value.capacity() == 2U && value(0U) == 1 && value(1U) == 2);"
    ),
    "explicit-element-type": (
        "  constexpr auto value = cljonic::vector<long>(short{1}, int{2}); "
        "static_assert(value.capacity() == 2U && value(0U) == 1L && value(1U) == 2L);"
    ),
    "explicit-capacity-empty": (
        "  constexpr auto value = cljonic::vector<int, 2>(); "
        "static_assert(value.count() == 0U && value.capacity() == 2U);"
    ),
    "explicit-capacity-pack": (
        "  constexpr auto value = cljonic::vector<int, 2>(short{1}); "
        "static_assert(value.count() == 1U && value.capacity() == 2U && value(0U) == 1);"
    ),
}

DIAGNOSTIC_CASES = {
    "untyped-empty": (
        "  (void)cljonic::vector();",
        "an empty Vector requires an explicit element type",
    ),
    "void-element-type": (
        "  (void)cljonic::vector<void>();",
        "the element type must satisfy NothrowCollectionElement",
    ),
    "invalid-element-type": (
        "  (void)cljonic::vector<NonStorable>();",
        "the element type must satisfy NothrowCollectionElement",
    ),
    "unsafe-deduced-conversion": (
        "  (void)cljonic::vector(1, 2LL);",
        "every argument must safely construct the element type without throwing",
    ),
    "unsafe-explicit-conversion": (
        "  (void)cljonic::vector<int>(2LL);",
        "every argument must safely construct the element type without throwing",
    ),
    "oversized-pack": (
        "  (void)cljonic::vector(1, 2, 3);",
        "argument count exceeds CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT",
    ),
    "oversized-explicit-pack": (
        "  (void)cljonic::vector<int>(1, 2, 3);",
        "argument count exceeds CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT",
    ),
    "explicit-capacity-too-small": (
        "  (void)cljonic::vector<int, 1>(1, 2);",
        "capacity must be at least the number of arguments",
    ),
    "explicit-capacity-over-maximum": (
        "  (void)cljonic::vector<int, 3>();",
        "capacity exceeds CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT",
    ),
}

PREAMBLE = """\
struct NonStorable {
    NonStorable() noexcept = default;
    NonStorable(const NonStorable&) = delete;
};
"""


def compile_case(compiler: list[str], include_dir: str, header: str, body: str) -> subprocess.CompletedProcess[str]:
    source = "\n".join(
        (
            "#define CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT 2",
            f'#include "{header}"',
            PREAMBLE,
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
                expected not in output
                for expected in ("cljonic::vector:", normalized(message))
            ):
                failures.append(f"{configuration}/{name}: targeted vector diagnostic not reported")

    if failures:
        print("vector-factory-compile-fail:failures:")
        print("\n".join(f"  {failure}" for failure in failures))
        return 1

    print("vector-factory-compile-fail:ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
