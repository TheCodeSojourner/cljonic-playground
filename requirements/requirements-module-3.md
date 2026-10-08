# cljonic Requirements - Module 3: Core Collection Types & Primitive Free Functions

## Purpose and Scope

This module defines the concrete, array-backed, bounded collection types (`Vector`, `Map`, `Set`, `Queue`, `String`), their current primitive member and free-function operations, and callable lookup forms. Sequence traversal interfaces are approved future work and are not part of the current collection API. Module 3 provides the stored collection building blocks used across all higher-order algorithms.

## Collection Family Requirements

λ REQ-COLL-001(x).
  ∀ supported_collection_family: includes vector, map, set, queue, and string
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-001A(x).
  ∀ (vector elements ∧ set elements ∧ queue elements ∧ map keys ∧ map values ∧ map-entry fields): satisfy the `NothrowCollectionElement` storage contract defined by `REQ-VAL-007A`
  ∧ ∀ String storage: uses its separately defined bounded ASCII-byte representation ∧ preserves the same non-throwing storage and destruction guarantees
  ∧ ∀ `MapEntry<K, V>` key field: additionally satisfies the stable equality capability required for map key admission, whether the `MapEntry` is embedded in a `Map` or instantiated standalone
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-002(x).
  ∀ context: the library provides a bounded vector with indexed lookup, indexed replacement, append, count, and stack-style pop/peek behavior where applicable
  ∧ ∀ (sequence conversion ∧ traversal): deferred future capabilities
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-002A(x).
  ∀ `Vector<T, N>` where `Index` ∧ `T` satisfy the same capabilities required by bounded indexed lookup: callable with `operator()(Index, fallback)`; `fallback` defaults to `T{}` when omitted
  ∧ ∀ operation: returns the element at a valid index or the fallback value when the index is invalid ∧ ¬∃ (mutation of the vector ∨ allocation ∨ throw ∨ change of vector order ∨ traversal state)
  ∧ ∀ context: `contains(vector, index)` remains the authoritative way to distinguish an invalid index from a valid index whose element equals `T{}` ∧ `get(vector, index)` and `get(vector, index, fallback)` remain behaviorally equivalent free-function forms
  ∧ ∀ negative_index (representable by the accepted index type): invalid
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-004(x).
  ∀ context: the library provides a bounded map with key/value association, lookup, association, removal, membership, and count
  ∧ ∀ (sequence conversion ∧ traversal): deferred future capabilities
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-004A(x).
  ∀ association of an existing map key: replaces its associated value in the returned map without increasing the map count or requiring additional capacity
  ∧ ∀ association of a new key: adds a key/value pair only when capacity is available
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-004B(x).
  ∀ `Map<K, V, N>` where `K` ∧ `V` satisfy the same capabilities required by map lookup: callable with `operator()(K, fallback)`; `fallback` defaults to `V{}` when omitted
  ∧ ∀ operation: returns the associated value for a present key or the fallback value when the key is absent ∧ ¬∃ (mutation of the map ∨ key_insertion ∨ allocation ∨ throw ∨ change of traversal state)
  ∧ ∀ context: `contains(map, key)` remains the authoritative way to distinguish a missing key from a present key whose value equals `V{}` ∧ `get(map, key)` and `get(map, key, fallback)` remain behaviorally equivalent free-function forms
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-004C(x).
  ∀ `Map<K, V, N>`: admits both `K` and `V` only when each satisfies `NothrowCollectionElement`
  ∧ ∀ `K`: additionally satisfies the stable equality capability required for key lookup
  ∧ ∀ storage_admission_requirement: enforced at the Map template boundary ∧ independent of operation-specific capabilities beyond key equality
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-005(x).
  ∀ context: the library provides a bounded set with membership, insertion, removal, and count
  ∧ ∀ (sequence conversion ∧ traversal): deferred future capabilities
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-005A(x).
  ∀ insertion of a set value already present: successful no-op in the returned set ∧ preserves the set count ∧ ¬∃ requirement of additional capacity
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-005B(x).
  ∀ `Set<T, N>` where `T` satisfies the stable equality capability required by set membership: callable with `operator()(T, fallback)`; `fallback` defaults to `T{}` when omitted
  ∧ ∀ operation: returns the matching stored element for a present value or the fallback value when the value is absent ∧ ¬∃ (mutation of the set ∨ value_insertion ∨ allocation ∨ throw ∨ reordering ∨ change of traversal state)
  ∧ ∀ context: `contains(set, value)` remains the authoritative boolean membership predicate ∧ distinguishes an absent value from a present value equal to `T{}` ∧ `get(set, value)` and `get(set, value, fallback)` remain behaviorally equivalent free-function forms
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-006(x).
  ∀ context: the library provides a bounded FIFO queue with insertion at the rear, removal at the front, peek, and count
  ∧ ∀ (sequence conversion ∧ traversal): deferred future capabilities
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-007(x).
  ∀ collection_capacity: encoded in each collection type ∧ bounded by a documented configuration-time limit
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-007A(x).
  ∃ permitted_path: collection capacity is zero
  ∧ ∀ zero_capacity_collection: valid empty owning value ∧ reports `count() == 0` ∧ `is_empty(collection) == true` ∧ returns documented default or fallback values for access ∧ preserves its value when an insertion operation cannot add an element
  ∧ ∀ future traversal_work: supports empty const traversal without accessing element storage, allocating, throwing, or invoking capacity-dependent arithmetic with zero as a divisor
  ∧ ∀ zero_capacity_collection: fails any compile-time construction that would require stored elements
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-008(x).
  ∀ context: `CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT` supplies the default and maximum permitted collection element count ∧ defaults to 1000 unless a later approved requirement changes it
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-009(x).
  ∀ public_API: distinguishes collection behavior from storage strategy ∧ ∀ user: able to use the collection contracts without depending on a particular internal representation
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-010(x).
  ∀ (map lookup ∧ association ∧ removal ∧ set membership ∧ insertion ∧ removal): bounded linear scans over stored elements
  ∧ ∃ permitted_path: future traversal work uses the same bounded storage inspection without changing the only-search-strategy rule for lookup and membership
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-010A(x).
  ∀ removal from unsorted `Map` ∧ `Set`: uses swap-and-remove after the target has been found — the final stored element, or final map key/value pair, is copied into the removed position before the count is decremented
  ∧ ∀ optimization: preserves membership and association semantics while allowing implementation traversal order to change
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-010B(x).
  ∀ supported `Map` ∧ `Set` ∧ `Queue` implementation: flat bounded array-backed storage
  ∧ ∀ `Map`: stores its active key/value pairs within its bounded array ∧ ∀ `Set`: stores its active values within its bounded array
  ∧ ∀ `Queue`: MAY use circular physical storage; its const traversal and any interoperability accessor expose active elements in logical FIFO order
  ∧ ∀ implementation: preserves collection semantics while maintaining bounded storage and a defined logical traversal range
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-011(x).
  ∀ (map ∧ set) iteration order: semantically unordered, consistent with their Clojure counterparts
  ∧ ∃ permitted_path: implementations exhibit a repeatable order ∧ ∀ caller: ¬∃ reliance on any particular order
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-012(x).
  ∀ string: bounded, array-backed collection with ordered ASCII byte storage and a null terminator immediately after its content
  ∧ ∀ terminator: ¬∃ counting as an element ∧ ∀ string_capacity: measured in content bytes excluding the terminator
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-012A(x).
  ∀ `String<N>`: callable with `operator()(Index, fallback)` for indexed lookup, where `Index` is any integral type and `fallback` defaults to `char{}` when omitted
  ∧ ∀ operation: returns the stored ASCII byte at a valid content index or the fallback character when the index is invalid
  ∧ ∀ (negative ∧ unrepresentable index): invalid rather than converted modulo to the unsigned key domain
  ∧ ∀ operation: ¬∃ (exposure of the null terminator as a content element ∨ mutation ∨ allocation ∨ throw)
  ∧ ∀ context: `contains(string, index)` remains the authoritative way to distinguish an invalid index from a valid index whose byte equals `char{}`
  rationale: modulo conversion would silently map negative indexes onto valid tail positions, hiding caller errors
  {source: stakeholder_decided, decided_by: original_spec_author, rationale_source: origin_artifact}

λ REQ-COLL-013(x).
  ∀ string: accepts only ASCII bytes in the range `0x01` through `0x7F`
  ∧ ∀ (embedded null byte ∧ byte above `0x7F`): handled according to the documented deterministic failure policy
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-013A(x).
  ∀ runtime `'.'` replacement policy for invalid bytes defined by `REQ-FN-027`: applies to external `std::string_view` and `std::span<const char>` imports
  ∧ ∀ direct String construction from compile-time literals: rejects invalid bytes at compile time
  ∧ ∀ public runtime constructor ∨ operation accepting raw byte input: defines an explicit checked-failure or replacement policy for invalid bytes ∧ ¬∃ implicit inheritance of the external-source import policy
  ∧ ∀ operation receiving an existing `String`: MAY assume that its stored content has already passed the string byte-validity rules ∧ operates only on valid stored bytes
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-013B(x).
  ∀ runtime `String<N>` construction from a raw character array: replaces each embedded null byte or byte above `0x7F` with the ASCII period character `'.'`
  ∧ ∀ runtime_replacement: preserves the input content length ∧ remains non-throwing and non-allocating ∧ ¬∃ alteration of the compile-time rejection policy for constant-evaluated construction
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-014(x).
  ∀ public C++ string type: named `cljonic::String<N>`, where `N` is the maximum content length in bytes and excludes the null terminator
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-015(x).
  ∀ context: the API supports explicit-capacity string-literal construction such that `cljonic::String<N>{literal}` is valid only when the literal's content length is no greater than `N`
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-016(x).
  ∀ explicit_capacity_string_literal_construction whose literal content length exceeds `N`: fails at compile time
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-017(x).
  ∀ context: the API supports capacity-inferred string-literal construction such that `cljonic::String{literal}` has the same semantics and type as `cljonic::String<content_length>{literal}`, where `content_length` excludes the literal's null terminator
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-018(x).
  ∀ context: the API supports pack-literal construction for `Map<K, V, N>` such that `cljonic::Map<K, V, N>{entries...}` folds `assoc` over one or more arguments, each of exact type `MapEntry<K, V>`, in argument order; no other argument type is accepted, whether or not it is implicitly convertible to `MapEntry<K, V>`
  ∧ ∀ later_argument whose key matches an earlier argument's key: replaces the earlier argument's value, consistent with `REQ-COLL-004A`
  ∧ ∀ pack_literal_construction: requires at least one argument ∧ the zero-argument empty map remains constructible only through ordinary default construction (`REQ-COLL-004`), which is not considered pack-literal construction
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-018A(x).
  ∀ `Map<K, V, N>` pack_literal_construction whose argument count exceeds `N`: fails at compile time, regardless of whether duplicate keys would have produced a smaller final count
  ∧ ∀ context: the API also supports capacity-inferred pack-literal construction such that `cljonic::Map{entries...}` has the same semantics and type as the explicitly sized form instantiated with the argument count
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-019(x).
  ∀ context: the API supports pack-literal construction for `Set<T, N>` such that runtime `cljonic::Set<T, N>{values...}` is equivalent to default-constructing an empty set and folding `conj` over each `T`-constructible argument in argument order, consistent with the no-op-on-duplicate semantics of `REQ-COLL-005A`
  ∧ ∀ duplicate_argument: produces one stored copy at runtime
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-019A(x).
  ∀ `Set<T, N>` pack_literal_construction whose argument count exceeds `N`: fails at compile time, regardless of whether duplicate values would have produced a smaller final count
  ∧ ∀ context: the API also supports capacity-inferred pack-literal construction such that `cljonic::Set{values...}` has the same semantics and type as the explicitly sized form instantiated with the argument count
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-019B(x).
  ∀ `Set<T, N>{values...}` construction evaluated as a constant expression where two arguments compare equal: fails at compile time
  ∧ ∀ compile_time_duplicate_rejection: preserves the runtime no-op-on-duplicate behavior required by `REQ-COLL-019` ∧ ¬∃ requirement of a runtime exception
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-020(x).
  ∀ context: the API supports pack-literal construction for `Queue<T, N>` such that `cljonic::Queue<T, N>{values...}` is equivalent to default-constructing an empty queue and folding `conj` over each `T`-constructible argument in argument order, producing FIFO order matching argument order
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-020A(x).
  ∀ `Queue<T, N>` pack_literal_construction whose argument count exceeds `N`: fails at compile time
  ∧ ∀ context: the API also supports capacity-inferred pack-literal construction such that `cljonic::Queue{values...}` has the same semantics and type as the explicitly sized form instantiated with the argument count
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-020B(x).
  ∀ context: the library models collection behavior using the Clojure-style capability definitions in `REQ-CAP-001` through `REQ-CAP-009` and `REQ-CAP-013`, including `Indexed`, `Lookup`, `Seqable`, `Associative`, and `Conjable`, while preserving the bounded, non-allocating, value-semantic implementation model
  ∧ ∀ `Seqable` capability ∧ its traversal operations: deferred according to the lifecycle rules in this module
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-020C(x).
  ∀ `Vector<T, N>`: satisfies the `Indexed`, `Lookup`, and Clojure-style `Associative` capability semantics under the supported bounded model ∧ satisfies `Seqable` when the deferred sequence capability is implemented for vectors
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-020D(x).
  ∀ `Map<K, V, N>`: satisfies the `Associative` and `Lookup` capability semantics under the supported bounded model ∧ satisfies `Seqable` when the deferred sequence capability is implemented for maps
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-020E(x).
  ∀ `Set<T, N>`: satisfies the `Lookup` capability for value-membership lookup, where a present value is the associated lookup result and an absent value follows the documented default or fallback policy
  ∧ ∀ `Queue<T, N>`: ¬∃ requirement to satisfy `Indexed` ∨ `Lookup` ∨ `Associative` ∧ exposes only the sequence and queue operations approved for its lifecycle
  ∧ ∀ both types: satisfy `Seqable` when the deferred sequence capability is implemented for them
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-020F(x).
  ∀ free_function: constrained by the capability required by the operation — `assoc` and `can_assoc` valid only for collections satisfying `Associative`, `get` only for collections satisfying `Lookup`, and `seq` only for collections satisfying `Seqable`
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-020G(x).
  ∀ context: `assoc` is defined for `Map`, `Vector`, and `String`
  ∧ ∀ `Map`: associates a key with a value ∧ replaces the existing value when the key already exists
  ∧ ∀ `Vector`: the key is an integer index ∧ the value is the vector's element type
  ∧ ∀ `String`: the key is an integer content index ∧ the value is a character subject to the String character-validity policy defined by `REQ-COLL-013` through `REQ-COLL-013B` and `REQ-COLL-020Q`
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-020H(x).
  ∀ `Vector<T, N>` ∧ `assoc(v, i, value)` at an existing logical index `i`: produces a new vector whose value at `i` is replaced with `value` ∧ the source vector remains unchanged ∧ all other logical elements retain their values and order
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-020I(x).
  ∀ `Vector<T, N>` ∧ `assoc(v, i, value)` where `i` equals the source vector's logical count ∧ the source vector has available capacity: appends `value` ∧ the resulting vector's logical count increases by one
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-020J(x).
  ∀ `Vector<T, N>` ∧ `assoc` index: valid only when it is non-negative, no greater than the source vector's logical count, and either less than the logical count or equal to the logical count when capacity remains
  ∧ ∀ invalid_index (including an index beyond the append position ∨ an append index at full capacity): produces an unchanged vector without throwing, allocating, or mutating the source
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-020K(x).
  ∀ collection_interface_semantics defined above: consistent with the bounded, non-throwing, non-allocating storage model defined elsewhere in this module and in the architecture requirements
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-020L(x).
  ∀ `Associative` collection ∧ `can_assoc(collection, key)`: determines whether `assoc(collection, key, value)` can produce its documented result without capacity or key-domain failure, independently of the value argument
  ∧ ∀ `Vector<T, N>`: returns true for an existing index, for the logical-count append index when capacity remains, and false for every invalid or full-capacity append index
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-020M(x).
  ∀ `String<N>` ∧ `assoc(s, i, value)` at an existing logical content index `i`: produces a new string whose ASCII character at `i` is replaced with `value` ∧ the source string remains unchanged ∧ all other content characters retain their values and order ∧ the null terminator remains immediately after the resulting content
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-020N(x).
  ∀ `String<N>` ∧ `assoc(s, i, value)` where `i` equals the source string's logical content count ∧ the source string has available content capacity: appends `value` ∧ the resulting string's logical content count increases by one ∧ its null terminator remains immediately after the new content
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-020O(x).
  ∀ `String<N>` ∧ `assoc` index of any integral type: valid only when it is representable by the normalized `std::size_t` key domain, non-negative, no greater than the source string's logical content count, and either less than the logical content count or equal to the logical content count when content capacity remains
  ∧ ∀ invalid_index (including a negative ∨ unrepresentable index ∨ an index beyond the append position ∨ an append index at full content capacity): produces an unchanged string without throwing, allocating, or mutating the source
  ∧ ∀ null_terminator: ¬∃ valid association ∨ lookup index
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-020P(x).
  ∀ `String<N>` ∧ `can_assoc(string, index)`: accepts any integral index type ∧ normalizes only indexes representable by the `std::size_t` key domain ∧ returns true for an existing content index or the logical content-count append index when content capacity remains
  ∧ ∀ (negative ∧ unrepresentable ∧ invalid ∧ full-capacity append index): returns false
  ∧ ∀ result: independent of the character value that would be associated
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-020Q(x).
  ∀ `String<N>`: String owns one character-validity policy for every operation that stores content
  ∧ ∀ invalid_character (NUL ∨ non-ASCII): a String content update whose invalid character is known during constant evaluation fails at compile time
  ∧ ∀ runtime String content update whose character is invalid: stores `'.'` in place of that character ∧ the replacement preserves the operation's documented index, capacity, immutability, and null-terminator semantics
  ∧ ∀ `String::assoc` ∧ `String::conj`: applies the String-owned character policy
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-020R(x).
  ∀ context: `get(const C&, const K&)` and `get(const C&, const K&, const R&)` are supported for `Lookup` or `Indexed` collections and return the same collection-specific result type as the corresponding callable lookup operation
  ∧ ∀ supplied fallback `R`: has the same type as that collection-specific lookup result
  ∧ ∀ `Map<K, V, N>`: the lookup result and `get` return type are the mapped value `V`, not `MapEntry<K, V>`
  ∧ ∀ `Set<T, N>` ∨ `Vector<T, N>`: the lookup result and `get` return type are the stored element type `T`
  ∧ ∀ `String<N>`: the lookup result and `get` return type are `char`
  ∧ ∀ context: the other supported generic free-function signatures are `contains(const C&, const K&) -> bool` for `Lookup` or `Indexed` collections; `assoc(const C&, const K&, const C::association_value_type&) -> C` and `can_assoc(const C&, const K&) -> bool` for `Associative` collections
  ∧ ∀ operation: `constexpr` ∧ `noexcept` ∧ non-mutating ∧ non-allocating ∧ constrained at the public API boundary by the required capability and collection-specific key/value domains
  ∧ ∀ (`Map` ∨ `Set`): `contains` accepts only `K` exactly matching the collection's declared `lookup_type` after removing cv-qualification and references; no implicit conversion to that type is admitted
  ∧ ∀ (`Vector` ∨ `String`): `contains` accepts an integral index type
  ∧ ∀ `Range`: `contains(const Range&, const K&)` is not a supported free-function signature; `Range::contains(index)` remains the bounded-observation member predicate specified by `REQ-FN-013A`
  ∧ ∀ `String`: the `assoc` value argument is `char`
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-020S(x).
  ∀ context: the lifecycle classification is `requirements-backed` for `Indexed`, `Lookup`, `Associative`, and `Conjable`, and for `count`, `get`, `contains`, `conj`, `assoc`, `can_assoc`, `dissoc`, `disj`, `peek`, and `pop` over their supported collection inputs
  ∧ ∀ (`Seqable` ∧ `seq` ∧ `first` ∧ `next` ∧ `rest`): remain `deferred` until the sequence requirements are implemented for the applicable collection inputs
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-020T(x).
  ∀ context: the lifecycle classification is also `requirements-backed` for `is_empty` over its supported sequenceable collection and producer inputs, `can_conj`, and the `full` predicate or equivalent capacity inspection over their supported bounded collection inputs
  ∧ ∀ (`empty` ∧ `not_empty`): remain `deferred` until their owning-value behavior is implementation-backed
  ∧ ∀ behavioral contracts for `into` ∧ `fits_into`: approved by Module 2 and Module 4 ∧ both operations remain `deferred` until producer and materialization support is implementation-backed
  {source: stakeholder_decided, decided_by: original_spec_author}

## Collection Equality

λ REQ-COLL-021(x).
  ∀ bounded_owning_collection_value where element, key, and value types satisfy the stable equality capability required by `REQ-NUM-007` and `REQ-CAP-010`: supports value equality through the native C++ `==` operator
  ∧ ∀ (`Vector` ∧ `String` ∧ `Queue`) equality: order-sensitive — two values are equal exactly when their logical counts are equal and their logical elements compare equal in logical order
  ∧ ∀ (`Map` ∧ `Set`) equality: order-insensitive — two values are equal exactly when their logical counts are equal and every logical entry or element of one value has a matching logical entry or element in the other under stable key/element equality, independent of storage or insertion order
  ∧ ∀ collection_equality: `constexpr` ∧ `noexcept` ∧ non-mutating ∧ non-allocating ∧ ¬∃ traversal beyond the collection's bounded logical count ∧ recursively defined so that nested owning collection values, bounded producer values, and composite variant values compare according to the same component-wise stable-equality rule
  ∧ ∀ collection whose element, key, or value type does not admit stable equality: ¬∃ provision of `==` ∧ fails at compile time when used in an equality position
  ∧ ∀ collection_equality: ¬∃ implication of Clojure cross-type numeric unification ∨ hash-based equality
  {source: stakeholder_decided, decided_by: original_spec_author}

## Deferred Sequence Traversal Mechanics

> **Deferred future work — non-binding.** The sequence traversal contracts below are approved future-work behavior, not current collection APIs. No supported collection currently exposes the semantic sequence operations `seq`, `first`, `next`, or `rest`. These contracts MUST remain deferred until a later increment separately propagates their implementation and tests for every collection family. Their presence here records the intended future behavior without making it implementation-ready now.

λ REQ-SEQ-001(x).
  ∀ context: the library defines sequence as a traversal behavior over immutable values in the cljonic collection family, not as a separate owning collection type
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-SEQ-002(x).
  ∀ sequence: supports determining emptiness ∧ obtaining the first value ∧ obtaining the remainder
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-SEQ-002A(x).
  ∀ (`next` ∧ `rest`): operate on the collection's documented logical traversal order
  ∧ ∀ nonempty_sequence: both operations produce an owning bounded result containing every logical element after the first ∧ ∀ empty_sequence: both operations produce the documented empty owning result
  ∧ ∀ (`next` ∧ `rest`): ¬∃ derivation of semantics from `pop()` because `pop()` MAY remove a different logical end for different collection kinds, including the last element of a vector and the first element of a queue
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-SEQ-002B(x).
  ∀ (`next` ∧ `rest`): preserve the source collection ∧ preserve the relative logical order of all retained elements ∧ apply the same element representation and capacity policy as `seq`
  ∧ ∀ maps: retained elements are value-semantic `MapEntry` values ∧ ∀ (sets ∧ maps): implementation traversal order MAY be repeatable but remains semantically unordered ∧ ∀ queues: retained elements remain in FIFO order
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-SEQ-003(x).
  ∀ collection that can be traversed as a sequence: exposes a sequence conversion operation ∧ the common traversal operations directly
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-SEQ-004(x).
  ∀ context: `seq`, `first`, `next`, `rest`, and `count` are available as generic free functions where the required traversal capability exists
  ∧ ∀ (direct collection calls ∧ calls on sequence values) where both apply: behaviorally equivalent
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-SEQ-005(x).
  ∀ sequence_traversal: ¬∃ mutation of the source collection
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-SEQ-006(x).
  ∀ sequence_traversal: bounded ∨ otherwise guaranteed not to allocate dynamically on supported embedded paths
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-SEQ-007(x).
  ∀ sequence_API: supports vector, map, set, queue, and string input from the cljonic collection family
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-SEQ-008(x).
  ∀ sequence_API: ¬∃ requirement of support for arbitrary external arrays, ranges, iterables, or user-defined container types
  ∧ ∀ supported_sequence_input: belongs to the cljonic collection family ∨ is explicitly added by an approved requirement
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-SEQ-009(x).
  ∀ sequenceable_collection_type: empty-sequence behavior consistent
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-SEQ-010(x).
  ∀ `seq` application: returns an immutable, value-semantic bounded vector containing the sequence elements of its input
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-SEQ-011(x).
  ∀ vector returned by `seq`: independently valid ∧ uses bounded automatic or static storage ∧ ¬∃ (dynamic_allocation ∨ retained dependency on the source collection)
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-SEQ-012(x).
  ∀ element produced by sequencing a map: value-semantic map-entry value containing exactly one key and its associated value ∧ independently valid ∧ ¬∃ retained dependency on the source map
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-SEQ-013(x).
  ∀ map_entry_value: sequenceable as a fixed two-element sequence whose first element is the key and whose last element is the value ∧ count is two ∧ indexed access at zero and one returns the key and value respectively
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-SEQ-014(x).
  ∀ `seq` applied to a map: returns an owning bounded vector of map-entry values ∧ the returned vector preserves the map's traversal results without making the traversal order semantically significant
  {source: stakeholder_decided, decided_by: original_spec_author}

## Primitive Free Functions & General Comparisons

λ REQ-FN-001(x).
  ∀ context: the primary user-facing operations are free functions rather than requiring users to learn collection-specific member APIs
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-FN-002(x).
  ∀ context: the supported free-function vocabulary includes at least the functions listed in the API Vocabulary Inventories, subject to each entry's lifecycle status and the capabilities of each input
  ∧ ∀ entry: only entries classified as `requirements-backed` constitute supported behavior
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-FN-002A(x).
  ∀ nonempty_map: `first` returns one map-entry value
  ∧ ∀ map_entry_value: `first` returns its key ∧ `last` returns its value ∧ `key` returns its key ∧ `val` returns its value
  ∧ ∀ composition: operations compose so that `first(first(map))` returns the key and `last(first(map))` returns the value
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-FN-002B(x).
  ∀ `last` applied to a map: MAY return the final map-entry value in the implementation's traversal order ∧ ∀ caller: ¬∃ reliance on which entry is returned because ordinary map traversal order is semantically unordered
  rationale: map traversal order is semantically unordered, so no specific entry can be a stable result
  {source: stakeholder_decided, decided_by: original_spec_author, rationale_source: origin_artifact}

λ REQ-FN-002C(x).
  ∀ canonical named comparison function ∈ {`equal`, `not_equal`, `less`, `less_equal`, `greater`, `greater_equal`}: uses full descriptive names
  ∧ ∀ short_alias ∈ {`eq`, `neq`, `lt`, `lte`, `gt`, `gte`}: ¬∃ requirement by the supported API
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-FN-002D(x).
  ∃ permitted_path: where the semantics and capabilities permit, binary comparisons also exposed through the corresponding native C++ operators `==`, `!=`, `<`, `<=`, `>`, and `>=`
  ∧ ∀ named_function: remains available for generic, constrained, or variadic use
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-FN-002E(x).
  ∀ context: `equal` represents general value equality, including recursively defined finite collection equality
  ∧ ∀ numeric_equality: separately specified capability or operation ∧ ¬∃ inference solely from the existence of general value equality
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-FN-002F(x).
  ∀ context: Clojure's `=` maps conceptually to cljonic general equality ∧ Clojure's numeric `==` maps conceptually to a separately specified numeric-equality operation
  ∧ ∀ C++ spelling `=`: ¬∃ introduction as a cljonic function because it is assignment syntax
  rationale: `=` is C++ assignment syntax, so a function by that name would be actively misleading
  {source: stakeholder_decided, decided_by: original_spec_author, rationale_source: origin_artifact}

λ REQ-FN-002G(x).
  ∀ context: `equal` implements general value equality over the supported stable-equality domain as defined by `REQ-FN-002E`, in the three arities of Clojure's `=`
  ∧ ∀ `equal(x)`: returns true for a single operand admitted by the compile-time domain gating below
  ∧ ∀ `equal(a, b)`: compares the two operands by the family rules below
  ∧ ∀ `equal(a, b, more...)`: holds exactly when every adjacent operand pair compares equal by those same family rules, evaluated left to right and short-circuited at the first unequal pair
  ∧ ∀ adjacent pair in the variadic form ∧ the single unary operand: individually satisfies the compile-time domain gating, so an unsupported or mixed pair fails compilation regardless of position in the argument list
  ∧ ∀ `equal` in every arity: `constexpr` ∧ `noexcept` ∧ non-mutating ∧ non-allocating
  ∧ ∀ operand ∧ ∀ stored component at any depth: its equality comparison is non-throwing, so the `noexcept` guarantee cannot be violated; a value whose comparison may throw (including an aggregate-like struct whose `operator==` is not `noexcept`) is outside the supported domain and fails at compile time
  ∧ ∀ context: `equal` is a named free-function operation distinct from the native `==` operators defined by `REQ-COLL-021` and `REQ-FN-014B`
  ∧ ∀ non_collection ∧ non_producer value: the fallthrough domain is the closed value domain — arithmetic scalars, scoped enumerations, aggregate-like structs with a non-throwing `operator==`, and `cljonic::Variant` composites of those; unscoped enumerations and pointer types (including pointers to functions and pointers to member functions) are outside the fallthrough domain and rejected at compile time
  ∧ ∀ floating_point_value at any depth (including nested inside any operand in any arity): rejected at compile time per `REQ-NUM-006`
  ∧ ∀ standard_library range ∨ container type at any depth (as an operand, as a stored component of a cljonic collection or producer, or as a `cljonic::Variant` alternative): rejected at compile time because `equal` is not part of the C++ interoperability surface
  ∧ ∀ standard_library `std::variant` (as an operand, stored component, or alternative): rejected at compile time because it is not a supported cljonic value type (`REQ-CAP-011`)
  ∧ ∀ both operands cljonic collections ∨ producers: equality classified by equality family — Vector, Queue, Cycle, Iterate, Range, Repeat, and Repeatedly form one sequential family (`SequentialEquality`) whose members are comparable by produced sequence (same elements in the same order, compared with identical element types) regardless of their concrete types; Map comparable only to Map; Set only to Set; String only to String
  ∧ ∀ (pairs belonging to incompatible families ∨ pairs mixing a cljonic value with a non-cljonic value ∨ operands outside the supported stable-equality domain): fail at compile time
  ∧ ∀ sequential_comparison: order-sensitive ∧ compares elements lazily element by element without eagerly materializing unbounded producers ∧ terminates at the first unequal element pair or the shorter exhausted logical count
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-FN-002H(x).
  ∀ context: `not_equal` implements the canonical negation of general value equality as the counterpart of `equal` (`REQ-FN-002G`), in the three arities of Clojure's `not=`
  ∧ ∀ `not_equal(x)`: returns false for a single operand admitted by the same compile-time domain gating as `equal`
  ∧ ∀ `not_equal(a, b)`: holds exactly when `equal(a, b)` is false
  ∧ ∀ `not_equal(a, b, more...)`: holds exactly when at least one adjacent operand pair does not compare equal ∧ is the logical negation of the conjunctive `equal` variadic form
  ∧ ∀ adjacent pair in the variadic form ∧ the single unary operand: individually satisfies the same compile-time domain gating as `equal`, so an unsupported or mixed pair fails compilation regardless of position in the argument list
  ∧ ∀ context: `not_equal` is a named free-function operation distinct from the native `!=` operators defined by `REQ-COLL-021` and `REQ-FN-014B`
  ∧ ∀ floating_point_value at any depth: rejected at compile time per `REQ-NUM-006` ∧ ¬∃ cross-type numeric unification under `REQ-NUM-002`, `REQ-NUM-006`, or the pinned `NumericEquality` vocabulary rule
  ∧ ∀ `not_equal`: `constexpr` ∧ `noexcept` ∧ non-mutating ∧ non-allocating ∧ its supported domain, equality-family classification, recursion over nested values, and termination behavior are identical to `equal` (`REQ-FN-002G`) because it delegates to `equal` and negates the result
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-FN-002M(x).
  ∀ `can_conj(collection, value)` defined for every `Conjable` collection (`Vector`, `Set`, `Map`, `Queue`, `String`): returns true when `conj` can produce its documented result without capacity failure
  ∧ ∀ (`Set` ∧ `Map`): an element or key already present returns true — insertion is a no-op or a replacement that needs no capacity — ∧ a new element or key returns true only when capacity remains
  ∧ ∀ (`Vector` ∧ `Queue`): returns true only when capacity remains; the value argument does not affect the outcome
  ∧ ∀ `String`: returns true only when capacity remains and the character is valid (non-NUL ASCII) ∧ returns false for an invalid character even when capacity remains ∧ `conj` still applies the String-owned runtime replacement policy (`REQ-COLL-020Q`)
  rationale: a `Set` element or `Map` key already present makes `conj` a successful insertion without requiring capacity
  {source: stakeholder_decided, decided_by: original_spec_author, rationale_source: origin_artifact}

λ REQ-FN-002P(x).
  ∀ callable `Map<K, V, N>` lookup form specified by `REQ-COLL-004B`: equivalent to the corresponding `get` overloads for the same map, key, value, and fallback arguments
  ∧ ∀ context: `operator[]` ¬∃ required ∨ provided as the map lookup syntax because its conventional insertion semantics conflict with cljonic's immutable bounded-map contract
  rationale: `operator[]` insertion semantics conflict with the immutable bounded-map contract
  {source: stakeholder_decided, decided_by: original_spec_author, rationale_source: origin_artifact}

λ REQ-FN-002Q(x).
  ∀ callable `Vector<T, N>` lookup form specified by `REQ-COLL-002A`: equivalent to the corresponding `get` overloads for the same vector, index, element type, and fallback arguments
  ∧ ∀ `contains(vector, index)`: non-throwing ∧ non-allocating ∧ consistent with both callable lookup forms and indexed access
  ∧ ∀ context: `operator[]` ¬∃ required ∨ provided as the vector lookup syntax because its conventional unchecked-access semantics conflict with cljonic's bounds-checked contract
  rationale: `operator[]` unchecked-access semantics conflict with the bounds-checked contract
  {source: stakeholder_decided, decided_by: original_spec_author, rationale_source: origin_artifact}

λ REQ-FN-002R(x).
  ∀ callable `Set<T, N>` lookup form specified by `REQ-COLL-005B`: equivalent to the corresponding `get` overloads for the same set, value, and fallback arguments
  ∧ ∀ set callable lookup: uses the same stable equality capability and bounded linear scan as `contains` ∧ ¬∃ provision of a boolean-returning `operator()` overload because `contains(set, value)` is the canonical membership predicate
  rationale: `contains(set, value)` is the canonical membership predicate, so a boolean `operator()` would be a redundant alias
  {source: stakeholder_decided, decided_by: original_spec_author, rationale_source: origin_artifact}

λ REQ-FN-002S(x).
  ∀ callable `String<N>` lookup operation specified by `REQ-COLL-012A`: equivalent to `get(string, index)` and `get(string, index, fallback)`, with the fallback defaulting to `char{}` when omitted
  ∧ ∀ `contains(string, index)`: non-throwing ∧ non-allocating ∧ consistent with the callable lookup operation ∧ the null terminator remains outside the lookup domain
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-FN-002T(x).
  ∀ context: `conj` is supported on every `Conjable` collection — `Vector`, `Set`, `Map`, `Queue`, and `String`
  ∧ ∀ `Vector`: appends the value at the end (the highest logical index) when capacity remains ∧ a full vector returns an unchanged copy
  ∧ ∀ `Queue`: enqueues the value at the rear/tail, preserving FIFO order ∧ a full queue returns an unchanged copy
  ∧ ∀ `Set`: adds the element when absent ∧ an element already present is a successful no-op that preserves the count (`REQ-COLL-005A`) ∧ a full set returns an unchanged copy
  ∧ ∀ `Map`: `conj(map, entry)` associates the entry's key with its value — an existing key replaces its associated value without increasing count, a new key is added only when capacity remains (`REQ-COLL-004A`) ∧ the argument is a `MapEntry<K, V>` ∧ a full map with a new key returns an unchanged copy
  ∧ ∀ `String`: `String::conj(char)` and `conj(string, char)` append the character at the logical count when capacity remains ∧ a full string returns an unchanged copy ∧ the character follows String's character-validity policy (`REQ-COLL-020Q`)
  ∧ ∀ `conj`: `constexpr` ∧ `noexcept` ∧ non-mutating ∧ non-allocating ∧ returns a new collection value ∧ preserves the source
  rationale: `String` is a bounded indexed character collection whose `assoc` operation already appends at the logical count; `conj` exposes the same natural append behavior while preserving String's character policy
  {source: stakeholder_decided, decided_by: original_spec_author, rationale_source: origin_artifact}

λ REQ-FN-002U(x).
  ∀ context: `assoc` also provides a variadic form `assoc(collection, key₁, value₁, key₂, value₂, …)` over every `Associative` collection (`Map`, `Vector`, `String`), matching Clojure's `(assoc coll k v & kvs)`
  ∧ ∀ variadic form: folds its key-value pairs into the result left to right, so `assoc(c, k₁, v₁, k₂, v₂) ≡ assoc(assoc(c, k₁, v₁), k₂, v₂)` ∧ the result is a distinct collection value ∧ the source is unchanged
  ∧ ∀ pair: individually admitted under the same collection-specific key/value domain as the single-pair form (`REQ-COLL-020G`) ∧ a pair outside that domain fails at compile time regardless of its position in the argument list
  ∧ ∀ `String` pair: the character value follows the single-pair invalid-character policy (`REQ-COLL-020Q`) — compile-time rejection or runtime `'.'` replacement
  ∧ ∀ pair whose key is valid for the collection but cannot produce its documented result because the accumulator is at full capacity for a new key (a map) or an append index (a vector or string): a per-pair no-op that leaves the accumulator unchanged ∧ does not prevent the remaining pairs from being applied ∧ neither a compile-time error nor a runtime exception, consistent with the total, non-throwing single-pair contract (`REQ-COLL-020J`, `REQ-COLL-020O`)
  ∧ ∀ odd trailing argument count (a key with no following value): rejected at compile time
  ∧ ∀ variadic form: requires at least two key-value pairs ∧ `constexpr` ∧ `noexcept` ∧ non-mutating ∧ non-allocating ∧ constrained at the public API boundary by `AssociativeCollection` ∧ the one-pair arity remains the existing single-pair form
  rationale: Clojure's `assoc` folds an arbitrary number of key-value pairs into a map or vector; cljonic mirrors that surface while preserving its bounded, total, non-throwing association semantics — an over-capacity or otherwise invalid pair is a per-pair no-op rather than a Clojure-style throw
  {source: stakeholder_decided, decided_by: original_spec_author, rationale_source: origin_artifact}

λ REQ-FN-002V(x).
  ∀ `String<N>`: provides public static `String<N>::character_is_valid(char)` returning true exactly when the character is non-NUL and its unsigned byte value is at most `0x7F`
  ∧ ∀ free function `character_is_valid(value)`: admitted when the argument is an integral type other than `bool` ∧ returns false if the value is not representable by `char` ∧ otherwise converts without narrowing and returns the String character-validity result
  ∧ ∀ function form: `constexpr` ∧ `noexcept` ∧ non-mutating ∧ non-allocating
  ∧ ∀ unsupported input type (including `bool`, floating-point, enumeration, and user-defined conversion types): rejected with one targeted compile-time diagnostic
  ∧ ∀ diagnostic fallback: constrained to unsupported input types, contains a targeted dependent `static_assert`, and is never a supported call target (`REQ-DIAG-003`, `REQ-DIAG-009`)
  rationale: expose String's character-validity rule for direct checks while ensuring integer inputs are representable before conversion and rejecting implicit conversions that may lose information
  {source: stakeholder_decided, decided_by: original_spec_author, rationale_source: origin_artifact}

λ REQ-FN-002W(x).
  ∀ `can_assoc(collection, key)` defined for every `Associative` collection: agrees with that collection's `assoc` key-domain and capacity policy ∧ ¬∃ acceptance of a value argument, because the value being associated never affects whether `assoc` can succeed
  ∧ ∀ `Map`: an existing key returns true ∧ an absent key returns true only when capacity remains
  ∧ ∀ (`Vector` ∧ `String`): an existing logical index returns true ∧ the logical-count append index returns true only when capacity remains
  rationale: the value being associated never affects whether `assoc` can succeed, so a value parameter would invite misuse
  {source: stakeholder_decided, decided_by: original_spec_author, rationale_source: origin_artifact}

λ REQ-FN-002X(x).
  ∀ `disj(set, values...)`: defined only for a `Set` and zero or more values each having exactly the Set's declared `value_type` ∧ any other first-argument or value type is rejected at the public API boundary
  ∧ ∀ zero values: returns an unchanged copy of the Set
  ∧ ∀ one or more values: removes each value if present, applying removals from left to right ∧ an absent value is a no-op ∧ repeated values are permitted and subsequent removal of a now-absent value remains a no-op ∧ result is equivalent to sequential application of the existing Set removal operation
  ∧ ∀ arity: `constexpr` ∧ `noexcept` ∧ non-mutating ∧ non-allocating ∧ returns a collection value ∧ preserves the source
  ∧ ∀ unsupported first-argument or value type: fails compilation with one targeted diagnostic naming `disj` and the violated Set/value-type domain (`REQ-DIAG-003`, `REQ-DIAG-009`)
  rationale: Clojure's `disj` accepts zero or more values, returns the collection unchanged when given none, and sequentially removes each supplied member; exact element types avoid silently narrowing or converting membership queries
  {source: stakeholder_decided, decided_by: original_spec_author, rationale_source: origin_artifact}

λ REQ-FN-003(x).
  ∀ generic_free_function: constrained by explicit concepts or equivalent compile-time requirements
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-FN-004(x).
  ∀ unsupported_operation where the limitation is knowable from the types: fails at compile time with useful diagnostics
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-FN-005(x).
  ∀ functional_operation: preserves input values
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-FN-006(x).
  ∀ (`map` ∧ `filter` ∧ similar transformations) where the result can exceed its target capacity: documented capacity policy
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-FN-007(x).
  ∀ functions over compatible collection ∧ sequence types: composable
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-FN-008(x).
  ∀ (`map` ∧ `filter` ∧ `reduce` ∧ similar higher-order operations): preserve the purity and input-preservation guarantees of the library
  ∧ ∀ supported_callback: non-throwing ∧ non-allocating ∧ non-mutating with respect to operation inputs ∧ deterministic for equal arguments
  ∧ ∀ (the library ∧ its supported callbacks): ¬∃ (I/O ∨ mutation of input collections or their elements ∨ dependence on hidden mutable state or mutable global state ∨ introduction of side effects independently of an explicitly approved effectful API contract)
  ∧ ∀ standard-library facility ∧ callback form whose use would violate the bounded, deterministic, no-heap, no-exception, input-preservation, or owning-result contracts: rejected by the library
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-FN-008A(x).
  ∀ higher_order_operation_constraint: enforces at the public API boundary every callable property that can be expressed portably in the C++ type system, including invocability with the documented arguments, compatible result types, required `noexcept` behavior, and required `constexpr` capability
  ∧ ∀ (non-allocating behavior ∨ absence of I/O ∨ absence of hidden mutable-state dependence ∨ input preservation ∨ semantic determinism): treated as behavioral contract obligations ∧ verified by implementation review, focused tests, no-heap checks, or other applicable quality gates rather than assumed from callable syntax or a concept name
  rationale: a concept name cannot prove behavioral properties; assuming otherwise would create false confidence in the type system
  {source: stakeholder_decided, decided_by: original_spec_author, rationale_source: origin_artifact}

λ REQ-FN-026(x).
  ∀ context: the core vocabulary includes `empty`, `is_empty`, and `not_empty` with the semantics defined by `REQ-BOUNDS-010A`
  ∧ ∀ `empty`: produces an empty owning value of the same supported collection type as its input
  ∧ ∀ `is_empty`: returns a boolean predicate result
  ∧ ∀ `not_empty`: preserves the input collection type and capacity ∧ returns an owning copy of the input when nonempty or the corresponding empty value when empty
  ∧ ∀ these_operations: preserve their inputs ∧ require no dynamic allocation or exceptions
  {source: stakeholder_decided, decided_by: original_spec_author}

## Traceability and Related Requirements

- **Downstream Artifact**: `Vector`, `MapEntry`, `Map`, `Set`, `Queue`, `String` class templates and core collection free functions (`character_is_valid`, `count`, `get`, `conj`, `assoc`, `dissoc`, `disj`, `peek`, `pop`, `first`, `next`, `rest`, `seq`).
- **Governed REQs**: `REQ-COLL-001`–`021`, `REQ-SEQ-001`–`014` (incl. `002A`–`002B`, deferred), `REQ-FN-001`–`008A`, `REQ-FN-026`.