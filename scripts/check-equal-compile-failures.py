#!/usr/bin/env python3
"""Verify that `equal` rejects unsupported operand pairs at compile time and
admits the supported domain, for both modular and single-header builds.

Covers the EqualFunction spec invariants:
- FloatingPointRejectedAtCompileTimeAtAnyDepth (REQ-NUM-006)
- CrossFamilyAndMixedPairsRejectedAtCompileTime
- OutsideSupportedDomainRejectedAtCompileTime
- StandardRangeTypesRejectedAsInteropSurface
- StandardRangeComponentsRejectedAtAnyDepth
- UnscopedEnumsAndPointersExcludedFromFallthrough
- EqualityComparisonNonThrowingAtAnyDepth
- SequentialElementsRequireIdenticalElementTypes
- NoCrossTypeNumericUnification (pinned NumericEquality rule)
- UnaryFormAdmitsSingleSupportedDomainOperand
- AdjacentPairsAndUnaryOperandCompileTimeGated
- VariadicFormConjoinsAdjacentPairEquality
"""

from __future__ import annotations

import os
import subprocess

FAILURE_CASES = {
    # Floating-point rejection at any depth.
    "Equal-float-scalar": "(void)cljonic::equal(1.0, 1.0);",
    "Equal-float-collection": ("(void)cljonic::equal(cljonic::Vector<double, 2>{}, "
                               "cljonic::Vector<double, 2>{});"),
    "Equal-float-map-value": ("(void)cljonic::equal(cljonic::Map<int, double, 2>{}, "
                              "cljonic::Map<int, double, 2>{});"),
    "Equal-float-producer": ("(void)cljonic::equal(cljonic::Repeat<double>{}, "
                             "cljonic::Repeat<double>{});"),
    "Equal-float-nested-variant": ("(void)cljonic::equal(std::variant<int, double>{1}, "
                                   "std::variant<int, double>{1});"),
    # Cross-family pairs.
    "Equal-map-vs-vector": "(void)cljonic::equal(cljonic::Map<int, int, 2>{}, cljonic::Vector<int, 2>{});",
    "Equal-set-vs-vector": "(void)cljonic::equal(cljonic::Set<int, 2>{}, cljonic::Vector<int, 2>{});",
    "Equal-string-vs-vector": "(void)cljonic::equal(cljonic::String<4>{}, cljonic::Vector<int, 2>{});",
    "Equal-map-vs-set": "(void)cljonic::equal(cljonic::Map<int, int, 2>{}, cljonic::Set<int, 2>{});",
    "Equal-string-vs-map": "(void)cljonic::equal(cljonic::String<4>{}, cljonic::Map<int, int, 2>{});",
    "Equal-set-vs-cycle": ("(void)cljonic::equal(cljonic::Set<int, 2>{}, "
                           "cljonic::cycle(cljonic::Vector<int, 2>{}));"),
    # Mixed cljonic vs non-cljonic pairs.
    "Equal-vector-vs-scalar": "(void)cljonic::equal(cljonic::Vector<int, 2>{1}, 1);",
    "Equal-scalar-vs-map": "(void)cljonic::equal(1, cljonic::Map<int, int, 2>{});",
    "Equal-string-vs-char-literal": "(void)cljonic::equal(cljonic::String<4>{\"a\"}, 'a');",
    # Standard-library interop-surface types are outside the equality domain.
    "Equal-std-vector": "(void)cljonic::equal(std::vector<int>{1}, std::vector<int>{1});",
    "Equal-std-string": "(void)cljonic::equal(std::string{\"a\"}, std::string{\"a\"});",
    "Equal-std-span": "(void)cljonic::equal(std::span<int>{}, std::span<int>{});",
    "Equal-std-string-view": ("(void)cljonic::equal(std::string_view{\"a\"}, "
                              "std::string_view{\"a\"});"),
    "Equal-std-map": "(void)cljonic::equal(std::map<int, int>{}, std::map<int, int>{});",
    # The standard-library variant is not a cljonic value type.
    "Equal-std-variant": ("(void)cljonic::equal(std::variant<int, long>{1}, "
                          "std::variant<int, long>{1});"),
    # Standard-range components are rejected at any depth.
    "Equal-nested-std-range-variant": ("(void)cljonic::equal(std::variant<int, std::vector<int>>{1}, "
                                        "std::variant<int, std::vector<int>>{1});"),
    "Equal-nested-std-range-collection": ("(void)cljonic::equal(cljonic::Vector<std::array<int, 2>, 2>{}, "
                                          "cljonic::Vector<std::array<int, 2>, 2>{});"),
    # Unscoped enums and pointers are outside the fallthrough domain.
    "Equal-unscoped-enum": ("(void)cljonic::equal(EqualUnscopedColor::EqualUnscopedRed, "
                            "EqualUnscopedColor::EqualUnscopedRed);"),
    "Equal-pointer": "int x = 0; int y = 0; (void)cljonic::equal(&x, &y);",
    # A throwing operator== is outside the non-throwing equality domain.
    "Equal-throwing-aggregate": ("ThrowingEqual a{}; ThrowingEqual b{}; "
                                  "(void)cljonic::equal(a, b);"),
    # Cross-type numeric and element-type rules.
    "Equal-scalar-cross-type": "(void)cljonic::equal(1, 1L);",
    "Equal-sequential-element-cross-type": ("(void)cljonic::equal(cljonic::Vector<int, 2>{1}, "
                                            "cljonic::Range<long>{1, 3, 1});"),
    "Equal-map-key-cross-type": ("(void)cljonic::equal(cljonic::Map<int, int, 2>{}, "
                                 "cljonic::Map<long, int, 2>{});"),
    # Callables are outside the stable-equality domain.
    "Equal-callable-bearing-producer": ("(void)cljonic::equal(cljonic::Iterate<int, int (*)(int)>{}, "
                                        "cljonic::Iterate<int, int (*)(int)>{});"),
    # Unary arity: only domain-admitted operands, float and interop rejected.
    "Equal-unary-float": "(void)cljonic::equal(1.0);",
    "Equal-unary-std-vector": "(void)cljonic::equal(std::vector<int>{});",
    "Equal-unary-std-string": "(void)cljonic::equal(std::string{});",
    "Equal-unary-float-collection": "(void)cljonic::equal(cljonic::Vector<double, 2>{});",
    "Equal-unary-float-map": "(void)cljonic::equal(cljonic::Map<int, double, 2>{});",
    # Variadic arity: every adjacent pair must be individually admissible.
    "Equal-variadic-mixed-scalar-collection": ("(void)cljonic::equal(1, cljonic::Vector<int, 2>{1}, 2);"),
    "Equal-variadic-cross-family-position": ("(void)cljonic::equal(1, 2, cljonic::Vector<int, 2>{});"),
    "Equal-variadic-cross-family-tail": ("(void)cljonic::equal(cljonic::Vector<int, 2>{1}, "
                                         "cljonic::Vector<int, 2>{1}, cljonic::Set<int, 2>{});"),
    "Equal-variadic-float-adjacent": "(void)cljonic::equal(1, 1.0, 1);",
    "Equal-variadic-element-cross-type": ("(void)cljonic::equal(cljonic::Vector<int, 2>{1}, "
                                          "cljonic::Range<long>{1, 3, 1}, cljonic::Vector<int, 2>{1});"),
    "Equal-variadic-std-range-tail": ("(void)cljonic::equal(1, 1, std::vector<int>{1});"),
    "Equal-variadic-mixed-family-chain": ("(void)cljonic::equal(cljonic::Map<int, int, 4>"
                                          "{cljonic::MapEntry<int, int>{1, 10}}, "
                                          "cljonic::Map<int, int, 4>{cljonic::MapEntry<int, int>{1, 10}}, "
                                          "cljonic::Set<int, 4>{1, 2}, cljonic::Set<int, 8>{2, 1});"),
}

PASS_CASES = {
    "Equal-scalar": "(void)cljonic::equal(1, 1);",
    "Equal-enum": ("enum class EqualColor { Red, Green }; "
                   "(void)cljonic::equal(EqualColor::Red, EqualColor::Red);"),
    "Equal-cljonic-variant": ("(void)cljonic::equal(cljonic::Variant<int, long>{1}, "
                              "cljonic::Variant<int, long>{1});"),
    "Equal-map-entry": ("(void)cljonic::equal(cljonic::MapEntry<int, int>{1, 2}, "
                        "cljonic::MapEntry<int, int>{1, 2});"),
    "Equal-vector-vs-range": ("(void)cljonic::equal(cljonic::Vector<int, 4>{1, 2, 3}, "
                              "cljonic::Range<int>{1, 4, 1});"),
    "Equal-vector-vs-queue": ("(void)cljonic::equal(cljonic::Vector<int, 4>{1, 2, 3}, "
                              "cljonic::Queue<int, 4>{1, 2, 3});"),
    "Equal-vector-vs-repeat": ("(void)cljonic::equal(cljonic::Vector<int, 4>{1, 1, 1}, "
                               "cljonic::Repeat<int>{1, 3U});"),
    "Equal-repeat-unbounded-pair": ("(void)cljonic::equal(cljonic::Repeat<int>{7}, "
                                    "cljonic::Repeat<int>{7});"),
    "Equal-cycle-vs-vector-early-exit": ("(void)cljonic::equal(cljonic::cycle(cljonic::Vector<int, 2>{1, 2}), "
                                         "cljonic::Vector<int, 4>{1, 2, 3});"),
    "Equal-map-pair": ("(void)cljonic::equal(cljonic::Map<int, int, 4>{cljonic::MapEntry<int, int>{1, 10}}, "
                       "cljonic::Map<int, int, 8>{cljonic::MapEntry<int, int>{1, 10}});"),
    "Equal-set-pair": ("(void)cljonic::equal(cljonic::Set<int, 4>{1, 2}, cljonic::Set<int, 8>{2, 1});"),
    "Equal-string-pair": ("(void)cljonic::equal(cljonic::String<8>{\"abc\"}, cljonic::String<4>{\"abc\"});"),
    "Equal-aggregate": ("EqualPixel a{1, 2}; EqualPixel b{1, 2}; (void)cljonic::equal(a, b);"),
    "Equal-nested-producer-parameter-equality": ("(void)cljonic::equal(cljonic::Vector<cljonic::Repeat<int>, 2>"
                                                 "{cljonic::Repeat<int>{7, 3U}}, "
                                                 "cljonic::Vector<cljonic::Repeat<int>, 2>"
                                                 "{cljonic::Repeat<int>{7, 3U}});"),
    "Equal-unary-scalar": "(void)cljonic::equal(1);",
    "Equal-unary-collection": "(void)cljonic::equal(cljonic::Vector<int, 2>{});",
    "Equal-unary-producer": "(void)cljonic::equal(cljonic::Repeat<int>{7});",
    "Equal-variadic-scalar": "(void)cljonic::equal(1, 1, 1, 1);",
    "Equal-variadic-sequential": ("(void)cljonic::equal(cljonic::Vector<int, 4>{1, 2, 3}, "
                                 "cljonic::Range<int>{1, 4, 1}, cljonic::Queue<int, 4>{1, 2, 3});"),
    "Equal-variadic-associative": ("(void)cljonic::equal("
                                  "cljonic::Map<int, int, 4>{cljonic::MapEntry<int, int>{1, 10}}, "
                                  "cljonic::Map<int, int, 8>{cljonic::MapEntry<int, int>{1, 10}}, "
                                  "cljonic::Map<int, int, 4>{cljonic::MapEntry<int, int>{1, 10}});"),

}

# Injected before main(): an aggregate-like struct with stable equality.
PREAMBLE = """
struct EqualPixel {
    int x;
    int y;
    [[nodiscard]] constexpr auto operator==(const EqualPixel& other) const noexcept -> bool {
        return x == other.x && y == other.y;
    }
};

struct ThrowingEqual {
    int x;
    [[nodiscard]] auto operator==(const ThrowingEqual& other) const -> bool {
        return x == other.x;
    }
};

enum EqualUnscopedColor { EqualUnscopedRed, EqualUnscopedGreen };
"""


def compile_case(compiler: list[str], include_dir: str, header: str, body: str, preamble: str = "") -> bool:
    source = "\n".join(
        (
            "#include <array>",
            "#include <map>",
            "#include <span>",
            "#include <string>",
            "#include <string_view>",
            "#include <variant>",
            "#include <vector>",
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


# Out-of-domain calls must report the targeted RejectionDiagnostic message
# (REQ-DIAG-009) rather than a raw list of rejected concept candidates.
DIAGNOSTIC_CASES = {
    "Equal-float-diagnostic": "(void)cljonic::equal(1.0, 1.0);",
    "Equal-mixed-diagnostic": "(void)cljonic::equal(cljonic::Vector<int, 2>{1}, 1);",
}


def diagnostic_message_reported(compiler: list[str], include_dir: str, header: str, body: str) -> bool:
    source = "\n".join(
        (
            "#include <vector>",
            f'#include "{header}"',
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
    return result.returncode != 0 and "outside the supported equality domain" in (result.stdout + result.stderr)


def main() -> int:
    compiler = os.environ.get("CXX", "c++").split()
    root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    configurations = (("modular", os.path.join(root, "src"), "cljonic-core.hpp"), ("single-header", root, "cljonic.hpp"))
    failures: list[str] = []

    for configuration, include_dir, header in configurations:
        for name, body in FAILURE_CASES.items():
            if compile_case(compiler, include_dir, header, body, preamble=PREAMBLE):
                failures.append(f"{configuration}/{name}: expected compile failure, but compiled")

        for name, body in PASS_CASES.items():
            if not compile_case(compiler, include_dir, header, body, preamble=PREAMBLE):
                failures.append(f"{configuration}/{name}: expected to compile, but failed")

        for name, body in DIAGNOSTIC_CASES.items():
            if not diagnostic_message_reported(compiler, include_dir, header, body):
                failures.append(f"{configuration}/{name}: targeted rejection diagnostic not reported")

    if failures:
        print("equal-compile-fail:failures:")
        print("\n".join(f"  {failure}" for failure in failures))
        return 1

    print("equal-compile-fail:ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())