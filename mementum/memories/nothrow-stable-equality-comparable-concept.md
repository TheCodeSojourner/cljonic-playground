---
type: Decision
symbol: 🎯
title: nothrow-stable-equality-comparable-concept
related: [concept-naming-convention.md, two-level-concept-model.md, refine-concepts-with-narrower-capabilities.md]
---
Added `NothrowStableEqualityComparable` to `cljonic-concepts.hpp`: `StableEqualityComparable<T> && NothrowCollectionElement<T>`. This is the shared admission contract for map keys and set elements (both need non-throwing storage AND stable equality), analogous to how `TotallyOrdered` layers `std::totally_ordered` on `StableEqualityComparable`.

`Map`'s `KeyType` template parameter now uses this concept directly instead of a separate trailing `requires` clause: `template <concepts::NothrowStableEqualityComparable KeyType, concepts::NothrowCollectionElement ValueType, std::size_t CapacityValue>`.

Propagated to: `specs/capabilities/concepts.allium` (new entity + actor), `vocabulary.md`, `tests/cljonic-concepts-spec-tests.cpp` (dedicated TEST_CASE with TRACE_IDs).

Next: `Set`'s element type (`src/cljonic-set.hpp`) currently only requires `StableEqualityComparable`, not also `NothrowCollectionElement`. When Set gets its requirements-first review pass, replace its element constraint with `NothrowStableEqualityComparable` for consistency with Map.
