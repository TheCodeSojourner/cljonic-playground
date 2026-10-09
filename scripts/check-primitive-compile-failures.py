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
                "each key must be convertible to the Map's key type",
            ),
            "non-convertible-trailing-key": (
                "struct DissocNotConvertible {}; (void)cljonic::dissoc(cljonic::Map<int, int, 4>{}, 1, DissocNotConvertible{});",
                "each key must be convertible to the Map's key type",
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
            "convertible-but-not-exact-value": (
                "(void)cljonic::disj(cljonic::Set<int, 4>{}, short{1});",
                "must have exactly the same type as the set's elements",
            ),
        },
        "pass_cases": {
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
            "map-convertible-key": (
                "(void)cljonic::contains(cljonic::Map<int, int, 4>{}, short{1});",
                "must have exactly the same type as the map's keys or the set's elements",
            ),
            "set-convertible-element": (
                "(void)cljonic::contains(cljonic::Set<int, 4>{}, short{1});",
                "must have exactly the same type as the map's keys or the set's elements",
            ),
            "vector-non-integral-index": (
                "(void)cljonic::contains(cljonic::Vector<int, 4>{}, 1.5);",
                "index must be an integer type",
            ),
            "string-non-integral-index": (
                "(void)cljonic::contains(cljonic::String<4>{}, 1.5);",
                "index must be an integer type",
            ),
        },
        "pass_cases": {
            "map-declared-key": "(void)cljonic::contains(cljonic::Map<int, int, 4>{}, 1);",
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