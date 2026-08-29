---
type: Decision
symbol: 🎯
title: clojure-contains-unification
---
Unified the membership predicate across all collection kinds to mirror Clojure's single `contains?` function.

- `contains` is the canonical lookup-domain membership predicate across all collection kinds:
  - Map → key presence (`contains(m, k)`)
  - Set → element presence (`contains(s, v)`)
  - Vector / String → index-in-range (`contains(xs, i)`)
- `valid_index` and String `valid()` were removed as public members; `contains` replaces them for index-in-range semantics. `valid_index`/`ValidIndex` terminology fully removed from vocabulary.
- Concept layer: `IndexedCollection` now requires `c.contains(i)` (not `c.valid_index(i)`), matching `AssociativeCollection`'s `c.contains(k)`.
- Free function `contains(c, x)` is a simple non-allocating, non-throwing forwarder to `collection.contains(x)`, added as `src/cljonic-contains.hpp`.
- Specs: new `specs/primitives/contains.allium` (10 obligations incl. `ContainsDoesNotPerformDefaultReturningAccess`); collection specs renamed `ValidIndexIsCanonicalIndexPredicate` → `ContainsIsCanonicalIndexPredicate`; concepts.allium `RequiresValidIndexPredicate`/`UsesValidIndexPredicate` → `RequiresContainsPredicate`/`UsesContainsPredicate`.
- Driving principle: this repo prioritizes maximum Clojure semantic compatibility over STL-style naming.
- Traceability snapshot must be regenerated after any predicate rename (`make traceability-spec-to-code-update-snapshot`).