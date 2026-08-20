---
type: Reference
title: cljonic Next Discussion Agenda
status: open
category: roadmap
tags: [roadmap, collections, api, keywords, map, set, string]
related: [collection-api-surface-discipline.md]
depends-on: []
---

## Priority order

1. Key/value type system for map and set
   - Decide allowed key categories (keyword enum, integral, enum, static string type, user-defined key types).
   - Define required concepts for keys and values.
   - Confirm equality semantics across scalar types.

2. Sentinel default ambiguity policy
   - Missing access returns `T{}` by design.
   - Decide library guidance for domains where `T{}` is a valid value.
   - Decide whether optional probe APIs (contains/has_index helpers only) are sufficient.

3. Collection API surface (Clojure-inspired)
   - Finalize names and overload strategy for get, assoc, dissoc, conj, contains, count, first, rest.
   - Choose member functions vs free functions vs both.
   - Define behavior for full-capacity assoc/conj (currently return unchanged value).

4. Map internal layout
   - Confirm linear search and insertion-order storage for small N.
   - Decide duplicate-key update semantics (replace in place in returned copy).
   - Decide removal compaction behavior and ordering guarantees.

5. Keyword enum catalog
   - Establish keyword naming conventions (global-only, no namespace separators).
   - Decide compile-time literal mapping API and diagnostics style.
   - Define runtime text-to-keyword fallback behavior, if any.

6. Static string design
   - Define capacity semantics and null termination rules.
   - Decide normalization and comparison behavior.
   - Confirm how static string interacts with keyword and map keys.

7. Constexpr/consteval boundaries
   - List which operations must be constexpr/consteval.
   - Identify any operations intentionally runtime-only.

8. Testing strategy before implementation
   - Define compile-time test cases (static_assert) for behavior contracts.
   - Define minimal acceptance criteria for first implementation milestone.

## First milestone proposal
- Implement keyword enum catalog API.
- Implement array-backed map with linear search and sentinel-return get.
- Add compile-time and runtime tests for the agreed behavior contracts.

## Current state (updated 2026-08-11)

Vector and count are implemented and passing all quality gates (lint, complexity, sanitizers, coverage, traceability, no-heap, docs).

Before implementing new collections, resolve the remaining open questions against the current requirements, architecture, and vocabulary sources:
- Collection C++ concept definition
- Meaning of the `_M` suffix
- Mixed-collection return type and capacity policy
- `Into` semantics
- Float element types in stored collections
- `count` for unbound generated collections
- String transform return types

Next collection to implement after these open questions are resolved: Range.
