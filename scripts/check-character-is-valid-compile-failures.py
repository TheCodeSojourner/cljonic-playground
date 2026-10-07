#!/usr/bin/env python3
"""Verify character_is_valid's rejected input types have targeted diagnostics.

The cases are compiled against both the modular and amalgamated public headers.
"""

from __future__ import annotations

import os
import re
import subprocess

PREAMBLE = """
struct CharacterIsValidUnsupported {
    constexpr operator char() const noexcept { return 'A'; }
};
"""

DIAGNOSTIC_CASES = {
    "bool": "(void)cljonic::character_is_valid(true);",
    "floating-point": "(void)cljonic::character_is_valid(65.0);",
    "enumeration": "enum class CharacterCode { value = 65 }; (void)cljonic::character_is_valid(CharacterCode::value);",
    "user-defined-conversion": "(void)cljonic::character_is_valid(CharacterIsValidUnsupported{});",
}

DIAGNOSTIC_ANCHOR = "cljonic::character_is_valid:"
DIAGNOSTIC_MESSAGE = "value must have a non-bool integral type"


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
        for name, body in DIAGNOSTIC_CASES.items():
            result = compile_source(compiler, include_dir, build_source(header, body))
            if result.returncode == 0:
                failures.append(f"{configuration}/{name}: expected compile failure, but compiled")
                continue
            output = normalized(result.stdout + result.stderr)
            if DIAGNOSTIC_ANCHOR not in output or DIAGNOSTIC_MESSAGE not in output:
                failures.append(f"{configuration}/{name}: targeted rejection diagnostic content not reported")

    if failures:
        print("character-is-valid-compile-fail:failures:")
        print("\n".join(f"  {failure}" for failure in failures))
        return 1

    print("character-is-valid-compile-fail:ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())