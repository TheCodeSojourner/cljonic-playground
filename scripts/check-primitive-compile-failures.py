#!/usr/bin/env python3
"""Check collection primitive rejection diagnostics and admitted calls."""

from __future__ import annotations

import os
import re
import subprocess

PRIMITIVES = {
    "count": {
        "diagnostic_anchor": "cljonic::count:",
        "diagnostic_cases": {
            "unsupported-scalar": (
                "(void)cljonic::count(42);",
                "expected a Vector, Map, Set, Queue, String, Range, Repeat, Cycle, Iterate, or Repeatedly value",
            ),
        },
        "pass_cases": {
            "vector-collection": "(void)cljonic::count(cljonic::Vector<int, 4>{1, 2});",
            "range-producer": "(void)cljonic::count(cljonic::Range<int>{0, 5});",
        },
    },
    "is_empty": {
        "diagnostic_anchor": "cljonic::is_empty:",
        "diagnostic_cases": {
            "unsupported-scalar": (
                "(void)cljonic::is_empty(42);",
                "value must be a collection or producer",
            ),
        },
        "pass_cases": {
            "vector-empty": "(void)cljonic::is_empty(cljonic::Vector<int, 4>{});",
            "queue-empty": "(void)cljonic::is_empty(cljonic::Queue<int, 4>{});",
            "string-empty": "(void)cljonic::is_empty(cljonic::String<4>{});",
            "empty-range-producer": "(void)cljonic::is_empty(cljonic::Range<int>{0, 0});",
            "nonempty-range-producer": "(void)cljonic::is_empty(cljonic::Range<int>{0, 5});",
        },
    },
    "empty": {
        "diagnostic_anchor": "cljonic::empty:",
        "diagnostic_cases": {
            "unsupported-scalar": (
                "(void)cljonic::empty(42);",
                "expected a Vector, Map, Set, Queue, or String value",
            ),
            "unsupported-producer": (
                "(void)cljonic::empty(cljonic::Range<int>{0, 5});",
                "expected a Vector, Map, Set, Queue, or String value",
            ),
        },
        "pass_cases": {
            "vector": "(void)cljonic::empty(cljonic::Vector<int, 4>{});",
            "map": "(void)cljonic::empty(cljonic::Map<int, int, 4>{});",
            "set": "(void)cljonic::empty(cljonic::Set<int, 4>{});",
            "queue": "(void)cljonic::empty(cljonic::Queue<int, 4>{});",
            "string": "(void)cljonic::empty(cljonic::String<4>{});",
        },
    },
    "first": {
        "diagnostic_anchor": "cljonic::first:",
        "diagnostic_cases": {
            "unsupported-scalar": (
                "(void)cljonic::first(42);",
                "expected a Vector, Map, Set, Queue, String, or MapEntry value",
            ),
            "unsupported-producer": (
                "(void)cljonic::first(cljonic::Range<int>{0, 5});",
                "expected a Vector, Map, Set, Queue, String, or MapEntry value",
            ),
        },
        "pass_cases": {
            "vector": "(void)cljonic::first(cljonic::Vector<int, 4>{10, 20});",
            "map": "(void)cljonic::first(cljonic::Map<int, int, 4>{cljonic::MapEntry<int, int>{1, 100}});",
            "set": "(void)cljonic::first(cljonic::Set<int, 4>{7});",
            "queue": "(void)cljonic::first(cljonic::Queue<int, 4>{5, 6});",
            "string": "(void)cljonic::first(cljonic::String<4>{\"AB\"});",
            "map-entry": "(void)cljonic::first(cljonic::MapEntry<int, int>{1, 100});",
        },
    },
    "dissoc": {
        "diagnostic_anchor": "cljonic::dissoc:",
        "diagnostic_cases": {
            "non-map-zero-keys": (
                "(void)cljonic::dissoc(cljonic::Set<int, 4>{});",
                "collection must be a Map; only Maps support dissoc",
            ),
            "non-map-with-key": (
                "(void)cljonic::dissoc(cljonic::Set<int, 4>{}, 1);",
                "first argument must be a Map; only Maps support dissoc",
            ),
            "non-map-multiple-keys": (
                "(void)cljonic::dissoc(cljonic::Set<int, 4>{}, 1, 2);",
                "first argument must be a Map; only Maps support dissoc",
            ),
            "non-convertible-key": (
                "struct DissocNotConvertible {}; (void)cljonic::dissoc(cljonic::Map<int, int, 4>{}, DissocNotConvertible{});",
                "each key must convert safely to the Map's key type",
            ),
            "non-convertible-trailing-key": (
                "struct DissocNotConvertible {}; (void)cljonic::dissoc(cljonic::Map<int, int, 4>{}, 1, DissocNotConvertible{});",
                "each key must convert safely to the Map's key type",
            ),
        },
        "pass_cases": {
            "zero-keys": "(void)cljonic::dissoc(cljonic::Map<int, int, 4>{});",
            "present-map-key": "(void)cljonic::dissoc(cljonic::Map{cljonic::MapEntry{1, 10}}, 1);",
            "absent-map-key": "(void)cljonic::dissoc(cljonic::Map<int, int, 4>{}, 1);",
            "multiple-keys": "(void)cljonic::dissoc(cljonic::Map{cljonic::MapEntry{1, 10}, cljonic::MapEntry{2, 20}}, 1, short{2});",
        },
    },
    "disj": {
        "diagnostic_anchor": "cljonic::disj:",
        "diagnostic_cases": {
            "non-set-collection": (
                "(void)cljonic::disj(cljonic::Vector<int, 4>{}, 1);",
                "first argument must be a Set",
            ),
            "lossy-value": (
                "(void)cljonic::disj(cljonic::Set<short, 4>{}, 1);",
                "must convert safely to the set's element type",
            ),
        },
        "pass_cases": {
            "widening-value": "(void)cljonic::disj(cljonic::Set<int, 4>{}, short{1});",
            "zero-values": "(void)cljonic::disj(cljonic::Set<int, 4>{1});",
            "one-value": "(void)cljonic::disj(cljonic::Set<int, 4>{1}, 1);",
            "multiple-values": "(void)cljonic::disj(cljonic::Set<int, 4>{1, 2}, 1, 2);",
            "repeated-values": "(void)cljonic::disj(cljonic::Set<int, 4>{1, 2}, 1, 2, 1);",
        },
    },
    "contains": {
        "diagnostic_anchor": "cljonic::contains:",
        "diagnostic_cases": {
            "unsupported-range": (
                "(void)cljonic::contains(cljonic::Range<int>{0, 5}, 1U);",
                "first argument must be a Map, Set, Vector, or String",
            ),
            "unsupported-queue": (
                "(void)cljonic::contains(cljonic::Queue<int, 4>{}, 1);",
                "first argument must be a Map, Set, Vector, or String",
            ),
            "map-lossy-key": (
                "(void)cljonic::contains(cljonic::Map<short, int, 4>{}, 1);",
                "must convert safely to the map's key type or the set's element type",
            ),
            "set-lossy-element": (
                "(void)cljonic::contains(cljonic::Set<short, 4>{}, 1);",
                "must convert safely to the map's key type or the set's element type",
            ),
            "vector-non-integral-index": (
                "(void)cljonic::contains(cljonic::Vector<int, 4>{}, 1.5);",
                "index must be a non-boolean, non-character integer type",
            ),
            "string-non-integral-index": (
                "(void)cljonic::contains(cljonic::String<4>{}, 1.5);",
                "index must be a non-boolean, non-character integer type",
            ),
        },
        "pass_cases": {
            "map-declared-key": "(void)cljonic::contains(cljonic::Map<int, int, 4>{}, 1);",
            "map-widening-key": "(void)cljonic::contains(cljonic::Map<int, int, 4>{}, short{1});",
            "set-widening-value": "(void)cljonic::contains(cljonic::Set<int, 4>{}, short{1});",
            "set-declared-element": "(void)cljonic::contains(cljonic::Set<int, 4>{}, 1);",
            "vector-integral-index": "(void)cljonic::contains(cljonic::Vector<int, 4>{}, -1);",
            "string-integral-index": "(void)cljonic::contains(cljonic::String<4>{}, 1U);",
        },
    },
}


def build_source(header: str, body: str) -> str:
    return "\n".join((f'#include "{header}"', "int main() {", f"  {body}", "  return 0;", "}"))


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
        for primitive, cases in PRIMITIVES.items():
            anchor = cases["diagnostic_anchor"]
            for name, (body, message) in cases["diagnostic_cases"].items():
                result = compile_source(compiler, include_dir, build_source(header, body))
                if result.returncode == 0:
                    failures.append(f"{configuration}/{primitive}/{name}: expected compile failure, but compiled")
                    continue
                output = normalized(result.stdout + result.stderr)
                if anchor not in output or normalized(message) not in output:
                    failures.append(f"{configuration}/{primitive}/{name}: targeted diagnostic content not reported")

            for name, body in cases["pass_cases"].items():
                result = compile_source(compiler, include_dir, build_source(header, body))
                if result.returncode != 0:
                    failures.append(f"{configuration}/{primitive}/{name}: expected to compile, but failed")

    if failures:
        print("primitive-compile-fail:failures:")
        print("\n".join(f"  {failure}" for failure in failures))
        return 1

    print("primitive-compile-fail:ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())