#!/usr/bin/env python3
"""Verify conversion boundaries against modular and generated headers."""

from __future__ import annotations

import os
import shlex
import subprocess
from pathlib import Path


def rejection_cases() -> dict[str, str]:
    cases = {
        "vector-pack": "(void)cljonic::Vector<short, 2>{1};",
        "set-pack": "(void)cljonic::Set<int, 2>{true};",
        "queue-pack": "(void)cljonic::Queue<int, 2>{'x'};",
        "entry-key": "(void)cljonic::MapEntry<int, int>{1.5, 2};",
        "entry-value": "(void)cljonic::MapEntry<int, int>{1, true};",
        "range": "(void)cljonic::Range<short>(1);",
        "repeat-value": "(void)cljonic::Repeat<short>(1);",
        "string-pointer": 'const char* text = "abc"; (void)cljonic::String<4>{text};',
        "string-conversion": "(void)cljonic::String<4>{TextConversion{}};",
        "range-proxy": "(void)cljonic::Vector<int, 2>{ProxyRange{}};",
        "range-value-reference-mismatch": "(void)cljonic::Vector<long long, 2>{MismatchedRange{}};",
        "throwing-range": "(void)cljonic::Vector<int, 2>{ThrowingRange{}};",
        "throwing-transform": (
            "int data[] = {1, 2}; auto source = data | std::views::transform([](int x) { return x; }); "
            "(void)cljonic::Vector<int, 2>{source};"
        ),
    }
    for collection in ("Vector", "Set", "Queue"):
        cases[f"{collection}-source"] = (
            f"int data[] = {{1, 2}}; (void)cljonic::{collection}<short, 2>{{std::span{{data}}}};"
        )
    for collection in ("Vector<int, 2>", "Queue<int, 2>", "Set<int, 2>", "String<2>"):
        bad_value = "256" if collection.startswith("String") else "1.5"
        for operation in ("conj", "can_conj"):
            cases[f"{collection}-{operation}"] = f"(void)cljonic::{operation}(cljonic::{collection}{{}}, {bad_value});"
            cases[f"{collection}-{operation}-member"] = f"(void)cljonic::{collection}{{}}.{operation}({bad_value});"
    for operation in ("assoc", "can_assoc"):
        for collection in ("Vector<int, 2>", "String<2>", "Map<int, int, 2>"):
            cases[f"{collection}-{operation}"] = f"(void)cljonic::{operation}(cljonic::{collection}{{}}, 0, 1.5);"
    for operation in ("contains", "get", "dissoc"):
        cases[f"map-{operation}"] = f"(void)cljonic::{operation}(cljonic::Map<int, int, 2>{{}}, 1.5);"
        cases[f"map-{operation}-user-conversion"] = (
            f"(void)cljonic::{operation}(cljonic::Map<int, int, 2>{{}}, IntConversion{{}});"
        )
    for collection in ("Vector<int, 2>", "Map<int, int, 2>", "Set<int, 2>", "String<2>"):
        cases[f"{collection}-fallback"] = f"(void)cljonic::get(cljonic::{collection}{{}}, 0, 1.5);"
    for operation in ("into", "fits_into"):
        cases[f"materialization-{operation}"] = (
            f"(void)cljonic::{operation}(cljonic::Vector<short, 2>{{}}, cljonic::Range<int>{{0, 2}});"
        )
    for count in ("1.5", "true", "'a'", "IntConversion{}"):
        cases[f"repeat-count-{count}"] = f"(void)cljonic::repeat({count}, 1);"
        cases[f"repeatedly-count-{count}"] = f"(void)cljonic::repeatedly({count}, []() noexcept {{ return 1; }});"
        cases[f"Repeat-count-{count}"] = f"(void)cljonic::Repeat<int>({count}, 1);"
        cases[f"Repeat-deduced-count-{count}"] = f"(void)cljonic::Repeat{{{count}, 1}};"
        cases[f"Repeatedly-constructor-count-{count}"] = (
            f"const auto step = []() noexcept {{ return 1; }}; "
            f"(void)cljonic::Repeatedly<int, decltype(step)>{{step, {count}}};"
        )
    cases["variadic-assoc"] = "(void)cljonic::assoc(cljonic::Map<int, int, 2>{}, 0, 1, 1.5, 2);"
    cases["variadic-dissoc"] = "(void)cljonic::dissoc(cljonic::Map<int, int, 2>{}, 0, 1.5);"
    cases["variadic-disj"] = "(void)cljonic::disj(cljonic::Set<int, 2>{}, 0, 1.5);"
    return cases


PREAMBLE = """
#include <cstddef>
#include <iterator>
#include <span>
#include <ranges>
#include <string_view>
struct IntConversion { operator int() const noexcept { return 1; } };
struct TextConversion { operator std::string_view() const noexcept { return "abc"; } };
struct ProxyRange {
    IntConversion values[2];
    auto begin() noexcept { return values; }
    auto end() noexcept { return values + 2; }
};
struct MismatchedIterator {
    using value_type = short;
    using difference_type = std::ptrdiff_t;
    using iterator_concept = std::input_iterator_tag;
    double* current{};
    auto operator*() const noexcept -> double& { return *current; }
    auto operator++() noexcept -> MismatchedIterator& { ++current; return *this; }
    void operator++(int) noexcept { ++current; }
    auto operator==(const MismatchedIterator&) const noexcept -> bool = default;
};
struct MismatchedRange {
    double values[2];
    auto begin() noexcept -> MismatchedIterator { return {values}; }
    auto end() noexcept -> MismatchedIterator { return {values + 2}; }
};
static_assert(std::ranges::input_range<MismatchedRange>);
static_assert(std::same_as<std::ranges::range_value_t<MismatchedRange>, short>);
static_assert(std::same_as<std::ranges::range_reference_t<MismatchedRange>, double&>);
struct ThrowingRange {
    int values[2];
    auto begin() { return values; }
    auto end() { return values + 2; }
};
"""


def main() -> int:
    root = Path(__file__).resolve().parent.parent
    compiler = shlex.split(os.environ.get("CXX", "c++"))
    failures = []
    for label, include_dir, header in (
        ("modular", root / "src", "cljonic-core.hpp"),
        ("single-header", root, "cljonic.hpp"),
    ):
        smoke = f'#include "{header}"\n{PREAMBLE}\nint main() {{ cljonic::Vector<long long, 2> v{{short{{1}}}}; return cljonic::get(v, 0) != 1; }}\n'
        baseline = subprocess.run(
            [*compiler, "-std=c++23", "-fsyntax-only", "-I", str(include_dir), "-x", "c++", "-"],
            input=smoke, text=True, capture_output=True, check=False,
        )
        if baseline.returncode != 0:
            print(f"{label}: positive compilation control failed\n{baseline.stderr}")
            return 1
        for name, body in rejection_cases().items():
            source = f'#include "{header}"\n{PREAMBLE}\nint main() {{ {body} }}\n'
            result = subprocess.run(
                [*compiler, "-std=c++23", "-fsyntax-only", "-I", str(include_dir), "-x", "c++", "-"],
                input=source, text=True, capture_output=True, check=False,
            )
            if result.returncode == 0:
                failures.append(f"{label}/{name}: unsafe conversion unexpectedly compiled")
    if failures:
        print("\n".join(failures))
        return 1
    print(f"implicit-conversion-compile-fail:ok:cases={2 * len(rejection_cases())}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
