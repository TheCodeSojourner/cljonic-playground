"""Reuse identical syntax-only compiler results within one validation process.

Inputs and compiler configuration must remain unchanged for the process lifetime.
No results persist between validation runs.
"""

from functools import cache
import subprocess


@cache
def compile_probe(compiler: tuple[str, ...], include_dir: str, source: str) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        [*compiler, "-std=c++23", "-fsyntax-only", "-I", include_dir, "-x", "c++", "-"],
        input=source,
        text=True,
        capture_output=True,
        check=False,
    )
