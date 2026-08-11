---
created: 2026-08-05
last_updated: 2026-08-07
status: draft
---

## Core Vocabulary

### CopyOnModifyCollection
- **Definition:** A fixed-capacity value type backed by static storage that returns a modified copy instead of mutating in place. In this repo, vector, set, map, and string all follow this model.
- **Deprecated Synonyms:** Copy-on-Modify Collection, bounded immutable collection, fixed-capacity collection, array-backed collection, deep copy on write, deep copying on write
- **Related:** Vector, Map, Set, String, SentinelBasedAccess, KeywordCatalog, LazySequence, ThreadingForm
- **Usage:** Specification and implementation
- **Examples:** `auto xs = Vector<int, 4>{1, 2, 3}; auto ys = conj(xs, 4);`

### SentinelBasedAccess
- **Definition:** The access model where missing or invalid lookup returns a default sentinel value such as `T{}` or `char{}` instead of raising an error or returning an explicit status object.
- **Deprecated Synonyms:** Sentinel-Based Access, sentinel access, sentinel return, default-value access
- **Related:** CopyOnModifyCollection, DefaultElement, ProbeFirstAccess
- **Usage:** Specification and implementation
- **Examples:** `auto value = get(m, key);` and callers use `contains(m, key)` or `has_index(xs, i)` before relying on the result.

### DefaultElement
- **Definition:** The default-initialized element value returned when a collection access cannot produce a valid logical element. `DefaultElement` names the returned value itself, while `SentinelBasedAccess` names the broader access model.
- **Deprecated Synonyms:** default element, collection default element
- **Related:** SentinelBasedAccess, ProbeFirstAccess, CopyOnModifyCollection
- **Usage:** Architecture, specification, implementation, tests, documentation, and user guidance
- **Examples:** `get(xs, 99)` returns the collection's `DefaultElement` when index `99` is invalid, and `first(xs)` returns the `DefaultElement` when `xs` is empty.

### ProbeFirstAccess
- **Definition:** The required calling discipline for ambiguous domains: first ask a presence or index-validity question, then perform sentinel-based access only when the probe succeeds.
- **Deprecated Synonyms:** Probe-First Access, guard-before-get, presence check
- **Related:** SentinelBasedAccess, DefaultElement, CopyOnModifyCollection
- **Usage:** Specification and implementation
- **Examples:** `if (contains(m, key)) { auto value = get(m, key); }`

### KeywordCatalog
- **Definition:** The canonical finite set of global keyword identities represented by the `cljonic::Keyword` enum class. The catalog defines which keyword values exist; it does not imply Clojure-style namespaced keywords.
- **Deprecated Synonyms:** Keyword Catalog, keyword enum, keyword registry, global keyword catalog
- **Related:** Keyword, String, CopyOnModifyCollection
- **Usage:** Specification and implementation
- **Examples:** `assoc(m, kw::name, value)` where `using kw = cljonic::Keyword;` provides ergonomic access to catalog members.

### Keyword
- **Definition:** A symbolic key value represented in code by a member of `cljonic::Keyword`. Keywords are global-only in the domain sense: they do not use Clojure-style namespace-qualified keyword identities, even though the C++ type itself may live in a namespace.
- **Deprecated Synonyms:** global keyword, keyword value, enum keyword
- **Related:** KeywordCatalog, Map, String, CopyOnModifyCollection
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** A `Map` may use a `Keyword` key such as `kw::name` or `kw::user_id`, with `using kw = cljonic::Keyword;` at the use site.

### Vector
- **Definition:** The cljonic fixed-capacity sequential collection type for ordered element storage with immutable copy-on-modify updates.
- **Deprecated Synonyms:** vector collection, bounded vector, fixed-capacity vector
- **Related:** StoredCollection, CopyOnModifyCollection, String, ProbeFirstAccess, CapacityConstruction
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** A `Vector` supports operations such as `get`, `assoc`, `conj`, `count`, `first`, and `rest`, with out-of-bounds indexed access returning a sentinel element.

### Map
- **Definition:** The cljonic fixed-capacity associative collection type with content-based key lookup, immutable copy-on-modify updates, and intentionally unspecified iteration order.
- **Deprecated Synonyms:** map collection, bounded map, fixed-capacity map
- **Related:** StoredCollection, CopyOnModifyCollection, Keyword, String, ContentEquality, SwapWithLastCompaction
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** A `Map` supports `assoc`, `dissoc`, `contains`, and `get`, with duplicate-key `assoc` replacing an existing value in the returned copy.

### Set
- **Definition:** The cljonic fixed-capacity uniqueness-preserving collection type with content-based membership semantics and immutable copy-on-modify updates.
- **Deprecated Synonyms:** set collection, bounded set, fixed-capacity set
- **Related:** StoredCollection, CopyOnModifyCollection, ContentEquality, UnchangedValueReturn, CapacityConstruction
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** A `Set` supports `conj`, `dissoc`, `contains`, and `count`, with equality defined by logical content rather than storage order.

### String
- **Definition:** The cljonic fixed-capacity string collection type. Its logical capacity is measured in characters, while its physical storage is measured in code units under the active StringEncoding. In UTF-8 mode, storage is sized for the worst-case number of code units needed to hold the declared character capacity.
- **Deprecated Synonyms:** StaticString, static string, fixed-capacity string, string collection
- **Related:** StringEncoding, CharacterCapacity, CodeUnitCapacity, StaticStorage, CopyOnModifyCollection, Keyword, Vector, Map, Set, CapacityConstruction
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** A `String<32>` means room for 32 characters; in UTF-8 mode its backing storage may reserve worst-case code-unit capacity to preserve that logical character capacity.

### StoredCollection
- **Definition:** A collection type that retains all elements in static array-backed memory and supports immutable copy-on-modify updates. The canonical stored collections are `Vector`, `Set`, `String`, and `Map`.
- **Deprecated Synonyms:** Stored Collection, array-backed collection, retained collection
- **Related:** Vector, Set, String, Map, CopyOnModifyCollection, GeneratedCollection, StaticStorage
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** `Vector`, `Set`, `String`, and `Map` are all stored collections; `Range`, `Repeat`, `Cycle`, `Iterate`, and `Repeatedly` are not.

### GeneratedCollection
- **Definition:** A collection type that computes elements on demand without retaining them in storage. Generated collections are finite by construction, referentially transparent in element access, and produce `Vector` or other concrete results when transformed. The canonical generated collections are `Range`, `Repeat`, `Cycle`, `Iterate`, and `Repeatedly`.
- **Deprecated Synonyms:** Generated Collection, computed collection, on-demand collection
- **Related:** Range, Repeat, Cycle, Iterate, Repeatedly, LazySequence, StoredCollection, SinkOperation
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** `Range(0, 10)` computes elements via arithmetic; `Repeat(val, n)` produces `n` copies of `val`; transforms like `map(f, range)` return `Vector` when materialized.

### Range
- **Definition:** A generated collection type that represents an integer sequence defined by start, end, and step parameters with Clojure-compatible four-form overloads. Range elements are computed as `start + i * step` and support compile-time precomputation when construction inputs are constants.
- **Deprecated Synonyms:** Range collection, integer range, generated sequence
- **Related:** GeneratedCollection, FixedWidthIntegralScalar, LazySequence, CollectionMaximumElementCount
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** `Range(0, 10)` produces integers 0–9; `Range(10, 0, -1)` descends; `Range()` defaults to `Range(0, MAX, 1)` where `MAX` is `CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT`.

### Repeat
- **Definition:** A generated collection type that produces a specified number of identical copies of a given element value. Repeat is referentially transparent: every index access returns the same element value.
- **Deprecated Synonyms:** Repeat collection, repetition sequence
- **Related:** GeneratedCollection, LazySequence, CollectionMaximumElementCount
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** `Repeat(42, 5)` produces five copies of `42`; `Repeat(val)` defaults to `CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT` copies; `Take(n, Repeat(val))` implements the Clojure idiom `(take n (repeat val))`.

### Cycle
- **Definition:** A generated collection type that repeatedly cycles through elements of a source collection for a specified cycle count. Element at index `i` is computed as `source[i % count(source)]`. Cycle is referentially transparent and finite by construction.
- **Deprecated Synonyms:** Cycle collection, cycling sequence
- **Related:** GeneratedCollection, LazySequence, StoredCollection, CollectionMaximumElementCount
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** `Cycle(Vector{1, 2}, 3)` produces `1, 2, 1, 2, 1, 2` (six elements total); `Cycle(source)` defaults to `CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT` cycles.

### Iterate
- **Definition:** A generated collection type that repeatedly applies a function to produce a sequence. Element at index `i` is computed by applying the function `i` times to a seed value. Iterate is referentially transparent and finite by construction.
- **Deprecated Synonyms:** Iterate collection, iterated sequence
- **Related:** GeneratedCollection, LazySequence, CollectionMaximumElementCount
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** `Iterate(inc, 0, 5)` produces `0, 1, 2, 3, 4` by repeatedly incrementing from `0`; `Iterate(f, seed)` defaults to `CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT` iterations.

### Repeatedly
- **Definition:** A generated collection type that calls a supplied function repeatedly to produce elements. Unlike `Iterate`, `Repeatedly` does not compose function results; each element is produced by an independent function call. The caller is responsible for ensuring the function is pure; non-pure functions violate referential transparency and produce non-deterministic access.
- **Deprecated Synonyms:** Repeatedly collection, repeated call sequence
- **Related:** GeneratedCollection, LazySequence, CollectionMaximumElementCount
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** `Repeatedly(rand, 10)` produces 10 random values; `Repeatedly(f)` defaults to `CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT` calls; purity is a precondition enforced only by caller discipline, not by the library.

### LazySequence
- **Definition:** A value-owning sequence view that defers element production and transformation until consumption, avoids hidden materialization, and may represent finite or infinite sources.
- **Deprecated Synonyms:** Lazy Sequence, lazy view, deferred sequence
- **Related:** GeneratedCollection, SinkOperation, ThreadingForm, StoredCollection
- **Usage:** Specification and implementation
- **Examples:** `auto xs = range(0, 10); auto ys = map(f, xs); auto out = into_vector<10>(ys);`

### SinkOperation
- **Definition:** An explicit operation that realizes a lazy sequence into a concrete bounded collection or terminal result. Materialization is never implicit.
- **Deprecated Synonyms:** Sink Operation, materialization sink, eager sink, realization endpoint
- **Related:** LazySequence, CopyOnModifyCollection
- **Usage:** Specification and implementation
- **Examples:** `into_vector<16>(xs)`, reduction endpoints, or other named sink operations.

### StringEncoding
- **Definition:** The cljonic-global compile-time enum-class setting that selects the supported string and text behavior for string and regex-related APIs. Consumers choose ASCII or UTF-8, and the remaining string semantics follow from that choice.
- **Deprecated Synonyms:** text encoding mode, string encoding policy
- **Related:** AsciiEncoding, Utf8Encoding, String, RegexProfile
- **Usage:** Specification and implementation
- **Examples:** A consumer chooses `cljonic::StringEncoding::Ascii` or `cljonic::StringEncoding::Utf8`, and string-related APIs compile and run under the selected setting.

### ThreadingForm
- **Definition:** A composition API that improves readability by passing the prior result through a sequence of steps in a defined argument position. The canonical forms are `thread_first`, `thread_last`, `as_thread`, `cond_thread_first`, and `cond_thread_last`.
- **Deprecated Synonyms:** Threading Form, threading macro, pipeline form
- **Related:** LazySequence, ValidityAdapter
- **Usage:** Specification and implementation
- **Examples:** `thread_first(xs, step1, step2)` and `thread_last(xs, step1, step2)`.

### ValidityAdapter
- **Definition:** The explicit wrapper `with_validity(x, pred)` that attaches a validity probe to a value so deferred some-threading APIs can short-circuit without relying on sentinel equality.
- **Deprecated Synonyms:** Validity Adapter, validity wrapper
- **Related:** ThreadingForm, SentinelBasedAccess, ProbeFirstAccess
- **Usage:** Specification and implementation
- **Examples:** `auto wrapped = with_validity(get(m, key), [](auto const& v) { return v != 0; });`

### CanonicalComparison
- **Definition:** The canonical comparison model in which `equal`, `less`, `less_equal`, `greater`, and `greater_equal` are variadic, deep where appropriate, and reject floating-point comparison under the default policy.
- **Deprecated Synonyms:** Canonical Comparison, strict comparison
- **Related:** FloatingPointExclusion, ComparisonArity, ClojureParity
- **Usage:** Specification and implementation
- **Examples:** `equal(a, b, c)` and `less(x, y, z)` return chained boolean results with zero-arity and one-arity calls evaluating to `true`.

### FloatingPointExclusion
- **Definition:** The default rule that floating-point values are not part of canonical comparison or the dynamic value model; any comparison involving floats requires an explicit override API such as `_with` forms.
- **Deprecated Synonyms:** Floating-Point Exclusion, strict float policy
- **Related:** CanonicalComparison, FloatingPointScalar
- **Usage:** Specification and implementation
- **Examples:** `equal(1.0, 1.0)` is rejected under canonical APIs, while an explicit comparator-based override may be allowed.

### CodeUnit
- **Definition:** The atomic storage unit used by cljonic text representations under the active StringEncoding. A code unit is the element type stored in the underlying string buffer.
- **Deprecated Synonyms:** text code unit, storage character unit
- **Related:** StringEncoding, CodeUnitCapacity, UnicodeScalarValue, String
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** In ASCII mode one character maps to one code unit, while in UTF-8 mode one character may occupy multiple code units.

### UnicodeScalarValue
- **Definition:** The unit of logical character semantics for cljonic strings in UTF-8 mode. A Unicode scalar value is the character unit counted, indexed, updated, and sliced by canonical string operations.
- **Deprecated Synonyms:** scalar character value, UTF-8 logical character
- **Related:** Utf8Encoding, String, CharacterCapacity, CodeUnit
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** In UTF-8 mode, `count`, `get`, `assoc`, and slicing operate on Unicode scalar values rather than raw code units.

### AsciiEncoding
- **Definition:** The StringEncoding mode in which cljonic strings interpret each character as a single ASCII code unit.
- **Deprecated Synonyms:** ASCII mode, ASCII string mode
- **Related:** StringEncoding, CodeUnit, CharacterCapacity, CodeUnitCapacity
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** In `AsciiEncoding`, a `String<32>` reserves space for 32 code units because one character always maps to one code unit.

### Utf8Encoding
- **Definition:** The StringEncoding mode in which cljonic strings store UTF-8 code units while preserving a logical character-capacity contract through worst-case storage sizing.
- **Deprecated Synonyms:** UTF-8 mode, UTF-8 string mode
- **Related:** StringEncoding, CodeUnit, UnicodeScalarValue, CharacterCapacity, CodeUnitCapacity
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** In `Utf8Encoding`, a `String<32>` may reserve up to four code units per declared character capacity slot.

### CharacterCapacity
- **Definition:** The logical maximum number of characters a cljonic string promises to hold under the active StringEncoding.
- **Deprecated Synonyms:** logical string capacity, character-count capacity
- **Related:** String, StringEncoding, UnicodeScalarValue, CodeUnitCapacity
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** A consumer reading `String<32>` understands the `32` as character capacity rather than raw byte or code-unit capacity.

### CodeUnitCapacity
- **Definition:** The physical underlying buffer capacity measured in code units, derived from character capacity and the active StringEncoding.
- **Deprecated Synonyms:** storage capacity, encoded buffer capacity
- **Related:** String, CodeUnit, CharacterCapacity, Utf8Encoding
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** In UTF-8 mode, code-unit capacity may be computed as character capacity times the maximum supported UTF-8 code-unit width.

### FixedWidthIntegralScalar
- **Definition:** A scalar numeric type with explicitly fixed bit width, intended to be part of the closed numeric domain for embedded-friendly deterministic arithmetic.
- **Deprecated Synonyms:** fixed-width integer, integral scalar, bounded integral scalar
- **Related:** SupportedIntegralScalarSet, ClosedNumericDomain, NumericPromotionPolicy
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** Fixed-width integral scalar support is expressed through named widths rather than vague platform-shaped types like `int` or `long`.

### SupportedIntegralScalarSet
- **Definition:** The explicitly supported set of fixed-width integral scalar categories available to the library under the current numeric policy.
- **Deprecated Synonyms:** integral type set, supported integral widths
- **Related:** FixedWidthIntegralScalar, ClosedNumericDomain, CommonTypeLattice
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** A strict profile may define the supported integral scalar set as `i8`, `i16`, `i32`, `i64`, `u8`, `u16`, `u32`, and `u64`.

### FloatingPointScalar
- **Definition:** A floating-point scalar category that may exist in the broader language environment but is restricted or excluded from canonical comparison and other strict-domain operations by project policy.
- **Deprecated Synonyms:** float scalar, floating numeric scalar
- **Related:** FloatingPointExclusion, ClosedNumericDomain
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** `float` or `double` values may be present in non-canonical or opt-in contexts even when canonical comparison rejects them.

### ClosedNumericDomain
- **Definition:** The finite, explicitly enumerated set of numeric value categories supported by the library under a given profile, with no implicit escape to unbounded numeric representations.
- **Deprecated Synonyms:** closed numeric set, bounded numeric domain
- **Related:** FixedWidthIntegralScalar, SupportedIntegralScalarSet, FloatingPointScalar, NumericPromotionPolicy, CommonTypeLattice, DeterministicOverflowPolicy, NoHeapConstraint
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** A strict profile may support only fixed-width integral numeric categories and reject any operation that would require an out-of-domain promoted type.

### NumericPromotionPolicy
- **Definition:** The rule set that determines whether and how numeric values are promoted for mixed-type or widened operations within the closed numeric domain.
- **Deprecated Synonyms:** numeric promotion, promotion policy, bounded numeric promotion
- **Related:** ClosedNumericDomain, CommonTypeLattice, DeterministicOverflowPolicy
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** Mixed-width integral operations may promote to a larger fixed-width result type only when that promoted type is part of the active closed numeric domain.

### CommonTypeLattice
- **Definition:** The compile-time promotion lattice that maps supported numeric input type combinations to one statically bounded common result type.
- **Deprecated Synonyms:** common-type lattice, compile-time promotion lattice, numeric type lattice
- **Related:** NumericPromotionPolicy, ClosedNumericDomain, CompileTimeEvaluation
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** A `uint16` plus `uint32` operation may resolve at compile time to a `uint32` result type if that edge exists in the common-type lattice.

### StaticallyBoundedResult
- **Definition:** The requirement that every promoted or computed numeric result type has a compile-time-bounded representation and known storage footprint.
- **Deprecated Synonyms:** statically bounded result, bounded promoted result
- **Related:** CommonTypeLattice, StaticStorage, NoHeapConstraint
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** A promoted numeric operation is valid only if its resolved result type remains fixed-width and statically sized.

### DeterministicOverflowPolicy
- **Definition:** The explicit, profile-defined rule for numeric overflow behavior, chosen so that overflow outcomes are stable, documented, and never hidden behind undefined or silently heap-backed behavior.
- **Deprecated Synonyms:** overflow policy, deterministic overflow behavior
- **Related:** NumericPromotionPolicy, DeterministicBehavior, ClosedNumericDomain
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** A profile may choose compile-time rejection for provable overflow cases, or a clearly documented runtime overflow rule such as wraparound or unchanged-result semantics where appropriate.

## Supporting Vocabulary

### ClojureParity
- **Definition:** The design goal of matching Clojure semantics where feasible without violating the repo's embedded, bounded, and deterministic constraints.
- **Deprecated Synonyms:** Clojure parity, Clojure-feel
- **Related:** LazySequence, CanonicalComparison, FunctionalStyle
- **Usage:** Architecture, specification, and documentation
- **Examples:** Zero-arity comparison returns `true`, lazy sequence behavior is preferred by default, and threading forms are adopted for readable composition.

### EmbeddedConstraint
- **Definition:** The platform constraint set that assumes embedded targets with bounded resources and therefore prioritizes fixed capacity, predictable execution, and explicit profiles.
- **Deprecated Synonyms:** embedded constraint, embedded systems constraint
- **Related:** NoHeapConstraint, NoExceptionConstraint, DeterministicBehavior, StaticStorage
- **Usage:** Architecture, specification, implementation, and documentation
- **Examples:** Collection APIs use fixed-capacity storage and avoid runtime allocation on strict profiles.

### NoHeapConstraint
- **Definition:** The rule that strict profiles must not perform heap allocation anywhere in the library's behavior or infrastructure.
- **Deprecated Synonyms:** no heap allocation, no-heap rule
- **Related:** EmbeddedConstraint, StaticStorage, DeterministicBehavior
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** Verification gates prohibit forbidden allocation APIs and bounded collections store data without dynamic allocation.

### NoExceptionConstraint
- **Definition:** The rule that library behavior is non-throwing and does not depend on exception-based control flow.
- **Deprecated Synonyms:** no exceptions, non-throwing rule
- **Related:** EmbeddedConstraint, SentinelBasedAccess, ContractPolicy, DeterministicBehavior
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** Missing access returns sentinel values instead of throwing, and contract failures route through explicit policy hooks.

### StaticStorage
- **Definition:** Storage with compile-time-bounded capacity and no dependency on dynamic allocation, typically realized with array-backed layouts.
- **Deprecated Synonyms:** static storage, array-backed storage
- **Related:** CopyOnModifyCollection, NoHeapConstraint, EmbeddedConstraint
- **Usage:** Architecture, specification, implementation, and documentation
- **Examples:** Vector, set, map, and string values use fixed-capacity array-backed storage.

### CollectionMaximumElementCount
- **Definition:** The compile-time configuration boundary that sets the maximum element count allowed for cljonic collections. In current documentation this boundary is exposed via the preprocessor macro CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT.
- **Deprecated Synonyms:** collection max element count, maximum collection element count, CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT macro
- **Related:** CapacityConstruction, EmbeddedConstraint, StaticStorage, DeterministicBehavior
- **Usage:** Architecture, specification, implementation, build, and documentation
- **Examples:** A project can raise the boundary by defining CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT before including cljonic headers or by setting it from compiler/build flags.

### DeterministicBehavior
- **Definition:** The requirement that behavior, failure modes, and profile-selected semantics remain predictable and stable for the same inputs and configuration.
- **Deprecated Synonyms:** deterministic semantics, deterministic execution
- **Related:** EmbeddedConstraint, NoHeapConstraint, NoExceptionConstraint, ContractPolicy
- **Usage:** Architecture, specification, tests, and documentation
- **Examples:** Full-capacity `conj` returns the unchanged value deterministically, and invalid runtime regex compilation returns a stable invalid-pattern sentinel.

### ContractPolicy
- **Definition:** The project-wide contract model governing public API boundaries, invariant checks, and the deterministic policy hook used when contract violations occur.
- **Deprecated Synonyms:** contract, contract boundary, contract handling
- **Related:** NoExceptionConstraint, DeterministicBehavior
- **Usage:** Architecture, specification, implementation, and documentation
- **Examples:** Public API preconditions are expressed as contracts and violations route through a selected terminate, trap, or assert policy.

### SemanticConcept
- **Definition:** A small, composable compile-time concept used to encode semantic API constraints and reject invalid combinations with clear diagnostics.
- **Deprecated Synonyms:** concept, semantic constraint
- **Related:** CanonicalComparison, ThreadingForm, StableHandleModel
- **Usage:** Specification, implementation, tests, and documentation
- **Examples:** Strict comparison excludes floating-point types through concept constraints, and deferred some-threading uses `probe_validatable` as a concept gate.

### FunctionalStyle
- **Definition:** The preferred programming style for the library: pure free functions, immutable return values, explicit sinks, and minimal hidden side effects.
- **Deprecated Synonyms:** functional style, immutable style
- **Related:** CopyOnModifyCollection, LazySequence, ThreadingForm, ClojureParity
- **Usage:** Architecture, specification, implementation, and documentation
- **Examples:** Canonical APIs are free-function-first, transforms remain lazy by default, and eager materialization is explicit.

### CanonicalCollectionOperationFamily
- **Definition:** The canonical free-operation surface for collection behavior: `count`, `get`, `assoc`, `dissoc`, `conj`, `contains`, `first`, and `rest`. Names and semantics default to Clojure-aligned behavior unless constrained divergence is explicitly documented.
- **Deprecated Synonyms:** canonical operation set, collection operation family
- **Related:** ClojureParity, FunctionalStyle, CopyOnModifyCollection, ProbeFirstAccess
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** A new collection operation should prefer one of the canonical names before introducing a new name, and any constrained semantic divergence is documented in specs and tests.

### HeaderOnlyDistribution
- **Definition:** The packaging model in which the library is delivered as headers only, with development sources organized separately from the generated distribution artifact.
- **Deprecated Synonyms:** header-only, header-only library
- **Related:** AmalgamatedHeader, StaticStorage
- **Usage:** Architecture, implementation, build, and documentation
- **Examples:** Production implementation units live as headers and are combined into a distributable single-header output.

### AmalgamatedHeader
- **Definition:** The generated single-header distribution artifact assembled from the per-function production headers that remain the source of truth during development.
- **Deprecated Synonyms:** amalgamated header, single-header distribution
- **Related:** HeaderOnlyDistribution, SourceLayout
- **Usage:** Implementation, build, tests, and documentation
- **Examples:** Build automation regenerates the distributable header from the production header tree.

### CompileTimeEvaluation
- **Definition:** The architectural distinction between behavior or validation performed during compile-time evaluation and behavior that remains available at runtime.
- **Deprecated Synonyms:** compile-time evaluation, constexpr/consteval boundary
- **Related:** SemanticConcept, RegexProfile
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** Range precomputes internal fields when inputs are compile-time constants, and some validations are intended to fail during compilation.

### Regex
- **Definition:** The cljonic regex abstraction for pattern-based text matching, designed to support profile-specific implementations without changing the public vocabulary.
- **Deprecated Synonyms:** regular expression, regex abstraction
- **Related:** RegexProfile, StableHandleModel, RegexPattern, RegexMatcher
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** Regex behavior is exposed through stable pattern and matcher abstractions rather than profile-specific public API names.

### RegexPattern
- **Definition:** The compiled or compile-time-formed regex value used as the reusable pattern handle for matching operations.
- **Deprecated Synonyms:** regex pattern, pattern handle, compiled pattern
- **Related:** Regex, PatternValidityProbe, InvalidPatternSentinel, StableHandleModel
- **Usage:** Specification, implementation, tests, and documentation
- **Examples:** A `RegexPattern` may come from a compile-time-oriented path or a runtime `re-pattern` path while preserving the same public handle model.

### RegexMatcher
- **Definition:** The regex matching abstraction that applies a RegexPattern to input text and exposes match behavior without changing public API shape across profiles.
- **Deprecated Synonyms:** regex matcher, matcher handle
- **Related:** Regex, RegexPattern, RegexProfile
- **Usage:** Specification, implementation, tests, and documentation
- **Examples:** A runtime `re-matcher` operation produces a `RegexMatcher` associated with a `RegexPattern` and an input string.

### RegexLiteral
- **Definition:** A regex expression provided in source form, especially in compile-time-oriented flows where the pattern text is part of the program and may be validated during compilation.
- **Deprecated Synonyms:** regex literal, pattern literal
- **Related:** RegexPattern, CompileTimeEvaluation, RegexProfile
- **Usage:** Specification, implementation, tests, and documentation
- **Examples:** Compile-time regex support may accept a `RegexLiteral` and produce a `RegexPattern` during compilation.

### RegexProfile
- **Definition:** The capability mode that selects compile-time-oriented regex behavior by default and optionally enables runtime regex support without changing the public regex vocabulary.
- **Deprecated Synonyms:** regex profile, compile-time regex profile, runtime regex profile
- **Related:** StableHandleModel, EmbeddedConstraint, Regex, RegexPattern, RegexMatcher, RegexLiteral
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** Strict embedded profiles may disable optional runtime regex support, while host-oriented profiles may permit `re-pattern` and `re-matcher`.

### StableHandleModel
- **Definition:** The API design rule that profile changes must not force a public regex API shape change; capabilities vary behind stable handles rather than by renaming the user-facing abstraction.
- **Deprecated Synonyms:** stable handle model, stable handle API
- **Related:** StableHandleModel, EmbeddedConstraint
- **Usage:** Architecture, specification, implementation, and documentation
- **Examples:** Compile-time and runtime regex modes share a stable pattern handle interface even when backing behavior differs.

### NamespaceAdoptionRoadmap
- **Definition:** The staged scope plan that defines which Clojure namespaces are considered high-fit, which belong to MVP, which are optional, and in what order they should be adopted.
- **Deprecated Synonyms:** namespace roadmap, high-fit namespace roadmap
- **Related:** MvpNamespaceCutLine, OptionalNamespaceCutLine, NamespacePhaseOrder, ClojureParity
- **Usage:** Architecture, specification, planning, and documentation
- **Examples:** `clojure.string` and `clojure.set` are in the MVP cut line, while `clojure.parallel` is optional and later-phase.

### MvpNamespaceCutLine
- **Definition:** The explicit boundary separating namespaces required for the minimum viable product from namespaces deferred beyond the initial system scope.
- **Deprecated Synonyms:** MVP namespace cut line, MVP scope boundary
- **Related:** NamespaceAdoptionRoadmap, OptionalNamespaceCutLine
- **Usage:** Architecture, specification, planning, and documentation
- **Examples:** `clojure.string`, `clojure.set`, `clojure.data`, `clojure.walk`, and `clojure.zip` define the current MVP namespace boundary.

### OptionalNamespaceCutLine
- **Definition:** The explicit boundary identifying namespaces that remain supported conceptually but are intentionally excluded from MVP scope.
- **Deprecated Synonyms:** optional namespace cut line, deferred namespace scope
- **Related:** NamespaceAdoptionRoadmap, MvpNamespaceCutLine
- **Usage:** Architecture, specification, planning, and documentation
- **Examples:** `clojure.edn`, `clojure.xml`, and `clojure.parallel` remain outside initial MVP scope.

### NamespacePhaseOrder
- **Definition:** The ordered rollout plan that sequences namespace adoption by foundation, structure, format, and performance concerns.
- **Deprecated Synonyms:** phase order, namespace phase order
- **Related:** NamespaceAdoptionRoadmap, MvpNamespaceCutLine, OptionalNamespaceCutLine
- **Usage:** Architecture, planning, and documentation
- **Examples:** Phase 1 focuses on foundation namespaces before structural or format-oriented namespaces are added.

### SourceLayout
- **Definition:** The structural organization rule separating production headers, test code, and generated distribution artifacts into distinct locations with clear source-of-truth boundaries.
- **Deprecated Synonyms:** source layout, production/test separation
- **Related:** HeaderOnlyDistribution, AmalgamatedHeader, QualityGate
- **Usage:** Architecture, implementation, build, tests, and documentation
- **Examples:** Production headers remain the development source of truth while tests live in a separate tree and generated output is treated as distribution.

### QualityGate
- **Definition:** A required validation checkpoint in CI or local workflows that enforces a non-optional engineering constraint before changes are accepted.
- **Deprecated Synonyms:** quality gate, enforcement gate
- **Related:** NoHeapVerification, DeterministicBehavior, SourceLayout
- **Usage:** Architecture, implementation, tests, CI, and documentation
- **Examples:** Linting, sanitizer runs, coverage thresholds, and docs sample compilation are all quality gates.

### NoHeapVerification
- **Definition:** The layered verification regime that proves strict profiles do not use heap allocation through compile-time or link-time prohibition, runtime counters, and binary symbol checks.
- **Deprecated Synonyms:** no-heap verification, heap-allocation verification gate
- **Related:** NoHeapConstraint, QualityGate, EmbeddedConstraint
- **Usage:** Architecture, implementation, tests, CI, and documentation
- **Examples:** Host verification profiles can fail the build when forbidden allocation symbols are present or runtime allocation counters are incremented.

## Behavioral Specification Vocabulary

### ComparisonArity
- **Definition:** The observable rule governing how comparison APIs behave at zero, one, and many arguments.
- **Deprecated Synonyms:** comparison arity, multi-arity comparison, variadic chained semantics
- **Related:** CanonicalComparison, ClojureParity
- **Usage:** Specification, tests, and documentation
- **Examples:** Zero-arity and one-arity comparison calls return `true`, while multi-argument calls evaluate chained comparison semantics.

### ContentEquality
- **Definition:** The equality model in which values compare by logical content rather than insertion order or storage position.
- **Deprecated Synonyms:** deep value comparison, order-independent equality, content-based equality
- **Related:** CanonicalComparison, CopyOnModifyCollection
- **Usage:** Specification, tests, and documentation
- **Examples:** Map and set equality ignore ordering and compare by contained logical values.

### CapacityConstruction
- **Definition:** The construction contract that supports both literal-deduced and explicit-capacity creation while rejecting oversized initializers at compile time.
- **Deprecated Synonyms:** collection construction pattern, explicit-capacity construction, literal-deduced construction
- **Related:** CopyOnModifyCollection, CompileTimeEvaluation, StaticStorage
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** Explicit-capacity empty construction is valid, but an initializer count that exceeds capacity is a compile-time failure.

### CardinalityModel
- **Definition:** The explicit representation of whether a sequence is finite or infinite and what size information is valid to expose for that sequence.
- **Deprecated Synonyms:** finite/infinite cardinality, cardinality representation
- **Related:** LazySequence, CompileTimeEvaluation
- **Usage:** Specification, implementation, tests, and documentation
- **Examples:** Infinite ranges do not report `max size_t` as a fake size, while finite ranges may report exact size when computable.

### InvalidPatternSentinel
- **Definition:** The stable invalid value returned when runtime regex compilation fails under the no-error policy.
- **Deprecated Synonyms:** invalid regex sentinel, invalid pattern value
- **Related:** RegexProfile, SentinelBasedAccess, DeterministicBehavior
- **Usage:** Specification, implementation, tests, and documentation
- **Examples:** An invalid runtime `re-pattern` result can be distinguished from a valid non-match through an explicit validity probe.

### PatternValidityProbe
- **Definition:** The explicit probe API used to distinguish an invalid pattern sentinel from a valid pattern that simply does not match.
- **Deprecated Synonyms:** pattern validity probe, `pattern_valid`
- **Related:** InvalidPatternSentinel, RegexProfile, ProbeFirstAccess
- **Usage:** Specification, implementation, tests, and documentation
- **Examples:** `pattern_valid(p)` is checked before treating a pattern handle as usable.

### StepDescriptor
- **Definition:** The canonical documented representation of a threading step, used to express step payload and diagnostics more explicitly than a bare callable.
- **Deprecated Synonyms:** explicit step descriptor, conditional descriptor
- **Related:** ThreadingForm, SemanticConcept
- **Usage:** Specification, implementation, tests, and documentation
- **Examples:** Threading documentation and diagnostics prefer descriptor-style steps even though direct callable shorthand remains supported.

### UnchangedValueReturn
- **Definition:** The contract that certain operations preserve and return the original logical value when the requested modification cannot be applied.
- **Deprecated Synonyms:** unchanged return, deterministic unchanged return
- **Related:** DeterministicBehavior, CopyOnModifyCollection, SentinelBasedAccess
- **Usage:** Specification, implementation, tests, and documentation
- **Examples:** Full-capacity `conj`, full-capacity `assoc`, and missing-key `dissoc` all return the unchanged value.

### SwapWithLastCompaction
- **Definition:** The removal strategy that deletes an existing element by replacing it with the last logical element and decrementing count, without preserving iteration order.
- **Deprecated Synonyms:** swap-with-last, swap-with-last erase, compaction removal
- **Related:** UnchangedValueReturn, CopyOnModifyCollection, DeterministicBehavior
- **Usage:** Specification, implementation, tests, and documentation
- **Examples:** `dissoc` on an existing map key compacts storage by moving the last entry into the removed slot.

### ProbeValidityConcept
- **Definition:** The explicit concept gate requiring a bool-convertible `probe_valid(x)` so deferred some-thread forms can short-circuit without relying on sentinel equality.
- **Deprecated Synonyms:** `probe_validatable`, validity concept gate
- **Related:** ValidityAdapter, SemanticConcept, ThreadingForm
- **Usage:** Specification, implementation, tests, and documentation
- **Examples:** Some-thread forms reject intermediate values that do not satisfy the `probe_validatable` concept.

## Relationship Notes

- CopyOnModifyCollection is the foundational value model for the repo.
- SentinelBasedAccess and ProbeFirstAccess define the canonical error-handling discipline for collection APIs.
- Keyword and KeywordCatalog define the canonical keyword identity model used by map-like structures.
- LazySequence and SinkOperation define the execution boundary between deferred pipelines and concrete results.
- ThreadingForm and ValidityAdapter define the readable composition model and the boundary for deferred some-thread semantics.
- CanonicalComparison and FloatingPointExclusion define the canonical comparison contract.
- ClosedNumericDomain, NumericPromotionPolicy, CommonTypeLattice, StaticallyBoundedResult, and DeterministicOverflowPolicy define the bounded numeric semantics that fit the embedded constraint model.
- ClojureParity and FunctionalStyle explain where semantics are intentionally borrowed from Clojure.
- EmbeddedConstraint, NoHeapConstraint, NoExceptionConstraint, StaticStorage, and DeterministicBehavior define the platform and execution constraints.
- ContractPolicy and SemanticConcept define how correctness constraints are expressed across layers.
- HeaderOnlyDistribution and AmalgamatedHeader define the packaging vocabulary for build and user documentation.
- CompileTimeEvaluation, RegexProfile, and StableHandleModel define capability boundaries that architecture must preserve across compile-time and runtime modes.
- NamespaceAdoptionRoadmap, MvpNamespaceCutLine, OptionalNamespaceCutLine, and NamespacePhaseOrder define architectural scope and rollout boundaries.
- SourceLayout, QualityGate, and NoHeapVerification define the structural and verification constraints that architecture must treat as first-class.
- ComparisonArity, ContentEquality, CapacityConstruction, and CardinalityModel define observable behavior contracts that should map directly into Allium specs.
- InvalidPatternSentinel, PatternValidityProbe, StepDescriptor, UnchangedValueReturn, SwapWithLastCompaction, and ProbeValidityConcept capture edge-case and rejection semantics that behavioral specs must name explicitly.