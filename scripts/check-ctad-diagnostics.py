#!/usr/bin/env python3
"""Verify the REQ-DIAG-012 class-template deduction diagnostic for `Range`.

An out-of-domain class template argument deduction (`Range{"Hello"}`) must fail
with exactly one targeted `static_assert` whose message is labeled and
user-facing, instead of the compiler's candidate list for a failed deduction.
Admitted deduction must be unaffected, and the type-level domain gate must keep
rejecting softly. The checks run against both the modular and the generated
single-header configuration.
"""

from __future__ import annotations

import os
import subprocess
import sys

# Out-of-domain deductions: each must fail with the targeted message.
FAILURE_CASES = {
    "Range-string-literal": 'auto value = cljonic::Range{"Hello"}; (void)value;',
    "Range-float-one-arg": "auto value = cljonic::Range{1.5}; (void)value;",
    "Range-float-two-arg": "auto value = cljonic::Range{1, 5.0}; (void)value;",
    "Range-float-three-arg": "auto value = cljonic::Range{1, 5, 2.0}; (void)value;",
    "Range-pointer": "auto value = cljonic::Range{static_cast<const char*>(nullptr)}; (void)value;",
    "Repeat-string-literal": 'auto value = cljonic::Repeat{"Hello"}; (void)value;',
    "Repeat-bad-count": 'auto value = cljonic::Repeat{"x", 7}; (void)value;',
    "Cycle-string-literal": 'auto value = cljonic::Cycle{"Hello"}; (void)value;',
    "Iterate-bad-step": 'auto value = cljonic::Iterate{1, "Hello"}; (void)value;',
    "Repeatedly-undeducible": "auto value = cljonic::Repeatedly{GenerateStep{}}; (void)value;",
    "Variant-undeducible": "auto value = cljonic::Variant{1}; (void)value;",
}

# Admitted deductions and explicit construction: each must compile unchanged.
PASS_CASES = {
    "Range-one-arg": "auto value = cljonic::Range{5}; (void)value;",
    "Range-two-arg": "auto value = cljonic::Range{2, 5}; (void)value;",
    "Range-three-arg": "auto value = cljonic::Range{1, 5, 2}; (void)value;",
    "Range-long-args": "auto value = cljonic::Range{1L, 5L}; (void)value;",
    "Range-explicit": "auto value = cljonic::Range<int>{1, 5, 2}; (void)value;",
    "Repeat-one-arg": "auto value = cljonic::Repeat{7}; (void)value;",
    "Repeat-two-arg": "auto value = cljonic::Repeat{3U, 7}; (void)value;",
    "Cycle-vector": "auto value = cljonic::Cycle{cljonic::Vector<int, 3>{1, 2, 3}}; (void)value;",
    "Cycle-factory": "auto value = cljonic::cycle(cljonic::Vector<int, 3>{1, 2, 3}); (void)value;",
    "Iterate-valid": "auto value = cljonic::Iterate{1, AddOne{}}; (void)value;",
    "Iterate-factory": "auto value = cljonic::iterate(AddOne{}, 1); (void)value;",
    "Repeatedly-factory": "auto value = cljonic::repeatedly(GenerateStep{}); (void)value;",
    "Variant-explicit": "auto value = cljonic::Variant<int, long>{1}; (void)value;",
    "Variant-default": "auto value = cljonic::Variant<int, long>{}; (void)value;",
}

# The type-level domain gates must stay soft: these must all be `false`, not
# ill-formed, and the file must compile.
GATE_CASES = """
static_assert(!cljonic::concepts::RangeElement<double>);
static_assert(!cljonic::concepts::RangeElement<unsigned>);
static_assert(!cljonic::concepts::RangeElement<const char*>);
static_assert(cljonic::concepts::RangeElement<int>);
"""

PREAMBLE = """
struct AddOne {
    constexpr auto operator()(int value) const noexcept -> int { return value + 1; }
};
struct GenerateStep {
    constexpr auto operator()() const noexcept -> int { return 7; }
};
"""

LABELS = ("cljonic::Range:", "cljonic::Repeat:", "cljonic::Cycle:", "cljonic::Iterate:", "cljonic::Repeatedly:",
          "cljonic::Variant:")
CANDIDATE_DUMP = "class template argument deduction failed"


def compile_source(
    compiler: list[str], include_dir: str, header: str, body: str, preamble: str = ""
) -> subprocess.CompletedProcess[str]:
    source = "\n".join(
        ("#include <concepts>", f'#include "{header}"', preamble, "int main() {", f"  {body}", "  return 0;", "}")
    )
    return subprocess.run(
        [*compiler, "-std=c++23", "-fsyntax-only", "-I", include_dir, "-x", "c++", "-"],
        input=source,
        text=True,
        capture_output=True,
        check=False,
    )


def main() -> int:
    compiler = os.environ.get("CXX", "c++").split()
    root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    configurations = (("modular", os.path.join(root, "src"), "cljonic-core.hpp"), ("single-header", root, "cljonic.hpp"))
    failures: list[str] = []

    for configuration, include_dir, header in configurations:
        for name, body in FAILURE_CASES.items():
            result = compile_source(compiler, include_dir, header, body, preamble=PREAMBLE)
            if result.returncode == 0:
                failures.append(f"{configuration}/{name}: expected compile failure, but compiled")
                continue
            output = result.stdout + result.stderr
            if not any(label in output for label in LABELS):
                failures.append(f"{configuration}/{name}: rejection does not carry a cljonic::<entity>: label")
            if CANDIDATE_DUMP in output:
                failures.append(f"{configuration}/{name}: rejection still reports a failed deduction candidate list")
            if output.count("error:") != 1:
                failures.append(f"{configuration}/{name}: expected exactly one diagnostic, got {output.count('error:')}")

        for name, body in PASS_CASES.items():
            if compile_source(compiler, include_dir, header, body, preamble=PREAMBLE).returncode != 0:
                failures.append(f"{configuration}/{name}: expected to compile, but failed")

        if compile_source(compiler, include_dir, header, GATE_CASES).returncode != 0:
            failures.append(f"{configuration}/soft-domain-gate: the type-level domain gate is no longer soft")

    if failures:
        print("ctad-diagnostics:failures:", file=sys.stderr)
        print("\n".join(f"  {failure}" for failure in failures), file=sys.stderr)
        return 1

    print("ctad-diagnostics:ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
