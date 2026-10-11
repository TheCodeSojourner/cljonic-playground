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

# Each case is (source body, expected message fragment).
DIAGNOSTIC_CASES = {
    "bool": ("(void)cljonic::character_is_valid(true);", "must be an integer character code"),
    "floating-point": ("(void)cljonic::character_is_valid(65.0);", "must be an integer character code"),
    "enumeration": (
        "enum class CharacterCode { value = 65 }; (void)cljonic::character_is_valid(CharacterCode::value);",
        "must be an integer character code",
    ),
    "user-defined-conversion": (
        "(void)cljonic::character_is_valid(CharacterIsValidUnsupported{});",
        "must be an integer character code",
    ),
    "non-array-c-string-pointer": (
        'const char* character = "A"; (void)cljonic::character_is_valid(character);',
        "must be an integer character code",
    ),
    "empty-c-string": ('(void)cljonic::character_is_valid("");', "one-character c-string"),
    "multi-character-c-string": ('(void)cljonic::character_is_valid("AB");', "one-character c-string"),
}

DIAGNOSTIC_ANCHOR = "cljonic::character_is_valid:"


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
            if DIAGNOSTIC_ANCHOR not in output or message not in output:
                failures.append(f"{configuration}/{name}: targeted rejection diagnostic content not reported")

    if failures:
        print("character-is-valid-compile-fail:failures:")
        print("\n".join(f"  {failure}" for failure in failures))
        return 1

    print("character-is-valid-compile-fail:ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())