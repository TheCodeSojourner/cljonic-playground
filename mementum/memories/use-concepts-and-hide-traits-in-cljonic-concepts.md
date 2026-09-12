---
type: Insight
symbol: 💡
title: use-concepts-and-hide-traits-in-cljonic-concepts
---
Prefer standard concepts at the public API boundary and keep trait-based machinery hidden inside `cljonic::concepts`.

Repo-specific concepts (CljonicCollection, SequenceableCollection, IndexedCollection, AssociativeCollection, StableEqualityComparable, TotallyOrdered, CopyableElement, etc.) are named in PascalCase per C++ Core Guidelines `NL.17` and live in `cljonic::concepts`. Traits remain hidden in `detail`/concept scaffolding.
