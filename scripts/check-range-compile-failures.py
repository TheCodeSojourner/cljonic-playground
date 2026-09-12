#!/usr/bin/env python3
"""Verify that statically oversized range sources fail during compilation."""

from __future__ import annotations

import os
import subprocess


CASES = {
    "Vector": (
        "cljonic::Vector<int, 2>{source}",
        "int",
    ),
    "Map": (
        "cljonic::Map<int, int, 2>{source}",
        "cljonic::MapEntry<int, int>",
    ),
    "Set": (
        "cljonic::Set<int, 2>{source}",
        "int",
    ),
    "Queue": (
        "cljonic::Queue<int, 2>{source}",
        "int",
    ),
    "String": (
        "cljonic::String<2>{source}",
        "char",
    ),
}

SOURCE_FORMS = {
    "span": lambda element, extent: f"std::span<const {element}, {extent}> source{{values}};",
    "array": lambda element, extent: f"std::array<{element}, {extent}> source{{}};",
    "builtin-array": lambda element, extent: f"const {element} source[{extent}]{{}};",
}


def value_declaration(element: str, extent: int) -> str:
    if element == "int":
        return f"static constexpr int values[{extent}] = {{}};"
    if element == "char":
        return f"static constexpr char values[{extent}] = {{}};"
    return f"static constexpr cljonic::MapEntry<int, int> values[{extent}] = {{}};"


def compile_case(compiler: list[str], include_dir: str, header: str, element: str, extent: int, expression: str) -> bool:
    source = "\n".join(
        (
            "#include <array>",
            "#include <span>",
            f'#include "{header}"',
            "int main() {",
            f"  {value_declaration(element, extent)}",
            expression,
            "  (void)source;",
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
    return result.returncode != 0


def main() -> int:
    compiler = os.environ.get("CXX", "c++").split()
    root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    configurations = (("modular", os.path.join(root, "src"), "cljonic-core.hpp"), ("single-header", root, "cljonic.hpp"))
    failures: list[str] = []

    for configuration, include_dir, header in configurations:
        for collection, (constructor, element) in CASES.items():
            for source_kind, source_factory in SOURCE_FORMS.items():
                extent = 4 if collection == "String" else 3
                expression = f"  {source_factory(element, extent)}\n  auto value = {constructor};"
                if not compile_case(compiler, include_dir, header, element, extent, expression):
                    failures.append(f"{configuration}/{collection}/{source_kind}")

    if failures:
        print("range-compile-fail:missing expected failures:")
        print("\n".join(f"  {failure}" for failure in failures))
        return 1

    print("range-compile-fail:ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
