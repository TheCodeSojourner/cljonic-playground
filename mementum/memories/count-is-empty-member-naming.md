---
type: Decision
symbol: 🎯
title: count-is-empty-member-naming
---
Collection cardinality is exposed by the `count()` member; emptiness is a free-function-only operation.

- All five owning collections (`Vector`, `Map`, `Set`, `Queue`, `String`) provide `count()`; `size()` and `empty()` are not aliases.
- `SequenceableCollection` requires non-throwing `count()` only. `is_empty(c)` is supported for collections and is equivalent to `count(c) == 0`; no `is_empty()` member is required or exposed.
- Producers remain outside the `is_empty` free-function domain. Their `count()` may be a bounded observation cap rather than exact complete cardinality.
- Scripts/spec_weed_check.py member whitelists must swap `size`/`empty` → `count`/`is_empty`.
- String lookup aligns to the other containers: `operator()(i)`, `operator()(i, fallback)`, and `get`, gated by `contains(i)` (Clojure contains? index-in-range).
- `operator[]` lookup is omitted from all collections (Clojure parity, not std::string).
- `contains` unifies the lookup-domain membership predicate across all collection kinds (Clojure `contains?`): maps test key presence, sets test element presence, vector/string test index-in-range. The former index-specific aliases were removed; `contains` is the sole public name for the index-in-range form.
- `Vector.size()`→`count()` also amended in vocabulary.md `Vector` Examples.