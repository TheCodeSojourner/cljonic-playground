#!/usr/bin/env python3
"""Verify the REQ-FN-027A source-construction admission contract.

A cljonic collection or producer is NOT a direct source constructor argument:
only a non-cljonic C++ range/view imports into a collection's source
constructor. A cljonic value that is not exactly the destination's element type
must fail to compile (materialization is the role of into, preflighted by
fits_into). A cljonic value whose type is exactly the element type is admitted
as one element (pack construction), and a sole argument in a
class-template-argument-deduction form is enclosed as one element.

Both the rejected and the admitted forms are controlled -- positive cases assert
compile-time behavior, not just successful compilation -- so a trivially-passing
harness fails.
"""

from __future__ import annotations

import os
import re
import subprocess

# Every rejected form must fail with the REQ-DIAG-010 targeted diagnostic
# (content-asserted, whitespace-normalized) rather than a generic element
# conversion error. Each case also pins the collection's operation token so a
# rejection routed through the wrong collection's overload cannot pass.
REQUIRED_MESSAGE_ANCHOR = "you cannot construct a"

# name -> (probe body, expected diagnostic operation token). Each entry must
# FAIL to compile: a cljonic collection or producer whose type is not the
# destination's element type is not an admitted source (REQ-FN-027A).
FAILURE_CASES = {
    "vector-from-vector": ("  cljonic::Vector<int, 4> value{cljonic::Vector<int, 2>{1, 2}}; (void)value;",
                           "cljonic::Vector:"),
    "vector-from-producer": ("  cljonic::Vector<int, 4> value{cljonic::Range<int>{0, 10, 1}}; (void)value;",
                             "cljonic::Vector:"),
    "set-from-set": ("  cljonic::Set<int, 4> value{cljonic::Set<int, 2>{1, 2}}; (void)value;",
                     "cljonic::Set:"),
    "set-from-producer": ("  cljonic::Set<int, 4> value{cljonic::Range<int>{0, 10, 1}}; (void)value;",
                          "cljonic::Set:"),
    "queue-from-queue": ("  cljonic::Queue<int, 4> value{cljonic::Queue<int, 2>{1, 2}}; (void)value;",
                         "cljonic::Queue:"),
    "queue-from-producer": ("  cljonic::Queue<int, 4> value{cljonic::Repeat<int>{7, 2U}}; (void)value;",
                            "cljonic::Queue:"),
    "map-from-map": ("  cljonic::Map<int, int, 4> value{cljonic::Map<int, int, 2>"
                     "{cljonic::MapEntry<int, int>{1, 2}}}; (void)value;",
                     "cljonic::Map:"),
    "map-from-entry-vector": ("  cljonic::Map<int, int, 4> value{cljonic::Vector<cljonic::MapEntry<int, int>, 2>"
                              "{cljonic::MapEntry<int, int>{1, 2}}}; (void)value;",
                              "cljonic::Map:"),
    "map-from-producer": ("  cljonic::Map<int, int, 4> value{cljonic::cycle("
                          "cljonic::Vector<cljonic::MapEntry<int, int>, 2>"
                          "{cljonic::MapEntry<int, int>{1, 2}})}; (void)value;",
                          "cljonic::Map:"),
    "string-from-string": ("  cljonic::String<4> value{cljonic::String<2>{\"ab\"}}; (void)value;",
                           "cljonic::String:"),
    "string-from-producer": ("  cljonic::String<4> value{cljonic::Repeat<char>{'a', 2U}}; (void)value;",
                             "cljonic::String:"),
}

# Each entry must COMPILE and its compile-time assertion must hold.
POSITIVE_CASES = {
    # SameTypeArgumentIsOneElement (explicit template argument list): a sole
    # cljonic collection or producer argument whose type is exactly the element
    # type is one element, never a source to materialize. This complements the
    # rejection set and guards the source constructor's element-type admission.
    "same-type-vector": ("  static_assert(cljonic::Vector<cljonic::Vector<int, 2>, 4>"
                         "{cljonic::Vector<int, 2>{1, 2}}.count() == 1U);"),
    "same-type-set": ("  static_assert(cljonic::Set<cljonic::Set<int, 2>, 4>"
                      "{cljonic::Set<int, 2>{1, 2}}.count() == 1U);"),
    "same-type-queue": ("  static_assert(cljonic::Queue<cljonic::Queue<int, 2>, 4>"
                        "{cljonic::Queue<int, 2>{1, 2}}.count() == 1U);"),
    "same-type-vector-producer-element": ("  static_assert(cljonic::Vector<cljonic::Range<int>, 4>"
                                          "{cljonic::Range<int>{0, 3, 1}}.count() == 1U);"),
    "same-type-queue-producer-element": ("  static_assert(cljonic::Queue<cljonic::Repeat<int>, 4>"
                                         "{cljonic::Repeat<int>{7, 2U}}.count() == 1U);"),
    "same-type-map-entry": ("  static_assert(cljonic::Map<int, int, 4>"
                            "{cljonic::MapEntry<int, int>{1, 2}}.count() == 1U);"),
    # EnclosureConstruction (CTAD, no explicit template argument list): a sole
    # cljonic collection or producer argument deduces a one-element destination.
    # Guides exist for Vector/Set/Queue only; Map and String intentionally keep
    # copy deduction (see mementum/memories/ctad-copy-deduction-vs-enclosure-guides.md)
    # and are therefore not enclosure candidates.
    "enclosure-vector": "  static_assert(cljonic::Vector{cljonic::Vector<int, 2>{1, 2}}.count() == 1U);",
    "enclosure-set": "  static_assert(cljonic::Set{cljonic::Set<int, 2>{1, 2}}.count() == 1U);",
    "enclosure-queue": "  static_assert(cljonic::Queue{cljonic::Queue<int, 2>{1, 2}}.count() == 1U);",
    "enclosure-vector-producer": "  static_assert(cljonic::Vector{cljonic::Repeat<int>{7, 3U}}.count() == 1U);",
    "enclosure-queue-producer": "  static_assert(cljonic::Queue{cljonic::Repeat<int>{7, 3U}}.count() == 1U);",
    "enclosure-set-producer": "  static_assert(cljonic::Set{cljonic::Range<int>{0, 5, 1}}.count() == 1U);",
    "enclosure-vector-cross-class": "  static_assert(cljonic::Vector{cljonic::Set<int, 2>{1, 2}}.count() == 1U);",
    "enclosure-queue-cross-class": "  static_assert(cljonic::Queue{cljonic::Set<int, 2>{1, 2}}.count() == 1U);",
    # C++ interoperability: a non-cljonic range/view source is admitted and
    # copied into owned storage for every collection.
    "interop-vector-span": ("  static constexpr int values[2] = {1, 2};\n"
                            "  constexpr cljonic::Vector<int, 4> value{std::span<const int>{values}};\n"
                            "  static_assert(value.count() == 2U);"),
    "interop-set-span": ("  static constexpr int values[2] = {1, 2};\n"
                         "  constexpr cljonic::Set<int, 4> value{std::span<const int>{values}};\n"
                         "  static_assert(value.count() == 2U);"),
    "interop-queue-span": ("  static constexpr int values[2] = {1, 2};\n"
                           "  constexpr cljonic::Queue<int, 4> value{std::span<const int>{values}};\n"
                           "  static_assert(value.count() == 2U);"),
    "interop-string-span": ("  static constexpr char values[2] = {'a', 'b'};\n"
                            "  constexpr cljonic::String<4> value{std::span<const char>{values}};\n"
                            "  static_assert(value.count() == 2U);"),
    "interop-map-entry-span": ("  static constexpr cljonic::MapEntry<int, int> values[1] = {{1, 2}};\n"
                               "  constexpr cljonic::Map<int, int, 4> "
                               "value{std::span<const cljonic::MapEntry<int, int>>{values}};\n"
                               "  static_assert(value.count() == 1U);"),
}


def _normalized(text: str) -> str:
    return re.sub(r"\s+", " ", text)


def compile_snippet(compiler: list[str], include_dir: str, header: str, body: str) -> tuple[bool, str]:
    source = "\n".join(
        (
            "#include <span>",
            f'#include "{header}"',
            "int main() {",
            body,
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
    return result.returncode == 0, _normalized(result.stdout + result.stderr)


def main() -> int:
    compiler = os.environ.get("CXX", "c++").split()
    root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    configurations = (
        ("modular", os.path.join(root, "src"), "cljonic-core.hpp"),
        ("single-header", root, "cljonic.hpp"),
    )
    failures: list[str] = []

    for configuration, include_dir, header in configurations:
        for name, (body, operation) in FAILURE_CASES.items():
            compiled, output = compile_snippet(compiler, include_dir, header, body)
            if compiled:
                failures.append(f"{configuration}/{name} (expected rejection, compiled)")
            elif REQUIRED_MESSAGE_ANCHOR not in output:
                failures.append(f"{configuration}/{name} (rejected without the targeted diagnostic)")
            elif operation not in output:
                failures.append(f"{configuration}/{name} (rejection not routed through {operation})")
        for name, body in POSITIVE_CASES.items():
            compiled, _ = compile_snippet(compiler, include_dir, header, body)
            if not compiled:
                failures.append(f"{configuration}/{name} (expected success, failed)")

    if failures:
        print("source-construction-compile-fail:missing expected behavior:")
        print("\n".join(f"  {failure}" for failure in failures))
        return 1

    print("source-construction-compile-fail:ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
