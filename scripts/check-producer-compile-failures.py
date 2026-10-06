#!/usr/bin/env python3
"""Verify the producer factory functions (repeat, cycle, iterate, repeatedly)
reject out-of-domain arguments with the targeted RejectionDiagnostic message
(REQ-DIAG-009) instead of a raw list of rejected concept candidates, for both
modular and single-header builds.

The message content is asserted, not just its stability: REQ-DIAG-009 requires
the fallback to name the operation and the violated producer-admission rule, so
the checks below pin the operation identity and the shared anchor.
"""

from __future__ import annotations

import os
import re
import subprocess

# Out-of-domain calls must fail to compile.
FAILURE_CASES = {
    "Cycle-non-source": "auto x = cljonic::cycle(42); (void)x;",
    "Repeatedly-non-callable": "auto x = cljonic::repeatedly(42); (void)x;",
    "Repeatedly-counted-non-callable": "auto x = cljonic::repeatedly(3, 42); (void)x;",
    "Iterate-non-callable": "auto x = cljonic::iterate(42, 1); (void)x;",
    "Repeat-non-storable": "auto x = cljonic::repeat(NonStorable{}); (void)x;",
    "Repeat-counted-non-storable": "auto x = cljonic::repeat(NonStorable{}, 3); (void)x;",
}

# In-domain calls must still compile.
PASS_CASES = {
    "Repeat-value": "auto x = cljonic::repeat(7); (void)x;",
    "Repeat-counted": "auto x = cljonic::repeat(7, 3); (void)x;",
    "Cycle-collection": "auto x = cljonic::cycle(cljonic::Vector<int, 3>{1, 2, 3}); (void)x;",
    "Iterate-step": "auto x = cljonic::iterate([](int v) noexcept { return v + 1; }, 0); (void)x;",
    "Repeatedly-named": "constexpr auto five = []() noexcept { return 5; }; auto x = cljonic::repeatedly(five); (void)x;",
    "Repeatedly-counted": "constexpr auto five = []() noexcept { return 5; }; auto x = cljonic::repeatedly(3, five); (void)x;",
}

PREAMBLE = """\
namespace {
struct NonStorable {
    NonStorable() = default;
    NonStorable(const NonStorable&) = delete;
};
} // namespace
"""

# Shared anchor for the producer-factory rejection message (REQ-DIAG-009).
DIAGNOSTIC_ANCHOR = "outside the supported producer domain"

# name -> (probe body, required operation identity)
DIAGNOSTIC_CASES = {
    "Cycle-diagnostic": ("(void)cljonic::cycle(42);", "cljonic::cycle:"),
    "Repeatedly-diagnostic": ("(void)cljonic::repeatedly(42);", "cljonic::repeatedly:"),
    "Iterate-diagnostic": ("(void)cljonic::iterate(42, 1);", "cljonic::iterate:"),
    "Repeat-diagnostic": ("(void)cljonic::repeat(NonStorable{});", "cljonic::repeat:"),
}


def compile_case(compiler: list[str], include_dir: str, header: str, body: str, preamble: str = "") -> bool:
    source = "\n".join(
        (
            f'#include "{header}"',
            preamble,
            "int main() {",
            f"  {body}",
            "  return 0;",
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
    return result.returncode == 0


def _normalized(text: str) -> str:
    return re.sub(r"\s+", " ", text)


def diagnostic_message_reported(
    compiler: list[str], include_dir: str, header: str, body: str, operation: str
) -> bool:
    source = "\n".join(
        (
            f'#include "{header}"',
            PREAMBLE,
            "int main() {",
            f"  {body}",
            "  return 0;",
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
    if result.returncode == 0:
        return False
    output = _normalized(result.stdout + result.stderr)
    return all(
        _normalized(needle) in output
        for needle in (operation, DIAGNOSTIC_ANCHOR)
    )


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
            if compile_case(compiler, include_dir, header, body, preamble=PREAMBLE):
                failures.append(f"{configuration}/{name}: expected compile failure, but compiled")

        for name, body in PASS_CASES.items():
            if not compile_case(compiler, include_dir, header, body, preamble=PREAMBLE):
                failures.append(f"{configuration}/{name}: expected to compile, but failed")

        for name, (body, operation) in DIAGNOSTIC_CASES.items():
            if not diagnostic_message_reported(compiler, include_dir, header, body, operation):
                failures.append(f"{configuration}/{name}: targeted rejection diagnostic content not reported")

    if failures:
        print("producer-compile-fail:failures:")
        print("\n".join(f"  {failure}" for failure in failures))
        return 1

    print("producer-compile-fail:ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
