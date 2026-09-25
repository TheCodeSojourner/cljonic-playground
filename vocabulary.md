---
created: 2026-08-05
last_updated: 2026-09-19
status: draft
---

## Core Vocabulary

## Current Scope

The current implementation and tests cover the core collection types (`Vector`,
`Map`, `Set`, `Queue`, and `String`), the active `Range`, `Repeat`, `Iterate`,
and `Repeatedly`
producers,
the active unbounded `cycle(source)` producer, their direct construction,
member-observation, callable collection forms, current primitive free-function
operations, and explicit producer materialization through `into` and
`fits_into`. Semantic sequence operations remain deferred future work for every
collection;
const range traversal and read-only C++ interoperability are active
infrastructure. Module 3 establishes the concrete, array-backed, bounded
collection types, their contiguous storage strategies, linear scan lookup
algorithms, swap-and-remove policies, and primitive free functions. Module 4's
active slice establishes `Range`, `Repeat`, `Iterate`, unbounded `cycle(source)`,
`Repeatedly`, and
explicit producer materialization. These terms govern stored collection and
producer building blocks used across all higher-order algorithms.

### Collection
- **Definition:** A bounded cljonic value or data structure admitted to the closed nominal collection domain and governed by collection-specific capacity, access, failure, and value-semantic rules.
- **Deprecated Synonyms:** collection value, cljonic collection
- **Related:** ClosedNominalCollectionDomain, CollectionKind, BoundedStorage, OwningValue
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `Vector`, `Map`, `Set`, `Queue`, and `String` are supported collection kinds in the closed domain.


### Sequence
- **Definition:** A collection or producer domain with a defined logical traversal sequence whose elements may support underflow, indexed access, or sequence observation according to its capabilities. A Sequence may also be semantically ordered, as with `Vector`, `String`, or `Queue`, but logical traversal order does not itself imply semantic ordering; `Map` and `Set` are sequenceable while remaining semantically unordered.
- **Deprecated Synonyms:** ordered sequence, sequential value
- **Related:** Collection, Sequenceable, Traversal, LogicalTraversalOrder, Producer
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** A future semantic traversal increment may define `seq`, `first`, `next`, and `rest`; current collections expose const range traversal without claiming those operations.


### Sequenceable
- **Definition:** The descriptive form of the `Seqable` capability: participation in the cljonic sequence-operation family under the applicable lifecycle, emptiness, counting, traversal, and sequence-conversion contracts. Being sequenceable does not by itself imply indexed access, lookup, association, contiguous storage, a particular iterator category, or a standard C++ range representation.
- **Deprecated Synonyms:** sequence capability
- **Related:** Sequence, Seqable, Traversal, ConstRangeTraversal
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** A collection may be sequenceable when the deferred `Seqable` capability is implemented; current collections expose `is_empty` and `count` without claiming the full sequence interface.


### Indexed
- **Definition:** A semantic capability for *efficient* (constant-time, non-traversing) integer-indexed access to a collection or producer's logical elements. An `Indexed` value defines a bounded logical index domain, provides non-mutating default-returning access for that domain, and provides `contains` as the non-throwing, non-allocating index-in-range predicate over that same integer domain. Negative indexes are outside the domain when representable by the accepted index type. `Indexed` refines `Lookup` for the integer lookup domain. `Indexed` is distinct from merely supporting positional access by traversal: a value that requires traversal to reach a position is not `Indexed`. `Indexed` also does not imply invocability (`IFn`, callable syntax): `Vector`/`Map`/`Set` are both `Indexed`/`Lookup`-family and invocable, but a producer such as `Range` can be `Indexed` without being invocable.
- **Deprecated Synonyms:** indexed access, indexed collection access, index access capability
- **Related:** Lookup, Contains, DefaultReturningResult, CapabilityConcept, Range
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `Vector`, `String`, and `Range` are `Indexed`; `contains(xs, i)` distinguishes an invalid index from a valid position whose value equals its default element. `Range`'s indexed access is arithmetic and O(1) over its available bounded prefix, but `Range` is not invocable like `Vector`.


### Lookup
- **Definition:** A semantic capability for non-mutating access through a collection-defined lookup domain. A `Lookup` collection defines its lookup key type, returns the associated value for a present key or the documented default or fallback result when absent, and provides `contains` to distinguish presence without inspecting the returned value. `Lookup` does not imply map-style association or mutation.
- **Deprecated Synonyms:** lookup access, general lookup capability, associative access
- **Related:** Indexed, Associative, Contains, DefaultReturningResult, CapabilityConcept
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `Map` looks up keys, `Set` looks up values, and `Vector` and `String` look up integer indexes.


### Seqable
- **Definition:** The canonical semantic capability for producing a collection's documented logical traversal representation. A `Seqable` collection provides sequence conversion without mutating the source, dynamic allocation, or exceptions; the result is an independently valid bounded value whose elements and order follow the collection's traversal semantics. `Seqable` remains independent of `Indexed`, `Lookup`, and `Associative` and is deferred until the sequence requirements are implementation-backed.
- **Deprecated Synonyms:** sequenceable capability, sequence capability
- **Related:** Sequenceable, Sequence, Seq, Traversal, ConstRangeTraversal, CapabilityConcept
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `Seqable` is a deferred capability for `Vector`, `Map`, `Set`, `Queue`, and `String`; its presence does not follow merely from `is_empty` or `count`.


### Associative
- **Definition:** A semantic capability for non-mutating association of a key and value into a collection, producing a new collection value. An `Associative` collection defines its key and value types, valid-key and capacity policy, preserves its source, and provides `can_assoc` as a non-throwing, non-allocating preflight predicate that agrees with `assoc` for immutable inputs. `Associative` does not by itself imply `Indexed` or `Lookup`; each concrete collection's lookup relationship is explicit.
- **Deprecated Synonyms:** associative capability, associative collection access
- **Related:** Lookup, Assoc, CanAssoc, CopyOnModifyCollection, CapabilityConcept
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `Map`, `Vector`, and `String` are `Associative`; `Set` is not associative because its supported update is `conj`, and `Queue` has no access or association capability by default.


### Traversal
- **Definition:** Observation of a collection or producer's active elements through its documented traversal mechanism and bounds, without implying materialization, mutation, indexed access, or a semantic ordering guarantee. The repeatable sequence used during observation is named `LogicalTraversalOrder`.
- **Deprecated Synonyms:** sequence traversal, iteration
- **Related:** Sequence, ProducerIteration, BoundedInspection, LogicalTraversalOrder
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `first`, `next`, and `rest` are deferred traversal operations whose behavior will be implemented for all collections in a future increment.


### ConstRangeTraversal
- **Definition:** Read-only logical traversal of a supported collection or other explicitly approved sequenceable value, such as `MapEntry`, through a const range-compatible mechanism that is usable from a const expression, exposes a read-only dereference result, advances without allocation or exceptions, and terminates after exactly the value's active element count in its documented `LogicalTraversalOrder`. ConstRangeTraversal is `constexpr`-capable where the value and element types permit it; it exposes only active elements and does not mutate the source value.
- **Deprecated Synonyms:** const range, read-only range traversal
- **Related:** Traversal, CapabilityConcept, Sequenceable, SequenceableCollection, MapEntry, ReadOnlyInteropAccessor, LogicalTraversalOrder, NoMutationConstraint
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** Queue traversal provides FIFO const range observation even when physical storage is circular; vector, map, set, and string expose their active contiguous ranges.


### LogicalTraversalOrder
- **Definition:** The collection-specific repeatable sequence in which active elements are observed by traversal operations, independent of physical storage layout and distinct from semantic ordering guarantees. A logical traversal order may be exposed for deterministic implementation behavior without making the collection semantically ordered.
- **Deprecated Synonyms:** logical iteration order, traversal order
- **Related:** Traversal, ConstRangeTraversal, Vector, Map, Set, Queue, String
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** Queue traversal is front-to-back FIFO; map and set traversal is repeatable but remains semantically unordered.


### ReadOnlyInteropAccessor
- **Definition:** The mandatory collection-owned C++ interoperability boundary for every supported collection. It is const-qualified and exposes a bounded, non-owning, read-only observation without extending source lifetime or exposing mutable storage access. It is distinct from ordinary internal `ConstRangeTraversal`, but may directly expose that traversal or be realized as a named const-qualified accessor or another explicitly identified const logical-range interoperability mechanism. Its concrete representation may be a `ContiguousConstView` or another const range-compatible form, depending on the collection's physical logical representation.
- **Deprecated Synonyms:** interoperability view accessor, const interop view
- **Related:** PlatformInteroperability, NonOwningView, StandardViewType, ContiguousConstView, ConstRangeTraversal
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `Vector::view()`, `Map::view()`, `Set::view()`, and `String::view()` provide contiguous standard views where applicable; Queue uses `begin()`/`end()` logical traversal and does not provide a contiguous view.


### ContiguousConstView
- **Definition:** A read-only non-owning interoperability observation whose complete active logical range is represented by one contiguous sequence of const elements or characters.
- **Deprecated Synonyms:** const span view, contiguous read-only view
- **Related:** ReadOnlyInteropAccessor, StandardViewType, ContiguousStorage, NonOwningView
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** A `String` content accessor may expose a `std::string_view`-like contiguous const view excluding its null terminator.


### Capacity
- **Definition:** The finite number of elements or logical units a bounded value can hold under its declared type and configuration. Capacity MAY be zero; a zero-capacity collection is a valid empty owning value rather than an invalid or uninitialized collection.
- **Deprecated Synonyms:** collection capacity, declared capacity
- **Related:** BoundedStorage, CollectionMaximumElementCount, CapacityConstruction, FullState, DefaultElement, ConstRangeTraversal
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `Vector<int, 4>` has capacity four, `String<32>` has a logical character capacity of thirty-two, and `Vector<int, 0>` is a valid empty collection that reports zero count, supports empty const traversal, and cannot store elements.


### FreeFunction
- **Definition:** A public operation expressed as a namespace-level function rather than a member of the value it observes or transforms.
- **Deprecated Synonyms:** free function API, namespace-level operation
- **Related:** SemanticPredicateName, Traversal, PreflightPredicate
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `contains(xs, i)` and `fits_into(destination, producer)` are free-function forms of orthogonal operations.


### BoundedStorage
- **Definition:** Storage with a finite, statically knowable bound that does not require dynamic allocation for supported operations.
- **Deprecated Synonyms:** bounded storage, fixed storage
- **Related:** StaticInspectableStorage, Capacity, NoHeapConstraint, EmbeddedConstraint
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** A collection's inline buffer is bounded storage whose capacity is visible from its type or configuration.


### SourceConstruction
- **Definition:** Construction of an owning cljonic collection directly from an external range or view source, copying source elements into the collection's bounded storage without retaining the source or borrowing its lifetime.
- **Deprecated Synonyms:** direct source import, bounded source import
- **Related:** Collection, Capacity, BoundedStorage, OwningValue, PlatformInteroperability, RangeViewMaterialization
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `Vector<int, 4>{source_range}`, `Map<Key, Value, 4>{source_view}`, and analogous constructors for `Set`, `Queue`, and `String` perform SourceConstruction.


### RangeViewMaterialization
- **Definition:** The bounded-prefix realization of a range or view source into an owning collection during SourceConstruction. A source with a statically known extent that exceeds the destination capacity is rejected at compile time; a dynamic or otherwise unknown extent is copied until the destination is full and then follows the collection's bounded-prefix policy.
- **Deprecated Synonyms:** range materialization, view materialization, source range realization
- **Related:** SourceConstruction, ConstRangeTraversal, NonOwningView, StandardViewType, Capacity, BoundedPrefixResult, ProducerMaterialization
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** A `std::span<const int, 3>` can be checked against `Vector<int, 2>` at compile time, while a dynamic `std::span<const int>` is copied only through the destination's available capacity.


### PlatformInteroperability
- **Definition:** The capability boundary describing which standard C++ and embedded-platform facilities cljonic may use while preserving its documented resource, diagnostic, and value semantics. PlatformInteroperability includes mandatory `ConstRangeTraversal` support for cljonic implementations and mandatory collection-owned `ReadOnlyInteropAccessor` support for every supported collection; a `ContiguousConstView` is conditional on the collection's physical logical representation. Public semantic cljonic free functions MUST NOT return standard ranges, standard views, borrowed iterators, or other lazy or non-owning traversal results. PlatformInteroperability does not require every collection to expose one contiguous standard view or define how applications use standard C++ ranges and views.
- **Deprecated Synonyms:** platform compatibility, standard-library interoperability
- **Related:** EmbeddedConstraint, NoHeapConstraint, NoExceptionConstraint, ConstRangeTraversal, ReadOnlyInteropAccessor, StandardViewType, ContiguousConstView
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** A capability may be implemented with standard C++ facilities only when their behavior satisfies the active cljonic profile.


### AggregateLikeStruct
- **Definition:** A user-defined aggregate-shaped type that may participate in an operation only when every capability required by that operation is explicitly satisfied.
- **Deprecated Synonyms:** aggregate-like type, aggregate struct
- **Related:** SimpleAggregateBoundary, StableEquality, TotalOrder
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** Storage of an aggregate-like struct does not by itself imply equality, ordering, parsing, or other capabilities.


### StableEquality
- **Definition:** A non-floating-point equality capability whose comparison is stable for the supported value and operation domain.
- **Deprecated Synonyms:** stable equality comparable, equality capability
- **Related:** GeneralEquality, NumericEquality, AggregateLikeStruct
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `stable_equality_comparable<T>` admits a type for equality-dependent operations when `a == b` returns `bool`.


### TotalOrder
- **Definition:** A stable, complete ordering capability layered on stable equality and expressed by a supported ordering relation.
- **Deprecated Synonyms:** totally ordered, total ordering capability
- **Related:** StableEquality, NumericPolicy
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `totally_ordered<T>` is required only by operations whose contract needs a total order.


### DiscreteNumericType
- **Definition:** A numeric type with discrete values suitable for bounded index, count, capacity, or conversion policies without floating-point equality semantics.
- **Deprecated Synonyms:** discrete numeric, integral numeric type
- **Related:** NumericPolicy, ExactConversion
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** An index policy may accept a discrete numeric type and reject a negative or non-representable value before access.


### NumericPolicy
- **Definition:** The explicit rule governing numeric comparison, representability, conversion, overflow, and failure for an operation.
- **Deprecated Synonyms:** numeric semantics, numeric contract
- **Related:** DiscreteNumericType, DeterministicOverflowPolicy, ExactConversion, LossyConversion
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** A conversion's numeric policy states whether narrowing is exact, checked, or lossy.


### FullState
- **Definition:** The bounded-capacity condition in which a collection cannot accept another element under the applicable insertion or update operation.
- **Deprecated Synonyms:** full collection state, capacity-full state
- **Related:** Capacity, PreflightPredicate, DeterministicBehavior
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `full` or an equivalent capacity inspection reports whether a bounded insertion can complete.


### Contains
- **Definition:** The canonical boolean free function modeled on Clojure's `contains?`; it tests whether its argument belongs to the applicable lookup or indexed domain without performing a default-returning access. For maps it tests key presence, for sets it tests element presence, for indexed collections (vector/string) it tests index-in-range, and for `Range` it tests whether an index is within the available bounded prefix.
- **Deprecated Synonyms:** `contains?`, contains predicate, key-presence check
- **Related:** FreeFunction, Indexed, Lookup, PreflightPredicate, VerbPredicate, IndexedProducer, Range
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `contains(m, key)` tests a map key, `contains(s, value)` tests set membership, `contains(xs, index)` tests whether an indexed collection position is valid, and `contains(r, index)` tests whether a Range position is available.


### FitsInto
- **Definition:** The canonical materialization-completeness preflight predicate; its C++ free-function spelling is `fits_into`. It returns a non-throwing, non-allocating boolean indicating whether the complete producer or explicitly supported external source result fits within the destination under the same cardinality, matching, filtering, transformation, capacity, representability, and overflow semantics as `into`.
- **Deprecated Synonyms:** `fits_into`, materialization-fit check, capacity-fit predicate
- **Related:** FreeFunction, ProducerMaterialization, CompleteResult, BoundedPrefixResult, PreflightPredicate, CapabilityPredicate
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `fits_into(destination, producer)` or `fits_into(destination, source)` is checked before `into(destination, producer)` or `into(destination, source)` when complete materialization matters.


### BoundedResult
- **Definition:** An owning result whose capacity and maximum represented cardinality are finite under the operation's documented constraints.
- **Deprecated Synonyms:** bounded output, finite result
- **Related:** CompleteResult, BoundedPrefixResult, BoundedStorage
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** A materialized producer result is a bounded result even when the producer itself is unbounded.


### PartialResult
- **Definition:** A result that does not contain the operation's complete result because a documented capacity or result policy limits what can be represented; the canonical cljonic term is `BoundedPrefixResult`. Direct source construction may intentionally produce this bounded result, while complete source materialization uses `fits_into` and `into`.
- **Deprecated Synonyms:** partial output, truncated result
- **Related:** BoundedPrefixResult, CompleteResult, FitsInto
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** An `into` operation may return a partial result when `fits_into` is false.


### ExactConversion
- **Definition:** A conversion that preserves the source value exactly in the destination representation and satisfies the applicable representability policy.
- **Deprecated Synonyms:** exact numeric conversion, lossless conversion
- **Related:** CheckedConversion, LossyConversion, NumericPolicy
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** A conversion is exact only when the destination can represent the source without information loss.


### CheckedConversion
- **Definition:** A conversion that verifies representability or validity and communicates failure through the documented checked-failure or preflight mechanism without throwing or allocating.
- **Deprecated Synonyms:** checked numeric conversion, validated conversion
- **Related:** ExactConversion, LossyConversion, CheckedFailureResult, PreflightPredicate
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** A checked conversion reports failure when a runtime value cannot be represented in the destination type.


### LossyConversion
- **Definition:** A conversion that may discard precision, range, or other source information under an explicitly documented numeric policy.
- **Deprecated Synonyms:** lossy numeric conversion, narrowing conversion
- **Related:** ExactConversion, CheckedConversion, NumericPolicy
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** A lossy conversion is permitted only when the governing operation explicitly documents that policy.


### Parsing
- **Definition:** The bounded interpretation of an input representation into a cljonic value with explicit validity, representability, and failure semantics.
- **Deprecated Synonyms:** value parsing, text parsing
- **Related:** CheckedConversion, CheckedFailureResult, DeterministicBehavior
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** Parsing invalid or unrepresentable input returns the documented non-throwing failure outcome.


### FiniteObservation
- **Definition:** Observation of a producer or value over a finite, explicitly bounded domain, without implying that an unbounded source has a finite complete result.
- **Deprecated Synonyms:** bounded observation, finite producer observation
- **Related:** BoundedInspection, FiniteStatus, ProducerIteration, UnboundedProducer
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** Inspecting the first bounded prefix of an unbounded producer is finite observation.


### FiniteStatus
- **Definition:** The non-throwing `is_finite()` predicate result that distinguishes a producer with a finite complete result from an UnboundedProducer. FiniteStatus governs complete-result preflight; it is distinct from the bounded observable traversal returned by `count()`.
- **Deprecated Synonyms:** boundedness flag, completion status
- **Related:** UnboundedProducer, PreflightPredicate, ProducerIteration, ProducerMaterialization
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `range(0, 5).is_finite()` is true, while a zero-step Range and `repeat(value)` report false even though both have capped traversal.


### FiniteDeepEquality
- **Definition:** Equality comparison that observes and compares only the documented finite domain of two values or producers, rather than requiring an unbounded source to terminate.
- **Deprecated Synonyms:** bounded deep equality, finite sequence equality
- **Related:** FiniteObservation, GeneralEquality, StableEquality
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** A finite deep-equality operation must document the observed bound when either input is an unbounded producer.


### BoundedInspection
- **Definition:** Read-only examination limited by a finite explicit or configured bound, preserving no-heap, no-exception, and deterministic behavior constraints.
- **Deprecated Synonyms:** bounded inspection, limited inspection
- **Related:** FiniteObservation, Traversal, NoHeapConstraint
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** A bounded inspection may examine producer elements up to a destination capacity or configured maximum.


### Producer
- **Definition:** An explicit, self-contained value representing a sequence or materialization source without owning the storage of its eventual materialized result. A Producer owns its parameters and MUST NOT borrow source storage, retain hidden mutable state, or depend on a source lifetime. Every Module 4 Producer exposes non-throwing `count()`, `is_finite()`, and const `begin()`/`end()` traversal. Finite forms return their exact runtime count; unbounded forms return the configured observable traversal cap and report false from `is_finite()`. `Range` is efficiently `Indexed`; `Repeat`, `Cycle`, `Iterate`, and `Repeatedly` are not `Indexed`. Cycle has only the public form `cycle(source)` and is always unbounded. Producers MUST NOT claim `Indexed` unless their positional access is genuinely O(1). Being `Indexed` does not imply invocability (`IFn`); no producer is invocable.
- **Deprecated Synonyms:** sequence producer, source producer
- **Related:** Sequence, ProducerOnlyResult, UnboundedProducer, ProducerIteration, ProducerMaterialization, OwningValue, Indexed, Iterate
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `range`, `repeat`, `cycle`, `iterate`, and `repeatedly` are producer families when their requirements are active.


### ProducerOnlyResult
- **Definition:** A public operation result whose value is a self-contained producer rather than an owning materialized collection. A ProducerOnlyResult is a result-status category, not a synonym for the Producer value/domain concept. A ProducerOnlyResult owns producer parameters, does not own materialized result storage, and does not borrow source storage or hidden mutable state.
- **Deprecated Synonyms:** producer-only result, unmaterialized result
- **Related:** Producer, UnboundedProducer, OwningValue, ProducerMaterialization
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** An unbounded `range` operation may return a ProducerOnlyResult that must be materialized into an explicit bounded destination.


### Range
- **Definition:** A producer describing an arithmetic sequence from an inclusive start to an exclusive end by a fixed step, defaulting to start `0` and step `1`. A zero step produces an infinite repetition of `start`, taking precedence over otherwise-empty-range cases including equal start and end; a nonzero step that moves away from the end produces an empty finite range. Finite Range forms return their exact count and `true` from `is_finite()`; zero-step forms return the configured traversal cap and `false`. `Range` exposes no canonical `start`/`end`/`step` observation; instead it is `Indexed`: `contains(r, i)` tests whether an index is within its available bounded prefix in O(1). Unlike `Vector`/`Map`/`Set`, `Range` is not invocable (not `IFn`); it exposes no callable operator, and positional value retrieval remains deferred future work. `Range` is never `Associative`/`Lookup`; `get`/key-based lookup are excluded.
- **Deprecated Synonyms:** range producer, arithmetic range
- **Related:** Producer, UnboundedProducer, CollectionMaximumElementCount, Contains, Indexed, CljonicRange
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `range(0, 5)` produces `0, 1, 2, 3, 4`; `range(0, 0, 0)` produces an infinite repetition of `0` rather than an empty range; when a range is larger than the system maximum, only the available bounded prefix is exposed.


### Repeat
- **Definition:** A Producer that owns one value and yields copies of that value. `repeat(value)` is unbounded; `repeat(value, count)` is finite and yields exactly `count` copies, including an empty result for zero. Finite Repeat returns its exact count and `true` from `is_finite()`; unbounded Repeat returns the configured traversal cap and `false`. `Repeat` is not `Indexed`, is not invocable (`IFn`), and exposes no `contains`, positional retrieval, key-based lookup, or `get`. Its finite result count governs complete append preflight; its unbounded form has no complete-result fit.
- **Deprecated Synonyms:** repeat producer, repeated-value producer
- **Related:** Producer, UnboundedProducer, ProducerMaterialization, PreflightPredicate, OwningValue
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `repeat(7, 3)` produces `7, 7, 7`; `repeat(7, 0)` produces no values; `repeat(7)` produces destination-bounded copies only through `into`.


### Cycle
- **Definition:** An unbounded Producer created only by `cycle(source)` that owns the source value or an independent copy of its source parameters. For a finite source, Cycle repeats the source sequence in logical traversal order from its beginning after each complete pass. For an unbounded source, Cycle preserves the source's observable traversal without requiring the source to reach an end, so bounded observation of `cycle(source)` produces the same prefix as bounded observation of `source`. Cycle returns the configured observable traversal cap and `false` from `is_finite()`; its complete result never fits a destination, while `into` may materialize a bounded prefix. An empty finite source is valid and produces empty observation.
- **Deprecated Synonyms:** cycle producer, repeating sequence producer
- **Related:** Producer, UnboundedProducer, ProducerMaterialization, PreflightPredicate, OwningValue, SequenceableProducer, CljonicCycle
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `cycle(Vector<int, 3>{1, 2, 3})` produces the repeating sequence `1, 2, 3, 1, 2, 3, ...`; `cycle(range())` has the same bounded observable prefix as `range()`; materializing the finite-source case into `Vector<int, 5>` yields `1, 2, 3, 1, 2`.


### MapEntry
- **Definition:** The bounded owning key-value pair representation used when a map operation exposes one map association as a value. Its key and value are named fields; typed `key` and `val` free functions are the planned user-facing accessors. Any future fixed two-element sequence behavior remains deferred until an indexed result representation is approved.
- **Deprecated Synonyms:** map entry, key-value entry
- **Related:** Map, Lookup, Seqable, ConstRangeTraversal, LogicalTraversalOrder, Traversal, OwningValue
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** A map-entry result owns its key and value rather than borrowing hidden map storage.


### GeneralEquality
- **Definition:** Equality over supported values using the applicable semantic equality capability, distinct from numeric equality and from storage identity.
- **Deprecated Synonyms:** structural equality, semantic equality
- **Related:** StableEquality, NumericEquality, FiniteDeepEquality
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** General equality compares supported collection content according to its documented logical semantics.


### NumericEquality
- **Definition:** Equality governed by the operation's numeric policy for supported numeric values, including representability and any permitted cross-type comparison rules.
- **Deprecated Synonyms:** numeric value equality, numeric comparison
- **Related:** GeneralEquality, StableEquality, NumericPolicy
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** Numeric equality must state whether values of different numeric types compare directly or require exact conversion.


### SemanticPredicateName
- **Definition:** A predicate name chosen to communicate the semantic question it answers, rather than exposing an access operation or an ambiguous truthy convention.
- **Deprecated Synonyms:** predicate naming policy, semantic predicate naming
- **Related:** CapabilityPredicate, StatePredicate, VerbPredicate, IsPredicatePrefix, CanPredicatePrefix, HasPredicatePrefix, ValidPredicatePrefix
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `is_empty`, `contains`, and `fits_into` use names that identify their semantic questions.


### CapabilityPredicate
- **Definition:** A non-throwing, non-allocating predicate that reports whether an operation's required capability or completion condition holds for its inputs.
- **Deprecated Synonyms:** capability check, operation precondition predicate
- **Related:** PreflightPredicate, SemanticPredicateName, FullState
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `can_add`, `can_assoc`, and `fits_into` are capability predicates when their contracts govern corresponding operations.


### IsPredicatePrefix
- **Definition:** The canonical `is_` prefix for a state or adjectival predicate derived from a Clojure question-mark function.
- **Deprecated Synonyms:** `is_` predicate, state predicate prefix
- **Related:** StatePredicate, SemanticPredicateName
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `empty?` maps to `is_empty`, and `zero?` maps to `is_zero`.


### CanPredicatePrefix
- **Definition:** The canonical `can_` prefix for a capability or feasibility predicate that asks whether an operation can complete under its documented conditions.
- **Deprecated Synonyms:** `can_` predicate, capability predicate prefix
- **Related:** CapabilityPredicate, PreflightPredicate
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `can_add` and `can_assoc` express operation-specific completion capability.


### HasPredicatePrefix
- **Definition:** The canonical `has_` prefix for a presence or possession predicate, including key or member presence where that domain applies.
- **Deprecated Synonyms:** `has_` predicate, presence predicate prefix
- **Related:** CapabilityPredicate, Associative
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** A `has_` predicate may report key presence without performing a default-returning lookup.


### ValidPredicatePrefix
- **Definition:** The canonical `valid_` prefix for a predicate that determines whether an input belongs to an operation's valid domain before access or conversion.
- **Deprecated Synonyms:** `valid_` predicate, validity predicate prefix
- **Related:** CapabilityPredicate, PreflightPredicate
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `contains(xs, i)` determines whether an index can produce a valid indexed access.


### KeywordEnumNameEntry
- **Definition:** A bounded mapping entry associating a supported keyword name with an application-defined scoped enumeration value.
- **Deprecated Synonyms:** keyword enum entry, enum-name entry
- **Related:** KeywordEnumNameMap, MapEntry, OwningValue
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** An entry can associate a keyword such as `:pending` with a scoped enumeration value.


### KeywordEnumNameMap
- **Definition:** A bounded map of keyword names to application-defined scoped enumeration values under the supported associative and capacity rules.
- **Deprecated Synonyms:** keyword-to-enum map, enum name map
- **Related:** KeywordEnumNameEntry, KeywordEnumNameContext, Map, Lookup
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** A keyword enum name map provides bounded lookup and documented missing-key behavior.


### KeywordEnumNameContext
- **Definition:** The explicit context required to interpret, validate, or map keyword names to scoped enumeration values without hidden global state.
- **Deprecated Synonyms:** enum-name mapping context, keyword mapping context
- **Related:** KeywordEnumNameEntry, KeywordEnumNameMap, KeywordEnumNameMapping, NoHiddenGlobalInitialization
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** A caller supplies the mapping context rather than relying on a library-managed global registry.


### KeywordEnumNameMapping
- **Definition:** The bounded, explicit operation that maps supported keyword names and scoped enumeration values according to a `KeywordEnumNameContext`.
- **Deprecated Synonyms:** keyword enum mapping, enum-name mapping
- **Related:** KeywordEnumNameEntry, KeywordEnumNameMap, KeywordEnumNameContext, Parsing
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** Mapping reports invalid, missing, or duplicate names through its documented non-throwing policy.

### CopyableElement
- **Definition:** A user-defined type admissible to a cljonic storage boundary when it is default-initializable and copyable, without requiring the copy operations to be non-throwing. This is the broader storage-admission gate that underpins the stricter non-throwing form used by bounded collections.
- **Deprecated Synonyms:** copyable collection element, default-initializable copyable type
- **Related:** NothrowCollectionElement, CopyOnModifyCollection, NoExceptionConstraint
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `CopyableElement<int>` is satisfied, and a user-defined type with a throwing copy constructor does not satisfy the stricter `NothrowCollectionElement` rule even if it remains copyable.


### NothrowCollectionElement
- **Definition:** A user-defined type admissible for storage in a cljonic collection because its default construction, copy construction, copy assignment, and destruction are all non-throwing. This storage-admission capability is independent of equality, ordering, hashing, parsing, traversal, and other operation-specific capabilities.
- **Deprecated Synonyms:** nothrow collection element, non-throwing collection element
- **Related:** CopyOnModifyCollection, NoExceptionConstraint, StaticInspectableStorage, AggregateLikeStruct, CopyableElement, NothrowStableEqualityComparable
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** A `Vector<T, N>`, `Set<T, N>`, or `Queue<T, N>` element, and a `Map<K, V, N>` key or value, must satisfy `NothrowCollectionElement` before storage admission.


### CopyOnModifyCollection
- **Definition:** A fixed-capacity value type backed by statically inspectable storage that returns a modified copy instead of mutating in place. Its observable contract is PersistentValueSemantics for a PersistentValue; Module 1 realizes updates through DeepCopyUpdate without structural sharing. In this repo, Vector, Map, Set, Queue, and String follow this model.
- **Deprecated Synonyms:** Copy-on-Modify Collection, bounded immutable collection, fixed-capacity collection, array-backed collection, deep copy on write, deep copying on write
- **Related:** PersistentValueSemantics, DeepCopyUpdate, StaticInspectableStorage, Vector, Map, Set, Queue, String, SentinelBasedAccess
- **Usage:** Specification and implementation
- **Examples:** `auto xs = Vector<int, 4>{1, 2, 3}; auto ys = conj(xs, 4);`


### SentinelBasedAccess
- **Definition:** The access model where missing or invalid lookup returns a default sentinel value such as `T{}` or `char{}` instead of raising an error or returning an explicit status object.
- **Deprecated Synonyms:** Sentinel-Based Access, sentinel access, sentinel return, default-value access
- **Related:** CopyOnModifyCollection, DefaultElement, ProbeFirstAccess
- **Usage:** Specification and implementation
- **Examples:** `auto value = get(m, key);` and callers use `contains(m, key)` or `contains(xs, i)` before relying on the result.


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


### Vector
- **Definition:** The cljonic fixed-capacity sequential collection type for ordered element storage with immutable copy-on-modify updates.
- **Deprecated Synonyms:** vector collection, bounded vector, fixed-capacity vector
- **Related:** CopyOnModifyCollection, String, CapacityConstruction, Indexed, LogicalTraversalOrder, ConstRangeTraversal, ReadOnlyInteropAccessor
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** `Vector<int, 4>{1, 2}` constructs a fixed-capacity value whose `count()`, `capacity()`, `operator()`, and `contains` provide direct member and free-function observation.


### Map
- **Definition:** The cljonic fixed-capacity associative collection type mapping unique, stably comparable keys to values using flat bounded array-backed storage and bounded linear scans with immutable copy-on-modify updates. Both keys and values satisfy the `NothrowCollectionElement` storage contract at template admission.
- **Deprecated Synonyms:** bounded map, fixed-capacity map, associative map
- **Related:** MapEntry, Associative, Lookup, Contains, LinearScan, SwapAndRemove, CopyOnModifyCollection, LogicalTraversalOrder, ConstRangeTraversal, ReadOnlyInteropAccessor
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** `Map<int, String<16>, 4>{}` creates a bounded associative collection supporting `assoc`, `dissoc`, `contains`, `get`, and callable lookup `m(k)`; a missing key returns `String<16>{}` or a supplied fallback. `Map{MapEntry{1, 10}, MapEntry{2, 20}}` deduces `Map<int, int, 2>` and folds `assoc` over each entry in argument order; every argument must be exactly `MapEntry<int, int>`, and at least one argument is required (use `Map<int, int, N>{}` for the empty case).


### Set
- **Definition:** The cljonic fixed-capacity unordered collection type storing unique elements using flat bounded array-backed storage and bounded linear scans with immutable copy-on-modify updates.
- **Deprecated Synonyms:** bounded set, fixed-capacity set, unique element collection
- **Related:** StableEquality, Contains, LinearScan, SwapAndRemove, CopyOnModifyCollection, LogicalTraversalOrder, ConstRangeTraversal, ReadOnlyInteropAccessor
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** `Set<int, 4>{1, 2, 3}` creates a bounded set supporting `conj`, `disj`, `contains`, `get`, and callable lookup `s(v)`.


### Queue
- **Definition:** The cljonic fixed-capacity FIFO sequential collection type supporting insertion at the rear, removal at the front, peek/pop observation, and const logical traversal with immutable copy-on-modify updates. Queue interoperability is provided by `begin()`/`end()` in FIFO order; Queue does not provide a contiguous `view` because circular storage may wrap.
- **Deprecated Synonyms:** bounded queue, fixed-capacity queue, FIFO queue
- **Related:** Sequence, CopyOnModifyCollection, Traversal, ConstRangeTraversal, LogicalTraversalOrder, ContiguousStorage, ReadOnlyInteropAccessor
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** `Queue<int, 4>{}` creates a bounded FIFO queue supporting `conj` (enqueue at rear), `peek` (front observation), and `pop` (removal from front); `can_conj()` reports whether another value fits, and `conj()` on a full queue returns an unchanged copy. `Queue{10, 20, 30}` deduces `Queue<int, 3>` and folds `conj` over each argument in argument order, establishing FIFO order matching argument order. `begin()`/`end()` preserve that order after physical storage wraps.


### String
- **Definition:** The cljonic fixed-capacity array-backed collection type with ordered ASCII byte storage (range `0x01`–`0x7F`) and an uncounted null terminator immediately following its content.
- **Deprecated Synonyms:** bounded string, fixed-capacity string, cljonic string
- **Related:** CopyOnModifyCollection, Capacity, Sequence, BoundedStorage, Indexed, CallableLookup, LogicalTraversalOrder, ConstRangeTraversal, ReadOnlyInteropAccessor
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** `String<32>{"hello"}` or capacity-inferred `String{"hello"}` stores valid ASCII bytes with a terminating null outside the counted content length. A fixed-extent `std::span<const char, N>` can use `String{span}` to deduce `String<N>`, while a dynamic-extent span uses an explicit destination capacity and bounded-prefix construction. `s(2)` and `get(s, 2)` return the byte at content index two; invalid indices return `char{}` or a supplied fallback, while `contains(s, index)` distinguishes valid content indices from the terminator and out-of-range indices.


### SwapAndRemove
- **Definition:** An $O(1)$ removal strategy for unsorted collections (`Map` and `Set`) where the target element or key-value entry is overwritten by the last active stored element before decrementing the count.
- **Deprecated Synonyms:** swap and remove, swap-with-back, unordered removal
- **Related:** Map, Set, ContiguousStorage, Dissoc, Disj
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** Calling `disj(set, val)` or `dissoc(map, key)` copies the final element into the vacated slot and decrements the collection count without preserving internal traversal order.


### ContiguousStorage
- **Definition:** Flat bounded array-backed internal storage (`std::array`-like) holding collection elements or key-value entries without indirection or node allocation. ContiguousStorage describes the physical backing storage; it does not by itself guarantee that a collection's complete active logical range is physically contiguous or representable by one standard view.
- **Deprecated Synonyms:** flat storage, contiguous array storage, inline array buffer
- **Related:** BoundedStorage, Capacity, StaticInspectableStorage, ContiguousConstView
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `std::array<MapEntry<K, V>, N>` and `std::array<T, N>` provide flat bounded backing storage; a circular queue may store its active logical range across two physical segments.


### CallableLookup
- **Definition:** Invocation of a collection instance via `operator()` providing concise read-only lookup with an optional fallback argument that defaults to the collection's default lookup result, behaviorally equivalent to `get`. CallableLookup mirrors Clojure's `IFn` invocability for `Vector`, `Map`, `Set`, and `String`; it MUST NOT be extended to a value that is not invocable in Clojure. A producer being `Indexed` does not by itself grant CallableLookup: `Range` is `Indexed` but not `IFn`, so it exposes no callable `operator()`.
- **Deprecated Synonyms:** callable collection, functional lookup syntax, operator() lookup, IFn
- **Related:** SentinelBasedAccess, DefaultElement, Indexed, Lookup
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `xs(2)` on a Vector, `m(key)` on a Map, `s(val)` on a Set, and `text(2)` on a String invoke callable lookup without mutating the collection; each uses the default fallback and is behaviorally equivalent to `get` without an explicit fallback. `Range<int>{0, 5}` has no such callable form.


### LinearScan
- **Definition:** A bounded $O(N)$ sequential search over the active stored elements or key-value entries of a collection whose lookup or membership contract requires linear search, currently `Map` and `Set`. LinearScan does not require every collection's logical traversal range to be physically contiguous and does not describe the general `ConstRangeTraversal` mechanism.
- **Deprecated Synonyms:** linear search, sequential scan, bounded scan
- **Related:** Map, Set, ContiguousStorage, Contains, StableEquality, LogicalTraversalOrder
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `contains(m, key)` performs a linear scan over active map entries matching key equality.


### Conj
- **Definition:** The primitive collection free function that returns a new collection with one or more elements added according to the target collection's type conventions (rear for `Vector` and `Queue`, membership insertion for `Set`).
- **Deprecated Synonyms:** conjoin, insert element
- **Related:** CopyOnModifyCollection, CanConj, PreflightPredicate, Vector, Set, Queue
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `conj(xs, 4)` appends 4 to a Vector; `conj(s, 4)` adds 4 to a Set if not already present.


### Assoc
- **Definition:** The primitive `Associative` free function that returns a new collection with the supplied key-value association added, replaced, or appended according to the collection's key-domain and capacity policy. Existing map keys, vector indexes, and string content indexes are replaced without increasing count; a vector or string key equal to logical count appends when capacity remains. Invalid keys and full-capacity append keys return an unchanged copy without throwing, allocating, or mutating the source. String character validity follows the documented replacement policy at runtime and constant-evaluation rejection policy.
- **Deprecated Synonyms:** associate, map assoc
- **Related:** Associative, Lookup, Map, Vector, String, MapEntry, CanAssoc, Dissoc, CopyOnModifyCollection
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `assoc(m, key, val)` updates an existing value or inserts a new pair when capacity remains; `assoc(v, i, value)` replaces an existing index or appends at `count(v)`; `assoc(s, i, ch)` applies the same index policy while preserving the null terminator.


### Dissoc
- **Definition:** The primitive associative free function that returns a new `Map` with the specified key and its associated value removed via swap-and-remove.
- **Deprecated Synonyms:** disassociate, map remove, erase key
- **Related:** Map, Assoc, SwapAndRemove, CopyOnModifyCollection
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `dissoc(m, key)` removes `key` if present and decrements count, returning an unchanged copy if absent.


### Disj
- **Definition:** The primitive set free function that returns a new `Set` with the specified element removed via swap-and-remove.
- **Deprecated Synonyms:** disjoin, set remove, erase element
- **Related:** Set, Conj, SwapAndRemove, CopyOnModifyCollection
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `disj(s, val)` removes `val` from the set if present and decrements count.


### Peek
- **Definition:** The primitive sequential free function that observes the first or accessible element without removal (`Vector` top/last, `Queue` front) returning `DefaultElement` if empty.
- **Deprecated Synonyms:** peek front, peek top
- **Related:** Pop, Queue, Vector, SentinelBasedAccess
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `peek(q)` observes the head of the FIFO queue; `peek(v)` observes the last element of the vector.


### Pop
- **Definition:** The primitive sequential free function that returns a new collection with the accessible element removed (`Vector` last element, `Queue` front element).
- **Deprecated Synonyms:** pop front, pop back, dequeue
- **Related:** Peek, Queue, Vector, CopyOnModifyCollection
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `pop(q)` dequeues the front element; `pop(v)` removes the trailing element.


### Seq
- **Definition:** The primitive traversal free function that converts a supported `Seqable` collection, explicitly approved sequenceable value such as `MapEntry`, or approved `Producer` into an owning, value-semantic bounded `Vector` of its logical traversal elements, using the applicable `ConstRangeTraversal`, `ProducerIteration`, element representation, and capacity policy. Seq does not imply support for arbitrary external ranges, iterables, arrays, or containers.
- **Deprecated Synonyms:** sequence conversion, to-seq, seq conversion
- **Related:** Seqable, ConstRangeTraversal, ProducerIteration, BoundedResult, Vector, MapEntry, Traversal
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `seq(m)` returns an owning `Vector<MapEntry<K, V>, N>` of map entries.


### CanConj
- **Definition:** The canonical preflight capability predicate (`can_conj`) checking whether `conj` can succeed without capacity overflow, returning true if capacity is available or if the item is an existing set element.
- **Deprecated Synonyms:** can_conj, can-conj predicate
- **Related:** Conj, CapabilityPredicate, PreflightPredicate, FullState, Set
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `can_conj(s, x)` returns true if `s` is not full or if `s` already contains `x`.


### CanAssoc
- **Definition:** The canonical `Associative` preflight predicate (`can_assoc`) checking whether `assoc` can produce its documented result without key-domain or capacity failure. It depends only on the collection and key, not the value. For `Map`, existing keys are valid and new keys require spare capacity; for `Vector` and `String`, existing indexes are valid and the logical-count append index is valid only when capacity remains. Invalid and full-capacity append keys return false.
- **Deprecated Synonyms:** can_assoc, can-assoc predicate
- **Related:** Assoc, Associative, CapabilityPredicate, PreflightPredicate, FullState, Map, Vector, String
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `can_assoc(m, k)` is true for an existing key or an insertable new key; `can_assoc(v, count(v))` and `can_assoc(s, count(s))` are true only when capacity remains.


### Iterate
- **Definition:** A `Producer` and `CljonicSource` whose element type satisfies `NothrowCollectionElement`, owning an initial value and a pure, non-allocating, copy-constructible step callback. `iterate(step, initial)` produces `initial` first and then applies `step` to the previously produced value for each subsequent element. Named functions, lambdas, and function objects are accepted; the factory stores the decayed callable value, including a copyable function pointer for a named function. The step may be moved from an rvalue during construction when supported, but the resulting producer remains copyable. The callback is not evaluated during construction. `Iterate` uses `CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT` as its observable traversal cap rather than claiming a finite complete result, may be consumed directly by source-taking free functions, and converts to an owning destination through `into`. Its construction and traversal are constexpr-capable when the supplied arguments are and remain usable at runtime. It is not `Indexed`, is not invocable (`IFn`), and exposes no `contains`, positional retrieval, key-based lookup, or `get`.
- **Deprecated Synonyms:** iterated sequence
- **Related:** Producer, CljonicSource, UnboundedProducer, ProducerMaterialization, PreflightPredicate, CollectionMaximumElementCount, OwningValue
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** `iterate([](int value) { return value + 1; }, 0)` produces the unbounded sequence `0, 1, 2, 3, ...`, observed only through its configured traversal cap or an explicit destination.


### Repeatedly
- **Definition:** A `Producer` and `CljonicSource` whose element type satisfies `NothrowCollectionElement`, owning a non-allocating, copy-constructible zero-argument step callback that produces a fresh element for every produced value; the step MAY maintain state or produce side effects, mirroring Clojure's `repeatedly`, but MUST NOT throw or allocate when invoked. `repeatedly(step)` is unbounded; `repeatedly(count, step)` produces exactly `count` elements, including an empty result when the count is zero, with the count placed before the step as in Clojure's `(repeatedly n f)` form. Named functions, lambdas, and function objects are accepted; the factory stores the decayed callable value, including a copyable function pointer for a named function. The step may be moved from an rvalue during construction when supported, but the resulting producer remains copyable. The callback is not evaluated during construction and is invoked exactly once per produced element during each materialization. The uncounted form uses `CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT` as its observable traversal cap rather than claiming a finite complete result, may be consumed directly by source-taking free functions, and converts to an owning destination through `into`; the counted form uses its runtime count for materialization. Construction and traversal are constexpr-capable when the supplied arguments are and remain usable at runtime. It is not `Indexed`, is not invocable (`IFn`), and exposes no `contains`, positional retrieval, key-based lookup, or `get`.
- **Deprecated Synonyms:** repeatedly sequence
- **Related:** Producer, CljonicSource, UnboundedProducer, ProducerMaterialization, PreflightPredicate, CollectionMaximumElementCount, OwningValue, Repeat
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** `repeatedly(3U, []() { return 7; })` produces the finite sequence `7, 7, 7`; the uncounted `repeatedly([]() { return 7; })` is unbounded, observed only through its configured traversal cap or an explicit destination.


### DeterministicOverflowPolicy
- **Definition:** The explicit, profile-defined rule for numeric overflow behavior, chosen so that overflow outcomes are stable, documented, and never hidden behind undefined or silently heap-backed behavior.
- **Deprecated Synonyms:** overflow policy, deterministic overflow behavior
- **Related:** DeterministicBehavior
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** A profile may choose compile-time rejection for provable overflow cases, or a clearly documented runtime overflow rule such as wraparound or unchanged-result semantics where appropriate.

## Result and Lifecycle Vocabulary

### OwningValue
- **Definition:** A self-contained cljonic value whose validity does not depend on another value, temporary, external storage, hidden borrowed state, or hidden result cache.
- **Deprecated Synonyms:** owning value, self-contained value
- **Related:** NonOwningView, CopyOnModifyCollection, PersistentValueSemantics
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** Collection values, string values, regex values, map-entry values, and producer parameters are owning values.


### NonOwningView
- **Definition:** A read-only observation of an existing cljonic value that does not own storage, does not extend source lifetime, and does not allow source mutation. A NonOwningView is not an owning cljonic result and MUST NOT be returned by a public semantic cljonic free function; it MAY be returned only by an explicitly identified collection-owned C++ interoperability accessor or equivalent const logical-range interoperability mechanism governed by the interoperability requirements, or used internally without escaping the operation.
- **Deprecated Synonyms:** non-owning view, borrowed view
- **Related:** OwningValue, ReadOnlyInteropAccessor, ContiguousConstView, String
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** A collection-owned interoperability accessor returns a non-owning view whose validity is limited by the source lifetime and invalidation rules.


### CompleteResult
- **Definition:** The full result defined by an operation's contract.
- **Deprecated Synonyms:** full result
- **Related:** BoundedPrefixResult, PreflightPredicate
- **Usage:** Requirements, specification, tests, and documentation
- **Examples:** When `fits_into` is true, the corresponding materialization operation returns a complete result.


### BoundedPrefixResult
- **Definition:** A bounded result that intentionally contains only a deterministic prefix or reduced subset because the complete result could not fit under the documented capacity or result policy.
- **Deprecated Synonyms:** partial result, prefix result
- **Related:** CompleteResult, PartialResult, PreflightPredicate, EffectiveBoundedPrefixBoundary
- **Usage:** Requirements, specification, implementation, tests, and documentation
- **Examples:** An over-capacity `into` operation may return a bounded-prefix result when its preflight indicates non-fit.


### EffectiveBoundedPrefixBoundary
- **Definition:** The adjusted exclusive endpoint a producer normalizes to when its requested result is an oversized finite form, marking where its bounded-prefix result ends.
- **Deprecated Synonyms:** effective endpoint boundary, adjusted exclusive end
- **Related:** BoundedPrefixResult
- **Usage:** Architecture, specification, and documentation
- **Examples:** An oversized finite Range normalizes its effective exclusive end to the EffectiveBoundedPrefixBoundary after its bounded prefix.


### DefaultReturningResult
- **Definition:** A documented default value result used when the requested access or lookup cannot produce a valid value.
- **Deprecated Synonyms:** default result, sentinel-return result
- **Related:** DefaultElement, CheckedFailureResult, ProbeFirstAccess
- **Usage:** Requirements, specification, implementation, tests, and documentation
- **Examples:** `get` on a missing key returns a default-returning result according to its contract.


### CheckedFailureResult
- **Definition:** A documented non-throwing, non-allocating failure outcome indicating an operation could not complete successfully under the library's failure model.
- **Deprecated Synonyms:** checked failure, explicit failure result
- **Related:** DefaultReturningResult, DeterministicBehavior, NoExceptionConstraint
- **Usage:** Requirements, specification, implementation, tests, and documentation
- **Examples:** A checked numeric conversion may return a checked-failure result when representability requirements are not met.


### PreflightPredicate
- **Definition:** A non-throwing, non-allocating predicate that measures the same completion and failure conditions as its paired operation. `fits_into(destination, source)` tests whether the complete source can be appended to the destination's remaining capacity; it is false for an UnboundedProducer regardless of its capped observable count.
- **Deprecated Synonyms:** preflight check, capability precheck
- **Related:** CompleteResult, BoundedPrefixResult, ProbeFirstAccess
- **Usage:** Requirements, specification, implementation, tests, and documentation
- **Examples:** `fits_into`, `can_add`, and operation-specific `has_` checks are preflight predicates when they govern completion semantics.


### LifecycleClassification
- **Definition:** The explicit API-surface status assigned to each public function under review: candidate, deferred, excluded, or requirements-backed.
- **Deprecated Synonyms:** lifecycle status, API classification
- **Related:** RequirementsBacked
- **Usage:** Requirements, architecture, specification governance, and documentation
- **Examples:** A function can remain candidate during feasibility review and becomes requirements-backed only after governing requirements are approved.


### RequirementsBacked
- **Definition:** A lifecycle classification indicating a function's public scope, governing behavior, and resource constraints are approved by stable requirements.
- **Deprecated Synonyms:** requirements backed, approved-by-requirements
- **Related:** LifecycleClassification, CandidateStatus, DeferredStatus, ExcludedStatus
- **Usage:** Requirements, architecture, specification governance, and documentation
- **Examples:** A requirements-backed function may still need detailed overload and diagnostics design, but it is already part of supported scope.


### CandidateStatus
- **Definition:** A lifecycle classification indicating a function is feasible and under review, but not approved supported behavior.
- **Deprecated Synonyms:** candidate, in-review status
- **Related:** LifecycleClassification, RequirementsBacked, DeferredStatus, ExcludedStatus
- **Usage:** Requirements, architecture, specification governance, and documentation
- **Examples:** Vocabulary presence alone does not move a candidate function into supported behavior.


### DeferredStatus
- **Definition:** A lifecycle classification indicating a function is intentionally postponed because required scope or governing requirements are not yet complete.
- **Deprecated Synonyms:** deferred, postponed status
- **Related:** LifecycleClassification, CandidateStatus, RelationModel
- **Usage:** Requirements, architecture, specification governance, and documentation
- **Examples:** Relational operations can be deferred until a complete relation model is approved.


### ExcludedStatus
- **Definition:** A lifecycle classification indicating a function is outside cljonic scope or incompatible with the project boundary and semantic model.
- **Deprecated Synonyms:** excluded, out-of-scope status
- **Related:** LifecycleClassification, CandidateStatus
- **Usage:** Requirements, architecture, specification governance, and documentation
- **Examples:** Runtime macro or reflection features can be excluded by boundary requirements.


### StandardViewType
- **Definition:** A read-only non-owning view type aligned with standard C++ view conventions and cljonic lifetime rules, used only as an explicitly identified collection interoperability representation. StandardViewType is optional, does not imply that every collection has one contiguous view, and MUST NOT be returned by a public semantic cljonic free function.
- **Deprecated Synonyms:** standard view, standard span-style view
- **Related:** NonOwningView, ReadOnlyInteropAccessor, ContiguousConstView, String
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** A read-only interoperability accessor may expose a standard view type that cannot outlive its source; semantic operations still return owning cljonic results.


### UnboundedProducer
- **Definition:** An explicit producer value with no finite caller-independent complete-result bound, requiring bounded materialization semantics. It reports false from `is_finite()` and has ordinary const traversal capped by `CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT`; that count is an observable traversal bound, not a complete cardinality.
- **Deprecated Synonyms:** infinite producer, open-ended producer
- **Related:** ProducerMaterialization, PreflightPredicate
- **Usage:** Requirements, specification, implementation, tests, and documentation
- **Examples:** Unbound forms of `range`, `repeat`, `cycle`, `iterate`, and `repeatedly` are unbounded producers until bounded by destination materialization.


### ProducerMaterialization
- **Definition:** The explicit process of appending a producer's elements to a copy of a selected bounded destination under complete-result preflight rules. `fits_into` determines whether the complete append fits the destination's remaining capacity; `into` returns the deterministic prefix when it does not.
- **Deprecated Synonyms:** producer realization, producer into-materialization
- **Related:** UnboundedProducer, PreflightPredicate, ProducerIteration
- **Usage:** Requirements, specification, implementation, tests, and documentation
- **Examples:** `into(destination, producer)` plus `fits_into(destination, producer)` defines producer materialization completeness.


### ProducerIteration
- **Definition:** The ordinary const traversal of a producer's elements from `begin()` to `end()`, bounded by `count()`. For an UnboundedProducer, the configured count is the observable traversal cap rather than a complete result cardinality. ProducerIteration is distinct from ProducerMaterialization, which appends those elements into an owning bounded result.
- **Deprecated Synonyms:** producer iteration, producer traversal
- **Related:** ProducerMaterialization, UnboundedProducer
- **Usage:** Architecture, specification, and documentation
- **Examples:** Free-function observation such as `first`, `next`, and `rest` performs ProducerIteration without necessarily invoking ProducerMaterialization.


### RelationModel
- **Definition:** The explicit model that must define row representation, capabilities, duplicate semantics, nested-result representation, traversal order, capacity arithmetic, preflight, and bounded failure behavior before relational operations are supported.
- **Deprecated Synonyms:** relational model, map/set relation contract
- **Related:** LifecycleClassification, DeferredStatus
- **Usage:** Requirements, architecture, specification, tests, and documentation
- **Examples:** `index`, `project`, `rename`, and `join` remain deferred until a relation model is approved.


### StatePredicate
- **Definition:** A predicate whose canonical naming uses `is_` to express a state or adjectival condition.
- **Deprecated Synonyms:** `is_` predicate
- **Related:** VerbPredicate
- **Usage:** Requirements, specification, implementation, tests, and documentation
- **Examples:** `is_empty`, `is_zero`, and `is_subset` are state predicates.


### VerbPredicate
- **Definition:** A readable direct-verb predicate name used when the predicate naturally expresses an action-like test.
- **Deprecated Synonyms:** direct-verb predicate
- **Related:** StatePredicate
- **Usage:** Requirements, specification, implementation, tests, and documentation
- **Examples:** `contains`, `starts_with`, `ends_with`, and `includes` are verb predicates.


## Supporting Vocabulary


### EmbeddedConstraint
- **Definition:** The platform constraint set that assumes embedded targets with bounded resources and therefore prioritizes fixed capacity, predictable execution, and explicit profiles.
- **Deprecated Synonyms:** embedded constraint, embedded systems constraint
- **Related:** StaticInspectableStorage, NoHeapConstraint, NoExceptionConstraint, NoRttiConstraint, SingleThreadedExecutionModel, DeterministicBehavior
- **Usage:** Architecture, specification, implementation, and documentation
- **Examples:** Collection APIs use fixed-capacity storage and avoid runtime allocation on strict profiles.


### NoHeapConstraint
- **Definition:** The rule that no supported configuration or public API path may allocate from or deallocate to dynamic storage, directly or transitively. It applies to construction, update, lookup, traversal, transformation, failure handling, and destruction, including standard-library and dependency behavior invoked by cljonic.
- **Deprecated Synonyms:** no heap allocation, no-heap rule
- **Related:** EmbeddedConstraint, StaticInspectableStorage, DeterministicBehavior
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** Verification gates prohibit forbidden allocation APIs and bounded collections store data without dynamic allocation.


### NoExceptionConstraint
- **Definition:** The rule that library behavior is non-throwing and does not depend on exception-based control flow.
- **Deprecated Synonyms:** no exceptions, non-throwing rule
- **Related:** EmbeddedConstraint, SentinelBasedAccess, DeterministicBehavior
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** Missing access returns sentinel values instead of throwing, and contract failures route through explicit policy hooks.


### NoMutationConstraint
- **Definition:** The rule that cljonic semantic operations, read-only traversal mechanisms, and supported callbacks supplied to higher-order operations MUST NOT mutate input collection values or expose a mutable path to source storage. Copy-on-modify operations satisfy this constraint by returning independent owning results.
- **Deprecated Synonyms:** no input mutation, read-only mutation rule
- **Related:** ReferentialTransparency, PersistentValueSemantics, DeterministicBehavior, NoHeapConstraint, ConstRangeTraversal, ReadOnlyInteropAccessor
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** A traversal may inspect a collection through const access, while `assoc` returns a changed copy and leaves its input unchanged.


### CollectionMaximumElementCount
- **Definition:** The compile-time configuration boundary that sets the maximum element count allowed for cljonic collections. The current preprocessor macro `CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT` is one configuration mechanism that exposes this boundary.
- **Deprecated Synonyms:** collection max element count, maximum collection element count, CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT macro
- **Related:** CapacityConstruction, StaticInspectableStorage, EmbeddedConstraint, DeterministicBehavior
- **Usage:** Architecture, specification, implementation, build, and documentation
- **Examples:** A project can set the boundary by defining `CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT` before including cljonic headers or from compiler/build flags; a collection declaration above it is a CompileTimeFailure.


### DeterministicBehavior
- **Definition:** The requirement that equal explicit inputs, arguments, and fixed configuration produce equivalent results, failure modes, and profile-selected semantics.
- **Deprecated Synonyms:** deterministic semantics, deterministic execution
- **Related:** EmbeddedConstraint, NoHeapConstraint, NoExceptionConstraint, ReferentialTransparency
- **Usage:** Architecture, specification, tests, and documentation
- **Examples:** Full-capacity `conj` returns the unchanged value deterministically, and invalid runtime regex compilation returns a stable invalid-pattern sentinel.


### HeaderOnlyDistribution
- **Definition:** The packaging model in which the library is delivered as headers only, with development sources organized separately from the generated distribution artifact. The generated AmalgamatedHeader exposes the distributable library through one public include.
- **Deprecated Synonyms:** header-only, header-only library
- **Related:** AmalgamatedHeader
- **Usage:** Architecture, implementation, build, and documentation
- **Examples:** Production implementation units live as headers and are combined into a distributable single-header output.


### AmalgamatedHeader
- **Definition:** The generated single-header distribution artifact assembled from the per-function production headers that remain the source of truth during development.
- **Deprecated Synonyms:** amalgamated header, single-header distribution
- **Related:** HeaderOnlyDistribution
- **Usage:** Implementation, build, tests, and documentation
- **Examples:** Build automation regenerates the distributable header from the production header tree.


### CompileTimeFailure
- **Definition:** A result classification indicating an operation or construction is rejected during compilation rather than producing a runtime value, used when a violation is statically knowable ahead of program execution.
- **Deprecated Synonyms:** compile-time failure, compile-time rejection
- **Related:** CapacityConstruction, CheckedFailureResult
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** An oversized Vector initializer and an oversized finite Range constructed in a constexpr context are both CompileTimeFailure outcomes.


## Behavioral Specification Vocabulary


### CapacityConstruction
- **Definition:** The construction contract that supports both literal-deduced and explicit-capacity creation. Oversized explicit-capacity initializers and declared capacities above CollectionMaximumElementCount are CompileTimeFailure outcomes; the latter diagnostic identifies both the declared capacity and configured maximum.
- **Deprecated Synonyms:** collection construction pattern, explicit-capacity construction, literal-deduced construction
- **Related:** CollectionMaximumElementCount, CompileTimeFailure, CopyOnModifyCollection
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** Explicit-capacity empty construction is valid, but an initializer count that exceeds capacity is a compile-time failure.


### ClosedNominalCollectionDomain
- **Definition:** The fixed set of types eligible for cljonic collection recognition: Vector, Map, Set, Queue, and String. Standard containers, external containers, and third-party types cannot enter this domain through structural similarity.
- **Deprecated Synonyms:** closed collection domain, nominal collection domain
- **Related:** NominalCollectionRecognition, CollectionKind, Vector, Map, Set, Queue, String
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `std::vector<int>` is not a member of the ClosedNominalCollectionDomain even if it provides operations similar to Vector.


### NominalCollectionRecognition
- **Definition:** The two-stage recognition model in which a type must first be admitted by cljonic-owned traits before an operation evaluates structural capabilities.
- **Deprecated Synonyms:** nominal admission, collection identity gate
- **Related:** ClosedNominalCollectionDomain, CollectionKind
- **Usage:** Architecture, implementation, and tests
- **Examples:** `cljonic_collection<T>` depends on cljonic-owned trait admission rather than matching an external container structurally.


### CollectionKind
- **Definition:** The closed discriminant used by nominal traits and concepts to distinguish the supported collection families.
- **Deprecated Synonyms:** collection category, nominal collection kind
- **Related:** ClosedNominalCollectionDomain, NominalCollectionRecognition, ProducerKind
- **Usage:** Architecture, implementation, and tests
- **Examples:** A collection trait classifies an admitted type as vector, map, set, queue, or string.


### ProducerKind
- **Definition:** The closed discriminant used by nominal producer traits and concepts to distinguish supported producer families.
- **Deprecated Synonyms:** producer category, nominal producer kind
- **Related:** ProducerConcept, CljonicProducer, CljonicRange, Producer, CollectionKind
- **Usage:** Architecture, implementation, and tests
- **Examples:** A producer trait classifies an admitted type as range, repeat, cycle, or iterate in the current active slice, with repeatedly reserved for a future producer family.


### CollectionConcept
- **Definition:** A C++20 concept that gates a type on its cljonic nominal collection identity (admission to the ClosedNominalCollectionDomain), rather than on structural similarity to an external container.
- **Deprecated Synonyms:** nominal concept, public nominal concept
- **Related:** ClosedNominalCollectionDomain, NominalCollectionRecognition, CollectionKind, CapabilityConcept, CljonicCollection, ProducerConcept
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** `CljonicCollection<T>` is a CollectionConcept that depends on cljonic-owned trait admission.


### ProducerConcept
- **Definition:** A C++20 concept that gates a type on its cljonic nominal producer identity, parallel to CollectionConcept but admitted through the producer domain and ProducerKind classification rather than the ClosedNominalCollectionDomain.
- **Deprecated Synonyms:** producer concept, nominal producer concept
- **Related:** Producer, ProducerKind, CljonicProducer, CljonicRange, CljonicSource, CollectionConcept
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** `CljonicProducer<T>` is a ProducerConcept that depends on cljonic-owned producer trait admission.


### CljonicCollection
- **Definition:** The C++ concept identifier implementing the CollectionConcept for the full closed collection domain; it admits a type to the ClosedNominalCollectionDomain through cljonic-owned traits.
- **Deprecated Synonyms:** cljonic_collection, cljonic collection concept
- **Related:** CollectionConcept, ClosedNominalCollectionDomain, CollectionKind, CljonicVector, CljonicMap, CljonicSet, CljonicQueue, CljonicString
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** `template<class T> concept CljonicCollection = detail::is_cljonic_collection_v<T>;`


### CljonicVector
- **Definition:** The C++ concept identifier implementing the nominal CollectionConcept for the Vector collection family.
- **Deprecated Synonyms:** cljonic_vector, cljonic vector concept
- **Related:** CollectionConcept, CljonicCollection, Vector, CollectionKind
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** `CljonicVector<Vector<int, 4>>` is satisfied while an external container is not.


### CljonicMap
- **Definition:** The C++ concept identifier implementing the nominal CollectionConcept for the Map collection family.
- **Deprecated Synonyms:** cljonic_map, cljonic map concept
- **Related:** CollectionConcept, CljonicCollection, Map, CollectionKind
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** `CljonicMap<Map<int, int, 4>>` is satisfied while an external container is not.


### CljonicSet
- **Definition:** The C++ concept identifier implementing the nominal CollectionConcept for the Set collection family.
- **Deprecated Synonyms:** cljonic_set, cljonic set concept
- **Related:** CollectionConcept, CljonicCollection, Set, CollectionKind
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** `CljonicSet<Set<int, 4>>` is satisfied while an external container is not.


### CljonicQueue
- **Definition:** The C++ concept identifier implementing the nominal CollectionConcept for the Queue collection family.
- **Deprecated Synonyms:** cljonic_queue, cljonic queue concept
- **Related:** CollectionConcept, CljonicCollection, Queue, CollectionKind
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** `CljonicQueue<Queue<int, 4>>` is satisfied while an external container is not.


### CljonicString
- **Definition:** The C++ concept identifier implementing the nominal CollectionConcept for the String collection family.
- **Deprecated Synonyms:** cljonic_string, cljonic string concept
- **Related:** CollectionConcept, CljonicCollection, String, CollectionKind
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** `CljonicString<String<16>>` is satisfied while an external container is not.


### CljonicProducer
- **Definition:** The C++ concept identifier implementing the ProducerConcept nominal-admission pattern: admission to the producer nominal identity through cljonic-owned trait specialization, parallel to but distinct from `CljonicCollection`.
- **Deprecated Synonyms:** cljonic_producer, cljonic producer concept
- **Related:** ProducerConcept, ProducerKind, CollectionConcept, CljonicCollection, CljonicSource, Producer, CljonicRange, CljonicRepeat
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** `CljonicProducer<Range<int>>` is satisfied while a stored collection type is not.


### CljonicRange
- **Definition:** The C++ concept identifier implementing the nominal producer identity for the Range producer family.
- **Deprecated Synonyms:** cljonic_range, cljonic range concept
- **Related:** CljonicProducer, Range, ProducerKind, ProducerConcept
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** `CljonicRange<Range<int>>` is satisfied while `Vector<int, 4>` is not.


### CljonicRepeat
- **Definition:** The C++ concept identifier implementing the nominal producer identity for the Repeat producer family.
- **Deprecated Synonyms:** cljonic_repeat, cljonic repeat concept
- **Related:** CljonicProducer, Repeat, ProducerKind, ProducerConcept
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** `CljonicRepeat<Repeat<int>>` is satisfied while `Vector<int, 4>` is not.


### CljonicCycle
- **Definition:** The C++ concept identifier implementing the nominal producer identity for the Cycle producer family.
- **Deprecated Synonyms:** cljonic_cycle, cljonic cycle concept
- **Related:** CljonicProducer, Cycle, ProducerKind, ProducerConcept
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** `CljonicCycle<Cycle<Vector<int, 3>>>` is satisfied while `Vector<int, 3>` is not.


### CljonicSource
- **Definition:** The C++ concept identifier admitting a stored collection or producer to the combined source domain used by materialization and producer operations. CljonicSource requires nominal admission as `CljonicCollection ∨ CljonicProducer` and structural admission as `NothrowConstInputRange`; nominal admission or throwing traversal alone is insufficient.
- **Deprecated Synonyms:** cljonic_source, cljonic source concept
- **Related:** CljonicCollection, CljonicProducer, ConstInputRange, NothrowConstInputRange, Producer, ProducerMaterialization
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** `CljonicSource<Range<int>>` and `CljonicSource<Vector<int, 4>>` are both satisfied because they provide const input traversal; an external container is not nominally admitted even when it models `std::ranges::input_range`.


### ConstInputRange
- **Definition:** The named structural capability requiring a value to provide input-range traversal from a const source expression. ConstInputRange is the weaker traversal capability refined by `NothrowConstInputRange`; it does not by itself imply nominal cljonic collection or producer admission, ownership, no-heap behavior, or exact cardinality.
- **Deprecated Synonyms:** const input range, read-only input range
- **Related:** CljonicSource, ConstRangeTraversal, NothrowConstInputRange, Traversal, Sequence, Producer
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `ConstInputRange<Vector<int, 4>>`, `ConstInputRange<Queue<int, 4>>`, and `ConstInputRange<Range<int>>` are satisfied by their const `begin()`/`end()` traversal.


### NothrowConstInputRange
- **Definition:** The strengthened structural capability requiring a `ConstInputRange` whose const `begin`, `end`, dereference, pre-increment, post-increment, and iterator/sentinel comparison operations are non-throwing. NothrowConstInputRange is the structural traversal requirement of `CljonicSource` and does not replace nominal source admission.
- **Deprecated Synonyms:** non-throwing const input range, noexcept const input range
- **Related:** CljonicSource, ConstInputRange, ConstRangeTraversal, NoExceptionConstraint, Traversal
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `NothrowConstInputRange<Vector<int, 4>>`, `NothrowConstInputRange<Queue<int, 4>>`, and `NothrowConstInputRange<Range<int>>` are satisfied by their non-throwing const traversal operations.


### CapabilityConcept
- **Definition:** A C++20 concept that expresses one of the named semantic capabilities a cljonic collection or producer must expose (`Indexed`, `Lookup`, `Seqable`, or `Associative`) in order to participate in an operation, layered on top of nominal identity.
- **Deprecated Synonyms:** capability concept, semantic capability gate
- **Related:** CollectionConcept, ProducerConcept, Indexed, Lookup, Seqable, Associative, CapabilityPredicate, SequenceableCollection, IndexedCollection, LookupCollection, AssociativeCollection, SequenceableProducer, IndexedProducer
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** `SequenceableCollection<C>` is a CapabilityConcept requiring non-throwing `is_empty` and `count` observation.


### SequenceableCollection
- **Definition:** The C++ concept identifier implementing the current sequenceable CapabilityConcept baseline, requiring non-throwing `is_empty` and `count` observation on an admitted collection. SequenceableCollection is intentionally bootstrapped and may grow only when a public operation needs stronger sequence behavior; it is not the complete semantic definition of `Sequenceable` and does not by itself imply `ConstRangeTraversal`, indexed access, lookup, contiguous storage, or a particular iterator category.
- **Deprecated Synonyms:** sequenceable_cljonic_collection, sequenceable collection concept
- **Related:** CapabilityConcept, Sequenceable, Traversal, ConstRangeTraversal, CljonicCollection, IndexedCollection, LookupCollection, AssociativeCollection
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** `SequenceableCollection<C>` requires `c.is_empty()` and `c.count()`.


### IndexedCollection
- **Definition:** The C++ concept identifier implementing the `Indexed` CapabilityConcept, extending the required lookup observation with callable indexed lookup and the `contains` (index-in-range) predicate.
- **Deprecated Synonyms:** indexed_cljonic_collection, indexed collection concept
- **Related:** CapabilityConcept, Indexed, Lookup, SequenceableCollection, CljonicCollection, Contains
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** `IndexedCollection<C>` requires `c(i)` and `c.contains(i)`.


### LookupCollection
- **Definition:** The C++ concept identifier implementing the `Lookup` CapabilityConcept for an admitted collection, requiring an explicit `lookup_type`, callable lookup, and a matching `contains` predicate without implying map-style key-to-value association.
- **Deprecated Synonyms:** lookup collection concept, general lookup capability
- **Related:** CapabilityConcept, SequenceableCollection, IndexedCollection, AssociativeCollection, Map, Set, Contains
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** `LookupCollection<Map<int, int, 4>>` and `LookupCollection<Set<int, 4>>` are satisfied when each exposes its lookup domain through `lookup_type`.


### AssociativeCollection
- **Definition:** The C++ concept identifier implementing the `Associative` CapabilityConcept, requiring key/value association, key-based callable lookup, membership testing, and the collection's immutable association/preflight surface.
- **Deprecated Synonyms:** associative_cljonic_collection, associative collection concept
- **Related:** CapabilityConcept, Associative, Lookup, SequenceableCollection, CljonicCollection, Contains, Assoc, CanAssoc
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** `AssociativeCollection<C>` requires `c(k)` and `c.contains(k)`.


### SequenceableProducer
- **Definition:** The C++ concept identifier implementing the producer-domain baseline CapabilityConcept, requiring non-throwing `count()` effective-size observation on an admitted producer. `count()` for a producer is a conservative maximum (saturated at `CollectionMaximumElementCount` for an unbounded or oversized form), distinct from Clojure's `Counted` interface, which implies an exact O(1) size. SequenceableProducer does not imply `IndexedProducer`, `LookupCollection`, or any positional access; free-function observation remains the canonical producer access path for producers that are not `IndexedProducer`.
- **Deprecated Synonyms:** sequenceable_cljonic_producer, sequenceable producer concept
- **Related:** CapabilityConcept, CljonicProducer, CljonicRange, IndexedProducer, SequenceableCollection
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** `SequenceableProducer<Range<int>>` requires only `r.count()`.


### IndexedProducer
- **Definition:** The C++ concept identifier implementing the `Indexed` CapabilityConcept for the producer domain, gated on `CljonicProducer` rather than `CljonicCollection`. It requires only `r.contains(i)` as the available-index predicate over the producer's bounded prefix, where `i` is an integer position, not a produced value. It deliberately does not require callable access, since a producer being `Indexed` does not imply it is invocable (`IFn`): `Range` is `Indexed` but not `IFn`, unlike `Vector`/`Map`/`Set`. Positional value retrieval (a `nth`-equivalent) remains deferred future work. Only producers with genuinely O(1) available-index testing may satisfy `IndexedProducer`; `Cycle`, `Iterate`, `Repeat`, and `Repeatedly` never do.
- **Deprecated Synonyms:** indexed_cljonic_producer, indexed producer concept
- **Related:** CapabilityConcept, CljonicProducer, CljonicRange, Indexed, IndexedCollection, Contains, SequenceableProducer
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** `IndexedProducer<Range<int>>` requires `r.contains(i)`; testing whether a position is available in `Range<int>{0, 1000000000}` is O(1).


### StableEqualityComparable
- **Definition:** The C++ concept identifier implementing the StableEquality capability: a non-floating-point equality comparison admitting a type for equality-dependent operations.
- **Deprecated Synonyms:** stable_equality_comparable, stable equality comparable concept
- **Related:** StableEquality, TotalOrder, TotallyOrdered, AggregateLikeStruct
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** `StableEqualityComparable<T>` is satisfied when `a == b` returns `bool` and `T` is not a floating-point type.


### TotallyOrdered
- **Definition:** The C++ concept identifier implementing the TotalOrder capability, layered on StableEqualityComparable and adding a strict ordering relation.
- **Deprecated Synonyms:** totally_ordered, totally ordered concept
- **Related:** TotalOrder, StableEqualityComparable, StableEquality, NumericPolicy
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** `TotallyOrdered<T>` requires `a < b` in addition to stable equality.


### NothrowStableEqualityComparable
- **Definition:** The C++ concept identifier combining StableEqualityComparable with NothrowCollectionElement, the shared admission contract for map keys and set elements.
- **Deprecated Synonyms:** nothrow_stable_equality_comparable, map key concept, set element concept
- **Related:** StableEqualityComparable, NothrowCollectionElement, Map, Set
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** `NothrowStableEqualityComparable<int>` is satisfied; a type with a throwing copy assignment is rejected even if it defines `operator==`.


### StaticInspectableStorage
- **Definition:** A storage property whose capacity, representation bound, and resource implications can be determined from a collection type or compile-time configuration.
- **Deprecated Synonyms:** static storage
- **Related:** CollectionMaximumElementCount, EmbeddedConstraint, NoHeapConstraint
- **Usage:** Requirements, architecture, implementation, tests, and documentation
- **Examples:** A fixed-capacity collection owns its buffer directly by value, allowing its capacity to be inspected from its type or configuration.


### PersistentValueSemantics
- **Definition:** The observable rule that an update returns a new independently valid value while leaving the prior value unchanged. This contract does not itself prescribe a storage algorithm.
- **Deprecated Synonyms:** persistent collection semantics, immutable update semantics
- **Related:** PersistentValue, CopyOnModifyCollection, DeepCopyUpdate, OwningValue
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** Updating a Vector produces a new value while the original Vector remains valid and unchanged.


### PersistentValue
- **Definition:** An owning value whose updates produce a new independently valid value while preserving the prior value unchanged and valid. PersistentValue names the value property; PersistentValueSemantics names the observable update contract, and DeepCopyUpdate is the current realization strategy for the supported fixed-capacity collections.
- **Deprecated Synonyms:** persistent collection value, immutable value
- **Related:** PersistentValueSemantics, OwningValue, CopyOnModifyCollection, DeepCopyUpdate
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** A `Vector`, `Map`, `Set`, `Queue`, or `String` remains a valid PersistentValue when `assoc`, `conj`, `dissoc`, `disj`, `pop`, or another supported update returns a changed copy.


### DeepCopyUpdate
- **Definition:** The Module 1 implementation strategy for persistent collection updates: stored elements are copied into the returned value without structural sharing, reference counting, or shared internal storage.
- **Deprecated Synonyms:** deep-copy update, copy-on-write update
- **Related:** PersistentValueSemantics, CopyOnModifyCollection
- **Usage:** Requirements, architecture, implementation, and tests
- **Examples:** A collection update returns a new owning collection instance containing copied stored elements.


### NoRttiConstraint
- **Definition:** The rule that library behavior and implementation do not use RTTI facilities, including `typeid`, `dynamic_cast`, or virtual-dispatch machinery.
- **Deprecated Synonyms:** no RTTI, RTTI-free rule
- **Related:** EmbeddedConstraint, NoExceptionConstraint
- **Usage:** Requirements, architecture, implementation, and tests
- **Examples:** Collection recognition uses compile-time traits rather than runtime type inspection.


### NoHiddenGlobalInitialization
- **Definition:** The rule that cljonic introduces no hidden global initialization or library-managed mutable global state.
- **Deprecated Synonyms:** no global initialization, no hidden global state
- **Related:** DeterministicBehavior, ReferentialTransparency
- **Usage:** Requirements, architecture, implementation, and tests
- **Examples:** A collection operation cannot rely on a library-initialized mutable registry to produce its result.


### SingleThreadedExecutionModel
- **Definition:** The library contract that neither requires nor provides synchronization, atomics, thread-local state, parallel execution, or concurrent-access safety. A multithreaded caller may use cljonic only when it confines each operation and value access to one thread at a time.
- **Deprecated Synonyms:** single-threaded model, non-concurrent execution model
- **Related:** DeterministicBehavior, NoHiddenGlobalInitialization
- **Usage:** Requirements, architecture, implementation, tests, and documentation
- **Examples:** cljonic does not synchronize concurrent accesses to a collection value.


### ReferentialTransparency
- **Definition:** The condition that, for equal explicit inputs and fixed configuration, an operation returns equivalent results without mutating inputs, performing I/O, reading or modifying hidden mutable state, or depending on it. The guarantee is conditional on the required operations of user-defined element, key, and value types being pure and non-allocating.
- **Deprecated Synonyms:** pure operation, functional purity
- **Related:** DeterministicBehavior, NoMutationConstraint, NoHeapConstraint, NoHiddenGlobalInitialization, OwningValue
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** Given equal Vector inputs and configuration, an update returns equivalent new values without changing its input.


### SimpleAggregateBoundary
- **Definition:** The admissible user-defined element, key, and value type boundary: simple aggregates are permitted only when every capability required by the applicable collection and operation is non-allocating and does not require forbidden runtime services. Storage alone does not require equality or ordering.
- **Deprecated Synonyms:** aggregate type boundary, user-defined aggregate boundary
- **Related:** NoHeapConstraint, NoExceptionConstraint, StaticInspectableStorage
- **Usage:** Requirements, architecture, implementation, and tests
- **Examples:** A user-defined aggregate may be stored in a Vector when its required construction, copying, moving, and destruction capabilities satisfy the applicable operation contract.


## Relationship Notes

- CopyOnModifyCollection is the foundational value model for the repo.
- SentinelBasedAccess and ProbeFirstAccess define the canonical error-handling discipline for collection APIs.
- DeterministicOverflowPolicy defines the bounded numeric semantics that fit the embedded constraint model.
- OwningValue, NonOwningView, StandardViewType, ReadOnlyInteropAccessor, and ContiguousConstView define ownership and lifetime semantics for values versus views.
- ConstRangeTraversal and LogicalTraversalOrder define read-only collection traversal independently of whether a contiguous interoperability view is available.
- CompleteResult, BoundedPrefixResult, DefaultReturningResult, CheckedFailureResult, and PreflightPredicate define canonical result-status and completion semantics.
- LifecycleClassification with RequirementsBacked, CandidateStatus, DeferredStatus, and ExcludedStatus defines API-surface governance vocabulary.
- UnboundedProducer and ProducerMaterialization define explicit producer-to-result boundaries.
- RelationModel governs when relational operations can move from deferred to requirements-backed.
- StatePredicate and VerbPredicate define canonical predicate naming constraints.
- EmbeddedConstraint, StaticInspectableStorage, NoHeapConstraint, NoExceptionConstraint, NoRttiConstraint, NoHiddenGlobalInitialization, SingleThreadedExecutionModel, and DeterministicBehavior define the platform and execution constraints.
- ClosedNominalCollectionDomain, NominalCollectionRecognition, and CollectionKind define which types may participate as cljonic collections.
- PersistentValue is the public value property; PersistentValueSemantics defines its observable update contract, while DeepCopyUpdate defines Module 1's required realization of that contract.
- HeaderOnlyDistribution and AmalgamatedHeader define the packaging vocabulary for build and user documentation.
- CompileTimeFailure and CapacityConstruction define observable construction behavior contracts that should map directly into Allium specs.
