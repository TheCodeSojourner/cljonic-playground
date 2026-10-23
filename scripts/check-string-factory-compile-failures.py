#!/usr/bin/env python3
"""Verify string factory admission and rejection for modular and single headers."""

from __future__ import annotations

import os
import re
import subprocess

FAILURE_CASES = {
    "untyped-empty": '  (void)cljonic::string();',
    "variadic-characters": "  (void)cljonic::string('h', 'i');",
    "non-literal-argument": "  (void)cljonic::string(1);",
    "view-without-capacity": (
        '  std::string_view view{"x"}; (void)cljonic::string(view);'
    ),
    "cljonic-collection-source": (
        "  cljonic::Vector<int, 2> values{1, 2}; (void)cljonic::string<2>(values);"
    ),
    "explicit-capacity-over-maximum": "  (void)cljonic::string<3>();",
    "oversized-literal": '  (void)cljonic::string("abc");',
    "literal-too-long-for-capacity": '  (void)cljonic::string<1>("abc");',
}

PASS_CASES = {
    "deduced-literal": (
        "  constexpr auto value = cljonic::string(\"Hi\"); "
        "static_assert(value.count() == 2U && value.capacity() == 2U && value(1) == 'i');"
    ),
    "explicit-capacity": (
        "  constexpr auto value = cljonic::string<2>(\"Hi\"); "
        "static_assert(value.count() == 2U && value.capacity() == 2U && value(1) == 'i');"
    ),
    "explicit-capacity-partial": (
        "  constexpr auto value = cljonic::string<2>(\"a\"); "
        "static_assert(value.count() == 1U && value.capacity() == 2U && value(0) == 'a');"
    ),
    "explicit-capacity-empty": (
        "  constexpr auto value = cljonic::string<2>(); "
        "static_assert(value.count() == 0U && value.capacity() == 2U);"
    ),
    "empty-zero-capacity": (
        "  constexpr auto value = cljonic::string<0>(); "
        "static_assert(value.count() == 0U && value.capacity() == 0U);"
    ),
    "deduced-empty-literal": (
        "  constexpr auto value = cljonic::string(\"\"); "
        "static_assert(value.count() == 0U && value.capacity() == 0U);"
    ),
    "view-import": (
        '  constexpr std::string_view view{"ab"}; '
        "  constexpr auto value = cljonic::string<2>(view); "
        "static_assert(value.view() == view);"
    ),
    "range-import": (
        "  static constexpr char source[] = {'a', 'b'}; "
        "  constexpr auto value = cljonic::string<2>(std::span{source}); "
        "static_assert(value.view() == \"ab\");"
    ),
}

DIAGNOSTIC_CASES = {
    "untyped-empty": (
        FAILURE_CASES["untyped-empty"],
        "an empty String requires an explicit capacity; write string<Capacity>()",
    ),
    "variadic-characters": (
        FAILURE_CASES["variadic-characters"],
        "the string factory takes a single text source; individual characters are not admitted",
    ),
    "non-literal-argument": (
        FAILURE_CASES["non-literal-argument"],
        "the argument must be a string literal, or with an explicit capacity a string_view or non-cljonic char "
        "range or view",
    ),
    "view-without-capacity": (
        FAILURE_CASES["view-without-capacity"],
        "a string_view's size is not part of its type; provide an explicit capacity: string<Capacity>(view)",
    ),
    "cljonic-collection-source": (
        FAILURE_CASES["cljonic-collection-source"],
        "a cljonic collection or producer is not a text source",
    ),
    "explicit-capacity-over-maximum": (
        FAILURE_CASES["explicit-capacity-over-maximum"],
        "capacity exceeds CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT",
    ),
    "oversized-literal": (
        FAILURE_CASES["oversized-literal"],
        "literal content length exceeds CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT",
    ),
    "literal-too-long-for-capacity": (
        FAILURE_CASES["literal-too-long-for-capacity"],
        "cljonic::String: literal too long for capacity",  # cljonic::String: literal too long for capacity
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
                expected not in output for expected in (normalized(message),)
            ):
                failures.append(f"{configuration}/{name}: targeted string diagnostic not reported")

    if failures:
        print("string-factory-compile-fail:failures:")
        print("\n".join(f"  {failure}" for failure in failures))
        return 1

    print("string-factory-compile-fail:ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
