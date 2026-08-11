# cljonic design notes

- Target: C++23 using concepts, constexpr, consteval.
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

- All cljonic collections are finite by construction; no true lazy evaluation is used.
- `CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT` serves as the system-wide "infinity" for unbound construction forms.
- Unbound construction forms (`Range()`, `Repeat(val)`, `Cycle(coll)`, etc.) default to `CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT` elements.
- The Clojure idiom `(take n (repeat val))` maps directly to `Take(n, Repeat(val))` where `Repeat(val)` is bounded by MAX.
- Clojure parity for named sequence operations is a priority within the bounded model.

## Locked interleave semantics

- Canonical `interleave` is lazy and shortest-terminating.
- `interleave` alternates sources in round-robin order.
- Termination occurs when any input source is exhausted.
- `interleave` returns a lazy sequence view, not a concrete collection.

## Locked range semantics

- `Range` element type is restricted to integer types; floating-point ranges are a compile-time error.
- Clojure-compatible four-form overload set: `Range()`, `Range(end)`, `Range(start, end)`, `Range(start, end, step)`.
- In `Range(end)`, the single argument is the exclusive upper bound; start defaults to 0, step defaults to 1.
- In `Range()`, start=0, step=1, count=`CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT`.
- `step = 0` produces `CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT` copies of `start`; `end` is ignored (consistent with Clojure's infinite-sequence-of-start behavior).
- `step > 0` and `start >= end` → empty range (count = 0).
- `step < 0` and `start <= end` → empty range (count = 0).
- `start = end` → empty range regardless of step.
- Count for a non-empty range: `ceil((end - start) / step)`; `(end - start)` and `step` always share the same sign for non-empty ranges, yielding a positive count.
- Negative start, end, and step are fully supported; the empty-range rules are sign-agnostic.
- Examples: `Range(-5, 5)` → 10 elements; `Range(10, -10, -1)` → 20 elements; `Range(-10, -5)` → 5 elements; `Range(5, -10, 1)` → empty; `Range(-10, 5, -1)` → empty.
- Compile-time precomputation is performed when construction inputs are compile-time constants.

## Locked collection type taxonomy

- Collection types are classified as stored or generated.
- Stored collections retain elements in memory: `Vector`, `Set`, `String`, `Map`.
- Generated collections compute elements on demand: `Range`, `Repeat`, `Cycle`, `Iterate`, `Repeatedly`.
- All collection types must satisfy two invariants: a known finite count at construction time, and referentially transparent element access — the same index always yields the same element.
- `Repeatedly` requires the caller-supplied function to be pure; non-pure functions produce non-deterministic access and are the caller's responsibility.
- Complete collection type set: `Vector`, `Range`, `Repeat`, `Cycle`, `Iterate`, `Repeatedly`, `Set`, `String`, `Map`.

### Generated collection element access

| Type                      | Element at index `i`            |
| ------------------------- | ------------------------------- |
| `Range(start, end, step)` | `start + i * step`              |
| `Repeat(val, n)`          | `val`                           |
| `Cycle(coll, n)`          | `coll[i % count(coll)]`         |
| `Iterate(f, seed, n)`     | apply `f` `i` times from `seed` |
| `Repeatedly(f, n)`        | `f()` — caller ensures purity   |

### Unbound construction defaults

- `Range()` → `Range(0, MAX, 1)`
- `Range(end)` → `Range(0, end, 1)`
- `Repeat(val)` → `Repeat(val, MAX)`
- `Cycle(coll)` → `Cycle(coll, MAX)`
- `Iterate(f, seed)` → `Iterate(f, seed, MAX)`
- `Repeatedly(f)` → `Repeatedly(f, MAX)`
- `MAX` is `CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT`

## Locked "By" variant rule

- Every free function that uses element equality or ordering internally has a corresponding `*By` variant accepting a key function.
- The key function extracts a comparison value from each element; the base function uses the element directly.
- Functions accepting a caller-supplied predicate do not require a `*By` variant; `Compose(pred, key_fn)` achieves the same effect.
- The `*By` rule applies uniformly across all collection types where the base function applies.
- Functions subject to the `*By` rule: `Sort`, `Distinct`, `Dedupe`, `Equal`, `Frequencies`, `IndexOf`, `LastIndexOf`, `Max`, `Min`, and set-namespace operations that use equality for membership (`Conj`, `Contains`, `Difference`, `Disj`, `Intersection`, `ToSet`, `Union`).

## Locked function applicability by collection type

- Generated collections (`Range`, `Repeat`, `Cycle`, `Iterate`, `Repeatedly`) do not support mutation operations: `Conj`, `Conj_M`, `Assoc`, `Dissoc`.
- `Sort`, `Shuffle`, and `Replace` apply to stored collections only and are not defined for generated collections.
- Free functions that transform a generated collection (`Filter`, `Map`, `Take`, `Drop`, etc.) return a `Vector` sized to the source count as worst-case capacity.
- Set-namespace functions (`Disj`, `Union`, `Intersection`, `Difference`, etc.) apply to `Set` only.
- `GroupBy`, `Frequencies`, and `Zipmap` produce `Map` as output; they do not require `Map` as input.

## Locked regex strategy

- C++23 standard library does not provide standardized compile-time regex.
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

## Locked C++23 usage guidelines

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

### Locked text encoding and string semantics

- Supported cljonic text encoding policies are ASCII and UTF-8 only.
- `String<N>` logical capacity is measured in characters, not raw code units.
- `String<N>` internal storage always allocates N+1 code units; the extra slot holds a null terminator that is always maintained.
- `String<N>` can therefore always be passed safely to APIs expecting a null-terminated C string without a separate copy or conversion step.
- In ASCII mode, one logical character maps to one code unit.
- In UTF-8 mode, underlying storage is sized for worst-case UTF-8 code-unit width so `String<N>` still means room for `N` logical characters.
- In UTF-8 mode, the unit of logical character semantics is the Unicode scalar value.
- `count` on string returns logical character count, not code-unit count.
- Canonical indexed string APIs are character-indexed:
	- `get(string, i)` refers to the `i`th logical character
	- `assoc(string, i, x)` updates the `i`th logical character
- String slicing is character-range-based and must preserve UTF-8 sequence boundaries.
- Regex behavior is text-encoding-policy-aware:
	- ASCII mode remains ASCII-oriented
	- UTF-8 mode accepts valid UTF-8 and preserves character boundaries in matching iteration
	- canonical regex behavior does not imply full Unicode grapheme, collation, or case-folding semantics
- Canonical string values in UTF-8 mode must be valid UTF-8.
- Invalid UTF-8 is rejected by canonical string and regex entry points:
	- compile-time-known invalid UTF-8 is a compile-time error
	- runtime invalid UTF-8 is not admitted into canonical string semantics; any future raw code-unit APIs are the only place invalid sequences may be accepted
- `nth` is not part of the canonical MVP collection surface today.
- If `nth` is adopted later for indexed collections, it follows the same indexing semantics as `get`; for string in UTF-8 mode, `nth` refers to the `n`th Unicode scalar value.

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
	- removal compacts storage using swap-with-last; map iteration order is intentionally unspecified.
- `conj`
	- immutable append or insert for vector, set, map, and string.
	- if capacity is full, returns unchanged value.
- `count`
	- returns logical element count, not capacity.
- string `count` returns logical character count.
- `first` and `rest`
	- empty source returns sentinel (`first`) or empty same-type (`rest`).
- string `get` and `assoc` are character-indexed, not code-unit-indexed.


### Locked comparator override direction

- Comparator override remains explicit per call through `_with` APIs (for example `equal_with`).
- Variadic chained semantics and comparison arity behavior remain aligned with canonical APIs.

### Locked composition mechanism

- Clojure-style threading composition is adopted through `thread_first` and `thread_last`.
- `thread_first(x, s1, s2, ...)` threads the previous result as the first argument position of each step.
- `thread_last(x, s1, s2, ...)` threads the previous result as the last argument position of each step.
- Clojure-style rebinding threading is adopted through `as_thread(x, s1, s2, ...)`, where each step consumes the current value directly.
- Conditional threading is adopted through `cond_thread_first` and `cond_thread_last`, where each step is applied only when its condition is true.
- The threading API is canonical for readability-oriented composition; operator-pipe adaptor style is not required.
- Threading steps must preserve no-heap and no-exception constraints and remain deterministic under current sentinel semantics.
- Threaded sequence transforms operate over bounded finite collections; no lazy evaluation is used.
- `some`-style threading variants (`some_thread_first`, `some_thread_last`) are deferred until an explicit validity concept is defined for reliable short-circuiting.

### Locked threading step grammar

- Canonical threading forms (`thread_first`, `thread_last`, `as_thread`, `cond_thread_first`, `cond_thread_last`) adopt dual-mode step input.
- Steps may be provided either as direct callables or as explicit step descriptors.
- Explicit step descriptors are the canonical documented form for diagnostics and examples.
- Direct callable steps are supported for ergonomic shorthand.
- Conditional threading forms accept conditional descriptors that carry both condition and step payload.
- Step-local arity or concept mismatch remains a compile-time error.

### Locked map internal layout behavior

- Map storage remains array-backed and linear-search for strict MVP profiles.
- Map key lookup and equality behavior are content-based; map iteration order is intentionally unspecified.
- `assoc` on an existing key replaces the value in the existing slot of the returned copy.
- `dissoc` on an existing key removes by swap-with-last compaction and decrements logical count.
- `dissoc` on a missing key returns unchanged value.
- For any concrete map value, traversal is deterministic for that value state; no cross-update order guarantee is provided.
- Behavior is selected to preserve Clojure-feel content semantics while minimizing constexpr work, move count, and diagnostic noise.

### Locked validity concept gate for deferred some-threading

- Sentinel equality (`T{}` checks) is never used as validity for short-circuit threading.
- Validity is defined by explicit probe API `probe_valid(x)` returning a bool-convertible result.
- Concept gate name is `probe_validatable`.
- `some_thread_first` and `some_thread_last` remain non-MVP and are enabled only for values that satisfy `probe_validatable`.
- If a threaded value type does not satisfy `probe_validatable`, some-thread APIs are compile-time rejected with step-local diagnostics.
- No implicit fallback validity sources are used (no bool-conversion fallback, no default-value checks, no zero or empty heuristics).
- Sentinel-based collection access policy remains unchanged for MVP (`contains` and `has_index` probe-first guidance still applies).

### Locked validity adapter contract for sentinel-based flows

- Adapter form `with_validity(x, pred)` is adopted for bridging sentinel-based values into validity-aware pipelines.
- Adapter stores value and predicate without heap allocation and preserves no-exception semantics.
- Adapter must be usable in constexpr evaluation when `x` and `pred` satisfy constexpr requirements.
- Adapter exposes validity only through `probe_valid` and does not alter underlying value-equality semantics.
- Adapter use is explicit at call sites; no implicit wrapping is performed by threading forms.
- Adapter is intended for deferred some-thread variants and does not change canonical MVP access or sentinel behavior.

### Locked validity diagnostics style for some-thread gate

- Concept gate failure anchor: "some_thread_* requires probe_validatable intermediate value type".
- Missing customization anchor: "no probe_valid customization found for threaded value type".
- Invalid probe return anchor: "probe_valid must return a bool-convertible result".

### Locked threading arity and rejection rules

- `thread_first`, `thread_last`, and `as_thread` accept one or more steps.
- `cond_thread_first` and `cond_thread_last` accept one or more conditional steps; a false condition skips its step and threads prior value forward unchanged.
- Calling any threading form with zero steps is a compile-time error.
- Step argument mismatch (arity or concept incompatibility) is a compile-time error with step-local diagnostics.
- `some`-style short-circuiting cannot rely on sentinel `T{}` and remains non-MVP even with explicit validity contracts.

## Locked session decisions (2026-08-11)

### Locked Collection concept bootstrap policy

- `concept Collection` is defined in `cljonic-concepts.hpp` and currently requires only `c.size() → std::size_t`.
- YAGNI policy applies: additional requirements (such as `operator[]` and `value_type`) are added to the concept only when the first free function that needs them is implemented.
- `count` is now a single generic template constrained on `concepts::Collection`; per-type overloads are no longer needed.

## Open Questions

### Critical — blocks free function implementation

- **`_M` suffix meaning**: `Conj_M`, `Count_M`, `Empty_M`, `Nth_M` appear in the cheatsheet but are never defined. Does `_M` mean mutable in-place? If so, which collections support it and what is the return type?

- **Mixed-collection return types**: When two collections of different types are combined, what is the result type and capacity? For example, `Concat(Vector{1,2}, Range(3,5))` must produce some `Vector<int, ?>` — the capacity policy is not locked. Same question applies to `Interleave` and other multi-collection operations.

- **`Into` semantics**: How does explicit materialization work? Does `Into(vec, range)` append range elements into vec? Or does `Into` always create a new collection from any source? The API shape and return type are not locked.

### Important — blocks specific collections

- **Float element types in stored collections**: Floating-point ranges are banned, but it is not decided whether `Vector<float, N>`, `Set<float, N>`, or `Map<float, int, N>` are allowed. The `Set` case is the most problematic since set membership is equality-based and float equality is unreliable.

- **`count` for unbound generated collections**: Does `count(Range())` return `CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT`? This must be explicit in the spec, not inferred.

- **String transform return types**: Does `Filter(is_upper, string)` return `String<N>` or `Vector<char, N>`? Does `Map(to_upper, string)` return a `String<N>`? The return type policy for transforms over `String` is not locked.

### Deferrable

- **Stack depth guidance for nested collections**: `Vector<Vector<int, 1000>, 1000>` consumes approximately 1MB of stack. No mechanism prevents this. Documentation guidance is the only mitigation; whether a concept gate or static_assert should warn on excessive nesting depth is undecided.

- **`CLJONIC_HAVE_*` feature flag pattern**: Only `CLJONIC_HAVE_VECTOR_IMPLEMENTATION` exists today. It is undecided whether all collections should follow this pattern to support incremental build and test progression.
