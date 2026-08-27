# cljonic Requirements - Module 3: Core Collection Types & Primitive Free Functions

## Purpose and Scope

This module defines the concrete, array-backed, bounded collection types (`Vector`, `Map`, `Set`, `Queue`, `String`), their primitive member and free-function operations, sequence traversal interfaces, and callable lookup forms. Module 3 provides the stored collection building blocks used across all higher-order algorithms.

## Collection Family Requirements

REQ-COLL-001. The supported collection family MUST include vector, map, set, queue, and string.

REQ-COLL-002. The library MUST provide a bounded vector with indexed lookup, indexed replacement, append, count, sequence conversion, traversal operations, and stack-style pop/peek behavior where applicable.

REQ-COLL-002A. A `Vector<T, N>` MUST be callable with `operator()(Index)` and `operator()(Index, T)` when `Index` and `T` satisfy the same capabilities required by bounded indexed lookup. The one-argument form MUST return the element at a valid index or `T{}` when the index is invalid. The two-argument form MUST return the element at a valid index or the supplied fallback value when the index is invalid. Neither form MUST mutate the vector, allocate, throw, or change vector order or traversal state. `valid_index(vector, index)` MUST remain the authoritative way to distinguish an invalid index from a valid index whose element equals `T{}`; `get(vector, index)` and `get(vector, index, fallback)` MUST remain behaviorally equivalent free-function forms. Negative indexes, when representable by the accepted index type, MUST be invalid.

REQ-COLL-004. The library MUST provide a bounded map with key/value association, lookup, association, removal, membership, count, sequence conversion, and traversal operations.

REQ-COLL-004A. Associating an existing map key MUST replace its associated value in the returned map without increasing the map count or requiring additional capacity. Associating a new key MUST add a key/value pair only when capacity is available.

REQ-COLL-004B. A `Map<K, V, N>` MUST be callable with `operator()(K)` and `operator()(K, V)` when `K` and `V` satisfy the same capabilities required by map lookup. The one-argument form MUST return the associated value for a present key or `V{}` when the key is absent. The two-argument form MUST return the associated value for a present key or the supplied fallback value when the key is absent. Neither form MUST mutate the map, insert a key, allocate, throw, or change traversal state. `contains(map, key)` MUST remain the authoritative way to distinguish a missing key from a present key whose value equals `V{}`; `get(map, key)` and `get(map, key, fallback)` MUST remain behaviorally equivalent free-function forms.

REQ-COLL-005. The library MUST provide a bounded set with membership, insertion, removal, count, sequence conversion, and traversal operations.

REQ-COLL-005A. Inserting a set value that is already present MUST be a successful no-op in the returned set, MUST preserve the set count, and MUST NOT require additional capacity.

REQ-COLL-005B. A `Set<T, N>` MUST be callable with `operator()(T)` and `operator()(T, T)` when `T` satisfies the stable equality capability required by set membership. The one-argument form MUST return the matching stored element for a present value or `T{}` when the value is absent. The two-argument form MUST return the matching stored element for a present value or the supplied fallback value when the value is absent. Neither form MUST mutate the set, insert a value, allocate, throw, reorder elements, or change traversal state. `contains(set, value)` MUST remain the authoritative boolean membership predicate and MUST distinguish an absent value from a present value equal to `T{}`; `get(set, value)` and `get(set, value, fallback)` MUST remain behaviorally equivalent free-function forms.

REQ-COLL-006. The library MUST provide a bounded FIFO queue with insertion at the rear, removal at the front, peek, count, sequence conversion, and traversal operations.

REQ-COLL-007. Collection capacity MUST be encoded in each collection type and bounded by a documented configuration-time limit.

REQ-COLL-008. `CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT` MUST supply the default and maximum permitted collection element count and MUST default to 1000 unless a later approved requirement changes it.

REQ-COLL-009. The public API MUST distinguish collection behavior from storage strategy. Users MUST be able to use the collection contracts without depending on a particular internal representation.

REQ-COLL-010. Map lookup, association, removal, membership, and traversal, and set membership, insertion, removal, and traversal MUST use bounded linear scans over their stored elements. Linear scanning MUST be the only search strategy in the supported collection family.

REQ-COLL-010A. Removal from unsorted `Map` and `Set` MUST use swap-and-remove after the target has been found: the final stored element, or final map key/value pair, MUST be copied into the removed position before the count is decremented. This optimization MUST preserve membership and association semantics while allowing implementation traversal order to change.

REQ-COLL-010B. The supported `Map`, `Set`, and `Queue` implementations MUST use flat contiguous bounded storage. A `Map` MUST store its active key/value pairs contiguously, a `Set` MUST store its active values contiguously, and a `Queue` MUST store its active logical elements contiguously. The implementation MUST preserve the collection semantics while maintaining these contiguous active ranges.

REQ-COLL-011. Map and set iteration order MUST be semantically unordered, consistent with their Clojure counterparts. Implementations MAY exhibit a repeatable order, but callers MUST NOT rely on any particular order.

REQ-COLL-012. The string MUST be a bounded, array-backed collection with ordered ASCII byte storage and a null terminator immediately after its content. The terminator MUST NOT count as an element, and string capacity MUST be measured in content bytes excluding the terminator.

REQ-COLL-013. A string MUST accept only ASCII bytes in the range `0x01` through `0x7F`. Embedded null bytes and bytes above `0x7F` MUST be handled according to the documented deterministic failure policy.

REQ-COLL-013A. The runtime `'.'` replacement policy for invalid bytes defined by `REQ-FN-027` MUST apply to external `std::string_view` imports only. Direct `String` construction from compile-time literals MUST reject invalid bytes at compile time. Any public runtime constructor or operation that accepts raw byte input MUST define an explicit checked-failure or replacement policy for invalid bytes; it MUST NOT inherit the `std::string_view` import policy implicitly. Operations that receive an existing `String` MAY assume that its stored content has already passed the string byte-validity rules and MUST operate only on valid stored bytes.

REQ-COLL-014. The public C++ string type MUST be named `cljonic::String<N>`, where `N` is the maximum content length in bytes and excludes the null terminator.

REQ-COLL-015. The API MUST support explicit-capacity string-literal construction such that `cljonic::String<N>{literal}` is valid only when the literal's content length is no greater than `N`.

REQ-COLL-016. An explicit-capacity string-literal construction whose literal content length exceeds `N` MUST fail at compile time.

REQ-COLL-017. The API MUST support capacity-inferred string-literal construction such that `cljonic::String{literal}` has the same semantics and type as `cljonic::String<content_length>{literal}`, where `content_length` excludes the literal's null terminator.

## Sequence Traversal Mechanics

REQ-SEQ-001. The library MUST define sequence as a traversal behavior over immutable values in the cljonic collection family, not as a separate owning collection type.

REQ-SEQ-002. A sequence MUST support determining emptiness, obtaining the first value, and obtaining the remainder.

REQ-SEQ-003. A collection that can be traversed as a sequence MUST expose a sequence conversion operation and the common traversal operations directly.

REQ-SEQ-004. `seq`, `first`, `next`, `rest`, and `count` MUST be available as generic free functions where the required traversal capability exists. Direct collection calls and calls on sequence values MUST be behaviorally equivalent where both apply.

REQ-SEQ-005. Sequence traversal MUST NOT mutate the source collection.

REQ-SEQ-006. Sequence traversal MUST be bounded or otherwise guaranteed not to allocate dynamically on supported embedded paths.

REQ-SEQ-007. The sequence API MUST support vector, map, set, queue, and string input from the cljonic collection family.

REQ-SEQ-008. The sequence API MUST NOT require support for arbitrary external arrays, ranges, iterables, or user-defined container types. Supported sequence inputs MUST belong to the cljonic collection family or be explicitly added by an approved requirement.

REQ-SEQ-009. Empty-sequence behavior MUST be consistent across all sequenceable collection types.

REQ-SEQ-010. `seq` MUST return an immutable, value-semantic bounded vector containing the sequence elements of its input.

REQ-SEQ-011. The vector returned by `seq` MUST be independently valid, use bounded automatic or static storage, and MUST NOT perform dynamic allocation or retain a dependency on the source collection.

REQ-SEQ-012. Each element produced by sequencing a map MUST be a value-semantic map-entry value containing exactly one key and its associated value. The map-entry value MUST be independently valid and MUST NOT retain a dependency on the source map.

REQ-SEQ-013. A map-entry value MUST be sequenceable as a fixed two-element sequence whose first element is the key and whose last element is the value. Its count MUST be two, and indexed access at zero and one MUST return the key and value respectively.

REQ-SEQ-014. `seq` applied to a map MUST return an owning bounded vector of map-entry values. The returned vector MUST preserve the map's traversal results without making the traversal order semantically significant.

## Primitive Free Functions & General Comparisons

REQ-FN-001. The primary user-facing operations MUST be free functions rather than requiring users to learn collection-specific member APIs.

REQ-FN-002. The supported free-function vocabulary MUST include at least the functions listed in the API Vocabulary Inventories, subject to each entry's lifecycle status and the capabilities of each input. Only entries classified as `requirements-backed` constitute supported behavior.

REQ-FN-002A. For a nonempty map, `first` MUST return one map-entry value. For a map-entry value, `first` MUST return its key, `last` MUST return its value, `key` MUST return its key, and `val` MUST return its value. These operations MUST compose so that `first(first(map))` returns the key and `last(first(map))` returns the value.

REQ-FN-002B. `last` applied to a map MAY return the final map-entry value in the implementation's traversal order, but callers MUST NOT rely on which entry is returned because ordinary map traversal order is semantically unordered.

REQ-FN-002C. The canonical named comparison functions MUST use full descriptive names: `equal`, `not_equal`, `less`, `less_equal`, `greater`, and `greater_equal`. Short aliases such as `eq`, `neq`, `lt`, `lte`, `gt`, or `gte` MUST NOT be required by the supported API.

REQ-FN-002D. Where the semantics and capabilities permit, binary comparisons SHOULD also be exposed through the corresponding native C++ operators `==`, `!=`, `<`, `<=`, `>`, and `>=`. Named functions MUST remain available for generic, constrained, or variadic use.

REQ-FN-002E. `equal` MUST represent general value equality, including recursively defined finite collection equality. Numeric equality MUST be a separately specified capability or operation and MUST NOT be inferred solely from the existence of general value equality.

REQ-FN-002F. Clojure's `=` MUST map conceptually to cljonic general equality, while Clojure's numeric `==` MUST map conceptually to a separately specified numeric-equality operation. The C++ spelling `=` MUST NOT be introduced as a cljonic function because it is assignment syntax.

REQ-FN-002M. `can_conj(collection, value)` MUST return true when `conj` can produce its documented result without capacity failure, including when a set already contains the value. `can_assoc(map, key, value)` MUST return true when the key already exists because `assoc` replaces its value without consuming capacity, and MUST return true for a new key only when capacity is available.

REQ-FN-002P. The callable `Map<K, V, N>` lookup forms specified by `REQ-COLL-004B` MUST be equivalent to the corresponding `get` overloads for the same map, key, value, and fallback arguments. `operator[]` MUST NOT be required or provided as the map lookup syntax because its conventional insertion semantics conflict with cljonic's immutable bounded-map contract.

REQ-FN-002Q. The callable `Vector<T, N>` lookup forms specified by `REQ-COLL-002A` MUST be equivalent to the corresponding `get` overloads for the same vector, index, element type, and fallback arguments. `valid_index(vector, index)` MUST be non-throwing, non-allocating, and consistent with both callable lookup forms and indexed access. `operator[]` MUST NOT be required or provided as the vector lookup syntax because its conventional unchecked-access semantics conflict with cljonic's bounds-checked contract.

REQ-FN-002R. The callable `Set<T, N>` lookup forms specified by `REQ-COLL-005B` MUST be equivalent to the corresponding `get` overloads for the same set, value, and fallback arguments. Set callable lookup MUST use the same stable equality capability and bounded linear scan as `contains`; it MUST NOT provide a boolean-returning `operator()` overload because `contains(set, value)` is the canonical membership predicate.

REQ-FN-003. Generic free functions MUST be constrained by explicit concepts or equivalent compile-time requirements.

REQ-FN-004. Unsupported operations MUST fail at compile time with useful diagnostics where the limitation is knowable from the types.

REQ-FN-005. Functional operations MUST preserve input values.

REQ-FN-006. `map`, `filter`, and similar transformations MUST have a documented capacity policy when the result can exceed its target capacity.

REQ-FN-007. Functions MUST be composable across compatible collection and sequence types.

REQ-FN-008. `map`, `filter`, `reduce`, and similar higher-order operations MUST preserve the purity and input-preservation guarantees of the library when provided callbacks that are themselves pure and non-allocating. The library MUST NOT introduce side effects or hidden mutable state independently of those callbacks.

REQ-FN-026. The core vocabulary MUST include `empty`, `is_empty`, and `not_empty` with the semantics defined by `REQ-BOUNDS-010A`. `empty` MUST produce an empty owning value of the same supported collection type as its input. `is_empty` MUST return a boolean predicate result. `not_empty` MUST preserve the input collection type and capacity and MUST return an owning copy of the input when nonempty or the corresponding empty value when empty. These operations MUST preserve their inputs and require no dynamic allocation or exceptions.

## Traceability and Related Requirements

- **Downstream Artifact**: `Vector`, `MapEntry`, `Map`, `Set`, `Queue`, `String` class templates and core collection free functions (`count`, `get`, `conj`, `assoc`, `dissoc`, `disj`, `peek`, `pop`, `first`, `next`, `rest`, `seq`).
- **Governed REQs**: `REQ-COLL-001`–`017`, `REQ-SEQ-001`–`014`, `REQ-FN-001`–`008`, `REQ-FN-026`.
