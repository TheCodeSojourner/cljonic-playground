---
type: Decision
symbol: 🎯
title: count-is-empty-member-naming
---
Collection cardinality is exposed by the `count()` member; emptiness is a free-function-only operation.

- All five owning collections (`Vector`, `Map`, `Set`, `Queue`, `String`) provide `count()`; `size()` and `empty()` are not aliases.
- `SequenceableCollection` requires non-throwing `count()` only. `is_empty(c)` accepts either a `SequenceableCollection` or `SequenceableProducer` and is equivalent to `count(c) == 0`; no `is_empty()` member is required or exposed.
- For an unbounded producer, `count()` and `is_empty` use the configured bounded observation cap rather than complete cardinality.
- A previous machine-specific heuristic checker with stale collection-member whitelists was retired; authoritative spec and API checks use Allium and current tests instead.
- String lookup aligns to the other containers: `operator()(i)`, `operator()(i, fallback)`, and `get`, gated by `contains(i)` (Clojure contains? index-in-range).
- `operator[]` lookup is omitted from all collections (Clojure parity, not std::string).
- `contains` unifies the lookup-domain membership predicate across all collection kinds (Clojure `contains?`): maps test key presence, sets test element presence, vector/string test index-in-range. The former index-specific aliases were removed; `contains` is the sole public name for the index-in-range form.
- `Vector.size()`→`count()` also amended in vocabulary.md `Vector` Examples.