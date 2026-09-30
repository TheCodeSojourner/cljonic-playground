# cljonic Requirements - Module 4: Sequence Producers & Materialization Pipeline

## Purpose and Scope

This module specifies sequence producer types (`Range`, `Repeat`, `Cycle`, `Iterate`, `Repeatedly`), materialization mechanisms (`into`, `fits_into`), and bounded C++ interoperability inputs (`std::span`, `std::string_view`). Module 4 bridges explicit sequence generators to concrete stored collections without dynamic allocation or hidden caching. The active implementation slice covers `Range`, `Repeat`, `Cycle`, `Iterate`, `Repeatedly`, and explicit producer materialization through `into` and `fits_into`. Collection-owned sequence traversal and interoperability accessors remain deferred future work and are not current collection APIs. Standard ranges and views MAY be used internally by future cljonic free-function implementations when they preserve the requirements in this module and MUST NOT be exposed as public cljonic result types. Direct use of standard ranges and views by cljonic applications is outside this library contract.

## Materialization & Producer Invariants

λ REQ-VAL-014(x).
  ∀ materialization_operation whose maximum possible result cardinality is not statically guaranteed to fit: uses an explicit bounded destination collection supplied at the call site
  ∧ ∀ producer_value: ¬∃ (owned result storage ∨ required result-capacity template parameter)
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-VAL-015(x).
  ∀ destination supplied to `into`: encodes the result capacity and result collection type
  ∧ ∀ `into(destination, source)`: returns a new destination-typed collection formed by appending source elements after the destination's existing logical elements ∧ leaves both input values unchanged
  ∧ ∀ `fits_into(destination, source)`: reports whether the complete source can be appended within the destination's remaining capacity
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-VAL-016(x).
  ∀ `count` applied to a materialized collection: returns the exact element count
  ∧ ∀ Module 4 producer: exposes a non-throwing, non-allocating `count` for its bounded observable traversal — finite forms return their exact runtime result count ∧ unbounded forms return `CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT`
  ∧ ∀ unbounded_count: a configured observable traversal cap ∧ ¬∃ claim that the producer has a finite complete result
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-VAL-017(x).
  ∀ compile_time cardinality composition: uses saturating arithmetic ∧ every composed producer cardinality is at most `CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT`
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-VAL-017A(x).
  ∀ Module 4 producer: exposes a non-throwing, non-allocating `is_finite` predicate
  ∧ ∀ `is_finite`: returns `false` exactly when the producer has no finite complete result, despite having a bounded observable traversal count
  ∧ ∀ source where `is_finite` is `false`: `fits_into` returns `false` ∧ ∀ finite_source: `fits_into` compares the source count to `destination.capacity() - destination.count()` without an overflow-prone addition
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-VAL-017B(x).
  ∀ context: `ConstInputRange` requires a usable input-range traversal from a const source expression ∧ is the structural traversal capability ∧ ¬∃ by-itself admission of a type to the cljonic source domain
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-VAL-017C(x).
  ∀ context: `NothrowConstInputRange` refines `ConstInputRange` by requiring non-throwing const `begin` and `end`, dereference, pre-increment, post-increment, and iterator/sentinel comparison operations
  ∧ ∀ `CljonicSource`: admits only a cljonic collection or producer that satisfies `NothrowConstInputRange`
  ∧ ∀ source-domain requirement: shared by `into`, `fits_into`, and producer operations that accept a `CljonicSource` ∧ a nominally admitted collection or producer that cannot provide non-throwing const input traversal ¬∃ satisfaction of `CljonicSource`
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-VAL-017D(x).
  ∀ Module 4 producer type ∈ {`Range`, `Repeat`, `Cycle`, `Iterate`, `Repeatedly`}: default-constructible ∧ nothrow default-constructible ∧ nothrow copy-constructible ∧ nothrow copy-assignable, so that producer values satisfy the `NothrowCollectionElement` storage contract ∧ MAY be stored as map values, vector elements, queue elements, or nested collection components
  ∧ ∀ default_constructed_producer: valid, deterministic producer value consistent with the documented default form — `Range{}` is the finite empty `Range` with default start, end, and step; `Repeat{}` is the finite empty repeat of the element default value; `Repeatedly{}` is the finite empty repeatedly producer over a default-constructed step; `Iterate{}` is the unbounded iterate producer over a default element value and default-constructed step; `Cycle{}` is the unbounded cycle producer over a default-constructed source
  ∧ ∀ producer whose component types are not themselves nothrow default-constructible: ¬∃ satisfaction of `NothrowCollectionElement` ∧ rejected at compile time when used in a storage position
  ∧ ∀ default_construction: ¬∃ (evaluation of producer callbacks ∨ allocation ∨ throw)
  rationale: nothrow default/copy operations let producer values satisfy the `NothrowCollectionElement` storage contract and be stored as collection elements
  {source: stakeholder_decided, decided_by: original_spec_author, rationale_source: origin_artifact}

## Sequence Equality & Deep Traversal Rules

λ REQ-SEQ-015(x).
  ∀ finite sequenceable values where the element types satisfy deep equality: equality MAY compare their elements in sequence order
  ∧ ∀ equality: ¬∃ beginning of an unbounded traversal
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-SEQ-016(x).
  ∀ sequenceable value statically known to be unbounded: ¬∃ provision of sequence equality — equality that compares the value's materialized or observed sequence fails at compile time rather than execute indefinitely
  ∧ ∀ restriction: ¬∃ prohibition of producer parameter equality, which compares only bounded stored parameters without traversing any produced sequence as governed by `REQ-FN-014B`
  rationale: comparing an unbounded sequence would never terminate, so compile-time failure replaces infinite runtime comparison
  {source: stakeholder_decided, decided_by: original_spec_author, rationale_source: origin_artifact}

λ REQ-SEQ-017(x).
  ∀ nested value: deep equality requires finite-observation capability recursively at every nested level
  ∧ ∀ finite outer collection with any nested value requiring unbounded traversal for equality: ¬∃ acquisition of deep equality
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-SEQ-018(x).
  ∀ finite nesting of bounded owning collections ∧ producer values where each value satisfies its storage and capability requirements: supported
  ∧ ∀ existence of nested producer values: ¬∃ cause of implicit materialization of those producers
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-SEQ-019(x).
  ∀ operation ∧ ∀ unbounded producer nested inside an owning collection ∨ another producer: ¬∃ implicit recursive materialization ∨ traversal
  ∧ ∀ bounded inspection ∨ materialization of nested producers: requires explicit caller-selected bounds at each materialized level
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-SEQ-020(x).
  ∀ context: the library distinguishes finite nesting depth from unbounded cardinality
  ∧ ∀ bounded_owning_value_model: ¬∃ requirement of genuinely self-referential infinite structural nesting; such support would require a separately approved indirection or lazy-reference capability
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-SEQ-021(x).
  ∀ operation producing sequenceable results where a useful finite result capacity can be derived from the operation and its inputs: ∧ preferred — returns an owning bounded cljonic value
  ∧ ∀ operation whose results are unbounded ∨ for which no useful finite capacity can be derived without imposing an arbitrary caller-independent limit: ∧ preferred — returns or exposes an explicit producer value
  ∧ ∀ runtime-varying result count alone: ¬∃ requirement of producer semantics when a safe compile-time capacity bound exists
  ∧ ∀ producer_value: self-contained ∧ owns its parameters ∧ ¬∃ (owned materialized result storage ∨ borrowed source storage ∨ hidden mutable state ∨ hidden arbitrary result-capacity limit)
  ∧ ∀ producer satisfying `CljonicSource`: consumable directly by source-taking free functions ∧ complete conversion into an owning destination uses `into`, with `fits_into` providing the completeness preflight where required
  {source: stakeholder_decided, decided_by: original_spec_author}

## Sequence Producer Specifications

λ REQ-FN-009(x).
  ∀ (`cycle` ∧ `iterate` ∧ `range` ∧ `repeat` ∧ `repeatedly`): standalone producer values or producer operations that ¬∃ (owned materialized result storage ∨ required result-capacity template parameter)
  ∧ ∀ producer_value: provides ordinary const `begin`/`end` traversal bounded by `count()` ∧ when it satisfies `CljonicSource`, consumable directly by source-taking free functions
  ∧ ∀ complete conversion into an explicit bounded destination: uses `into`
  ∧ ∀ unbounded_form: terminates traversal after `CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT` elements ∧ produces at most the destination's remaining capacity ∧ MAY return a deterministic prefix
  ∧ ∀ finite_form where the result fits the destination: produces its complete result
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-FN-009A(x).
  ∀ context: the only approved public Cycle spelling is `cycle(source)`, where `source` is a supported Sequence value — a finite collection ∨ an unbounded Producer
  ∧ ∀ Cycle: stores an independent owned copy of the source value ∧ ∀ Producer_source: that copy consists of the producer's parameters and state ∧ ¬∃ materialized result sequence
  ∧ ∀ Cycle: ¬∃ (borrowed source storage ∨ owned materialized result storage)
  ∧ ∀ finite_source: Cycle repeats the source from its beginning after each complete pass
  ∧ ∀ unbounded_source: Cycle preserves the source's observable traversal without requiring the source to reach an end ∧ bounded observation of `cycle(source)` produces the same prefix as bounded observation of `source`
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-FN-009B(x).
  ∀ finite `source` supplied to `cycle` that is empty: valid ∧ bounded observation of the Cycle producer is empty
  ∧ ∀ unbounded `source` supplied to `cycle`: remains unbounded ∧ Cycle continues to report the configured observable traversal cap ∧ `false` from `is_finite()`
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-FN-010(x).
  ∀ (`cycle` ∧ `iterate` ∧ `range` without a finite end ∧ `repeat` without a count ∧ `repeatedly` without a count): `is_finite()` returns `false` ∧ complete-result preflight reports that the result does not fit
  ∧ ∀ (counted `repeat` ∧ counted `repeatedly` ∧ finite `range`): return `true` from `is_finite()` ∧ use their runtime result count for preflight and bounded `into` materialization
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-FN-011(x).
  ∀ producer value ∈ {`Range`, `Repeat`, `Cycle`, `Iterate`, `Repeatedly`}: owns its parameters ∧ remains valid independently of other values ∧ ¬∃ (allocation of result storage ∨ retained borrowed dependency on source collections, callbacks, or input values)
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-FN-012(x).
  ∀ `range`: uses an inclusive start and exclusive end ∧ default start `0` ∧ default step `1`
  ∧ ∀ `range` with step zero: produces an infinite repetition of `start`
  ∧ ∀ nonzero step that moves away from the end: produces an empty finite range ∧ ∀ equal start and end with nonzero step: produces an empty range
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-FN-013(x).
  ∀ `range` with both equal start and end and a zero step: zero-step infinite repetition takes precedence over empty-range behavior
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-FN-013A(x).
  ∀ `Range`: provides a non-throwing, non-allocating, constant-time `contains` predicate over its available bounded-prefix index domain
  ∧ ∀ `contains(range, index)`: reports whether the index is available for bounded observation, not whether the index is present as a produced value
  ∧ ∀ `Range`: ¬∃ (key-based lookup ∨ `get` ∨ callable lookup); positional value retrieval remains deferred until explicitly approved
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-FN-013B(x).
  ∀ `repeat(value)`: constructs an unbounded `Repeat<T>` producer that owns a copy of `value` ∧ produces the same value indefinitely
  ∧ ∀ `repeat(value, count)`: constructs a finite `Repeat<T>` producer that owns a copy of `value` ∧ produces exactly `count` copies, including an empty result when `count` is zero
  ∧ ∀ `Repeat<T>`: exposes `count()` and `is_finite()` as specified by `REQ-VAL-016` and `REQ-VAL-017A` ∧ ¬∃ (exposure of `contains` ∨ positional retrieval ∨ key-based lookup ∨ `get` ∨ callable lookup)
  ∧ ∀ counted_form: uses `count` as its exact runtime result count for `into` and `fits_into` ∧ ∀ uncounted_form: materializes at most the explicit destination's remaining capacity ∧ reports `false` from `fits_into`
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-FN-013C(x).
  ∀ `iterate(step, initial)` where `T` satisfies `NothrowCollectionElement` ∧ `Step` is copy constructible ∧ a const `Step` object can be invoked without throwing with one argument of type `T` and an exact result of type `T`: constructs an unbounded `Iterate<T, Step>` producer
  ∧ ∀ factory: stores `std::decay_t<Step>` — named functions are accepted and stored as copyable function pointers ∧ lambdas and function objects are stored as their decayed value types
  ∧ ∀ `Step`: ∧ preferred — move constructible ∧ construction ∧ preferred — moves from an rvalue step when that operation is available ∧ ¬∃ requirement of move-only steps because the resulting producer is a copyable value
  ∧ ∀ producer: owns a copy of `initial` ∧ a copy of the decayed `step` callback ∧ its validity does not depend on the lifetime of either construction argument
  ∧ ∀ bounded_traversal: produces `initial` first, then applies `step` to the previously produced value to produce each subsequent value
  ∧ ∀ `Iterate`: exposes `count()` and `is_finite()` as specified by `REQ-VAL-016` and `REQ-VAL-017A` ∧ ¬∃ evaluation of `step` during construction ∧ ¬∃ (exposure of `contains` ∨ positional retrieval ∨ key-based lookup ∨ `get` ∨ callable lookup)
  ∧ ∀ `Iterate` where `T`, `Step`, and the supplied arguments support constant evaluation: usable in constant evaluation ∧ remains callable at runtime ∧ materializes at most the explicit destination's remaining capacity ∧ reports `false` from `fits_into`
  rationale: move-only steps are not required because the resulting producer must remain a copyable value
  {source: stakeholder_decided, decided_by: original_spec_author, rationale_source: origin_artifact}

λ REQ-FN-013D(x).
  ∀ `repeatedly(step)`: constructs an unbounded `Repeatedly<T, Step>` producer that owns a decayed copy of the zero-argument step callback ∧ produces a fresh element by invoking the step for each produced element
  ∧ ∀ `repeatedly(count, step)`: constructs a finite `Repeatedly<T, Step>` producer that produces exactly `count` elements, including an empty result when `count` is zero ∧ invokes the step exactly once per produced element during each complete materialization
  ∧ ∀ counted_form: places the count before the step, mirroring Clojure's `(repeatedly n f)` form
  ∧ ∀ factory: accepts named functions, lambdas, and function objects ∧ stores `std::decay_t<Step>` — named functions are accepted and stored as copyable function pointers ∧ lambdas and function objects are stored as their decayed value types
  ∧ ∀ `Step`: copy constructible ∧ the step MAY be moved from an rvalue during construction when supported
  ∧ ∀ `T`: satisfies `NothrowCollectionElement` ∧ a const `Step` object invocable without throwing with zero arguments and an exact result of type `T`
  ∧ ∀ `repeatedly` step: MAY maintain state or produce side effects ∧ remains non-allocating ∧ ¬∃ throw when invoked
  ∧ ∀ producer: owns a copy of the decayed `step` callback ∧ its validity does not depend on the lifetime of the construction argument
  ∧ ∀ callback: ¬∃ evaluation during construction ∧ ∀ zero_count: ¬∃ callback evaluation during traversal
  ∧ ∀ `Repeatedly`: exposes `count()` and `is_finite()` as specified by `REQ-VAL-016` and `REQ-VAL-017A` ∧ ¬∃ (exposure of `contains` ∨ positional retrieval ∨ key-based lookup ∨ `get` ∨ callable lookup) ∧ usable in constant evaluation when `Step` and the supplied arguments support constant evaluation ∧ remains usable at runtime ∧ materializes at most the explicit destination's remaining capacity in uncounted form ∧ reports `false` from `fits_into` in uncounted form
  rationale: mirrors Clojure's `(repeatedly n f)` argument order for the counted form
  {source: stakeholder_decided, decided_by: original_spec_author, rationale_source: origin_artifact}

λ REQ-FN-014(x).
  ∀ callback supplied to `iterate`: pure ∧ non-allocating
  ∧ ∀ callback supplied to `repeatedly`: MAY maintain state or produce side effects, mirroring Clojure's `repeatedly` ∧ remains non-allocating ∧ ¬∃ throw when invoked
  ∧ ∀ counted_form: invokes a callback exactly once per produced element during each `into` call when materialized completely ∧ ∀ uncounted_form: treated as a potentially infinite producer
  rationale: mirrors Clojure's semantics where `repeatedly` may be stateful/side-effecting while `iterate` steps must be pure
  {source: stakeholder_decided, decided_by: original_spec_author, rationale_source: origin_artifact}

λ REQ-FN-014A(x).
  ∀ producer form that can produce an unbounded sequence — including open-ended `range`, `cycle`, uncounted `repeat`, uncounted `repeatedly`, and unbounded `iterate`, regardless of whether two such producers currently appear to produce the same values: the sequence-equality restriction applies
  ∧ ∀ restriction: prohibits equality between produced sequences ∧ ¬∃ prohibition of the producer parameter equality governed by `REQ-FN-014B`
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-FN-014B(x).
  ∀ producer equality: uses producer parameter equality
  ∧ ∀ producer whose stored parameters all admit stable equality: provides the native C++ `==` operator over its own type with producer parameter equality semantics ∧ provides a separate explicitly named structural-comparison free function named `parameters_equal` with semantics identical to its `==` ∧ satisfies the `StableEqualityComparable` and `NothrowStableEqualityComparable` capabilities so that it MAY be used directly as a map key or set element and as a composite component under `REQ-CAP-010`
  ∧ ∀ (`==` ∧ `parameters_equal`): compare only the producer's bounded stored parameters ∧ `constexpr` ∧ `noexcept` ∧ non-mutating ∧ non-allocating ∧ O(1) in the producer's stored parameters ∧ ¬∃ (traversal ∨ observation ∨ materialization of any produced sequence) ∧ ¬∃ implication that two producers with equal parameters produce equal materialized sequences ∧ treat producers with different stored parameters as unequal even when their produced or observed sequences coincide
  ∧ ∀ producer whose stored parameters include a callable component: ¬∃ provision of `==` ∨ `parameters_equal` ∧ ¬∃ satisfaction of `StableEqualityComparable` ∧ rejected at compile time when used in a key or set-element position
  rationale: callable components cannot participate in a stable value-equality comparison, so admitting them would make map/set membership ill-defined
  {source: stakeholder_decided, decided_by: original_spec_author, rationale_source: origin_artifact}

λ REQ-FN-014C(x).
  ∀ producer_construction: ¬∃ evaluation of producer callbacks
  ∧ ∀ `into` call ∧ ∀ element-producing callback: evaluated as needed for each produced element ∧ ∀ repeated `into` calls over the same producer: repeat those callback evaluations rather than reuse a hidden realization cache
  rationale: a hidden realization cache would retain state between calls and silently change side-effecting callback semantics
  {source: stakeholder_decided, decided_by: original_spec_author, rationale_source: AI_inferred}

## C++ Import Interoperability and Materialization

λ REQ-FN-027(x).
  ∀ `into` ∧ `fits_into`: support explicit read-only C++ interop sources in addition to supported cljonic producers ∧ these free functions are the complete-materialization boundary for source imports
  ∧ ∀ `std::span<const T>` source: `into(destination, source)` copies elements into a new collection of the destination's collection type and capacity ∧ `fits_into(destination, source)` tests whether the complete copy fits
  ∧ ∀ `std::string_view` source ∧ `String<N>` destination: `into(destination, source)` copies the source's bytes into a new owning `String<N>` ∧ `fits_into(destination, source)` tests whether the complete source content is valid and fits within `N` content bytes
  ∧ ∀ overload: leaves the source and destination unchanged ∧ ¬∃ (retention of the external view ∨ allocation ∨ throw) ∧ uses the existing deterministic bounded-prefix behavior when the complete result does not fit
  ∧ ∀ runtime `std::string_view` import: replaces every source byte outside the approved `0x01` through `0x7F` range, including `0x00`, with the ASCII period byte `0x2E` in the returned `String<N>` ∧ the replacement occurs before the bounded prefix is selected, so the returned value contains the first `N` converted content bytes when the source has more than `N` bytes
  ∧ ∀ `fits_into(destination, source)`: returns `false` when any source byte is invalid or when the converted content does not fit ∧ returns `true` only when every source byte is valid and the complete content length is no greater than `N`
  ∧ ∀ constant_evaluation where source bytes are invalid ∨ source content is longer than `N`: compile-time diagnostic rather than replacement or bounded-prefix materialization
  ∧ ∀ `std::string_view` input: ¬∃ requirement or copy of a source null terminator as content ∧ an explicitly included null byte is an invalid source byte under this policy
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-FN-027A(x).
  ∃ permitted_path: a supported collection provides direct construction from a compatible bounded C++ source when its destination type and capacity are encoded by the constructed collection type
  ∧ ∀ direct_source_construction: copies source elements or converted characters into owned collection storage ∧ leaves the source unchanged ∧ remains non-allocating, non-throwing, and independent of the source lifetime
  ∧ ∀ `String<N>`: supports direct construction from `std::span<const char, Extent>` sources — for a fixed span extent greater than `N`, construction fails at compile time; for a dynamic extent that exceeds `N`, construction returns the deterministic bounded prefix permitted by `N`
  ∧ ∀ String span import: applies the String invalid-byte replacement policy, including replacing embedded null bytes and bytes above `0x7F` with `'.'`
  ∧ ∀ source extent that is a compile-time constant greater than the destination capacity: construction fails at compile time ∧ ∀ source extent known only at runtime that exceeds capacity: direct construction returns the deterministic bounded prefix permitted by the destination capacity
  ∧ ∀ direct_source_construction: documented as bounded construction rather than complete materialization ∧ ¬∃ introduction of an optional, result, exception, or other checked-constructor error channel
  {source: stakeholder_decided, decided_by: original_spec_author}

## Deferred Internal Range and View Implementation Support

> **Deferred future work — non-binding.** The following range and interoperability requirements are approved future-work contracts. They do not require any current collection to expose a traversal interface or interoperability accessor.

λ REQ-PLAT-017(x).
  ∀ supported collection type ∈ {`Vector`, `Map`, `Set`, `Queue`, `String`}: provides a const range-compatible logical traversal mechanism sufficient for cljonic free-function implementations to visit each active element without depending on collection-specific storage details
  ∧ ∀ traversal_mechanism: non-mutating ∧ non-allocating ∧ non-throwing ∧ `constexpr`-capable where supported ∧ exposes only active elements ∧ preserves each collection's documented logical traversal order
  ∧ ∀ mechanism: provides the equivalent of const `begin`/`end` traversal ∧ the exact iterator and sentinel types remain an implementation detail unless required by a public interoperability contract
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-PLAT-017A(x).
  ∀ supported collection type `C`: const traversal usable from a const collection expression ∧ exposes a read-only dereference result ∧ advances without throwing or allocating ∧ terminates at an end sentinel after exactly `C.count()` active elements
  ∧ ∀ `Queue` traversal: enumerates elements from front to back in FIFO order regardless of physical storage layout
  ∧ ∀ `String` traversal: excludes its null terminator
  ∧ ∀ `Map` traversal: exposes value-semantic `MapEntry` elements ∧ ∀ `Set` traversal: exposes its stored values
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-PLAT-018(x).
  ∀ supported collection: provides an explicitly identified, const-qualified, read-only C++ interoperability accessor or equivalent const logical-range interoperability mechanism
  ∧ ∃ permitted_path: a collection provides `std::span<const T>`-like access only when its active logical elements are represented as one contiguous range ∧ a `String` provides `std::string_view`-like access for its stored content excluding its null terminator
  ∧ ∀ accessor ∨ equivalent mechanism: non-allocating ∧ non-throwing ∧ `constexpr`-capable where supported ∧ exposes only active logical content ∧ ¬∃ (mutable references ∨ mutable iterators ∨ mutable views into collection storage)
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-PLAT-019(x).
  ∃ permitted_path: cljonic free-function implementations use `std::ranges` algorithms and `std::views` for bounded internal traversal, selection, transformation, or reduction
  ∧ ∀ such_use: remains contained within the operation ∧ preserves cljonic's bounded, deterministic, no-heap, no-exception, input-preservation, and owning-result contracts
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-PLAT-020(x).
  ∀ public cljonic semantic free function: ¬∃ return of (standard ranges ∨ standard views ∨ borrowed iterators ∨ other lazy or non-owning traversal results)
  ∧ ∀ traversal-based operation: returns its documented owning cljonic value, scalar value, predicate result, checked result, or producer value
  ∧ ∀ prohibition: does not apply to the explicitly identified collection-owned C++ interoperability accessors governed by `REQ-PLAT-018`, `REQ-PLAT-021`, and `REQ-PLAT-022`
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-PLAT-021(x).
  ∀ C++ interoperability accessor: const-qualified observer returning non-owning, read-only observations whose validity extends only while the source collection remains alive and unmodified
  ∧ ∀ accessor: ¬∃ (extension of the source lifetime ∨ retention of hidden state ∨ public mutable path to collection storage)
  ∧ ∀ read_only_guarantee: applies to the returned element or character access, not merely to the accessor's member-function qualifier
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-PLAT-022(x).
  ∀ C++ interoperability accessor (when provided): exposes the active logical range in the collection's documented representation — vector, set, and queue elements; map entries; and string content excluding its null terminator
  ∧ ∀ queue_accessor: preserves logical FIFO order
  ∧ ∀ queue with wrapped circular storage: ¬∃ exposure of a single contiguous `std::span<const T>` unless it first provides a representation that makes the complete active logical range contiguous without allocating or mutating the source
  ∧ ∀ (map ∧ set) accessor: ¬∃ implication of an ordered semantic contract where none is specified
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-PLAT-022A(x).
  ∀ `Queue`: provides its const range-compatible logical traversal as the interoperability mechanism for FIFO access ∧ ¬∃ requirement to provide a single contiguous standard view when its active logical range is physically wrapped
  ∧ ∀ queue interoperability accessor: MAY return a single `std::span<const T>` only when the complete active logical range is already contiguous ∧ ¬∃ (allocation ∨ copy into a temporary buffer ∨ mutation ∨ normalization of the queue solely to manufacture such a span)
  ∧ ∃ permitted_path: a wrapped queue exposes no contiguous standard-view accessor
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-PLAT-023(x).
  ∀ cljonic user application's direct use of (`std::ranges` ∨ `std::views` ∨ collection interoperability accessors): an application design decision outside the cljonic requirements
  ∧ ∀ context: cljonic documents only the collection interoperability and traversal support required by its own public operations
  {source: stakeholder_decided, decided_by: original_spec_author}

## Traceability and Related Requirements

- **Downstream Artifact**: `Range`, `Repeat`, `Cycle`, `Iterate`, `Repeatedly` producer templates, `into`, `fits_into`, and internal collection traversal support.
- **Governed REQs**: `REQ-VAL-014`–`017D`, `REQ-SEQ-015`–`021`, `REQ-FN-009`–`014C` (incl. `013A`–`013D`), `REQ-FN-027`–`027A`, `REQ-PLAT-017`–`023` (incl. `017A`, `022A`; deferred).