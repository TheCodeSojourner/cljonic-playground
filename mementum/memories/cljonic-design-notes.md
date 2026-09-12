---
type: Insight
symbol: 💡
title: cljonic-design-notes
---
cljonic core design decisions:
- C++23, concepts and constexpr throughout
- Embedded: no heap, no exceptions; std::array-backed storage only
- Value semantics: copy-on-modify (not Clojure persistent internals)
- Error model: missing/invalid access returns `T{}` sentinel; no status returns
- Caller contract: caller uses contains/has_index before relying on sentinel
- Keywords: global-only, no namespaces; enum catalog representation (not perfect-hash)
- Map keys: may be non-keyword; keyword keys must come from the enum catalog
- Map internals: linear search, small N assumed
- Spec-weed traceability: use `allium plan` IDs + `rg -F` matching; bash `while read` produced false uncovered ID in this repo
- Two-level concept model: `CollectionConcept` (nominal identity, `Cljonic*`) gates admission; `CapabilityConcept` (structural: `SequenceableCollection`, `IndexedCollection`, `AssociativeCollection`; value: `StableEqualityComparable`, `TotallyOrdered`) gates capability on top.
- Value concepts: `StableEqualityComparable` (base, rejects float/double) and `TotallyOrdered` (derived) both required; cannot use only `TotallyOrdered` because it over-constrains equality-only key/element types.
