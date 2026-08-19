# cljonic C++ Concept Inventory

This document inventories the C++ concepts and supporting compile-time traits
needed to implement the behavior defined by
[cljonic-requirements.md](cljonic-requirements.md).

The inventory is implementation-oriented. Each entry identifies a concept,
trait, or compile-time metadata facility that can constrain a public API,
select an implementation, form a result type, or validate a data-structure
boundary.

## Design rules

### Prefer standard facilities

The library MUST use a standard concept or trait directly when it expresses the
requirement completely. cljonic MUST NOT introduce a public concept or trait
that merely renames an equivalent standard facility.

Standard facilities used directly include:

- `std::default_initializable`
- `std::copyable`, `std::movable`, and `std::destructible`
- `std::constructible_from` and `std::convertible_to`
- `std::invocable` and `std::same_as`
- `std::integral`, `std::signed_integral`, `std::unsigned_integral`, and `std::floating_point`
- `std::is_enum_v` and the standard `std::is_nothrow_*` traits
- `std::ranges::range` and `std::ranges::sized_range` where their semantics apply

The following are therefore not cljonic inventory entries by themselves:

- `DefaultElement` as a spelling of `std::default_initializable`;
- `CopyableValue` as a spelling of `std::copyable`;
- `MovableValue` as a spelling of `std::movable`;
- `InvocableWith` as a spelling of `std::invocable`.

### Prefer concepts for public constraints

Public templates and free functions SHOULD use named concepts in their
parameters or `requires` clauses. A cljonic concept is justified when it:

1. combines multiple constraints into a meaningful user-facing capability;
2. checks a cljonic free-function expression protocol;
3. adds a cljonic semantic policy not provided by the standard library; or
4. gives a user-facing diagnostic a precise corrective meaning.

Supporting traits MAY be used internally when a compile-time policy or metadata
cannot be expressed as an operation. Such traits are part of this inventory
when they constrain an API, select a data structure, or determine a result
type. They SHOULD NOT replace a public concept where a concept can expose the
same requirement.

### Concept names are diagnostic interfaces

Names MUST identify the capability the caller needs and, where practical, the
action needed to fix the violation. Names such as `Valid`, `Supported`,
`Allowed`, or `Compatible` without a qualified capability are not sufficient.

Examples of diagnostic-facing names include:

- `VectorElement<T>`;
- `MapKey<K>`;
- `SetElement<T>`;
- `Sequenceable<C>`;
- `MaterializableInto<P, D>`;
- `StableEqualityComparable<T>`;
- `StableTotalOrderable<T>`;
- `ExactlyConvertible<From, To>`; and
- `CollectionCapacityWithinConfiguredMaximum<N>`.

### Concepts versus static assertions

The implementation SHOULD use a concept when failure is knowable from types,
expressions, or non-type template arguments. This allows the compiler to
identify the failed named constraint at the public API boundary.

`static_assert` remains appropriate for:

- value-dependent constant-expression checks such as literal length;
- diagnostics that need to display computed values;
- constructor invariants involving several dependent values; and
- implementation invariants that are not API participation constraints.

Diagnostics MUST be tested for their intended meaning, not compiler-specific
wording.

## Phase 0: Existing bootstrap concepts

These concepts exist in `src/cljonic-concepts.hpp` and support the current
vector/count nucleus.

### `VectorElement<T>`

- Kind: public concept
- Definition: `std::default_initializable<T> && std::copyable<T>`
- Used by: `Vector` element storage and vector construction
- Requirements: `REQ-VAL-007`, `REQ-COLL-002`, `REQ-DIAG-001`

### `NothrowVectorElement<T>`

- Kind: public concept
- Definition: `VectorElement<T>` plus non-throwing default construction and copy assignment
- Used by: vector storage and vector construction
- Requirements: current vector implementation contract

### `NothrowElementConstruction<T, U>`

- Kind: public concept
- Definition: implicit convertibility to `T` plus non-throwing construction of `T` from `U`
- Used by: bounded vector element construction
- Rationale: combines the conversion and exception guarantees required by one constructor operation

### `Collection<C>`

- Kind: bootstrap concept under review
- Current definition: `c.size()` is convertible to `std::size_t`
- Decision: retain only until the free-function collection concepts below replace its uses
- Follow-up: do not expand this concept into a vague general collection hierarchy

## Phase 1: Value and comparison capabilities

### `StableEqualityComparable<T>`

- Kind: public concept
- Definition: the cljonic equality expression exists and the type is admitted by the stable-equality policy
- Used by: map keys, set elements, membership, distinctness, frequency counting, finite equality
- Requirements: `REQ-VAL-007`, `REQ-SEQ-015` through `REQ-SEQ-017`, `REQ-NUM-004` through `REQ-NUM-007`
- Constraint: raw floating-point types MUST NOT satisfy the default policy

### `StableTotalOrderable<T>`

- Kind: public concept
- Definition: the cljonic ordering expression exists and the type is admitted by the stable-order policy
- Used by: sorting, ordered traversal, `sort_by`, ordered comparisons
- Requirements: `REQ-VAL-007`, `REQ-NUM-004`, `REQ-NUM-005`, `REQ-NUM-007`
- Constraint: raw floating-point types MUST NOT satisfy the default policy

### `FiniteDeepEqualityComparable<T>`

- Kind: public concept
- Definition: recursive equality is permitted without traversing an unbounded producer
- Used by: `equal` and nested collection equality
- Requirements: `REQ-SEQ-015` through `REQ-SEQ-020`

### `MapKey<K>`

- Kind: public composed concept
- Definition: stable equality, required value storage, and required key construction capabilities
- Used by: `Map<K, V, N>` and map lookup operations
- Requirements: `REQ-VAL-007`, `REQ-COLL-004`, `REQ-NUM-002`, `REQ-PLAT-015`, `REQ-PLAT-028`

### `SetElement<T>`

- Kind: public composed concept
- Definition: stable equality and required value storage capabilities
- Used by: `Set<T, N>` and set algebra
- Requirements: `REQ-VAL-007`, `REQ-COLL-005`, `REQ-NUM-002`, `REQ-PLAT-015`

## Phase 2: Collection expression protocols

These concepts MUST be expressed in terms of cljonic's canonical free
functions, not merely in terms of member functions.

### `Countable<C>`

- Requires: `count(c)` with a count-like result
- Used by: `count`, sequence operations, producers, and capacity calculations
- Requirements: `REQ-VAL-016`, `REQ-SEQ-004`

### `EmptyCheckable<C>`

- Requires: `empty(c)` returning `bool`
- Used by: sequence and collection underflow checks
- Requirements: `REQ-BOUNDS-008`, `REQ-BOUNDS-010`, `REQ-SEQ-002`

### `CapacityAware<C>`

- Requires: capacity observation appropriate to the collection
- Used by: bounded insertion, capacity predicates, and result formation
- Requirements: `REQ-BOUNDS-010`, `REQ-COLL-007`, `REQ-COLL-008`

### `Traversable<C>`

- Requires: the common traversal expressions applicable to `C`
- Used by: `first`, `next`, `rest`, `seq`, and traversal transformations
- Requirements: `REQ-SEQ-001` through `REQ-SEQ-009`

### `Sequenceable<C>`

- Requires: `seq(c)` returning a supported owning bounded sequence value
- Used by: generic sequence operations and `into`
- Requirements: `REQ-SEQ-003`, `REQ-SEQ-004`, `REQ-SEQ-010` through `REQ-SEQ-014`

### `Indexable<C, I>`

- Requires: `get(c, i)` and `valid_index(c, i)` with compatible semantics
- Used by: vector and map-entry indexed access
- Requirements: `REQ-BOUNDS-001` through `REQ-BOUNDS-011`, `REQ-COLL-002A`, `REQ-SEQ-013`

### `Associative<M, K>`

- Requires: `get(m, k)`, `contains(m, k)`, and the applicable association operations
- Used by: maps and key-based free functions
- Requirements: `REQ-COLL-004`, `REQ-COLL-004A`, `REQ-COLL-004B`, `REQ-FN-002P`

### `Conjable<C, T>`

- Requires: `conj(c, value)` and `can_conj(c, value)`
- Used by: vector, set, and applicable collection construction
- Requirements: `REQ-COLL-002`, `REQ-COLL-005`, `REQ-FN-002M`

### `StackLike<C>`

- Requires: `peek(c)` and `pop(c)` with documented empty behavior
- Used by: vector stack-style operations
- Requirements: `REQ-COLL-002`

### `QueueLike<C, T>`

- Requires: bounded rear insertion, front removal, and front inspection
- Used by: `Queue<T, N>` operations
- Requirements: `REQ-COLL-006`, `REQ-ERR-004`

### `SetLike<S, T>`

- Requires: stable membership, insertion, removal, and count expressions
- Used by: `Set<T, N>` and set algebra
- Requirements: `REQ-COLL-005` through `REQ-COLL-005B`, `REQ-FN-023`

### `MapEntry<E, K, V>`

- Requires: key/value access and two-element sequence behavior
- Used by: map sequencing and map-entry traversal
- Requirements: `REQ-SEQ-012` and `REQ-SEQ-013`

### `Viewable<C>`

- Requires: the documented read-only standard view expression for `C`
- Used by: `view(collection)` and platform interoperability
- Requirements: `REQ-PLAT-017` through `REQ-PLAT-023`

## Phase 3: Result, preflight, and materialization concepts

### `CanAssoc<M, K, V>`

- Requires: `can_assoc(m, k, v)` and the corresponding `assoc` expression
- Used by: bounded map association
- Requirements: `REQ-FN-002M`, `REQ-COLL-004A`, `REQ-TEST-004A`

### `FitsInto<D, P>`

- Requires: `fits_into(destination, producer)` and `into(destination, producer)`
- Used by: complete producer materialization
- Requirements: `REQ-BOUNDS-012` through `REQ-BOUNDS-016`, `REQ-FN-009` through `REQ-FN-014C`

### `FitsSetAlgebra<S1, S2>`

- Requires: `fits_set_algebra(left, right)` for compatible set operands
- Used by: complete set-algebra preflight
- Requirements: `REQ-FN-023`, `REQ-BOUNDS-017`

### `FitsPrint<D, T>`

- Requires: `fits_print(destination, value, maps...)` and `print_to(destination, value, maps...)`
- Used by: bounded debug formatting
- Requirements: `REQ-FN-002K`, `REQ-PLAT-033` through `REQ-PLAT-042`

### `Producer<P>`

- Requires: producer observation and materialization expressions
- Used by: `range`, `repeat`, `cycle`, `iterate`, `repeatedly`, and `into`
- Requirements: `REQ-VAL-014` through `REQ-VAL-017`, `REQ-FN-009` through `REQ-FN-014C`

### `MaterializableInto<P, D>`

- Requires: producer `P` can materialize into explicit bounded destination `D`
- Used by: `into`
- Requirements: `REQ-VAL-014`, `REQ-VAL-015`, `REQ-BOUNDS-012` through `REQ-BOUNDS-016`

## Phase 4: Callable concepts

### `BooleanPredicate<F, T>`

- Requires: invocation with `T` and an exact `bool` result
- Used by: `filter`, `take_while`, `drop_while`, `some`, and predicate families
- Requirements: `REQ-FN-003`, `REQ-FN-008`, `REQ-FN-025`

### `UnaryTransform<F, Input, Output>`

- Requires: invocation with `Input` and a result convertible to `Output`
- Used by: `map`, `mapv`, `keep`, `replace`, and string transformations
- Requirements: `REQ-FN-002G`, `REQ-FN-005` through `REQ-FN-008`

### `ComposableWith<Outer, Inner>`

- Requires: the return type of `Inner` is accepted by `Outer`
- Used by: right-to-left `comp`
- Requirements: `REQ-FN-002H`, `REQ-TEST-064`

### `BoundedCallable<F>`

- Requires: bounded object size and the required copy, move, destruction, and exception guarantees
- Used by: stored closures, `comp`, callable constructors, and `Atom::swap`
- Requirements: `REQ-VAL-021`, `REQ-FN-002H`, `REQ-FN-002I`, `REQ-FN-025`

## Phase 5: Numeric concepts

### `FixedWidthNumeric<T>`

- Adds: fixed-width numeric policy beyond `std::integral` or `std::floating_point`
- Used by: arithmetic, conversion, parsing, and capacity calculations
- Requirements: `REQ-NUM-000` through `REQ-NUM-017`

### `DiscreteNumeric<T>`

- Requires: a numeric domain suitable for deterministic stepping
- Used by: `range` and integer-step operations
- Requirements: `REQ-NUM-003`, `REQ-FN-012`

### `BitwiseIntegral<T>`

- Requires: fixed-width bitwise operation expressions and valid shift policy
- Used by: the fixed-width bitwise family
- Requirements: `REQ-NUM-017`

### `ExactlyConvertible<From, To>`

- Requires: a compile-time-known exact representable conversion
- Used by: default numeric conversions
- Requirements: `REQ-NUM-008` through `REQ-NUM-013`

### `CheckedConvertible<From, To>`

- Requires: an explicitly checked conversion path with a documented failure result
- Used by: runtime conversion operations
- Requirements: `REQ-NUM-009` through `REQ-NUM-013`

### `CheckedArithmetic<T>`

- Requires: the checked arithmetic operation and its applicable preflight expressions
- Used by: `add`, `sub`, `mult`, `div`, `quot`, `rem`, and `mod`
- Requirements: `REQ-NUM-015` through `REQ-NUM-017`

## Phase 6: String, regex, and formatting concepts

### `BoundedString<S>`

- Requires: bounded owning ASCII string expressions and explicit content capacity
- Used by: `String<N>` and string transformations
- Requirements: `REQ-COLL-012` through `REQ-COLL-017`, `REQ-FN-022`

### `AsciiString<S>`

- Requires: the documented `0x01` through `0x7F` content policy
- Used by: construction, parsing, transformation, and formatting
- Requirements: `REQ-COLL-013`, `REQ-PLAT-038` through `REQ-PLAT-041`

### `BoundedRegex<R>`

- Requires: bounded pattern storage and the supported matching expressions
- Used by: `Regex`, runtime patterns, matchers, and match results
- Requirements: `REQ-VAL-018`, `REQ-FN-015` through `REQ-FN-020`, `REQ-PLAT-011`

### `PrintableTo<D, T>`

- Requires: bounded formatting into explicit destination `D`
- Used by: `print_to` and recursive debug formatting
- Requirements: `REQ-FN-002K`, `REQ-PLAT-033` through `REQ-PLAT-042`

### `SupportedEnumKey<E>`

- Requires: scoped enum identity and the equality/storage expressions required by map or set use
- Used by: application-defined enum keys and set elements
- Requirements: `REQ-PLAT-028`, `REQ-PLAT-035`, `REQ-PLAT-038` through `REQ-PLAT-042`

## Supporting compile-time traits and metadata

These are not preferred over concepts. They are included because they provide
compile-time information that cannot be recovered from a normal expression
concept and that affects API or data-structure formation.

### Capability policy traits

```text
is_stable_equality<T>
is_stable_total_order<T>
is_finite_observable<T>
is_non_allocating_value<T>
is_supported_aggregate<T>
```

These traits SHOULD remain in `cljonic::detail` and SHOULD be surfaced through
named public concepts where a public constraint is needed.

### Collection metadata

```text
element_type_of<C>
key_type_of<C>
mapped_type_of<C>
collection_kind_of<C>
capacity_of<C>
maximum_capacity_of<C>
view_type_of<C>
```

These metadata facilities support capacity validation, free-function
constraints, standard view selection, and result-type formation.

### Producer and result metadata

```text
producer_value_type<P>
producer_cardinality<P>
is_finite_producer<P>
is_unbounded_producer<P>
result_value_type<Operation, Arguments...>
result_capacity_of<Operation, Arguments...>
```

These are justified only where they affect a public result type, bounded
materialization, or compile-time rejection. They MUST NOT be used to hide
runtime cardinality or to introduce implicit materialization.

### Capacity concept

```cpp
template <std::size_t capacity>
concept CollectionCapacityWithinConfiguredMaximum =
    capacity <= CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT;
```

This concept constrains collection declarations and derived result capacities.

## Deliberately excluded from the C++ concept inventory

The following remain requirements, specifications, or tests rather than
concepts or traits:

- whether an implementation actually performs dynamic allocation;
- callback purity;
- runtime source lifetime of a view;
- agreement between a preflight predicate and its operation;
- deterministic traversal order;
- preservation of input values;
- whether a runtime result is complete or a bounded prefix;
- absence of hidden mutable state;
- the approved relation model for `index`, `project`, `rename`, and `join`.

They MUST still be documented and tested. They are excluded here only because
they do not constrain a declaration through a directly implementable C++
concept or compile-time metadata facility.

## Incremental implementation checklist

1. Preserve the Phase 0 concepts while current vector/count code depends on them.
2. Add comparison and element concepts before map or set key constraints.
3. Add collection expression concepts as each free-function family is specified.
4. Add capacity metadata before derived-capacity result types.
5. Add producer and preflight concepts before producer materialization.
6. Add callable concepts before higher-order operations.
7. Add numeric concepts before numeric convenience functions.
8. Add string, regex, formatting, and enum concepts with their corresponding APIs.
9. Add compile-failure tests for every public concept boundary.
10. Remove bootstrap concepts that become redundant after the refined concepts are adopted.

Every new concept or supporting trait MUST identify its requirements, public
consumers, dependencies, compile-time acceptance cases, and compile-time
rejection cases before it is used by a public API.
