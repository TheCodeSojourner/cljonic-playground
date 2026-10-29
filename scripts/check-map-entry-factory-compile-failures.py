#!/usr/bin/env python3
"""Verify map_entry factory admission and rejection for modular and single headers."""

from __future__ import annotations

import os
import re
import subprocess

from compile_probe import compile_probe

FAILURE_CASES = {
    "zero-arguments": "  (void)cljonic::map_entry();",
    "one-argument": "  (void)cljonic::map_entry(1);",
    "three-arguments": "  (void)cljonic::map_entry(1, 2, 3);",
    "floating-point-key": "  (void)cljonic::map_entry(1.0, 2.0);",
    "unsafe-explicit-conversion": "  (void)cljonic::map_entry<int, int>(1LL, 10);",
}

PASS_CASES = {
    "deduced-key-and-value": (
        "  constexpr auto value = cljonic::map_entry(1, 10L); "
        "static_assert(value.key == 1 && value.value == 10L);"
    ),
    "explicit-types": (
        "  constexpr auto value = cljonic::map_entry<long, long>(short{1}, short{10}); "
        "static_assert(value.key == 1L && value.value == 10L);"
    ),
    "constexpr-noexcept": (
        "  static_assert(noexcept(cljonic::map_entry(1, 2))); "
        "constexpr auto value = cljonic::map_entry(3, 4); "
        "static_assert(value == cljonic::MapEntry{3, 4});"
    ),
    "value-semantic-copy": (
        "  const auto original = cljonic::map_entry(5, 6); "
        "const auto copy = original; "
        "static_assert(noexcept(original == copy)); "
        "return (original == copy && copy.key == 5 && copy.value == 6) ? 0 : 1;"
    ),
}

DIAGNOSTIC_CASES = {
    "zero-arguments": (
        FAILURE_CASES["zero-arguments"],
        "requires exactly one key argument and one value argument",
    ),
    "one-argument": (
        FAILURE_CASES["one-argument"],
        "requires exactly one key argument and one value argument",
    ),
    "three-arguments": (
        FAILURE_CASES["three-arguments"],
        "requires exactly one key argument and one value argument",
    ),
    "floating-point-key": (
        FAILURE_CASES["floating-point-key"],
        "key and value types must satisfy MapEntry element requirements",
    ),
    "unsafe-explicit-conversion": (
        FAILURE_CASES["unsafe-explicit-conversion"],
        "the key and value must safely construct their declared types without throwing",
    ),
}


def compile_case(
    compiler: list[str], include_dir: str, header: str, body: str, *, has_explicit_return: bool = False
) -> subprocess.CompletedProcess[str]:
    source = "\n".join(
        (
            f'#include "{header}"',
            "int main() {",
            body,
            "" if has_explicit_return else "  return 0;",
            "}",
        )
    )
    return compile_probe(tuple(compiler), include_dir, source)


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
            result = compile_case(
                compiler, include_dir, header, body, has_explicit_return=name == "value-semantic-copy"
            )
            if result.returncode != 0:
                failures.append(f"{configuration}/{name}: expected to compile, but failed")

        for name, (body, message) in DIAGNOSTIC_CASES.items():
            result = compile_case(compiler, include_dir, header, body)
            output = normalized(result.stdout + result.stderr)
            if result.returncode == 0 or any(
                expected not in output for expected in ("cljonic::map_entry:", normalized(message))
            ):
                failures.append(f"{configuration}/{name}: targeted map_entry diagnostic not reported")

    if failures:
        print("map-entry-factory-compile-fail:failures:")
        print("\n".join(f"  {failure}" for failure in failures))
        return 1

    print("map-entry-factory-compile-fail:ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
