---
type: Decision
symbol: 🎯
title: factory-consistency-for-user-createable-values
---
Every user-createable cljonic value type in the public surface should expose a lowercase factory as its idiomatic creation surface: `vector`, `hash_map`, and now `map_entry`.

`MapEntry` qualified because map traversal yields `MapEntry` values, range sources are spans of entries, and Map pack construction requires the exact entry type. It was the last public value type where users still wrote brace initialization.

A factory here adds no capability; it duplicates the equivalent constructor. The justification is surface consistency, not new behavior. When a consistency argument appears, check `REQ-FN-029B`/`029C` first: approved naming is often already recorded, so the work is promotion to an active clause (here `REQ-FN-029E`), not new design.

Dimension rule: a fixed-shape result (`MapEntry`) has no capacity parameter and no spare-capacity form; only bounded collections carry capacity.

Propagation order stays requirements → vocabulary → architecture → spec → tests → code → docs.
