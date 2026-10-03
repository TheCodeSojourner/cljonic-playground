#!/usr/bin/env python3
"""Verify that `not_equal` rejects unsupported operand pairs at compile time and
admits the supported domain, for both modular and single-header builds.

Covers the NotEqualFunction spec invariants:
- DomainGatingIdenticalToEqual (REQ-FN-002H)
- AdjacentPairsAndUnaryOperandCompileTimeGated (REQ-FN-002H)
- UnaryFormReturnsFalseForSingleSupportedDomainOperand (REQ-FN-002H)

The rejection set is deliberately identical to `equal`'s
(scripts/check-equal-compile-failures.py) because `not_equal` delegates to
`equal` under the same compile-time domain gating.
"""

from __future__ import annotations

import os
import re
import subprocess

FAILURE_CASES = {
    # Floating-point rejection at any depth.
    "NotEqual-float-scalar": "(void)cljonic::not_equal(1.0, 1.0);",
    "NotEqual-float-collection": ("(void)cljonic::not_equal(cljonic::Vector<double, 2>{}, "
                                  "cljonic::Vector<double, 2>{});"),
    "NotEqual-float-map-value": ("(void)cljonic::not_equal(cljonic::Map<int, double, 2>{}, "
                                 "cljonic::Map<int, double, 2>{});"),
    "NotEqual-float-producer": ("(void)cljonic::not_equal(cljonic::Repeat<double>{}, "
                                "cljonic::Repeat<double>{});"),
    "NotEqual-float-nested-variant": ("(void)cljonic::not_equal(std::variant<int, double>{1}, "
                                      "std::variant<int, double>{1});"),
    # Cross-family pairs.
    "NotEqual-map-vs-vector": "(void)cljonic::not_equal(cljonic::Map<int, int, 2>{}, cljonic::Vector<int, 2>{});",
    "NotEqual-set-vs-vector": "(void)cljonic::not_equal(cljonic::Set<int, 2>{}, cljonic::Vector<int, 2>{});",
    "NotEqual-string-vs-vector": "(void)cljonic::not_equal(cljonic::String<4>{}, cljonic::Vector<int, 2>{});",
    "NotEqual-map-vs-set": "(void)cljonic::not_equal(cljonic::Map<int, int, 2>{}, cljonic::Set<int, 2>{});",
    "NotEqual-string-vs-map": "(void)cljonic::not_equal(cljonic::String<4>{}, cljonic::Map<int, int, 2>{});",
    "NotEqual-set-vs-cycle": ("(void)cljonic::not_equal(cljonic::Set<int, 2>{}, "
                              "cljonic::cycle(cljonic::Vector<int, 2>{}));"),
    # Mixed cljonic vs non-cljonic pairs.
    "NotEqual-vector-vs-scalar": "(void)cljonic::not_equal(cljonic::Vector<int, 2>{1}, 1);",
    "NotEqual-scalar-vs-map": "(void)cljonic::not_equal(1, cljonic::Map<int, int, 2>{});",
    "NotEqual-string-vs-char-literal": "(void)cljonic::not_equal(cljonic::String<4>{\"a\"}, 'a');",
    # Standard-library interop-surface types are outside the equality domain.
    "NotEqual-std-vector": "(void)cljonic::not_equal(std::vector<int>{1}, std::vector<int>{1});",
    "NotEqual-std-string": "(void)cljonic::not_equal(std::string{\"a\"}, std::string{\"a\"});",
    "NotEqual-std-span": "(void)cljonic::not_equal(std::span<int>{}, std::span<int>{});",
    "NotEqual-std-string-view": ("(void)cljonic::not_equal(std::string_view{\"a\"}, "
                                 "std::string_view{\"a\"});"),
    "NotEqual-std-map": "(void)cljonic::not_equal(std::map<int, int>{}, std::map<int, int>{});",
    # The standard-library variant is not a cljonic value type.
    "NotEqual-std-variant": ("(void)cljonic::not_equal(std::variant<int, long>{1}, "
                             "std::variant<int, long>{1});"),
    # Cross-type numeric and element-type rules.
    "NotEqual-scalar-cross-type": "(void)cljonic::not_equal(1, 1L);",
    "NotEqual-sequential-element-cross-type": ("(void)cljonic::not_equal(cljonic::Vector<int, 2>{1}, "
                                               "cljonic::Range<long>{1, 3, 1});"),
    "NotEqual-map-key-cross-type": ("(void)cljonic::not_equal(cljonic::Map<int, int, 2>{}, "
                                    "cljonic::Map<long, int, 2>{});"),
    # Callables are outside the stable-equality domain.
    "NotEqual-callable-bearing-producer": ("(void)cljonic::not_equal(cljonic::Iterate<int, int (*)(int)>{}, "
                                           "cljonic::Iterate<int, int (*)(int)>{});"),
    # Unary arity: only domain-admitted operands, float and interop rejected.
    "NotEqual-unary-float": "(void)cljonic::not_equal(1.0);",
    "NotEqual-unary-std-vector": "(void)cljonic::not_equal(std::vector<int>{});",
    "NotEqual-unary-std-string": "(void)cljonic::not_equal(std::string{});",
    "NotEqual-unary-float-collection": "(void)cljonic::not_equal(cljonic::Vector<double, 2>{});",
    "NotEqual-unary-float-map": "(void)cljonic::not_equal(cljonic::Map<int, double, 2>{});",
    # Variadic arity: every adjacent pair must be individually admissible.
    "NotEqual-variadic-mixed-scalar-collection": ("(void)cljonic::not_equal(1, cljonic::Vector<int, 2>{1}, 2);"),
    "NotEqual-variadic-cross-family-position": ("(void)cljonic::not_equal(1, 2, cljonic::Vector<int, 2>{});"),
    "NotEqual-variadic-cross-family-tail": ("(void)cljonic::not_equal(cljonic::Vector<int, 2>{1}, "
                                            "cljonic::Vector<int, 2>{1}, cljonic::Set<int, 2>{});"),
    "NotEqual-variadic-float-adjacent": "(void)cljonic::not_equal(1, 1.0, 1);",
    "NotEqual-variadic-element-cross-type": ("(void)cljonic::not_equal(cljonic::Vector<int, 2>{1}, "
                                             "cljonic::Range<long>{1, 3, 1}, cljonic::Vector<int, 2>{1});"),
    "NotEqual-variadic-std-range-tail": ("(void)cljonic::not_equal(1, 1, std::vector<int>{1});"),
    "NotEqual-variadic-mixed-family-chain": ("(void)cljonic::not_equal(cljonic::Map<int, int, 4>"
                                             "{cljonic::MapEntry<int, int>{1, 10}}, "
                                             "cljonic::Map<int, int, 4>{cljonic::MapEntry<int, int>{1, 10}}, "
                                             "cljonic::Set<int, 4>{1, 2}, cljonic::Set<int, 8>{2, 1});"),
}

PASS_CASES = {
    "NotEqual-scalar": "(void)cljonic::not_equal(1, 1);",
    "NotEqual-enum": ("enum class NotEqualColor { Red, Green }; "
                      "(void)cljonic::not_equal(NotEqualColor::Red, NotEqualColor::Red);"),
    "NotEqual-cljonic-variant": ("(void)cljonic::not_equal(cljonic::Variant<int, long>{1}, "
                                 "cljonic::Variant<int, long>{1});"),
    "NotEqual-map-entry": ("(void)cljonic::not_equal(cljonic::MapEntry<int, int>{1, 2}, "
                           "cljonic::MapEntry<int, int>{1, 2});"),
    "NotEqual-vector-vs-range": ("(void)cljonic::not_equal(cljonic::Vector<int, 4>{1, 2, 3}, "
                                 "cljonic::Range<int>{1, 4, 1});"),
    "NotEqual-vector-vs-queue": ("(void)cljonic::not_equal(cljonic::Vector<int, 4>{1, 2, 3}, "
                                 "cljonic::Queue<int, 4>{1, 2, 3});"),
    "NotEqual-vector-vs-repeat": ("(void)cljonic::not_equal(cljonic::Vector<int, 4>{1, 1, 1}, "
                                  "cljonic::Repeat<int>{1, 3U});"),
    "NotEqual-repeat-unbounded-pair": ("(void)cljonic::not_equal(cljonic::Repeat<int>{7}, "
                                       "cljonic::Repeat<int>{7});"),
    "NotEqual-cycle-vs-vector-early-exit": ("(void)cljonic::not_equal(cljonic::cycle(cljonic::Vector<int, 2>{1, 2}), "
                                            "cljonic::Vector<int, 4>{1, 2, 3});"),
    "NotEqual-map-pair": ("(void)cljonic::not_equal(cljonic::Map<int, int, 4>{cljonic::MapEntry<int, int>{1, 10}}, "
                          "cljonic::Map<int, int, 8>{cljonic::MapEntry<int, int>{1, 10}});"),
    "NotEqual-set-pair": ("(void)cljonic::not_equal(cljonic::Set<int, 4>{1, 2}, cljonic::Set<int, 8>{2, 1});"),
    "NotEqual-string-pair": ("(void)cljonic::not_equal(cljonic::String<8>{\"abc\"}, cljonic::String<4>{\"abc\"});"),
    "NotEqual-aggregate": ("NotEqualPixel a{1, 2}; NotEqualPixel b{1, 2}; (void)cljonic::not_equal(a, b);"),
    "NotEqual-nested-producer-parameter-equality": ("(void)cljonic::not_equal(cljonic::Vector<cljonic::Repeat<int>, 2>"
                                                    "{cljonic::Repeat<int>{7, 3U}}, "
                                                    "cljonic::Vector<cljonic::Repeat<int>, 2>"
                                                    "{cljonic::Repeat<int>{7, 3U}});"),
    "NotEqual-unary-scalar": "(void)cljonic::not_equal(1);",
    "NotEqual-unary-collection": "(void)cljonic::not_equal(cljonic::Vector<int, 2>{});",
    "NotEqual-unary-producer": "(void)cljonic::not_equal(cljonic::Repeat<int>{7});",
    "NotEqual-variadic-scalar": "(void)cljonic::not_equal(1, 1, 1, 1);",
    "NotEqual-variadic-sequential": ("(void)cljonic::not_equal(cljonic::Vector<int, 4>{1, 2, 3}, "
                                     "cljonic::Range<int>{1, 4, 1}, cljonic::Queue<int, 4>{1, 2, 3});"),
    "NotEqual-variadic-associative": ("(void)cljonic::not_equal("
                                      "cljonic::Map<int, int, 4>{cljonic::MapEntry<int, int>{1, 10}}, "
                                      "cljonic::Map<int, int, 8>{cljonic::MapEntry<int, int>{1, 10}}, "
                                      "cljonic::Map<int, int, 4>{cljonic::MapEntry<int, int>{1, 10}});"),
}

# Out-of-domain calls must report the targeted RejectionDiagnostic message
# (REQ-DIAG-009) rather than a raw list of rejected concept candidates. The
# message content is asserted, not just its stability: REQ-DIAG-009 requires the
# fallback to name the operation and the violated domain rule. The taxonomy is
# shared with scripts/check-equal-compile-failures.py.
DIAGNOSTIC_ANCHOR = "outside the supported equality domain"
DIAGNOSTIC_OPERATION = "cljonic::not_equal:"
DIAGNOSTIC_SHARED_DOMAIN = "shares with cljonic::equal"
DIAGNOSTIC_TAXONOMY = (
    "Floating-point values",
    "callables",
    "pointers",
    "unscoped enums",
    "standard-library range and container types",
    "the standard-library variant",
)

# name -> (probe body, case-specific required substrings)
DIAGNOSTIC_CASES = {
    "NotEqual-unary-diagnostic": ("(void)cljonic::not_equal(1.0);", ()),
    "NotEqual-binary-diagnostic": (
        "(void)cljonic::not_equal(cljonic::Vector<int, 2>{1}, 1);",
        ("mutually comparable cljonic family pair",
         "sequential [Vector, Queue, and all producers]"),
    ),
    "NotEqual-variadic-diagnostic": (
        "(void)cljonic::not_equal(1, cljonic::Vector<int, 2>{1}, 2);",
        ("With three or more operands",
         "sequential [Vector, Queue, and all producers]"),
    ),
}

# Injected before main(): an aggregate-like struct with stable equality.
PREAMBLE = """
struct NotEqualPixel {
    int x;
    int y;
    [[nodiscard]] constexpr auto operator==(const NotEqualPixel& other) const noexcept -> bool {
        return x == other.x && y == other.y;
    }
};
"""


def build_source(header: str, preamble: str, body: str) -> str:
    return "\n".join(
        (
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


def compile_case(compiler: list[str], include_dir: str, header: str, body: str, preamble: str = "") -> bool:
    source = build_source(header, preamble, body)
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
    compiler: list[str], include_dir: str, header: str, body: str, required: tuple[str, ...]
) -> bool:
    source = build_source(header, "", body)
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
    needles = (
        DIAGNOSTIC_OPERATION,
        DIAGNOSTIC_ANCHOR,
        DIAGNOSTIC_SHARED_DOMAIN,
        *DIAGNOSTIC_TAXONOMY,
        *required,
    )
    return all(_normalized(needle) in output for needle in needles)


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

        for name, (body, required) in DIAGNOSTIC_CASES.items():
            if not diagnostic_message_reported(compiler, include_dir, header, body, required):
                failures.append(f"{configuration}/{name}: targeted rejection diagnostic content not reported")

    if failures:
        print("not-equal-compile-fail:failures:")
        print("\n".join(f"  {failure}" for failure in failures))
        return 1

    print("not-equal-compile-fail:ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())