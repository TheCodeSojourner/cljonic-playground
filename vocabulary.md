---
created: 2026-08-05
last_updated: 2026-08-28
status: draft
---

## Core Vocabulary

## Current Scope

The current implementation and tests cover the core collection types (`Vector`,
`Map`, `Set`, `Queue`, and `String`), their direct construction, member-observation,
callable forms, sequence traversal interfaces, and primitive free-function
operations defined in Module 3. Module 3 establishes the concrete, array-backed,
bounded collection types, their contiguous storage strategies, linear scan lookup
algorithms, swap-and-remove policies, and primitive free functions. These terms
govern stored collection building blocks used across all higher-order algorithms.

### Collection
- **Definition:** A bounded cljonic value or data structure admitted to the closed nominal collection domain and governed by collection-specific capacity, access, failure, and value-semantic rules.
- **Deprecated Synonyms:** collection value, cljonic collection
- **Related:** ClosedNominalCollectionDomain, CollectionKind, BoundedStorage, OwningValue
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `Vector`, `Map`, `Set`, `Queue`, and `String` are supported collection kinds in the closed domain.


### Sequence
- **Definition:** An ordered collection or producer domain whose elements have a defined traversal order and may support underflow, indexed access, or sequence observation according to its capabilities.
- **Deprecated Synonyms:** ordered sequence, sequential value
- **Related:** Collection, Sequenceable, Traversal, Producer
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** A `Vector` is a sequence; a producer may represent a sequence without owning materialized storage.


### Sequenceable
- **Definition:** A capability indicating that a nominal cljonic collection exposes non-throwing sequence observation, including emptiness and element count, without requiring indexed access.
- **Deprecated Synonyms:** sequence capability
- **Related:** Sequence, Traversal
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** A sequenceable collection supports `is_empty` and `count` under the applicable contract.


### Traversal
- **Definition:** Ordered observation of a collection or producer's elements under its documented bounds, without implying materialization, mutation, or indexed access.
- **Deprecated Synonyms:** sequence traversal, iteration
- **Related:** Sequence, ProducerIteration, BoundedInspection
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `first`, `next`, and `rest` are traversal operations when supported by the applicable sequence contract.


### Capacity
- **Definition:** The finite number of elements or logical units a bounded value can hold under its declared type and configuration.
- **Deprecated Synonyms:** collection capacity, declared capacity
- **Related:** BoundedStorage, CollectionMaximumElementCount, CapacityConstruction, FullState
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `Vector<int, 4>` has capacity four, while `String<32>` has a logical character capacity of thirty-two.


### ValidIndex
- **Definition:** The indexed-access condition that an index belongs to the logical element domain of a collection or sequence; the canonical C++ predicate spelling is `valid_index`.
- **Deprecated Synonyms:** valid index, index validity
- **Related:** IndexedAccess, PreflightPredicate, DefaultElement
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `valid_index(xs, i)` is checked before an indexed `get` when the indexed-access capability applies.


### FreeFunction
- **Definition:** A public operation expressed as a namespace-level function rather than a member of the value it observes or transforms.
- **Deprecated Synonyms:** free function API, namespace-level operation
- **Related:** SemanticPredicateName, Traversal, PreflightPredicate
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `valid_index(xs, i)` and `fits_into(destination, producer)` are free-function forms of orthogonal operations.


### BoundedStorage
- **Definition:** Storage with a finite, statically knowable bound that does not require dynamic allocation for supported operations.
- **Deprecated Synonyms:** bounded storage, fixed storage
- **Related:** StaticInspectableStorage, Capacity, NoHeapConstraint, EmbeddedConstraint
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** A collection's inline buffer is bounded storage whose capacity is visible from its type or configuration.


### PlatformInteroperability
- **Definition:** The capability boundary describing which standard C++ and embedded-platform facilities cljonic may use while preserving its documented resource, diagnostic, and value semantics.
- **Deprecated Synonyms:** platform compatibility, standard-library interoperability
- **Related:** EmbeddedConstraint, NoHeapConstraint, NoExceptionConstraint
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
- **Related:** NumericPolicy, ValidIndex, ExactConversion
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


### IndexedAccess
- **Definition:** A capability that provides access to a collection element by an index together with a `valid_index` preflight predicate for the same logical index domain.
- **Deprecated Synonyms:** indexed collection access, index access capability
- **Related:** ValidIndex, DefaultReturningResult, Contains
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** An indexed collection can use `valid_index` before a default-returning indexed `get`.


### AssociativeAccess
- **Definition:** A capability that provides key-based lookup over an admitted collection with a documented key type, value type, membership predicate, and missing-key result.
- **Deprecated Synonyms:** associative collection access, key-based access
- **Related:** Map, Contains, DefaultReturningResult
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** A map's associative access uses key presence before relying on a default-returning lookup.


### Contains
- **Definition:** The canonical boolean free function modeled on Clojure's `contains?`; it tests whether its argument belongs to a collection's lookup domain without performing a default-returning access. For maps it tests key presence, for sets it tests element presence, and for indexed collections it tests index validity.
- **Deprecated Synonyms:** `contains?`, contains predicate, key-presence check
- **Related:** FreeFunction, AssociativeAccess, IndexedAccess, ValidIndex, PreflightPredicate, VerbPredicate
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `contains(m, key)` tests a map key, `contains(s, value)` tests set membership, and `contains(xs, index)` tests whether an indexed position is valid.


### FitsInto
- **Definition:** The canonical materialization-completeness preflight predicate; its C++ free-function spelling is `fits_into`. It returns a non-throwing, non-allocating boolean indicating whether the complete producer result fits within the destination under the same cardinality, matching, filtering, transformation, capacity, representability, and overflow semantics as `into`.
- **Deprecated Synonyms:** `fits_into`, materialization-fit check, capacity-fit predicate
- **Related:** FreeFunction, ProducerMaterialization, CompleteResult, BoundedPrefixResult, PreflightPredicate, CapabilityPredicate
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `fits_into(destination, producer)` is checked before `into(destination, producer)` when complete materialization matters.


### BoundedResult
- **Definition:** An owning result whose capacity and maximum represented cardinality are finite under the operation's documented constraints.
- **Deprecated Synonyms:** bounded output, finite result
- **Related:** CompleteResult, BoundedPrefixResult, BoundedStorage
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** A materialized producer result is a bounded result even when the producer itself is unbounded.


### PartialResult
- **Definition:** A result that does not contain the operation's complete result because a documented capacity or result policy limits what can be represented; the canonical cljonic term is `BoundedPrefixResult`.
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
- **Related:** BoundedInspection, ProducerIteration, UnboundedProducer
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** Inspecting the first bounded prefix of an unbounded producer is finite observation.


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
- **Definition:** An explicit value representing a sequence or materialization source without owning the storage of its eventual materialized result.
- **Deprecated Synonyms:** sequence producer, source producer
- **Related:** Sequence, UnboundedProducer, ProducerIteration, ProducerMaterialization
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `range`, `repeat`, `cycle`, `iterate`, and `repeatedly` are producer families when their requirements are active.


### MapEntry
- **Definition:** The bounded owning key-value pair representation used when a map operation exposes one map association as a value.
- **Deprecated Synonyms:** map entry, key-value entry
- **Related:** Map, AssociativeAccess, OwningValue
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
- **Related:** CapabilityPredicate, StatePredicate, VerbPredicate
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `is_empty`, `contains`, `valid_index`, and `fits_into` use names that identify their semantic questions.


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
- **Related:** CapabilityPredicate, AssociativeAccess
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** A `has_` predicate may report key presence without performing a default-returning lookup.


### ValidPredicatePrefix
- **Definition:** The canonical `valid_` prefix for a predicate that determines whether an input belongs to an operation's valid domain before access or conversion.
- **Deprecated Synonyms:** `valid_` predicate, validity predicate prefix
- **Related:** ValidIndex, CapabilityPredicate, PreflightPredicate
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `valid_index` determines whether an index can produce a valid indexed access.


### KeywordEnumNameEntry
- **Definition:** A bounded mapping entry associating a supported keyword name with an application-defined scoped enumeration value.
- **Deprecated Synonyms:** keyword enum entry, enum-name entry
- **Related:** KeywordEnumNameMap, MapEntry, OwningValue
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** An entry can associate a keyword such as `:pending` with a scoped enumeration value.


### KeywordEnumNameMap
- **Definition:** A bounded map of keyword names to application-defined scoped enumeration values under the supported associative and capacity rules.
- **Deprecated Synonyms:** keyword-to-enum map, enum name map
- **Related:** KeywordEnumNameEntry, KeywordEnumNameContext, Map, AssociativeAccess
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** A keyword enum name map provides bounded lookup and documented missing-key behavior.


### KeywordEnumNameContext
- **Definition:** The explicit context required to interpret, validate, or map keyword names to scoped enumeration values without hidden global state.
- **Deprecated Synonyms:** enum-name mapping context, keyword mapping context
- **Related:** KeywordEnumNameEntry, KeywordEnumNameMap, NoHiddenGlobalInitialization
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** A caller supplies the mapping context rather than relying on a library-managed global registry.


### KeywordEnumNameMapping
- **Definition:** The bounded, explicit operation that maps supported keyword names and scoped enumeration values according to a `KeywordEnumNameContext`.
- **Deprecated Synonyms:** keyword enum mapping, enum-name mapping
- **Related:** KeywordEnumNameEntry, KeywordEnumNameMap, KeywordEnumNameContext, Parsing
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** Mapping reports invalid, missing, or duplicate names through its documented non-throwing policy.

### CopyOnModifyCollection
- **Definition:** A fixed-capacity value type backed by statically inspectable storage that returns a modified copy instead of mutating in place. Its observable contract is PersistentValueSemantics; Module 1 realizes updates through DeepCopyUpdate without structural sharing. In this repo, Vector, Map, Set, Queue, and String follow this model.
- **Deprecated Synonyms:** Copy-on-Modify Collection, bounded immutable collection, fixed-capacity collection, array-backed collection, deep copy on write, deep copying on write
- **Related:** PersistentValueSemantics, DeepCopyUpdate, StaticInspectableStorage, Vector, Map, Set, Queue, String, SentinelBasedAccess
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







### Vector
- **Definition:** The cljonic fixed-capacity sequential collection type for ordered element storage with immutable copy-on-modify updates.
- **Deprecated Synonyms:** vector collection, bounded vector, fixed-capacity vector
- **Related:** CopyOnModifyCollection, String, CapacityConstruction, IndexedAccess, ValidIndex
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** `Vector<int, 4>{1, 2}` constructs a fixed-capacity value whose `count()`, `capacity()`, `operator()`, and `valid_index` provide direct member and free-function observation.


### Map
- **Definition:** The cljonic fixed-capacity associative collection type mapping unique keys to values using contiguous storage and linear scans with immutable copy-on-modify updates.
- **Deprecated Synonyms:** bounded map, fixed-capacity map, associative map
- **Related:** MapEntry, AssociativeAccess, Contains, SwapAndRemove, CopyOnModifyCollection
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** `Map<int, String<16>, 4>{}` creates a bounded associative collection supporting `assoc`, `dissoc`, `contains`, `get`, and callable lookup `m(k)`.


### Set
- **Definition:** The cljonic fixed-capacity unordered collection type storing unique elements using contiguous storage and linear scans with immutable copy-on-modify updates.
- **Deprecated Synonyms:** bounded set, fixed-capacity set, unique element collection
- **Related:** StableEquality, Contains, SwapAndRemove, CopyOnModifyCollection
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** `Set<int, 4>{1, 2, 3}` creates a bounded set supporting `conj`, `disj`, `contains`, `get`, and callable lookup `s(v)`.


### Queue
- **Definition:** The cljonic fixed-capacity FIFO sequential collection type supporting insertion at the rear, removal at the front, and peek/pop observation with immutable copy-on-modify updates.
- **Deprecated Synonyms:** bounded queue, fixed-capacity queue, FIFO queue
- **Related:** Sequence, CopyOnModifyCollection, Traversal
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** `Queue<int, 4>{}` creates a bounded FIFO queue supporting `conj` (enqueue at rear), `peek` (front observation), and `pop` (removal from front).


### String
- **Definition:** The cljonic fixed-capacity array-backed collection type with ordered ASCII byte storage (range `0x01`–`0x7F`) and an uncounted null terminator immediately following its content.
- **Deprecated Synonyms:** bounded string, fixed-capacity string, cljonic string
- **Related:** CopyOnModifyCollection, Capacity, Sequence, BoundedStorage
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** `String<32>{"hello"}` or capacity-inferred `String{"hello"}` stores valid ASCII bytes with a terminating null outside the counted content length.


### SwapAndRemove
- **Definition:** An $O(1)$ removal strategy for unsorted collections (`Map` and `Set`) where the target element or key-value entry is overwritten by the last active stored element before decrementing the count.
- **Deprecated Synonyms:** swap and remove, swap-with-back, unordered removal
- **Related:** Map, Set, ContiguousStorage, Dissoc, Disj
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** Calling `disj(set, val)` or `dissoc(map, key)` copies the final element into the vacated slot and decrements the collection count without preserving internal traversal order.


### ContiguousStorage
- **Definition:** Flat internal array-backed storage (`std::array`) holding active collection elements or key-value entries in a single contiguous memory block without indirection or node allocation.
- **Deprecated Synonyms:** flat storage, contiguous array storage, inline array buffer
- **Related:** BoundedStorage, Vector, Map, Set, Queue, String
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `std::array<MapEntry<K, V>, N>` and `std::array<T, N>` store all active items contiguously within the collection value.


### CallableLookup
- **Definition:** Invocation of a collection instance via `operator()` providing concise read-only lookup with optional fallback default, behaviorally equivalent to `get`.
- **Deprecated Synonyms:** callable collection, functional lookup syntax, operator() lookup
- **Related:** SentinelBasedAccess, DefaultElement, IndexedAccess, AssociativeAccess
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `xs(2)` on a Vector, `m(key)` on a Map, and `s(val)` on a Set invoke callable lookup without mutating the collection.


### LinearScan
- **Definition:** An $O(N)$ sequential traversal over contiguous stored elements or key-value pairs used exclusively for search, lookup, association, and membership testing across bounded collections.
- **Deprecated Synonyms:** linear search, sequential scan, bounded scan
- **Related:** Map, Set, ContiguousStorage, Contains, StableEquality
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `contains(m, key)` performs a linear scan over active map entries matching key equality.


### Conj
- **Definition:** The primitive collection free function that returns a new collection with one or more elements added according to the target collection's type conventions (rear for `Vector` and `Queue`, membership insertion for `Set`).
- **Deprecated Synonyms:** conjoin, append, insert element
- **Related:** CopyOnModifyCollection, CanConj, PreflightPredicate, Vector, Set, Queue
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `conj(xs, 4)` appends 4 to a Vector; `conj(s, 4)` adds 4 to a Set if not already present.


### Assoc
- **Definition:** The primitive associative free function that returns a new `Map` with the supplied key-value association added or replaced.
- **Deprecated Synonyms:** associate, map assoc, put
- **Related:** Map, MapEntry, CanAssoc, Dissoc, CopyOnModifyCollection
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `assoc(m, key, val)` updates the value if `key` exists (without consuming extra capacity) or inserts the new pair if capacity remains.


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
- **Definition:** The primitive traversal free function that converts any sequenceable collection or input into an owning, value-semantic `Vector` of its traversal elements (or `MapEntry` elements for maps).
- **Deprecated Synonyms:** sequence conversion, to-seq, seq conversion
- **Related:** Sequenceable, Vector, MapEntry, Traversal
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `seq(m)` returns an owning `Vector<MapEntry<K, V>, N>` of map entries.


### CanConj
- **Definition:** The canonical preflight capability predicate (`can_conj`) checking whether `conj` can succeed without capacity overflow, returning true if capacity is available or if the item is an existing set element.
- **Deprecated Synonyms:** can_conj, can-conj predicate
- **Related:** Conj, CapabilityPredicate, PreflightPredicate, FullState, Set
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `can_conj(s, x)` returns true if `s` is not full or if `s` already contains `x`.


### CanAssoc
- **Definition:** The canonical preflight capability predicate (`can_assoc`) checking whether `assoc` can succeed without capacity overflow, returning true if the key already exists or if spare capacity remains.
- **Deprecated Synonyms:** can_assoc, can-assoc predicate
- **Related:** Assoc, CapabilityPredicate, PreflightPredicate, FullState, Map
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `can_assoc(m, k, v)` returns true if `contains(m, k)` is true or if `m` is not full.


### Iterate
- **Definition:** A generated collection type that repeatedly applies a function to produce a sequence. Element at index `i` is computed by applying the function `i` times to a seed value. Iterate is referentially transparent and finite by construction when a finite count is supplied; unbounded or omitted forms use the synthesis cap `CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT` rather than a fake finite size.
- **Deprecated Synonyms:** Iterate collection, iterated sequence
- **Related:** CollectionMaximumElementCount
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** `Iterate(inc, 0, 5)` produces `0, 1, 2, 3, 4` by repeatedly incrementing from `0`; `Iterate(f, seed)` defaults to `CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT` iterations.













































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
- **Definition:** A read-only observation of an existing cljonic value that does not own storage, does not extend source lifetime, and does not allow source mutation.
- **Deprecated Synonyms:** non-owning view, borrowed view
- **Related:** OwningValue, String
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `view(collection)` returns a non-owning view whose validity is limited by the source lifetime and invalidation rules.





### CompleteResult
- **Definition:** The full result defined by an operation's contract.
- **Deprecated Synonyms:** full result
- **Related:** BoundedPrefixResult, PreflightPredicate
- **Usage:** Requirements, specification, tests, and documentation
- **Examples:** When `fits_into` is true, the corresponding materialization operation returns a complete result.



### BoundedPrefixResult
- **Definition:** A bounded result that intentionally contains only a deterministic prefix or reduced subset because the complete result could not fit under the documented capacity or result policy.
- **Deprecated Synonyms:** partial result, prefix result
- **Related:** CompleteResult, PreflightPredicate, EffectiveBoundedPrefixBoundary
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
- **Definition:** A non-throwing, non-allocating predicate that measures the same completion and failure conditions as its paired operation.
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
- **Related:** LifecycleClassification, CandidateStatus
- **Usage:** Requirements, architecture, specification governance, and documentation
- **Examples:** Relational operations can be deferred until a complete relation model is approved.



### ExcludedStatus
- **Definition:** A lifecycle classification indicating a function is outside cljonic scope or incompatible with the project boundary and semantic model.
- **Deprecated Synonyms:** excluded, out-of-scope status
- **Related:** LifecycleClassification, CandidateStatus
- **Usage:** Requirements, architecture, specification governance, and documentation
- **Examples:** Runtime macro or reflection features can be excluded by boundary requirements.


### StandardViewType
- **Definition:** A read-only non-owning view type aligned with standard C++ view conventions and cljonic lifetime rules.
- **Deprecated Synonyms:** standard view, standard span-style view
- **Related:** NonOwningView, String
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `view(collection)` exposes a standard view type that cannot outlive its source.



### UnboundedProducer
- **Definition:** An explicit producer value with no finite caller-independent complete-result bound, requiring bounded materialization semantics.
- **Deprecated Synonyms:** infinite producer, open-ended producer
- **Related:** ProducerMaterialization, PreflightPredicate
- **Usage:** Requirements, specification, implementation, tests, and documentation
- **Examples:** Unbound forms of `range`, `repeat`, `cycle`, `iterate`, and `repeatedly` are unbounded producers until bounded by destination materialization.



### ProducerMaterialization
- **Definition:** The explicit process of turning a producer into an owning bounded result in a selected destination under complete-result preflight rules.
- **Deprecated Synonyms:** producer realization, producer into-materialization
- **Related:** UnboundedProducer, PreflightPredicate, ProducerIteration
- **Usage:** Requirements, specification, implementation, tests, and documentation
- **Examples:** `into(destination, producer)` plus `fits_into(destination, producer)` defines producer materialization completeness.



### ProducerIteration
- **Definition:** The bounded traversal of a producer's elements up to its effective size, as distinct from ProducerMaterialization, which realizes those elements into an owning bounded result.
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
- **Related:** ClosedNominalCollectionDomain, NominalCollectionRecognition
- **Usage:** Architecture, implementation, and tests
- **Examples:** A collection trait classifies an admitted type as vector, map, set, queue, or string.


### CollectionConcept
- **Definition:** A C++20 concept that gates a type on its cljonic nominal collection identity (admission to the ClosedNominalCollectionDomain), rather than on structural similarity to an external container.
- **Deprecated Synonyms:** nominal concept, public nominal concept
- **Related:** ClosedNominalCollectionDomain, NominalCollectionRecognition, CollectionKind, CapabilityConcept
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** `cljonic_collection<T>` is a CollectionConcept that depends on cljonic-owned trait admission.


### CapabilityConcept
- **Definition:** A C++20 concept that expresses a semantic capability a cljonic collection must expose (sequenceable, indexed, or associative observation) in order to participate in an operation, layered on top of nominal collection identity.
- **Deprecated Synonyms:** capability concept, semantic capability gate
- **Related:** CollectionConcept, Sequenceable, IndexedAccess, AssociativeAccess, CapabilityPredicate
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** `sequenceable_cljonic_collection<C>` is a CapabilityConcept requiring non-throwing `is_empty` and `count` observation.



### StaticInspectableStorage
- **Definition:** A storage property whose capacity, representation bound, and resource implications can be determined from a collection type or compile-time configuration.
- **Deprecated Synonyms:** static storage
- **Related:** CollectionMaximumElementCount, EmbeddedConstraint, NoHeapConstraint
- **Usage:** Requirements, architecture, implementation, tests, and documentation
- **Examples:** A fixed-capacity collection owns its buffer directly by value, allowing its capacity to be inspected from its type or configuration.



### PersistentValueSemantics
- **Definition:** The observable rule that an update returns a new independently valid value while leaving the prior value unchanged. This contract does not itself prescribe a storage algorithm.
- **Deprecated Synonyms:** persistent collection semantics, immutable update semantics
- **Related:** CopyOnModifyCollection, DeepCopyUpdate, OwningValue
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** Updating a Vector produces a new value while the original Vector remains valid and unchanged.



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
- **Related:** DeterministicBehavior, NoHiddenGlobalInitialization, OwningValue
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
- OwningValue, NonOwningView, and StandardViewType define ownership and lifetime semantics for values versus views.
- CompleteResult, BoundedPrefixResult, DefaultReturningResult, CheckedFailureResult, and PreflightPredicate define canonical result-status and completion semantics.
- LifecycleClassification with RequirementsBacked, CandidateStatus, DeferredStatus, and ExcludedStatus defines API-surface governance vocabulary.
- UnboundedProducer and ProducerMaterialization define explicit producer-to-result boundaries.
- RelationModel governs when relational operations can move from deferred to requirements-backed.
- StatePredicate and VerbPredicate define canonical predicate naming constraints.
- EmbeddedConstraint, StaticInspectableStorage, NoHeapConstraint, NoExceptionConstraint, NoRttiConstraint, NoHiddenGlobalInitialization, SingleThreadedExecutionModel, and DeterministicBehavior define the platform and execution constraints.
- ClosedNominalCollectionDomain, NominalCollectionRecognition, and CollectionKind define which types may participate as cljonic collections.
- PersistentValueSemantics defines the public value contract, while DeepCopyUpdate defines Module 1's required realization of that contract.
- HeaderOnlyDistribution and AmalgamatedHeader define the packaging vocabulary for build and user documentation.
- CompileTimeFailure and CapacityConstruction define observable construction behavior contracts that should map directly into Allium specs.