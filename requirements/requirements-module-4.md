# cljonic Requirements - Module 4: Sequence Producers & Materialization Pipeline

## Conversion Admission

Conversion admission is governed by `REQ-CONV-001`–`003`; exact callback-result types are specified by `REQ-FN-013C`–`013D`. This section introduces no additional obligations.

## Purpose and Scope

This module specifies sequence producer types (`Range`, `Repeat`, `Cycle`, `Iterate`, `Repeatedly`), materialization (`into`) and its completeness preflight (`fits_into`), and bounded C++ interoperability inputs (`std::span`, `std::string_view`) for direct source construction. Module 4 bridges explicit sequence generators to concrete stored collections without dynamic allocation or hidden caching. The active implementation slice covers `Range`, `Repeat`, `Cycle`, `Iterate`, `Repeatedly`, and explicit producer materialization through `into`, preflighted by `fits_into`. Sequence conversion (`seq`) remains deferred future work; `first`, `next`, and `rest` are active traversal APIs, with `rest` forwarding to `next` for every supported producer. Collection-owned const logical-range traversal and read-only C++ interoperability accessors are also active. Standard ranges and views MAY be used internally by cljonic free-function implementations when they preserve the requirements in this module and MUST NOT be exposed as public cljonic result types. Direct use of standard ranges and views by cljonic applications is outside this library contract.

## Materialization & Producer Invariants

λ REQ-VAL-014(x).
  ∀ materialization_operation whose maximum possible result cardinality is not statically guaranteed to fit: uses an explicit bounded destination collection supplied at the call site
  ∧ ∀ producer_value: ¬∃ (owned result storage ∨ required result-capacity template parameter)
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-VAL-015(x).
  ∀ destination supplied to `into`: encodes the result capacity and result collection type
  ∧ ∀ `into(destination, source)`: returns a new destination-typed collection formed by folding source values in traversal order through the destination's insertion semantics ∧ leaves both input values unchanged
  ∧ ∀ `Vector` ∨ `Queue` destination: appends values after its existing logical contents ∧ preserves FIFO order for `Queue`
  ∧ ∀ `Set` destination: inserts distinct values ∧ duplicate insertion consumes no capacity
  ∧ ∀ `Map` destination: associates compatible `MapEntry` values ∧ a matching key replaces its value without consuming capacity ∧ later entries for the same key replace earlier entries
  ∧ ∀ `String` destination: appends source characters in order ∧ valid non-NUL ASCII characters are preserved
  ∧ ∀ invalid `String` source character: constant evaluation is rejected by the String compile-time validation diagnostic ∧ runtime insertion replaces that character with `'.'` and continues traversal
  ∧ ∀ `String` destination: runtime materialization processes characters until capacity is full ∧ invalid characters before capacity exhaustion are normalized and do not stop traversal
  ∧ ∀ `Set` destination: a duplicate value is accepted as a no-op even when full
  ∧ ∀ `Map` destination: a `MapEntry` with an existing key is accepted as a value replacement even when full
  ∧ ∀ destination: stops before the first source value that requires unavailable capacity ∧ never skips a rejected value to process later values
  ∧ ∀ unbounded_source: traversal remains bounded by `CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT` even when Set duplicates or Map key replacements consume no capacity
  ∧ ∀ `fits_into(destination, source)`: reports whether all source values can be inserted under these same semantics ∧ ¬∃ requirement that source count equals result count growth
  {source: stakeholder_decided, decided_by: human, timestamp: 2026-10-29}

λ REQ-VAL-016(x).
  ∀ public free function `count(value)`: admits values satisfying `SequenceableCollection` or `SequenceableProducer` ∧ returns the non-throwing, non-allocating count observation defined for that value
  ∧ ∀ materialized collection: `count` returns the exact logical element count
  ∧ ∀ Module 4 producer: exposes a non-throwing, non-allocating count observation for its bounded traversal — finite forms return their exact runtime result count except that finite `Range` returns `min(complete cardinality, CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT)` ∧ unbounded forms return `CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT`
  ∧ ∀ finite `Range`: count saturation does not change its finite complete-result status ∧ a Cycle over that source uses the saturated observed count for tail-position wraparound
  ∧ ∀ unbounded_count: a configured observable traversal cap ∧ ¬∃ claim that the producer has a finite complete result
  {source: stakeholder_decided, decided_by: human, timestamp: 2026-10-28}

λ REQ-VAL-017(x).
  ∀ compile_time cardinality composition: uses saturating arithmetic ∧ every composed producer cardinality is at most `CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT`
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-VAL-017A(x).
  ∀ Module 4 producer: exposes a non-throwing, non-allocating `is_finite` predicate
  ∧ ∀ `is_finite`: returns `false` exactly when the producer has no finite complete result, despite having a bounded observable traversal count
  ∧ ∀ source where `is_finite` is `false`: `fits_into` returns `false` regardless of duplicate values or matching keys
  ∧ ∀ finite_source ∧ (`Vector` ∨ `Queue`) destination: `fits_into` compares the source count to `destination.capacity() - destination.count()` without an overflow-prone addition
  ∧ ∀ finite_source ∧ (`Set` ∨ `Map`) destination: `fits_into` accounts for existing destination values or keys and earlier source values or keys ∧ MAY traverse the source and evaluate producer callbacks ∧ returns `false` at the first insertion requiring unavailable capacity ∧ otherwise returns `true` after the complete traversal
  ∧ ∀ finite_source ∧ `String` destination: `fits_into` compares source count to remaining character capacity without evaluating or rejecting character validity, because runtime materialization normalizes invalid characters
  ∧ ∀ preflight: preserves both input values ∧ shares no realization cache with `into` ∧ callback effects through references or static storage MAY remain observable across the independent preflight and materialization traversals
  ∧ ∀ value-sensitive preflight followed by `into`: a completeness prediction applies when the subsequent traversal yields the same values; ¬∃ guarantee for callbacks whose external state changes those values between traversals
  {source: stakeholder_decided, decided_by: human, timestamp: 2026-10-29}

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
  ∀ sequenceable_value with bounded_logical_observation ∧ deep_equality(element_type): ∃ permitted_path: ordered_elementwise_equality
  ∧ ∀ equality: ¬∃ beginning of an unbounded traversal
  {source: stakeholder_decided, decided_by: human, timestamp: 2026-10-15}

λ REQ-SEQ-016(x).
  ∀ sequence_equality over admitted_producers: compare(bounded_logical_observations) ∧ ¬reject_solely_for(unbounded_complete_sequence)
  ∧ equal_observations ≡ same(observed_count ∧ ordered_elements) | ¬claim(equality_beyond_observation_bound)
  ∧ producer_parameter_equality: distinct ∧ governed_by(`REQ-FN-014B`)
  {source: stakeholder_decided, decided_by: human, timestamp: 2026-10-15}

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
  ∧ ∀ unbounded_form: traverses at most `CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT` elements ∧ adds at most the destination's remaining capacity in new elements or keys ∧ duplicate insertion and matching-key replacement MAY consume further source values within that traversal cap ∧ MAY return a deterministic prefix under `REQ-VAL-015`
  ∧ ∀ finite_form where the result fits the destination: produces its complete result
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-FN-009A(x).
  ∀ context: the only approved public Cycle spelling is `cycle(source)`, where `source` is a supported cljonic collection ∨ producer satisfying `CljonicSource`, whether finite ∨ unbounded
  ∧ ∀ Cycle: stores an independent owned copy of the source value ∧ ∀ Producer_source: that copy consists of the producer's parameters and state ∧ ¬∃ materialized result sequence
  ∧ ∀ Cycle: ¬∃ (borrowed source storage ∨ owned materialized result storage)
  ∧ ∀ finite_source: Cycle repeats the source from its beginning after each complete pass
  ∧ ∀ unbounded_source: Cycle preserves the source's observable traversal without requiring the source to reach an end ∧ bounded observation of `cycle(source)` produces the same prefix as bounded observation of `source`
  {source: stakeholder_decided, decided_by: human, timestamp: 2026-10-15}

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

λ REQ-FN-013I(x).
  ∀ user-facing Range creation guidance: prefers the lowercase `range` factory at runtime and compile time ∧ the public `Range<T>` type and its existing constructors remain supported
  ∧ ∀ deduced `range(end)`, `range(start, end)`, or `range(start, end, step)`: returns `Range<T>` where every supplied argument has the same unqualified signed integral type `T`, matching successful constructor deduction without common-type promotion
  ∧ ∀ explicitly typed `range<T>()`, `range<T>(end)`, `range<T>(start, end)`, or `range<T>(start, end, step)`: returns `Range<T>` ∧ `T` satisfies `RangeElement` ∧ every supplied argument follows the existing Range constructor safe implicit conversion rules under `REQ-CONV-001`
  ∧ ∀ zero-argument `range<T>()`: returns the default empty Range with start `0`, end `0`, and step `1` ∧ untyped `range()` is rejected because the element type cannot be deduced
  ∧ ∀ `range`: behaves identically to the equivalent Range constructor, including default parameters, exclusive endpoint, zero-step precedence, empty ranges, finite cardinality saturation, and bounded observation under `REQ-FN-012`, `REQ-FN-013`, `REQ-VAL-016`, `REQ-VAL-017A`, and `REQ-VAL-017D`
  ∧ ∀ `range`: `constexpr` ∧ `noexcept` ∧ non-allocating ∧ owns its parameters ∧ preserves input arguments ∧ ¬∃ materialization of result storage during construction
  ∧ ∀ rejected factory call: compile-time rejection ∧ targeted operation diagnostic under `REQ-DIAG-009` ∧ ¬∃ fallback result ∨ silent narrowing ∨ implicit common-type promotion
  {source: stakeholder_decided, decided_by: human, timestamp: 2026-10-29}

λ REQ-FN-013A(x).
  ∀ `Range`: provides a non-throwing, non-allocating, constant-time `Range::contains` member predicate over available bounded-observation positions; this predicate does not make `Range` `Indexed`
  ∧ ∀ `range.contains(index)`: reports whether the position is available for bounded observation, not whether the index is present as a produced value
  ∧ ∀ `Range`: the generic `contains(range, index)` free-function form is excluded
  ∧ ∀ `Range`: ¬∃ (`Indexed` capability ∨ key-based lookup ∨ `get` ∨ callable lookup); positional value retrieval remains deferred until explicitly approved
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-FN-013B(x).
  ∀ user-facing Repeat creation guidance: prefers the existing lowercase `repeat` factory at runtime and compile time ∧ the public `Repeat<T>` type and its existing constructors remain supported
  ∧ ∀ `repeat(value)`: constructs an unbounded `Repeat<T>` producer that owns a copy of `value` ∧ produces the same value indefinitely
  ∧ ∀ `repeat(count, value)`: constructs a finite `Repeat<T>` producer that owns a copy of `value` ∧ produces exactly the normalized count of copies under `REQ-CONV-002E`, including an empty result when `count` ≤ 0
  ∧ ∀ counted `Repeat` construction: uses `Repeat<T>{count, value}` ∨ deduced `Repeat{count, value}` ∧ when deduced, repeated element type is determined by `value` ∧ finite count admission follows `REQ-CONV-002E`
  ∧ ∀ counted factory ∨ constructor: first argument is always the count ∧ second argument is always the repeated value ∧ ¬∃ alternate value-first overload ∨ argument-order inference
  ∧ ∀ `Repeat<T>`: exposes `count()` and `is_finite()` as specified by `REQ-VAL-016` and `REQ-VAL-017A` ∧ ¬∃ (exposure of `contains` ∨ positional retrieval ∨ key-based lookup ∨ `get` ∨ callable lookup)
  ∧ ∀ counted_form: uses the normalized count as its exact source count for `into` and `fits_into`, not as destination count growth for deduplicating or key-replacing insertion ∧ ∀ uncounted_form: materializes under `REQ-VAL-015` within the existing traversal cap ∧ reports `false` from `fits_into`
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-FN-013C(x).
  ∀ `iterate(step, initial)` where `T` satisfies `NothrowCollectionElement` ∧ `Step` is copy constructible ∧ a const `Step` object can be invoked without throwing with one argument of type `T` and an exact result of type `T`: constructs an unbounded `Iterate<T, Step>` producer
  ∧ ∀ factory: stores `std::decay_t<Step>` — named functions are accepted and stored as copyable function pointers ∧ lambdas and function objects are stored as their decayed value types
  ∧ ∀ `Step`: ∧ preferred — move constructible ∧ construction ∧ preferred — moves from an rvalue step when that operation is available ∧ ¬∃ requirement of move-only steps because the resulting producer is a copyable value
  ∧ ∀ producer: owns a copy of `initial` ∧ a copy of the decayed `step` callback ∧ its validity does not depend on the lifetime of either construction argument
  ∧ ∀ bounded_traversal: produces `initial` first, then applies `step` to the previously produced value to produce each subsequent value
  ∧ ∀ `Iterate`: exposes `count()` and `is_finite()` as specified by `REQ-VAL-016` and `REQ-VAL-017A` ∧ ¬∃ evaluation of `step` during construction ∧ ¬∃ (exposure of `contains` ∨ positional retrieval ∨ key-based lookup ∨ `get` ∨ callable lookup)
  ∧ ∀ `Iterate` where `T`, `Step`, and the supplied arguments support constant evaluation: usable in constant evaluation ∧ remains callable at runtime ∧ materializes under `REQ-VAL-015` within the existing traversal cap ∧ reports `false` from `fits_into`
  rationale: move-only steps are not required because the resulting producer must remain a copyable value
  {source: stakeholder_decided, decided_by: original_spec_author, rationale_source: origin_artifact}

λ REQ-FN-013D(x).
  ∀ `repeatedly(step)`: constructs an unbounded `Repeatedly<T, Step>` producer that owns a decayed copy of the zero-argument step callback ∧ produces a fresh element by invoking the step for each produced element
  ∧ ∀ `repeatedly(count, step)`: constructs a finite `Repeatedly<T, Step>` producer that produces exactly the normalized count of elements under `REQ-CONV-002E`, including an empty result when `count` ≤ 0 ∧ invokes the step exactly once per produced element during each complete materialization
  ∧ ∀ counted_form: places the count before the step, mirroring Clojure's `(repeatedly n f)` form
  ∧ ∀ factory: accepts named functions, lambdas, and function objects ∧ stores `std::decay_t<Step>` — named functions are accepted and stored as copyable function pointers ∧ lambdas and function objects are stored as their decayed value types
  ∧ ∀ `Step`: copy constructible ∧ the step MAY be moved from an rvalue during construction when supported
  ∧ ∀ `T`: satisfies `NothrowCollectionElement` ∧ a `Step` object invocable without throwing with zero arguments and an exact result of type `T`
  ∧ ∀ `Repeatedly` traversal: invokes a copy of the producer's stored step callback ∧ mutable state held by the stored callback value starts from its stored state for each traversal
  ∧ ∀ `repeatedly` step: MAY maintain state or produce side effects ∧ state reached through references or static storage remains shared across traversals ∧ remains non-allocating ∧ ¬∃ throw when invoked
  ∧ ∀ producer: owns a copy of the decayed `step` callback ∧ its validity does not depend on the lifetime of the construction argument
  ∧ ∀ callback: ¬∃ evaluation during construction ∧ ∀ zero_count: ¬∃ callback evaluation during traversal
  ∧ ∀ `Repeatedly`: exposes `count()` and `is_finite()` as specified by `REQ-VAL-016` and `REQ-VAL-017A` ∧ ¬∃ (exposure of `contains` ∨ positional retrieval ∨ key-based lookup ∨ `get` ∨ callable lookup) ∧ usable in constant evaluation when `Step` and the supplied arguments support constant evaluation ∧ remains usable at runtime ∧ materializes under `REQ-VAL-015` within the existing traversal cap in uncounted form ∧ reports `false` from `fits_into` in uncounted form
  rationale: mirrors Clojure's `(repeatedly n f)` argument order for the counted form
  {source: stakeholder_decided, decided_by: original_spec_author, rationale_source: origin_artifact}

λ REQ-FN-013E(x).
  ∀ `next` of a finite counted `Repeat`: return a finite `Repeat` with the same repeated value and count `max(count - 1, 0)` — `next(repeat(3, 7))` yields a finite `Repeat` of two sevens ∧ `next(repeat(1, 7))` and `next(repeat(0, 7))` each yield an empty finite `Repeat` of sevens
  ∧ ∀ `next` of an unbounded `repeat(value)`: return an unbounded `Repeat<T>` of the same value with its observation policy unchanged
  ∧ ∀ result: independently owning ∧ self-contained ∧ preserves the input ∧ works coherently with `count`, `is_finite`, `is_empty`, `into`, and `fits_into` under their existing producer contracts
  {source: stakeholder_decided, decided_by: human, timestamp: 2026-10-28}

λ REQ-FN-013F(x).
  ∀ `next` of a `Range` with zero step: return a logically unbounded `Range` with the same start, end, and zero step, preserving the configured observation cap ∧ repeated `next` does not exhaust the logically unbounded result
  ∧ ∀ nonzero-step `Range` with an empty logical result: return an empty `Range` with the original start, end, and step unchanged
  ∧ ∀ nonzero-step `Range` with exactly one logical element: return an empty `Range` with start and end both equal to the original exclusive end and with the original step, without computing `start + step`
  ∧ ∀ nonzero-step `Range` with more than one logical element: return a `Range` whose start is the original start plus the original step and whose end and step are unchanged
  ∧ ∀ result: preserve the source element type and logical generation order ∧ preserve the input ∧ `constexpr` ∧ `noexcept` ∧ non-mutating ∧ non-allocating ∧ ¬∃ RTTI
  {source: stakeholder_decided, decided_by: human, timestamp: 2026-10-28}

λ REQ-FN-013G(x).
  ∀ `next` of a `Cycle` with a finite, nonempty source: return the same `Cycle` type with its starting position advanced by one modulo the source's observed count, so the resulting cyclic sequence begins with the original second element
  ∧ ∀ `next` of a `Cycle` with an empty finite source: preserve the empty observation
  ∧ ∀ `next` of a `Cycle` with an unbounded source: return the same `Cycle` type over `next(source)` with its starting position reset, preserving the Cycle observation cap and unbounded status
  ∧ ∀ result: independently owning ∧ self-contained ∧ preserves the input ∧ preserves logical generation order ∧ works coherently with `count`, `is_finite`, `is_empty`, `into`, and `fits_into`
  ∧ ∀ operation: `constexpr` ∧ `noexcept` ∧ non-allocating ∧ ¬∃ RTTI ∧ ¬∃ implicit materialization
  ∧ ∀ callback invocation when the Cycle source is unbounded: follows `next(source)` semantics
  {source: stakeholder_decided, decided_by: human, timestamp: 2026-10-28}

λ REQ-FN-013H(x).
  ∀ `next` of an `Iterate` with a nonzero observation cap: invoke its step exactly once and use the step result as the returned `Iterate`'s initial value ∧ preserve the observation cap
  ∧ ∀ `next` of an `Iterate` with a zero observation cap: return an equivalent `Iterate` without invoking the step
  ∧ ∀ `next` of a finite `Repeatedly` with positive count: invoke one copy of its stored step exactly once ∧ discard the produced value ∧ store the advanced step copy in the returned `Repeatedly` ∧ decrement its count by one
  ∧ ∀ `next` of an empty finite `Repeatedly`: return an equivalent empty finite `Repeatedly` without invoking its step
  ∧ ∀ `next` of an unbounded `Repeatedly` with a nonzero observation cap: invoke one copy of its stored step exactly once ∧ discard the produced value ∧ store the advanced step copy in the returned `Repeatedly` ∧ preserve the observation cap
  ∧ ∀ `next` of an unbounded `Repeatedly` with a zero observation cap: return an equivalent `Repeatedly` without invoking its step
  ∧ ∀ step state held by value: advancement is stored only in the returned `Repeatedly` ∧ the source remains unchanged
  ∧ ∀ step effects through references or static storage: effects occur when `next` is called and remain shared
  ∧ ∀ result: same producer type ∧ independently owning ∧ preserves input producer value ∧ `constexpr` when arguments support it ∧ `noexcept` ∧ non-allocating ∧ ¬∃ RTTI ∧ ¬∃ materialization of the remaining sequence
  {source: stakeholder_decided, decided_by: human, timestamp: 2026-10-28}

λ REQ-FN-014(x).
  ∀ callback supplied to `iterate`: pure ∧ non-allocating
  ∧ ∀ callback supplied to `repeatedly`: MAY maintain state or produce side effects, mirroring Clojure's `repeatedly` ∧ remains non-allocating ∧ ¬∃ throw when invoked
  ∧ ∀ counted_form: invokes a callback exactly once per produced element during each `into` call when materialized completely ∧ ∀ uncounted_form: treated as a potentially infinite producer
  rationale: mirrors Clojure's semantics where `repeatedly` may be stateful/side-effecting while `iterate` steps must be pure
  {source: stakeholder_decided, decided_by: original_spec_author, rationale_source: origin_artifact}

λ REQ-FN-014A(x).
  ∀ producer with unbounded_complete_sequence: sequence_equality_admission = bounded_observation_admission ∧ retain(element_domain_constraints)
  ∧ ∀ admitted_sequence_comparison: traverse_only(configured_bounded_observation) ∧ ¬claim(complete_infinite_sequence_equality)
  {source: stakeholder_decided, decided_by: human, timestamp: 2026-10-15}

λ REQ-FN-014B(x).
  ∀ native_producer_equality: uses producer_parameter_equality ∧ distinct_from(`equal`_bounded_observation_equality)
  ∧ ∀ producer whose stored parameters all admit stable equality: provides only the native C++ `==` operator over its own type with producer parameter equality semantics ∧ ¬∃ (any separately named producer-comparison free function) ∧ satisfies the `StableEqualityComparable` and `NothrowStableEqualityComparable` capabilities so that it MAY be used directly as a map key or set element and as a composite component under `REQ-CAP-010`
  ∧ ∀ native producer `==`: compare only the producer's bounded stored parameters ∧ `constexpr` ∧ `noexcept` ∧ non-mutating ∧ non-allocating ∧ O(1) in the producer's stored parameters ∧ ¬∃ (traversal ∨ observation ∨ materialization of any produced sequence) ∧ ¬∃ implication that two producers with equal parameters produce equal materialized sequences ∧ treat producers with different stored parameters as unequal even when their produced or observed sequences coincide
  ∧ ∀ producer whose stored parameters include a callable component: ¬∃ provision of `==` ∧ ¬∃ satisfaction of `StableEqualityComparable` ∧ rejected at compile time when used in a key or set-element position
  rationale: callable components cannot participate in a stable value-equality comparison, so admitting them would make map/set membership ill-defined
  {source: stakeholder_decided, decided_by: human, timestamp: 2026-10-16, rationale_source: origin_artifact}

λ REQ-FN-014C(x).
  ∀ producer_construction: ¬∃ evaluation of producer callbacks
  ∧ ∀ `into` call ∧ ∀ element-producing callback: evaluated as needed for each produced element ∧ ∀ repeated `into` calls over the same producer: repeat those callback evaluations rather than reuse a hidden realization cache
  rationale: a hidden realization cache would retain state between calls and silently change side-effecting callback semantics
  {source: stakeholder_decided, decided_by: original_spec_author, rationale_source: origin_artifact}

## C++ Import Interoperability and Materialization

λ REQ-FN-027(x).
  ∀ `into` ∧ `fits_into`: materialization and its completeness preflight operate over cljonic sources only
  ∧ ∀ `into(destination, source)` ∧ `fits_into(destination, source)` call: the destination satisfies exactly one of `CljonicVector`, `CljonicSet`, `CljonicMap`, `CljonicQueue`, or `CljonicString` ∧ the source is exactly a `CljonicCollection` or `CljonicProducer` satisfying `CljonicSource`
  ∧ ∀ admitted source: its actual traversal reference provides values admitted by the destination's insertion operation under the existing safe implicit conversion and non-throwing construction rules (`REQ-CONV-001`–`003`) ∧ `Map` destinations require compatible nominal `MapEntry` values, not arbitrary pairs or alternating keys and values ∧ `String` destinations require values safely convertible to `char`
  ∧ ∀ admitted `into` ∧ `fits_into`: share the same destination/source compatibility domain
  ∧ ∀ source outside that domain — including any non-cljonic C++ range or view such as `std::span<const T>` or `std::string_view` — ¬∃ admitted as an `into` or `fits_into` source
  ∧ ∀ destination outside those five collection families — including every producer — ¬∃ admitted as an `into` or `fits_into` destination
  ∧ ∀ `String` destination: `into` appends valid characters, rejects invalid characters during constant evaluation using the String compile-time validation diagnostic, and normalizes invalid characters to `'.'` at runtime while continuing until capacity is exhausted
  ∧ ∀ `String` destination: `fits_into` measures only finite source count against remaining capacity and does not evaluate character validity
  ∧ external C++ interoperability sources are materialized only through direct source construction (`REQ-FN-027A`) ∧ ¬∃ (retention of the external view ∨ allocation ∨ throw) by `into` or `fits_into`
  ∧ ∀ admitted `into` ∧ `fits_into`: usable in constant evaluation when the arguments support it ∧ non-throwing ∧ non-allocating ∧ non-mutating with respect to the input values
  ∧ ∀ admitted `fits_into`: returns whether the complete source can be inserted under `REQ-VAL-015` and `REQ-VAL-017A` ∧ returns `false` for every unbounded producer
  ∧ ∀ rejected `fits_into` call: fails at compile time with the targeted rejection diagnostic governed by `REQ-DIAG-009`
  ∧ ∀ rejected `into` call: fails at compile time with the targeted rejection diagnostic governed by `REQ-DIAG-009`
  {source: stakeholder_decided, decided_by: human, timestamp: 2026-10-29, rationale_source: origin_artifact}

λ REQ-FN-027A(x).
  ∃ permitted_path: a supported collection provides direct construction from a compatible bounded C++ source when its destination type and capacity are encoded by the constructed collection type
  ∧ ∀ direct_source_construction: copies source elements or converted characters into owned collection storage ∧ leaves the source unchanged ∧ remains non-allocating, non-throwing, and independent of the source lifetime
  ∧ ∀ `String<N>`: supports direct construction from `std::span<const char, Extent>` sources — for a fixed span extent greater than `N`, construction fails at compile time; for a dynamic extent that exceeds `N`, construction returns the deterministic bounded prefix permitted by `N`
  ∧ ∀ String span import: applies the String invalid-byte replacement policy, including replacing embedded null bytes and bytes above `0x7F` with `'.'`
  ∧ ∀ source extent that is a compile-time constant greater than the destination capacity: construction fails at compile time ∧ ∀ source extent known only at runtime that exceeds capacity: direct construction returns the deterministic bounded prefix permitted by the destination capacity
  ∧ ∀ direct_source_construction: documented as bounded construction rather than complete materialization ∧ ¬∃ introduction of an optional, result, exception, or other checked-constructor error channel
  ∧ ∀ constructor_argument: a cljonic_collection ∨ cljonic_producer is admitted only when its type is exactly the destination's element type — in that case it is one element (pack construction), mirroring a Clojure collection literal whose contents are elements (`#{(range 10)}`)
  ∧ ∀ constructor_argument whose type is a cljonic_collection ∨ cljonic_producer that is ¬the destination's element type: ¬∃ admitted as a direct source constructor argument — materializing a cljonic collection or producer source is the role of `into` ∧ `fits_into` (`REQ-FN-027`), never the constructor; the direct source constructor admits only non-cljonic C++ interoperability sources
  ∧ ∀ enclosure_construction (a class-template-argument-deduction form, no explicit template argument list) whose sole argument is a cljonic_collection ∨ cljonic_producer: deduces the destination with that source value as its element type and a capacity of one, producing a one-element value that encloses the argument — mirroring a Clojure collection literal (`[v]`, `#{v}`) — ¬∃ copy or element materialization
  ∧ ∀ same-type_copy_construction (an explicit template argument list naming the argument's own destination type and capacity): remains a copy of the argument, distinct from enclosure_construction
  {source: stakeholder_decided, decided_by: original_spec_author}

## Internal Range and View Implementation Support

The following range and interoperability requirements are binding contracts: every supported collection provides a const range-compatible logical traversal mechanism and a read-only C++ interoperability accessor.

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
- **Governed REQs**: `REQ-VAL-014`–`017`, `REQ-VAL-017A`–`017D`, `REQ-SEQ-015`–`021`, `REQ-FN-009`, `REQ-FN-009A`–`009B`, `REQ-FN-010`–`013`, `REQ-FN-013A`–`013I`, `REQ-FN-014`, `REQ-FN-014A`–`014C`, `REQ-FN-027`, `REQ-FN-027A`, `REQ-PLAT-017`, `REQ-PLAT-017A`, `REQ-PLAT-018`–`022`, `REQ-PLAT-022A`, `REQ-PLAT-023`.