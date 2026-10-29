#!/usr/bin/env python3
"""Check collection primitive rejection diagnostics and admitted calls."""

from __future__ import annotations

import os
import argparse
import re
import subprocess

PRIMITIVES = {
    "get": {
        "diagnostic_anchor": "cljonic::get:",
        "diagnostic_cases": {
            "unsupported-scalar": (
                "(void)cljonic::get(42, 0);",
                "the first argument must be a Map, Set, Vector, or String",
            ),
            "unsupported-queue": (
                "(void)cljonic::get(cljonic::Queue<int, 4>{}, 0);",
                "the first argument must be a Map, Set, Vector, or String",
            ),
            "unsupported-producer": (
                "(void)cljonic::get(cljonic::Range<int>{0, 5}, 0U);",
                "the first argument must be a Map, Set, Vector, or String",
            ),
            "invalid-lookup-key": (
                "(void)cljonic::get(cljonic::Map<int, int, 4>{}, 1.5);",
                "the key must safely convert to the Map's key type or the Set's element type",
            ),
            "invalid-index": (
                "(void)cljonic::get(cljonic::Vector<int, 4>{}, 1.5);",
                "a Vector or String index must be a non-boolean, non-character integer type",
            ),
            "unsafe-fallback": (
                "(void)cljonic::get(cljonic::Vector<int, 4>{}, 0, 1.5);",
                "the fallback must safely convert to the collection's lookup result type",
            ),
        },
        "pass_cases": {
            "string-source": '(void)cljonic::into(cljonic::String<4>{}, cljonic::String<4>{"ab"});',
            "vector-char-source": "(void)cljonic::into(cljonic::String<4>{}, cljonic::Vector<char, 2>{'a', 'b'});",
            "vector-index": "(void)cljonic::get(cljonic::Vector<int, 4>{1, 2}, 0U);",
            "map-key": "(void)cljonic::get(cljonic::Map<int, long, 4>{}, short{1}, int{2});",
            "set-value": "(void)cljonic::get(cljonic::Set<int, 4>{1}, short{1});",
            "string-index": "(void)cljonic::get(cljonic::String<4>{\"ab\"}, 0U, 'x');",
        },
    },
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
    "peek": {
        "diagnostic_anchor": "cljonic::peek:",
        "diagnostic_cases": {
            "unsupported-scalar": (
                "(void)cljonic::peek(42);",
                "the first argument must be a Vector or Queue",
            ),
            "unsupported-set": (
                "(void)cljonic::peek(cljonic::Set<int, 4>{1});",
                "the first argument must be a Vector or Queue",
            ),
            "unsupported-producer": (
                "(void)cljonic::peek(cljonic::Range<int>{0, 5});",
                "the first argument must be a Vector or Queue",
            ),
        },
        "pass_cases": {
            "vector": "(void)cljonic::peek(cljonic::Vector<int, 4>{1, 2});",
            "queue": "(void)cljonic::peek(cljonic::Queue<int, 4>{1, 2});",
            "empty-vector": "(void)cljonic::peek(cljonic::Vector<int, 4>{});",
            "empty-queue": "(void)cljonic::peek(cljonic::Queue<int, 4>{});",
            "zero-capacity-vector": "(void)cljonic::peek(cljonic::Vector<int, 0>{});",
            "zero-capacity-queue": "(void)cljonic::peek(cljonic::Queue<int, 0>{});",
        },
    },
    "pop": {
        "diagnostic_anchor": "cljonic::pop:",
        "diagnostic_cases": {
            "unsupported-scalar": (
                "(void)cljonic::pop(42);",
                "the argument must be a Vector or Queue collection; scalar values, producers, and other collection "
                "types are not supported",
            ),
            "unsupported-set": (
                "(void)cljonic::pop(cljonic::Set<int, 4>{1});",
                "the argument must be a Vector or Queue collection; scalar values, producers, and other collection "
                "types are not supported",
            ),
            "unsupported-producer": (
                "(void)cljonic::pop(cljonic::Range<int>{0, 5});",
                "the argument must be a Vector or Queue collection; scalar values, producers, and other collection "
                "types are not supported",
            ),
        },
        "pass_cases": {
            "queue": "(void)cljonic::pop(cljonic::Queue<int, 4>{1, 2});",
        },
    },
    "rest": {
        "diagnostic_anchor": "cljonic::rest:",
        "diagnostic_cases": {
            "unsupported-scalar": (
                "(void)cljonic::rest(42);",
                "expected a Vector, Map, Set, Queue, String, Range, Repeat, Iterate, Repeatedly, or supported Cycle value",
            ),
            "unsupported-map-entry": (
                "(void)cljonic::rest(cljonic::MapEntry<int, int>{1, 2});",
                "expected a Vector, Map, Set, Queue, String, Range, Repeat, Iterate, Repeatedly, or supported Cycle value",
            ),
            "unsupported-external-range": (
                "int values[]{1, 2}; (void)cljonic::rest(values);",
                "expected a Vector, Map, Set, Queue, String, Range, Repeat, Iterate, Repeatedly, or supported Cycle value",
            ),
        },
        "pass_cases": {
            "vector": "(void)cljonic::rest(cljonic::Vector<int, 4>{1, 2});",
            "map": "(void)cljonic::rest(cljonic::Map<int, int, 4>{cljonic::MapEntry{1, 2}});",
            "set": "(void)cljonic::rest(cljonic::Set<int, 4>{1, 2});",
            "queue": "(void)cljonic::rest(cljonic::Queue<int, 4>{1, 2});",
            "string": "(void)cljonic::rest(cljonic::String<4>{\"AB\"});",
            "empty-vector": "(void)cljonic::rest(cljonic::Vector<int, 4>{});",
            "zero-capacity-vector": "(void)cljonic::rest(cljonic::Vector<int, 0>{});",
            "range": "(void)cljonic::rest(cljonic::Range<int>{0, 5});",
            "repeat": "(void)cljonic::rest(cljonic::repeat(3, 7));",
            "cycle": "(void)cljonic::rest(cljonic::cycle(cljonic::Vector<int, 3>{1, 2, 3}));",
            "iterate": "(void)cljonic::rest(cljonic::iterate([](int value) noexcept { return value + 1; }, 0));",
            "repeatedly": "(void)cljonic::rest(cljonic::repeatedly(3, []() noexcept { return 7; }));",
        },
    },
    "has_next": {
        "diagnostic_anchor": "cljonic::has_next:",
        "diagnostic_cases": {
            "unsupported-scalar": (
                "(void)cljonic::has_next(42);",
                "expected a Vector, Map, Set, Queue, or String value",
            ),
            "unsupported-map-entry": (
                "(void)cljonic::has_next(cljonic::MapEntry<int, int>{1, 2});",
                "expected a Vector, Map, Set, Queue, or String value",
            ),
            "unsupported-producer": (
                "(void)cljonic::has_next(cljonic::Range<int>{0, 5});",
                "expected a Vector, Map, Set, Queue, or String value",
            ),
            "unsupported-external-range": (
                "int values[]{1, 2}; (void)cljonic::has_next(values);",
                "expected a Vector, Map, Set, Queue, or String value",
            ),
        },
        "pass_cases": {
            "vector": "(void)cljonic::has_next(cljonic::Vector<int, 4>{1, 2});",
            "map": "(void)cljonic::has_next(cljonic::Map<int, int, 4>{cljonic::MapEntry{1, 2}});",
            "set": "(void)cljonic::has_next(cljonic::Set<int, 4>{1, 2});",
            "queue": "(void)cljonic::has_next(cljonic::Queue<int, 4>{1, 2});",
            "string": "(void)cljonic::has_next(cljonic::String<4>{\"AB\"});",
            "empty-vector": "(void)cljonic::has_next(cljonic::Vector<int, 4>{});",
            "zero-capacity-vector": "(void)cljonic::has_next(cljonic::Vector<int, 0>{});",
        },
    },
    "is_empty": {
        "diagnostic_anchor": "cljonic::is_empty:",
        "diagnostic_cases": {
            "unsupported-scalar": (
                "(void)cljonic::is_empty(42);",
                "value must be a collection or producer",
            ),
            "unsupported-external-range": (
                "int values[]{1, 2}; (void)cljonic::is_empty(values);",
                "value must be a collection or producer",
            ),
            "unsupported-count-shaped-type": (
                "struct CountShaped { auto count() const noexcept -> std::size_t { return 0; } }; "
                "(void)cljonic::is_empty(CountShaped{});",
                "value must be a collection or producer",
            ),
            "unsupported-map-entry": (
                "(void)cljonic::is_empty(cljonic::MapEntry<int, int>{1, 2});",
                "value must be a collection or producer",
            ),
        },
        "pass_cases": {
            "vector-empty": "(void)cljonic::is_empty(cljonic::Vector<int, 4>{});",
            "map-empty": "(void)cljonic::is_empty(cljonic::Map<int, int, 4>{});",
            "set-empty": "(void)cljonic::is_empty(cljonic::Set<int, 4>{});",
            "queue-empty": "(void)cljonic::is_empty(cljonic::Queue<int, 4>{});",
            "string-empty": "(void)cljonic::is_empty(cljonic::String<4>{});",
            "empty-range-producer": "(void)cljonic::is_empty(cljonic::Range<int>{0, 0});",
            "nonempty-range-producer": "(void)cljonic::is_empty(cljonic::Range<int>{0, 5});",
            "repeat-producer": "(void)cljonic::is_empty(cljonic::Repeat{7});",
            "cycle-producer": "(void)cljonic::is_empty(cljonic::cycle(cljonic::Vector<int, 1>{1}));",
            "iterate-producer": (
                "(void)cljonic::is_empty(cljonic::iterate([](int value) noexcept { return value + 1; }, 0));"
            ),
            "repeatedly-producer": "(void)cljonic::is_empty(cljonic::repeatedly([]() noexcept { return 7; }));",
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
    "not_empty": {
        "diagnostic_anchor": "cljonic::not_empty:",
        "diagnostic_cases": {
            "unsupported-scalar": (
                "(void)cljonic::not_empty(42);",
                "expected a Vector, Map, Set, Queue, or String value",
            ),
            "unsupported-producer": (
                "(void)cljonic::not_empty(cljonic::Range<int>{0, 5});",
                "expected a Vector, Map, Set, Queue, or String value",
            ),
            "unsupported-repeat": (
                "(void)cljonic::not_empty(cljonic::Repeat{1});",
                "expected a Vector, Map, Set, Queue, or String value",
            ),
            "unsupported-cycle": (
                "(void)cljonic::not_empty(cljonic::cycle(cljonic::Vector{1}));",
                "expected a Vector, Map, Set, Queue, or String value",
            ),
            "unsupported-iterate": (
                "(void)cljonic::not_empty(cljonic::iterate([](int x) noexcept { return x; }, 0));",
                "expected a Vector, Map, Set, Queue, or String value",
            ),
            "unsupported-repeatedly": (
                "(void)cljonic::not_empty(cljonic::repeatedly([]() noexcept { return 1; }));",
                "expected a Vector, Map, Set, Queue, or String value",
            ),
            "unsupported-map-entry": (
                "(void)cljonic::not_empty(cljonic::MapEntry{1, 2});",
                "expected a Vector, Map, Set, Queue, or String value",
            ),
            "unsupported-external-range": (
                "int values[]{1, 2}; (void)cljonic::not_empty(values);",
                "expected a Vector, Map, Set, Queue, or String value",
            ),
            "unsupported-count-shaped-type": (
                "struct CountShaped { auto count() const noexcept -> std::size_t { return 0; } }; "
                "(void)cljonic::not_empty(CountShaped{});",
                "expected a Vector, Map, Set, Queue, or String value",
            ),
        },
        "pass_cases": {
            "vector": "(void)cljonic::not_empty(cljonic::Vector{1, 2});",
            "map": "(void)cljonic::not_empty(cljonic::Map{cljonic::MapEntry{1, 2}});",
            "set": "(void)cljonic::not_empty(cljonic::Set{1, 2});",
            "queue": "(void)cljonic::not_empty(cljonic::Queue{1, 2});",
            "string": "(void)cljonic::not_empty(cljonic::String{\"ab\"});",
            "empty": "(void)cljonic::not_empty(cljonic::Vector<int, 4>{});",
            "zero-capacity": "(void)cljonic::not_empty(cljonic::Vector<int, 0>{});",
            "nested": "(void)cljonic::not_empty(cljonic::Vector{cljonic::Vector{1, 2}});",
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
    "next": {
        "diagnostic_anchor": "cljonic::next:",
        "diagnostic_cases": {
            "unsupported-scalar": (
                "(void)cljonic::next(42);",
                "expected a Vector, Map, Set, Queue, String, Range, Repeat, Iterate, Repeatedly, or supported Cycle "
                "value",
            ),
            "unsupported-map-entry": (
                "(void)cljonic::next(cljonic::MapEntry<int, int>{1, 100});",
                "expected a Vector, Map, Set, Queue, String, Range, Repeat, Iterate, Repeatedly, or supported Cycle "
                "value",
            ),
            "unsupported-external-range": (
                "int values[]{1, 2}; (void)cljonic::next(values);",
                "expected a Vector, Map, Set, Queue, String, Range, Repeat, Iterate, Repeatedly, or supported Cycle "
                "value",
            ),
        },
        "pass_cases": {
            "vector": "(void)cljonic::next(cljonic::Vector<int, 4>{10, 20});",
            "map": "(void)cljonic::next(cljonic::Map<int, int, 4>{cljonic::MapEntry<int, int>{1, 100}});",
            "set": "(void)cljonic::next(cljonic::Set<int, 4>{7, 8});",
            "queue": "(void)cljonic::next(cljonic::Queue<int, 4>{10, 20});",
            "string": '(void)cljonic::next(cljonic::String<4>{"ab"});',
            "finite-repeat": "(void)cljonic::next(cljonic::Repeat{3U, 7});",
            "unbounded-repeat": "(void)cljonic::next(cljonic::Repeat{7});",
            "iterate": "(void)cljonic::next(cljonic::iterate([](int value) noexcept { return value + 1; }, 0));",
            "finite-repeatedly": "(void)cljonic::next(cljonic::repeatedly(3U, []() noexcept { return 7; }));",
            "unbounded-repeatedly": "(void)cljonic::next(cljonic::repeatedly([]() noexcept { return 7; }));",
            "finite-range": "(void)cljonic::next(cljonic::Range<int>{0, 5});",
            "unbounded-range": "(void)cljonic::next(cljonic::Range<int>{0, 5, 0});",
            "finite-cycle": "(void)cljonic::next(cljonic::cycle(cljonic::Vector<int, 3>{1, 2, 3}));",
            "iterate-cycle": (
                "(void)cljonic::next(cljonic::cycle(cljonic::iterate([](int value) noexcept { return value + 1; }, 0)));"
            ),
            "unbounded-cycle": (
                "(void)cljonic::next(cljonic::cycle(cljonic::Range<int>{0, 0, 0}));"
            ),
            "empty": "(void)cljonic::next(cljonic::Vector<int, 4>{});",
            "zero-capacity": "(void)cljonic::next(cljonic::Vector<int, 0>{});",
        },
    },
    "fits_into": {
        "diagnostic_anchor": "cljonic::fits_into:",
        "diagnostic_cases": {
            "unsupported-scalar-source": (
                "(void)cljonic::fits_into(cljonic::Vector<int, 4>{}, 42);",
                "expected a Vector, Set, Map, Queue, or String destination and a compatible cljonic collection or producer source",
            ),
            "unsupported-destination": (
                "(void)cljonic::fits_into(42, cljonic::Range<int>{0, 5});",
                "expected a Vector, Set, Map, Queue, or String destination and a compatible cljonic collection or producer source",
            ),
            "lossy-source-element": (
                "(void)cljonic::fits_into(cljonic::Vector<short, 4>{}, cljonic::Range<int>{0, 5});",
                "expected a Vector, Set, Map, Queue, or String destination and a compatible cljonic collection or producer source",
            ),
        },
        "pass_cases": {
            "finite-range": "(void)cljonic::fits_into(cljonic::Vector<int, 8>{}, cljonic::Range<int>{0, 5});",
            "unbounded-range": "(void)cljonic::fits_into(cljonic::Vector<int, 8>{}, cljonic::Range<int>{0, 5, 0});",
            "vector-source": "(void)cljonic::fits_into(cljonic::Vector<int, 8>{}, cljonic::Vector<int, 4>{1, 2});",
            "populated-destination": "(void)cljonic::fits_into(cljonic::Vector<int, 8>{1, 2}, cljonic::Range<int>{0, 5});",
        },
    },
    "into": {
        "diagnostic_anchor": "cljonic::into:",
        "diagnostic_cases": {
            "unsupported-scalar-source": (
                "(void)cljonic::into(cljonic::Vector<int, 4>{}, 42);",
                "expected a Vector, Set, Map, Queue, or String destination and a compatible cljonic collection or producer source",
            ),
            "unsupported-destination": (
                "(void)cljonic::into(42, cljonic::Range<int>{0, 5});",
                "expected a Vector, Set, Map, Queue, or String destination and a compatible cljonic collection or producer source",
            ),
            "lossy-source-element": (
                "(void)cljonic::into(cljonic::Vector<short, 4>{}, cljonic::Range<int>{0, 5});",
                "expected a Vector, Set, Map, Queue, or String destination and a compatible cljonic collection or producer source",
            ),
        },
        "pass_cases": {
            "finite-range": "(void)cljonic::into(cljonic::Vector<int, 8>{}, cljonic::Range<int>{0, 5});",
            "unbounded-range": "(void)cljonic::into(cljonic::Vector<int, 8>{}, cljonic::Range<int>{0, 5, 0});",
            "vector-source": "(void)cljonic::into(cljonic::Vector<int, 8>{}, cljonic::Vector<int, 4>{1, 2});",
            "populated-destination": "(void)cljonic::into(cljonic::Vector<int, 8>{1, 2}, cljonic::Range<int>{0, 5});",
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

for operation in ("into", "fits_into"):
    materialization = PRIMITIVES[operation]
    for label, statement in {
        "producer-destination": f"(void)cljonic::{operation}(cljonic::Range<int>{{0, 2}}, cljonic::Range<int>{{0, 2}});",
        "external-source": f"int values[]{{1, 2}}; (void)cljonic::{operation}(cljonic::Vector<int, 4>{{}}, values);",
        "lossy-set-source": f"(void)cljonic::{operation}(cljonic::Set<short, 4>{{}}, cljonic::Range<int>{{0, 2}});",
        "lossy-queue-source": f"(void)cljonic::{operation}(cljonic::Queue<short, 4>{{}}, cljonic::Range<int>{{0, 2}});",
        "map-non-entry-source": (
            f"(void)cljonic::{operation}(cljonic::Map<int, int, 4>{{}}, cljonic::Vector<int, 4>{{1, 2}});"
        ),
        "map-mismatched-entry-source": (
            f"(void)cljonic::{operation}(cljonic::Map<long, int, 4>{{}}, "
            "cljonic::Vector<cljonic::MapEntry<int, int>, 1>{cljonic::MapEntry{1, 2}});"
        ),
    }.items():
        materialization["diagnostic_cases"][label] = (
            statement,
            "expected a Vector, Set, Map, Queue, or String destination and a compatible cljonic collection or producer source",
        )
    for label, statement in {
        "set-destination": f"(void)cljonic::{operation}(cljonic::Set<int, 4>{{}}, cljonic::Range<int>{{0, 2}});",
        "queue-destination": f"(void)cljonic::{operation}(cljonic::Queue<int, 4>{{}}, cljonic::Range<int>{{0, 2}});",
        "map-destination": (
            f"(void)cljonic::{operation}(cljonic::Map<int, int, 4>{{}}, "
            "cljonic::Map<int, int, 4>{cljonic::MapEntry{1, 2}});"
        ),
        "map-entry-producer": (
            f"(void)cljonic::{operation}(cljonic::Map<int, int, 4>{{}}, cljonic::repeat(3, cljonic::MapEntry{{1, 2}}));"
        ),
        "string-destination": (
            f'(void)cljonic::{operation}(cljonic::String<4>{{}}, cljonic::String<4>{{"ab"}});'
        ),
        "string-vector-char-source": (
            f"(void)cljonic::{operation}(cljonic::String<4>{{}}, cljonic::Vector<char, 2>{{'a', 'b'}});"
        ),
        "set-widening-source": (
            f"(void)cljonic::{operation}(cljonic::Set<long long, 4>{{}}, cljonic::Vector<short, 2>{{short{{1}}}});"
        ),
        "queue-widening-source": (
            f"(void)cljonic::{operation}(cljonic::Queue<long long, 4>{{}}, cljonic::Range<int>{{0, 2}});"
        ),
        "zero-capacity-map": (
            f"(void)cljonic::{operation}(cljonic::Map<int, int, 0>{{}}, cljonic::repeat(0, cljonic::MapEntry{{1, 2}}));"
        ),
    }.items():
        materialization["pass_cases"][label] = statement


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
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--primitive", choices=PRIMITIVES)
    parser.add_argument("--list-primitives", action="store_true")
    args = parser.parse_args()
    if args.list_primitives:
        print("\n".join(PRIMITIVES))
        return 0
    compiler = os.environ.get("CXX", "c++").split()
    root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    configurations = (
        ("modular", os.path.join(root, "src"), "cljonic-core.hpp"),
        ("single-header", root, "cljonic.hpp"),
    )
    failures: list[str] = []

    for configuration, include_dir, header in configurations:
        for primitive, cases in PRIMITIVES.items():
            if args.primitive and primitive != args.primitive:
                continue
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