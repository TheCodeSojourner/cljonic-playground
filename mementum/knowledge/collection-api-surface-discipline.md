---
type: Reference
title: Collection API Surface Discipline
status: active
category: api
tags: [collections, api, clojure-semantics, header-layout]
related: [src/vector.hpp, src/concepts.hpp, specs/collections/vector.allium, tests/vector_spec_tests.cpp]
depends-on: []
---

Collection-facing free operations should default to Clojure-like semantics and naming when embedded constraints allow it. This applies to operation families such as count/get/assoc/dissoc/conj/contains/first/rest.

Rules:
- Member operations use Clojure-parity names: `count()` and `is_empty()` replace `size()` and `empty()` entirely across all collections (`Vector`, `Map`, `Set`, `Queue`, `String`).
- `operator[]` is omitted from all collections (Clojure parity, not std::string). Callable lookup uses `operator()(index)`, `operator()(index, fallback)`, and `get`, gated by `valid_index`.
- C++ concept identifiers use PascalCase per C++ Core Guidelines `NL.17`: `Cljonic*` for nominal identity, `SequenceableCollection`/`IndexedCollection`/`AssociativeCollection` for capabilities, `StableEqualityComparable`/`TotallyOrdered` for values.
- Preserve behavioral shape first; any constrained divergence must be explicit in specs and tests.
- A shared function name across structures is acceptable only when each accepted structure has valid, explicit semantics.
- Out-of-scope structures must be rejected by contract rather than relying on informal expectations.
- Keep development headers flat under src/ for discovery and include simplicity; future collection headers should follow the same flat pattern.

This page governs API-shape consistency across specifications, test intent, and header organization.
