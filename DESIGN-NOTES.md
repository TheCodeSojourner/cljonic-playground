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

## Locked C++26 usage guidelines

### Contracts

- Use contracts at public API boundaries and critical invariants.
- Contract violation handling must be deterministic and non-throwing.
- Route contract failures through one policy hook (for example terminate, trap, or assert), selected by build profile.
- Use a portability wrapper so native contracts and fallback assertions can share one project API.
- Use contracts for logic invariants, not for expected user-data variability already handled by sentinel semantics.

### Concepts

- Define semantic concepts for key roles (for example comparable under strict policy, keyword-key, sequence source, sink target).
- Reject invalid API combinations at compile time.
- Keep concepts small and composable to improve diagnostics.
- Keep strict floating-point comparison exclusion encoded in concept constraints for canonical comparison APIs.

### Functional style

- Prefer pure free functions and immutable return style.
- Keep transforms lazy by default and sinks explicit.
- Avoid hidden side effects in pipeline operations.
- Keep eager materialization APIs clearly named.

### ARM optimization guidance

- Optimize by target profile (for example Cortex-M versus Cortex-A), not one generic tuning set.
- Prefer contiguous storage and predictable iteration.
- Minimize branch-heavy hot paths in lookups and comparisons.
- Favor static polymorphism in hot paths and avoid virtual dispatch where possible.
- Keep frequently moved view/state objects compact and trivially copyable where practical.
- Tune only after measurement on representative ARM hardware workloads.

## Locked packaging model

- The library is header-only.
- Source organization separates production and tests directories.
- Production implementation units are one function per file.
- Implementation-unit files are headers within the production source tree.
- A build automation step generates one distributable amalgamated header from all production headers.
- The amalgamated header is treated as distribution output, while per-function headers remain the development source of truth.

## Locked quality and verification toolchain

### Testing framework

- Catch2 is the primary testing framework.
- Catch2 generators are required for parameterized and combinational coverage in tests.

### Source layout

- Production and test code are separated into dedicated directories.

### Sanitizers

- Sanitizers are required in host CI profiles.
- At minimum, AddressSanitizer and UndefinedBehaviorSanitizer are enabled in host test profiles.
- ThreadSanitizer is enabled for applicable host parallel profiles.
- Sanitizers are profile-gated and not required for constrained embedded target builds.

### Linting

- Linting is required in CI.
- clang-tidy and clang-format checks are enforced as quality gates.

### Documentation and docs testing

- Doxygen is required for public API documentation.
- Every public function and data structure includes at least one sample code snippet.
- Documented sample code is automatically compiled and tested in CI.
- Doxygen HTML documentation is generated automatically in CI.

### Coverage policy

- Coverage enforcement is required in host CI profiles.
- 100% line coverage is required for core MVP modules under the strict host profile.
- Explicitly documented exclusions are allowed only for generated code, defensive unreachable branches, or platform stubs.

### No-heap verification

- A built-in verification gate ensures heap allocation is never used in strict profiles.
- Verification uses layered checks: compile or link-time allocation prohibition, runtime allocation-counter assertions, and binary symbol checks for forbidden allocation APIs.

## Locked session decisions (2026-08-05)

### Locked key and value type model for map and set (MVP)

- MVP map and set key categories are: keyword enum values, integral types, enum types, and fixed-capacity static string.
- User-defined key types are not part of strict MVP by default; they are allowed only through an explicit non-MVP concept gate.
- MVP value categories are non-floating scalar types and nested collection types that satisfy deep equality semantics.
- Canonical comparison paths preserve compile-time floating-point exclusion.

### Locked sentinel ambiguity policy

- Missing or invalid access continues to return default sentinel `T{}`.
- No explicit error or status return channel is added.
- Probe-first usage is required guidance for ambiguous domains:
	- associative access uses `contains` before `get`
	- indexed access uses `has_index` before `get`

### Locked core collection API surface and capacity behavior

- Canonical API style is free-function-first; member wrappers are optional and non-canonical.
- Canonical MVP collection operations are: `get`, `assoc`, `dissoc`, `conj`, `contains`, `count`, `first`, `rest`.
- Full-capacity `assoc` or `conj` returns unchanged value deterministically.
- Invalid index or key keeps mutation-style operations unchanged and keeps accessors sentinel-based.

### Locked MVP API matrix (summary)

- `get`
	- vector and string index out of bounds returns sentinel element (`T{}` or `char{}`).
	- set and map missing lookup returns sentinel value `T{}`.
- `contains`
	- presence probe API for vector, set, map, and string.
- `has_index`
	- index validity probe for vector and string.
- `assoc`
	- immutable copy-on-modify update for vector, map, and string.
	- map duplicate key updates value in returned copy.
- `dissoc`
	- immutable removal for set and map; missing target returns unchanged value.
	- removal compacts storage; map preserves insertion order of remaining entries.
- `conj`
	- immutable append or insert for vector, set, map, and string.
	- if capacity is full, returns unchanged value.
- `count`
	- returns logical element count, not capacity.
- `first` and `rest`
	- empty source returns sentinel (`first`) or empty same-type (`rest`).
	- lazy sequence overloads must remain non-materializing by default.

### Locked comparator override direction

- Comparator override remains explicit per call through `_with` APIs (for example `equal_with`).
- Variadic chained semantics and comparison arity behavior remain aligned with canonical APIs.
