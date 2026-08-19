# cljonic Concepts Proposal

This document proposes the concept set I believe is needed to fully implement the cljonic library described in [cljonic-requirements.md](cljonic-requirements.md), given the current bootstrap concepts in [src/cljonic-concepts.hpp](src/cljonic-concepts.hpp).

The current file is only a thin starting layer. The requirements describe a broader, bounded, value-semantic, single-header C++ library with explicit failure semantics, bounded capacity behavior, producer semantics, view lifetimes, and collection algebra. The concept model therefore needs to be layered and requirement-driven rather than limited to a few convenience checks.

## Design principle

The concept model should follow the same hierarchy as the requirements:

1. value and lifetime behavior
2. collection and boundedness behavior
3. result status and preflight behavior
4. numeric/text/set/map/function semantics
5. domain-specific relation and producer behavior

This preserves the project’s durable rule: architecture, specs, and code are constrained by explicit behavior, not by “what happens to compile.”

## 1) Core value and lifetime concepts

These are foundational because the requirements repeatedly require:

- immutable-by-default value semantics
- no hidden allocation
- no external storage lifetime dependence
- explicit result-status behavior
- bounded finite behavior

### `ValueSemantics<T>`

A type that is copyable, movable, and destructible, and whose semantics are not based on hidden borrowed state or external object lifetime.

This is the minimum semantic contract for user-visible cljonic values.

### `OwningValue<T>`

A self-contained cljonic value whose validity does not depend on external storage or hidden caches.

Examples include collection values, string values, regex results, map entries, producer configuration values, and other first-class library values.

### `BorrowedView<T>`

A non-owning, read-only observation of an existing value.

Requirements for this concept include:

- no ownership of storage
- no extension of source lifetime
- no mutation through the view
- view validity tied to source lifetime

### `StableEqualityComparable<T>`

A type whose equality semantics are explicit and stable, not pointer identity or hidden object state.

This matters for set membership, map key semantics, and bounded equality behavior.

### `TotalOrderable<T>`

A type that supports a total ordering when ordering is part of the supported semantics.

This is a capability, not an assumed property of every value type.

### `DefaultConstructibleElement<T>`

A type that can be default-initialized in a way that satisfies storage and default-element conventions.

### `NothrowMovable<T>`

Move construction and move assignment are `noexcept` when the library requires bounded, embedded-safe move behavior.

### `NoAllocation<T>`

A type whose normal construction, copying, movement, lookup, traversal, transformation, and destruction do not require dynamic allocation.

This is a library-level contract, not just a property of one operation.

---

## 2) Collection and bounds concepts

These concepts map directly to the bounded collection domain in the requirements.

### `Collection<C>`

A finite logical container with a size and deterministic empty semantics.

This is the general container concept for the library.

### `SizedCollection<C>`

A collection whose logical cardinality is available via `size()` and convertible to `std::size_t`.

This is close to the current concept in [src/cljonic-concepts.hpp](src/cljonic-concepts.hpp), but it should be more explicit and tied to bounded semantics.

### `BoundedCollection<C>`

A collection whose capacity is known and finite under the operation’s documented constraints.

Capacity may be compile-time-known or runtime-known, but it must be auditable from the type or instance contract.

### `ContiguousCollection<C>`

A collection whose elements are laid out in a contiguous, indexable memory model.

This fits `Vector`, `String`, and similar bounded container families.

### `IndexableCollection<C, I>`

A collection that supports valid indexing and a defined failure or default behavior when an index is invalid.

This concept must be paired with a result policy and failure contract.

### `AppendableCollection<C, T>`

A collection that supports appending or pushing a value while honoring capacity and bounded-result policy.

### `InsertableCollection<C, K, V>`

A map-like or set-like collection that supports insertion semantics while preserving boundedness and failure policy.

### `MutableCollection<C>`

A collection whose updates return a new value rather than mutating the input in place.

### `EmptySafeCollection<C>`

A collection with explicit and deterministic behavior for empty cases.

### `FiniteCapacityAware<C>`

A collection whose capacity or maximum size is observable and can be used in preflight and bounded-result decisions.

### `ViewableCollection<C>`

A collection that supports read-only viewing without losing lifetime guarantees.

---

## 3) Result-status and preflight concepts

This is the most important conceptual addition relative to the current file. The requirements are explicit that every operation must have a documented result status and, where appropriate, a preflight predicate.

### `ResultStatus<S>`

A value or enum-like type representing one of the required statuses:

- complete result
- bounded-prefix result
- default-returning result
- checked-failure result
- producer-only result

### `CompleteResult<R>`

A result object that represents the full valid output of an operation.

### `BoundedPrefixResult<R>`

A deterministic truncated result produced when the complete result cannot fit within the documented capacity or policy.

### `DefaultReturningResult<R>`

A documented default value for absent, invalid, or unavailable results.

### `CheckedFailureResult<R>`

A non-throwing, non-allocating failure indicator that explicitly communicates that the requested result could not be completed under the library’s rules.

### `ProducerResult<P>`

An explicit producer value that represents a materialization source or stream without owning the final result storage.

### `PreflightPredicate<P>`

A non-throwing, non-allocating predicate that answers whether the corresponding operation can produce a complete valid result under the same conditions as the operation itself.

### `OperationOutcome<Op, T>`

A concept tying an operation to its contractually defined outcome category and failure behavior.

### `FitsIntoOperation<O, Dest>`

An operation that has a canonical completeness check for whether a full result fits into the destination representation.

This is the core of the “universal bounded-result and preflight policy” described in the requirements.

---

## 4) Numeric concepts

The requirements define a broader numeric family than just capacity arithmetic and indexing.

### `FixedWidthInteger<T>`

An integer type with explicit width and fixed-size semantics.

### `CheckedArithmetic<T>`

Arithmetic operations on the type are checked and must not silently wrap or reinterpret values.

### `RepresentableIn<T, U>`

A value of type `T` can be represented in type `U` under the library’s representability rules.

### `ExplicitlyConvertible<T, U>`

A conversion whose behavior is explicit and documented, including failure semantics when conversion is lossy or impossible.

### `LosslessConvertible<T, U>`

A conversion that preserves the value exactly.

### `ScalarComparable<T>`

A scalar type with defined comparison semantics that are not accidental C++ promotion behavior.

### `DivisionSafe<T>`

A numeric type whose division-by-zero behavior is explicit and documented.

### `BitwiseIntegral<T>`

A fixed-width integral type with defined bitwise operations and explicit semantics.

### `FloatingPointStable<T>`

A floating point type where comparison, conversion, and selection semantics are governed by documented stability rules.

---

## 5) String and text concepts

The requirements treat strings as a first-class bounded value layer, not just parser or debug output helpers.

### `StringLike<S>`

A bounded owning text value with known length and capacity, and explicit result semantics for transformations.

### `AsciiPreserving<S>`

A string-like type whose operations preserve ASCII constraints and bounded semantics.

### `StringViewLike<V, S>`

A non-owning string observation tied to the lifetime of an underlying string value.

### `StringAppender<S, Dest>`

A transform or operation that writes into a destination while honoring capacity and bounded-result contracts.

### `StringTransformResult<R>`

A result type whose complete, bounded-prefix, default, or failure behavior is documented by the operation.

---

## 6) Set, map, queue, and relation concepts

The requirements explicitly distinguish set algebra, map semantics, queue semantics, and relation-model requirements.

### `SetLike<S, T>`

A bounded unordered collection with stable equality semantics and algebraic operations such as union, intersection, and difference.

### `MapLike<M, K, V>`

A bounded associative collection with key lookup, insertion, replacement, and explicit failure or default semantics.

### `QueueLike<Q, T>`

A bounded FIFO collection with deterministic enqueue/dequeue behavior.

### `AssociativeCollection<M, K, V>`

A more general key-based collection concept covering lookup, membership, and insertion.

### `SubsetPredicate<A, B>`

A predicate indicating whether `A` is a subset of `B`.

### `SupersetPredicate<A, B>`

A predicate indicating whether `A` is a superset of `B`.

### `RelationRow<R>`

A row representation used in a relation model before relational operations are supported.

### `RelationModel<R>`

A full relation model defining:

- row representation
- key and value capabilities
- duplicate semantics
- nested result behavior
- traversal order
- capacity arithmetic
- complete-result preflight
- bounded failure behavior

This is explicitly required before relational operations such as `index`, `project`, `rename`, and `join` are supported.

---

## 7) Callable / functional concepts

The requirements require bounded free-function-first convenience functionality rather than runtime type dispatch or hidden mutation.

### `CallableObject<F, Args...>`

A function-like object that is invocable with the required arguments.

### `NoCaptureCallable<F>`

A callable whose behavior does not rely on implicit callback retention, hidden mutable cache state, or runtime dispatch.

### `FreeFunctionLike<F>`

An ordinary function object or free function representation that works with cljonic’s bounded functional API.

### `ComposableFunction<F, G>`

A concept capturing the requirement that `comp` supports ordinary right-to-left composition of functions.

### `PredicateFunction<P, T>`

A function-like predicate used for filtering or selection.

### `ResultTransformingFunction<F, In, Out>`

A transformation function whose result type obeys the same bounded-result policy as the library.

### `ClosureStorage<C>`

A type that stores a closure or function object as a value while obeying existing bounded closure storage and non-allocation constraints.

---

## 8) Producer concepts

The requirement language strongly distinguishes owning values from explicit producers.

### `Producer<P>`

A value that represents a materialization source or stream without owning the final materialized result state.

### `FiniteProducer<P>`

A producer whose source is finite and has explicit length or termination semantics.

### `MaterializableFrom<P, C>`

A producer that can be materialized into the given collection type.

### `IntoTarget<P, C>`

A producer or source whose materialization contract is defined via an `into`-like target and bounded-result requirements.

---

## 9) Enum / keyword / human-readable name concepts

These are specific to application-defined enum support and keyword-like map keys.

### `ScopedEnum<E>`

An `enum class` type used as a domain-specific key.

### `KeywordLike<K>`

A name-like, stable, explicit domain symbol concept used as a map key or set element.

### `EnumKey<K>`

A key value that satisfies the application domain’s explicit identity and comparison rules.

### `DebugNameProvider<E>`

A type that can provide a human-readable debug name without changing the enum’s identity or storage semantics.

---

## 10) Recommended full initial concept set

If I were implementing the actual concept suite for this library, I would start with these as the canonical public set:

1. `ValueSemantics`
2. `OwningValue`
3. `BorrowedView`
4. `NoAllocation`
5. `Collection`
6. `SizedCollection`
7. `BoundedCollection`
8. `ContiguousCollection`
9. `IndexableCollection`
10. `AppendableCollection`
11. `InsertableCollection`
12. `MutableCollection`
13. `SetLike`
14. `MapLike`
15. `QueueLike`
16. `StringLike`
17. `Producer`
18. `CompleteResult`
19. `BoundedPrefixResult`
20. `DefaultReturningResult`
21. `CheckedFailureResult`
22. `PreflightPredicate`
23. `FixedWidthInteger`
24. `CheckedArithmetic`
25. `RepresentableIn`
26. `ExplicitlyConvertible`
27. `ScalarComparable`
28. `CallableObject`
29. `PredicateFunction`
30. `ComposableFunction`
31. `ScopedEnum`
32. `KeywordLike`
33. `RelationModel`
34. `ResultStatus`

That is the concept set I think is needed to implement the library in a disciplined, requirement-driven way.

## Why this is the right shape

The existing concepts in [src/cljonic-concepts.hpp](src/cljonic-concepts.hpp) are useful but incomplete because they only model:

- default-initializable value storage
- copyability
- convertibility
- collection size detection

They do not model the library’s real constraints:

- bounded capacity and preflight
- result-status classification
- no-allocation requirements
- string and set algebra semantics
- producer semantics
- numeric safety
- relation model gating
- view lifetime rules

Those are exactly the central concerns described in [cljonic-requirements.md](cljonic-requirements.md).

## Summary

The concept model should be a requirement-driven concept lattice, not a list of convenience compile checks. The minimum set should cover the real cljonic contract: bounded values, collection semantics, explicit result behavior, numeric safety, text support, map/set/queue semantics, producers, callables, and relation gating.

That is the conceptual foundation I would propose before we decide which parts are immediate implementation work and which parts belong to later library layers.
