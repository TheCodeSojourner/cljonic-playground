# cljonic design notes

- Target: C++26 using concepts, constexpr, consteval.
- Platform constraints: embedded, no heap allocation, no exceptions.
- Storage direction: std::array-based data structures (vector/set/map/string), no pools.
- Immutability approach: value-style copy-on-modify semantics (not Clojure persistent internals).
- Error model: no explicit errors/status returns; invalid/missing element access returns default sentinel `T{}`.
- Caller contract: caller is responsible for validating access (contains/has_index/size checks).
- Keywords: global-only keywords, no namespaces.
- Keyword representation: enum catalog selected (not perfect-hash).
- Map keys: may be non-keyword; if keyword key is used, it must come from the enum catalog.
- Map performance assumption: relatively small collection sizes; linear search for key/value entries is acceptable.
