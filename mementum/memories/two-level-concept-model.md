---
type: Decision
symbol: 🎯
title: two-level-concept-model
---
The library uses a two-level C++20 concept model that gates every public collection and free-function surface:

- Level 1 `CollectionConcept` (nominal admission): `CljonicCollection`, `CljonicVector`, `CljonicMap`, `CljonicSet`, `CljonicQueue`, `CljonicString` — gate on admission to the `ClosedNominalCollectionDomain` and `CollectionKind`, NOT structural similarity to external containers.
- Level 2 `CapabilityConcept` (semantic gates, layered on nominal identity): `SequenceableCollection` (`count`/`is_empty`), `IndexedCollection` (`operator(i)`/`valid_index`), `AssociativeCollection` (`operator(k)`/`contains`), plus value concepts `StableEqualityComparable` and `TotallyOrdered`.

`StableEqualityComparable` (rejects float/double) is the base value concept; `TotallyOrdered` extends it. Both are required — `TotallyOrdered` alone would over-constrain equality-only key/element types.

Vocabulary terms `CollectionConcept` and `CapabilityConcept` are recorded in `vocabulary.md`. Architecture documents this model in the Concept Architecture section.