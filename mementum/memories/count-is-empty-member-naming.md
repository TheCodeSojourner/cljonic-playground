---
type: Decision
symbol: 🎯
title: count-is-empty-member-naming
---
Container member operations use Clojure-parity names instead of STL names.

- Add `count()` and `is_empty()` to all five containers (`Vector`, `Map`, `Set`, `Queue`, `String`); remove `size()` and `empty()` entirely (no aliases).
- Free functions `count(c)`/`is_empty(c)` dispatch on the `count()`/`is_empty()` members.
- Scripts/spec_weed_check.py member whitelists must swap `size`/`empty` → `count`/`is_empty`.
- String lookup aligns to the other containers: `operator()(i)`, `operator()(i, fallback)`, and `get`, gated by `contains(i)` (Clojure contains? index-in-range).
- `operator[]` lookup is omitted from all collections (Clojure parity, not std::string).
- `contains` unifies the lookup-domain membership predicate across all collection kinds (Clojure `contains?`): maps test key presence, sets test element presence, vector/string test index-in-range. `valid_index` and String `valid()` are deprecated synonyms for the index-in-range form.
- `Vector.size()`→`count()` also amended in vocabulary.md `Vector` Examples.