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

## Locked comparison semantics

- Canonical deep comparison API names: `equal`, `less`, `less_equal`, `greater`, `greater_equal`.
- Canonical APIs are variadic (multi-arity) and follow chained semantics.
- Clojure-like arities are adopted for comparisons: zero-arity and one-arity calls return `true`.
- `equal` performs deep value comparison across nested collections.
- Map and set equality is order-independent by content.
- Ordering comparisons are scalar-focused; deep ordering of map/set is not part of canonical behavior.

## Locked floating-point policy

- Strict default policy: no floating-point comparisons.
- Any direct comparison involving floating-point under canonical APIs is a compile-time error.
- Dynamic value type excludes floating-point variants entirely.
- Runtime behavior therefore also forbids floating-point comparison by construction.
- Optional float comparison behavior, if needed, is explicit per call via comparator-based override APIs (for example `equal_with`).
- Implicit override is not allowed.

## Locked collection construction pattern

- The same declaration pattern applies to vector, set, map, and string.
- Literal-deduced construction is supported for ergonomic declarations.
- Explicit-capacity construction is also supported for bounded embedded control.
- Explicit-capacity empty construction is valid.
- If initializer element count exceeds explicit capacity, construction fails at compile time.

## Locked lazy sequence policy

- Sequence-producing APIs use lazy behavior by default where practical.
- Priority lazy sources include range, repeat, and cycle.
- Lazy transforms should compose without hidden materialization.
- Materialization is explicit at sink operations (for example into vector/map/set/string, or reduction endpoints).
- Infinite lazy sources are allowed, and are intended to be consumed with bounded or short-circuit consumers.
- Lazy views use value-owning view objects rather than borrowing-only views.
- Clojure parity is a priority for lazy sequence semantics where feasible.

## Locked interleave semantics

- Canonical `interleave` is lazy and shortest-terminating.
- `interleave` alternates sources in round-robin order.
- Termination occurs when any input source is exhausted.
- `interleave` returns a lazy sequence view, not a concrete collection.

## Locked range semantics

- Range is lazy by default.
- Infinite logical views do not report size as max size_t.
- Infinite and finite cardinality are represented explicitly; finite ranges may provide exact size when computable.
- Compile-time precomputation is performed whenever construction inputs are compile-time constants.
- The same range logic is valid at runtime when inputs are runtime values.
- Step equal to zero normalizes to an empty finite range.

## Locked regex strategy

- C++26 standard library does not provide standardized compile-time regex.
- CTRE is the selected compile-time regex library for this project.
- Regex support is optional by profile and should remain aligned with embedded constraints.
- Runtime regex support is allowed as an opt-in profile to support Clojure-style `re-pattern` and `re-matcher`.
- Default regex profile remains compile-time-oriented; runtime regex profile is explicit and optional.
- Regex API uses a stable handle model so profile changes do not force API shape changes.
- Under no-error policy, invalid runtime patterns return an invalid pattern sentinel.
- `pattern_valid(p)` is provided to distinguish invalid pattern sentinel from non-matching behavior.

## Locked high-fit namespace adoption roadmap

- This roadmap covers only high-fit namespaces from clojure proper for embedded constraints.

### MVP namespace cut line

- clojure.string
- clojure.set
- clojure.data
- clojure.walk
- clojure.zip

### Optional namespace cut line

- clojure.edn
- clojure.math
- clojure.uuid
- clojure.instant
- clojure.xml
- clojure.core.reducers
- clojure.parallel

### Phase order

- Phase 1 (foundation): clojure.string, clojure.set, clojure.data, clojure.math, clojure.uuid.
- Phase 2 (structural): clojure.walk, clojure.zip.
- Phase 3 (formats): clojure.edn, clojure.instant, clojure.xml.
- Phase 4 (performance): clojure.core.reducers, clojure.parallel.

### Acceptance tests by namespace

- clojure.string: fixed-capacity transforms, deterministic truncation or sentinel behavior, no hidden allocation.
- clojure.set: union/intersection/difference/subset semantics align with value-equality rules.
- clojure.data: diff behavior is stable on nested collections and respects deep equality.
- clojure.walk: prewalk/postwalk support nested map/vector/set/string trees with immutable output.
- clojure.zip: zipper navigation and edit operations preserve structure and value semantics.
- clojure.edn: reader and writer subset round-trip for supported value forms.
- clojure.math: numeric APIs exclude floating-point comparisons under strict policy.
- clojure.uuid: parse/format/equality semantics are deterministic and fixed-storage.
- clojure.instant: parse/format semantics are deterministic and profile-compatible.
- clojure.xml: bounded subset parse/write behavior is explicit and deterministic.
- clojure.core.reducers: lazy composition and reduction behavior avoid hidden materialization.
- clojure.parallel: optional profile preserves deterministic single-thread fallback.
