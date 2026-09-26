#!/usr/bin/env python3
"""Verify that composite keys containing floating-point or callable components fail
during compilation, that producers with non-default-constructible components are
rejected in storage positions (REQ-VAL-017D), and that approved composite, storable
float-bearing, and producer-storage uses compile, for both modular and single-header
builds."""

from __future__ import annotations

import os
import subprocess

FAILURE_CASES = {
    "Map-variant-float": ("cljonic::Map<std::variant<int, double>, int, 4>", ""),
    "Set-variant-float": ("cljonic::Set<std::variant<int, float>, 4>", ""),
    "MapEntry-variant-float": ("cljonic::Set<cljonic::MapEntry<std::variant<int, double>, int>, 4>", ""),
    "Vector-float-key": ("cljonic::Set<cljonic::Vector<double, 3>, 4>", ""),
    "Map-vector-float-value": ("cljonic::Map<cljonic::Vector<double, 3>, int, 4>", ""),
    "Variant-callable": ("cljonic::Set<std::variant<int, int (*)(int)>, 4>", ""),
    "Iterate-callable": ("cljonic::Set<cljonic::Iterate<int, int (*)(int)>, 4>", ""),
    "Repeat-float-set-element": ("cljonic::Set<cljonic::Repeat<double>, 4>", ""),
    "Cycle-float-source-set-element": ("cljonic::Set<cljonic::Cycle<cljonic::Vector<double, 3>>, 4>", ""),
    "Variant-float-repeat": ("cljonic::Set<std::variant<int, cljonic::Repeat<double>>, 4>", ""),
    "Variant-iterate-callable": ("cljonic::Set<std::variant<int, cljonic::Iterate<int, int (*)(int)>>, 4>", ""),
}

PASS_CASES = {
    "Map-float-variant-value": "cljonic::Map<int, std::variant<int, double>, 4>{};",
    "Set-variant-collection": "cljonic::Set<std::variant<int, cljonic::Vector<int, 3>>, 4>{};",
    "Map-collection-key": "cljonic::Map<cljonic::Vector<int, 3>, int, 4>{};",
    "Vector-float-element": "cljonic::Vector<double, 4>{1.0, 2.0};",
    "Map-float-value": "cljonic::Map<int, double, 4>{};",
    "Set-scalar": "cljonic::Set<int, 4>{};",
    "Producer-stored-as-value": "cljonic::Vector<cljonic::Repeat<int>, 4>{cljonic::Repeat<int>{}, cljonic::Repeat<int>{7, 3U}};",
    "Producer-default-form": "cljonic::Repeat<int>{}; cljonic::Range<int>{}; cljonic::Iterate<int, AddOne>{}; cljonic::Repeatedly<int, Generate>{}; cljonic::Cycle<cljonic::Vector<int, 4>>{};",
    "Range-set-element": "cljonic::Set<cljonic::Range<int>, 4>{cljonic::Range<int>{1, 5, 2}, cljonic::Range<int>{1, 5, 3}};",
    "Range-map-key": "cljonic::Map<cljonic::Range<int>, int, 4>{cljonic::MapEntry<cljonic::Range<int>, int>{cljonic::Range<int>{1, 5, 2}, 10}};",
    "Repeat-set-element": "cljonic::Set<cljonic::Repeat<int>, 4>{cljonic::Repeat<int>{7, 3U}, cljonic::Repeat<int>{8, 2U}};",
    "Cycle-set-element": "cljonic::Set<cljonic::Cycle<cljonic::Vector<int, 4>>, 4>{cljonic::cycle(cljonic::Vector<int, 4>{1, 2, 3}), cljonic::cycle(cljonic::Vector<int, 4>{1, 2, 4})};",
    "Variant-producer-key": "cljonic::Set<std::variant<int, cljonic::Range<int>>, 4>{};",
    "Vector-of-producers-equality": "cljonic::Vector<cljonic::Range<int>, 4> a{cljonic::Range<int>{1, 5, 2}, cljonic::Range<int>{2, 4}}; cljonic::Vector<cljonic::Range<int>, 4> b{cljonic::Range<int>{1, 5, 2}, cljonic::Range<int>{2, 4}}; (void)(a == b);",
    "Parameters-equal-call": "(void)cljonic::parameters_equal(cljonic::Range<int>{1, 5, 2}, cljonic::Range<int>{1, 5, 2}); (void)cljonic::parameters_equal(cljonic::Repeat<int>{7, 3U}, cljonic::Repeat<int>{7, 3U}); (void)cljonic::parameters_equal(cljonic::cycle(cljonic::Vector<int, 4>{1, 2, 3}), cljonic::cycle(cljonic::Vector<int, 4>{1, 2, 3}));",
}

# Extra source lines injected before main() for each case (shared preamble):
PREAMBLE = """
struct NonDefaultStep {
    constexpr explicit NonDefaultStep(int) noexcept {}
    constexpr auto operator()(int v) const noexcept -> int { return v; }
};
struct NonDefaultGen {
    constexpr explicit NonDefaultGen(int) noexcept {}
    constexpr auto operator()() const noexcept -> int { return 0; }
};
struct AddOne {
    constexpr AddOne() noexcept = default;
    constexpr auto operator()(int v) const noexcept -> int { return v + 1; }
};
struct Generate {
    constexpr Generate() noexcept = default;
    constexpr auto operator()() const noexcept -> int { return 42; }
};
"""

# Producer storage rejection cases (REQ-VAL-017D): a non-default-constructible
# step cannot be stored in a bounded collection.
FAILURE_PREAMBLE_CASES = {
    "Iterate-non-default-step-storage": "cljonic::Vector<cljonic::Iterate<int, NonDefaultStep>, 2>{};",
    "Repeatedly-non-default-step-storage": "cljonic::Map<int, cljonic::Repeatedly<int, NonDefaultGen>, 2>{};",
}


def compile_case(compiler: list[str], include_dir: str, header: str, body: str, preamble: str = "") -> bool:
    source = "\n".join(
        (
            "#include <variant>",
            f'#include "{header}"',
            preamble,
            "int main() {",
            f"  {body}",
            "  (void)0;",
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


def main() -> int:
    compiler = os.environ.get("CXX", "c++").split()
    root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    configurations = (("modular", os.path.join(root, "src"), "cljonic-core.hpp"), ("single-header", root, "cljonic.hpp"))
    failures: list[str] = []

    for configuration, include_dir, header in configurations:
        for name, (declaration, _extra) in FAILURE_CASES.items():
            body = f"{declaration} _value{{}};"
            if compile_case(compiler, include_dir, header, body):
                failures.append(f"{configuration}/{name}: expected compile failure, but compiled")

        for name, body in FAILURE_PREAMBLE_CASES.items():
            if compile_case(compiler, include_dir, header, body, preamble=PREAMBLE):
                failures.append(f"{configuration}/{name}: expected compile failure, but compiled")

        for name, body in PASS_CASES.items():
            if not compile_case(compiler, include_dir, header, body, preamble=PREAMBLE):
                failures.append(f"{configuration}/{name}: expected to compile, but failed")

    if failures:
        print("variant-compile-fail:failures:")
        print("\n".join(f"  {failure}" for failure in failures))
        return 1

    print("variant-compile-fail:ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())