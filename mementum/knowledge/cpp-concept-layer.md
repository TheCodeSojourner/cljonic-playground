---
type: Reference
title: C++ Concept Layer
status: active
category: architecture
tags: [concepts, naming, vocabulary, core-guidelines, clojure-parity, collections, string]
related:
  - /mementum/memories/concept-naming-convention.md
  - /mementum/memories/two-level-concept-model.md
  - /mementum/memories/count-is-empty-member-naming.md
  - /mementum/memories/vocab-use-pascalcase-canonically.md
  - /mementum/memories/cljonic-naming-convention.md
  - /mementum/memories/use-concepts-and-hide-traits-in-cljonic-concepts.md
  - /mementum/memories/architecture-lambda-notation.md
depends-on: [architecture.md, vocabulary.md]
---

# C++ Concept Layer

This synthesis consolidates the durable concept-layer and naming conventions for
the cljonic codebase. It governs how vocabulary, architecture, specs, tests, and
code name and constrain the C++ type system.

## Naming convention

All C++ code follows the C++ Core Guidelines.

- Types and concepts: PascalCase (concepts per `NL.17`).
- Namespaces: lowercase_with_underscores (Clojure-analog `cljonic::core`,
  `cljonic::set`, `cljonic::string`).
- Free and member functions: snake_case with Clojure parity (`count`,
  `is_empty`, `contains`).
- Because cljonic users are developers, C++ concept identifiers are first-class
  vocabulary terms, not hidden references under Examples.

## Two-level concept model

Every public collection and free-function surface is gated by a two-level C++20
concept model.

- Level 1 `CollectionConcept` (nominal admission): `CljonicCollection`,
  `CljonicVector`, `CljonicMap`, `CljonicSet`, `CljonicQueue`, `CljonicString`.
  Gate on admission to `ClosedNominalCollectionDomain` and `CollectionKind`, not
  structural similarity to external containers. Keep the `Cljonic` prefix.
- Level 2 `CapabilityConcept` (semantic gates layered on nominal identity):
  - Structural: `SequenceableCollection` (`count`/`is_empty`),
    `IndexedCollection` (`operator(i)`/`contains(index)`),
    `AssociativeCollection` (`operator(k)`/`contains`).
  - Value: `StableEqualityComparable` (base, rejects float/double),
    `TotallyOrdered` (derived).
  - Capability/value concepts drop the `Cljonic` prefix.

`StableEqualityComparable` is the base value concept; `TotallyOrdered` extends it.
Both are required because `TotallyOrdered` alone would over-constrain
equality-only key/element types.

## Container member naming

Clojure parity, not STL naming.

- Every container (`Vector`, `Map`, `Set`, `Queue`, `String`) provides `count()`
  and `is_empty()`; `size()` and `empty()` are removed entirely (no aliases).
- Free functions `count(c)`/`is_empty(c)` dispatch on these members.
- `operator[]` is omitted from all collections. Lookup uses
  `operator()(index)`, `operator()(index, fallback)`, and `get`, gated by
  `contains`. `contains` is the unified Clojure-parity membership predicate
  across all collection kinds (map key presence, set element presence,
  vector/string index-in-range); legacy index aliases are removed.

## Machinery placement

Repo-specific concepts live in `cljonic::concepts`. Trait-based machinery stays
hidden in `detail`/concept scaffolding; standard concepts are preferred at the
public API boundary.

## Architecture style

`architecture.md` is written in nucleus lambda notation, not human prose. Prose
becomes `<section>_intro` / `<area>_guidance` lambdas; traceability lists become
`λ traceability_authorities` / `λ current_implementation_boundary`. C++ code
fences and section structure (S5 > S4 > S3 > S2 > S1) are preserved.