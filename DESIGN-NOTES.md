# cljonic design notes archive

This file is intentionally reduced to an archival note. The normative project decisions are defined in [cljonic-requirements.md](cljonic-requirements.md), and that file is now the source of truth for supported behavior, constraints, and required capability decisions.

The detailed design-note content was migrated into numbered REQ clauses and removed because it duplicated, drifted from, or was weaker than the requirements document. Historical rationale remains available in version control history, but no further design-note text is authoritative for project behavior.

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

- **Mixed-collection return types (collection type and capacity)**: When two collections of different types are combined, what is the result collection type and capacity? For example, `Concat(Vector{1,2}, Range(3,5))` must produce some `Vector<int, ?>` — should stored collections always be the result type? Should capacity be sum of counts, maximum, or explicit parameter? Same question applies to `Interleave` and other multi-collection operations.

- **Mixed-collection element type resolution**: When combining collections with different element types (e.g., `Vector<int8_t>` and `Vector<int32_t>`), what is the result element type? Must all inputs be homogeneous (same element type), or does the implementation deduce a common type? How does the strict floating-point policy interact with mixed-type operations?

- **`Into` semantics**: How does explicit materialization work? Does `Into(vec, range)` append range elements into vec? Or does `Into` always create a new collection from any source? The API shape and return type are not locked.

### Important — blocks specific collections

- **Float element types in stored collections**: Floating-point ranges are banned, but it is not decided whether `Vector<float, N>`, `Set<float, N>`, or `Map<float, int, N>` are allowed. The `Set` case is the most problematic since set membership is equality-based and float equality is unreliable.

- **`count` for unbound generated collections**: Does `count(Range())` return `CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT`? This must be explicit in the spec, not inferred.

- **String transform return types**: Does `Filter(is_upper, string)` return `String<N>` or `Vector<char, N>`? Does `Map(to_upper, string)` return a `String<N>`? The return type policy for transforms over `String` is not locked.

### Deferrable

- **Stack depth guidance for nested collections**: `Vector<Vector<int, 1000>, 1000>` consumes approximately 1MB of stack. No mechanism prevents this. Documentation guidance is the only mitigation; whether a concept gate or static_assert should warn on excessive nesting depth is undecided.

- **`CLJONIC_HAVE_*` feature flag pattern**: Only `CLJONIC_HAVE_VECTOR_IMPLEMENTATION` exists today. It is undecided whether all collections should follow this pattern to support incremental build and test progression.
