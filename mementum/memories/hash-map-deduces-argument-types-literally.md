---
type: Decision
symbol: 🎯
title: hash-map-deduces-argument-types-literally
related: [factory-consistency-for-user-createable-values.md]
---
`hash_map` deduces its key type from the first argument and its value type from the second, then takes capacity as `sizeof...(Args) / 2`. It does not special-case `MapEntry` arguments as "whole associations."

So `hash_map(map_entry(2, 200), map_entry(3, 300))` correctly yields `Map<MapEntry<int, int>, MapEntry<int, int>, 1>`: one element whose key is the entry `(2→200)` and whose value is the entry `(3→300)`. This is intended behavior, confirmed by the stakeholder (2026-10-23), not a deduction defect.

`MapEntry` is a legitimate map key: `REQ-COLL-001A` requires the entry's key field to satisfy stable equality "whether the MapEntry is embedded in a Map or instantiated standalone." The admission concepts therefore accept it by design.

Consequence: do not propose diagnostics that reject `MapEntry` arguments to `hash_map`, deduced or explicit. Nested entry keys and values are a supported shape.

For flat pairs, `hash_map(k, v, ...)` is a readability convention, not a workaround. For an entry value, use `map_entry(...)`.
