#!/usr/bin/env python3
"""Verify the producer factory functions (range, repeat, cycle, iterate, repeatedly)
reject out-of-domain arguments with the targeted RejectionDiagnostic message
(REQ-DIAG-009) instead of a raw list of rejected concept candidates, for both
modular and single-header builds.

The message content is asserted, not just its stability: REQ-DIAG-009 requires
the fallback to name the operation and the violated producer-admission rule, so
the checks below pin the operation identity and its required rule text.
"""

from __future__ import annotations

import os
import re
from compile_probe import compile_probe

# Out-of-domain calls must fail to compile.
FAILURE_CASES = {
    "Range-empty": "(void)cljonic::range();",
    "Range-extra-arguments": "(void)cljonic::range(0, 5, 1, 2);",
    "Range-typed-extra-arguments": "(void)cljonic::range<int>(0, 5, 1, 2);",
    "Range-unsigned": "(void)cljonic::range(5U);",
    "Range-boolean": "(void)cljonic::range(true);",
    "Range-floating": "(void)cljonic::range(5.0);",
    "Range-mixed-types": "(void)cljonic::range(0, 5L);",
    "Range-mixed-step": "(void)cljonic::range(0, 5, 1L);",
    "Range-typed-unsigned": "(void)cljonic::range<unsigned int>(5U);",
    "Range-typed-void": "(void)cljonic::range<void>();",
    "Range-narrowing": "(void)cljonic::range<int>(5LL);",
    "Range-floating-conversion": "(void)cljonic::range<int>(5.0);",
    "Range-start-conversion": "(void)cljonic::range<int>(0LL, 5);",
    "Range-step-conversion": "(void)cljonic::range<int>(0, 5, 1LL);",
    "Cycle-non-source": "auto x = cljonic::cycle(42); (void)x;",
    "Repeatedly-non-callable": "auto x = cljonic::repeatedly(42); (void)x;",
    "Repeatedly-counted-non-callable": "auto x = cljonic::repeatedly(3, 42); (void)x;",
    "Iterate-non-callable": "auto x = cljonic::iterate(42, 1); (void)x;",
    "Repeat-non-storable": "auto x = cljonic::repeat(NonStorable{}); (void)x;",
    "Repeat-counted-non-storable": "auto x = cljonic::repeat(3U, NonStorable{}); (void)x;",
    "Repeat-value-first-factory": "(void)cljonic::repeat(cljonic::Vector{7}, 3);",
    "Repeat-value-first-constructor": "(void)cljonic::Repeat<cljonic::Vector<int, 1>>{cljonic::Vector{7}, 3};",
    "Repeat-value-first-deduction": "(void)cljonic::Repeat{cljonic::Vector{7}, 3};",
}

# In-domain calls must still compile.
PASS_CASES = {
    "Range-deduced": "static_assert(cljonic::range(5) == cljonic::Range{5});",
    "Range-bounds": "static_assert(cljonic::range(0, 5) == cljonic::Range{0, 5});",
    "Range-step": "static_assert(cljonic::range(5, 0, -2) == cljonic::Range{5, 0, -2});",
    "Range-typed-empty": "static_assert(cljonic::range<int>() == cljonic::Range<int>{});",
    "Range-typed-end": "static_assert(cljonic::range<long>(5) == cljonic::Range<long>{5});",
    "Range-typed-bounds": "static_assert(cljonic::range<long>(0, 5) == cljonic::Range<long>{0, 5});",
    "Range-typed-step": "static_assert(cljonic::range<long>(5, 0, -2) == cljonic::Range<long>{5, 0, -2});",
    "Repeat-value": "auto x = cljonic::repeat(7); (void)x;",
    "Repeat-counted": "auto x = cljonic::repeat(3U, 7); (void)x;",
    "Repeat-counted-constructor": "auto x = cljonic::Repeat<int>{3U, 7}; (void)x;",
    "Repeat-counted-deduction": "auto x = cljonic::Repeat{3U, 7}; (void)x;",
    "Repeat-signed-count": "static_assert(cljonic::repeat(3, 7).count() == 3);",
    "Repeat-negative-count": "static_assert(cljonic::Repeat{-2, 7}.count() == 0);",
    "Repeat-numeric-order": "static_assert(cljonic::repeat(7, 3U).count() == 7);",
    "Repeatedly-signed-count": "static_assert(cljonic::repeatedly(3, []() noexcept { return 7; }).count() == 3);",
    "Repeatedly-negative-count": "static_assert(cljonic::repeatedly(-2, []() noexcept { return 7; }).count() == 0);",
    "Cycle-collection": "auto x = cljonic::cycle(cljonic::Vector<int, 3>{1, 2, 3}); (void)x;",
    "Iterate-step": "auto x = cljonic::iterate([](int v) noexcept { return v + 1; }, 0); (void)x;",
    "Repeatedly-named": "constexpr auto five = []() noexcept { return 5; }; auto x = cljonic::repeatedly(five); (void)x;",
    "Repeatedly-counted": "constexpr auto five = []() noexcept { return 5; }; auto x = cljonic::repeatedly(3U, five); (void)x;",
}

ZERO_CAP_PASS_CASES = {
    "Iterate-next-does-not-invoke-step": """\
static_assert([] {
    auto calls = 0;
    const auto source = cljonic::iterate([&calls](int value) noexcept { ++calls; return value + 1; }, 7);
    const auto tail = cljonic::next(source);
    return calls == 0 && source.count() == 0 && tail.count() == 0;
}());
""",
}

PREAMBLE = """\
namespace {
struct NonStorable {
    NonStorable() = default;
    NonStorable(const NonStorable&) = delete;
};
} // namespace
"""

# name -> (probe body, required operation identity, required rule substring)
DIAGNOSTIC_CASES = {
    "Range-empty-diagnostic": ("(void)cljonic::range();", "cljonic::range:", "explicit element type"),
    "Range-type-diagnostic": ("(void)cljonic::range(5U);", "cljonic::range:", "signed integral"),
    "Range-arity-diagnostic": ("(void)cljonic::range(0, 5, 1, 2);", "cljonic::range:", "zero through three"),
    "Range-mixed-diagnostic": ("(void)cljonic::range(0, 5L);", "cljonic::range:", "same signed integral type"),
    "Range-conversion-diagnostic": ("(void)cljonic::range<int>(5LL);", "cljonic::range:", "safely convert"),
    "Cycle-diagnostic": ("(void)cljonic::cycle(42);", "cljonic::cycle:", "cannot be used to build a Cycle"),
    "Repeatedly-diagnostic": (
        "(void)cljonic::repeatedly(42);",
        "cljonic::repeatedly:",
        "cannot be used to build a Repeatedly",
    ),
    "Iterate-diagnostic": ("(void)cljonic::iterate(42, 1);", "cljonic::iterate:", "cannot be used to build an Iterate"),
    "Repeat-diagnostic": ("(void)cljonic::repeat(NonStorable{});", "cljonic::repeat:", "cannot be repeated"),
}


def compile_case(
    compiler: list[str], include_dir: str, header: str, body: str, preamble: str = "", *, expect_success: bool = False
) -> bool:
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
    result = compile_probe(tuple(compiler), include_dir, source)
    if expect_success and result.returncode != 0:
        print(f"producer-compile-fail: compiler output for {header}:\n{result.stdout}{result.stderr}")
    return result.returncode == 0


def _normalized(text: str) -> str:
    return re.sub(r"\s+", " ", text)


def diagnostic_message_reported(
    compiler: list[str], include_dir: str, header: str, body: str, operation: str, required: str
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
    result = compile_probe(tuple(compiler), include_dir, source)
    if result.returncode == 0:
        return False
    output = _normalized(result.stdout + result.stderr)
    return all(_normalized(needle) in output for needle in (operation, required))


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
            if not compile_case(compiler, include_dir, header, body, preamble=PREAMBLE, expect_success=True):
                failures.append(f"{configuration}/{name}: expected to compile, but failed")

        for name, (body, operation, required) in DIAGNOSTIC_CASES.items():
            if not diagnostic_message_reported(compiler, include_dir, header, body, operation, required):
                failures.append(f"{configuration}/{name}: targeted rejection diagnostic content not reported")

    # The complete core header requires String<1>; isolate the zero-cap producer contract.
    for name, body in ZERO_CAP_PASS_CASES.items():
        zero_cap_compiler = [*compiler, "-DCLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT=0"]
        if not compile_case(
            zero_cap_compiler, os.path.join(root, "src"), "cljonic-next.hpp", body, expect_success=True
        ):
            failures.append(f"modular/{name}: zero-cap behavior failed to compile")

    if failures:
        print("producer-compile-fail:failures:")
        print("\n".join(f"  {failure}" for failure in failures))
        return 1

    print("producer-compile-fail:ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
