# cljonic Requirements - Module 3: Core Collection Types & Primitive Free Functions

## Purpose and Scope

This module defines the concrete, array-backed, bounded collection types (`Vector`, `Map`, `Set`, `Queue`, `String`), their current primitive member and free-function operations, and callable lookup forms. `first`, `next`, and `rest` are active traversal operations; sequence conversion remains approved future work. Module 3 provides the stored collection building blocks used across all higher-order algorithms.

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
  ∧ ∀ sequence conversion (`seq`): deferred future capability
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
  ∀ `String<N>`: callable with `operator()(Index, fallback)` for indexed lookup, where `Index` is an integral type other than `bool` or a character type and `fallback` defaults to `char{}` when omitted
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
  ∀ runtime `'.'` replacement policy for invalid bytes defined by `REQ-FN-027A`: applies to direct `String` source construction from external `std::string_view` and `std::span<const char>` imports
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
  ∀ `Vector<T, N>`: satisfies the `Indexed`, `Lookup`, and Clojure-style `Associative` capability semantics under the supported bounded model ∧ satisfies `Seqable` when the deferred `Seqable` capability is implemented for vectors
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-020D(x).
  ∀ `Map<K, V, N>`: satisfies the `Associative` and `Lookup` capability semantics under the supported bounded model ∧ satisfies `Seqable` when the deferred `Seqable` capability is implemented for maps
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-020E(x).
  ∀ `Set<T, N>`: satisfies the `Lookup` capability for value-membership lookup, where a present value is the associated lookup result and an absent value follows the documented default or fallback policy
  ∧ ∀ `Queue<T, N>`: ¬∃ requirement to satisfy `Indexed` ∨ `Lookup` ∨ `Associative` ∧ exposes only the sequence and queue operations approved for its lifecycle
  ∧ ∀ both types: satisfy `Seqable` when the deferred `Seqable` capability is implemented for them
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-020F(x).
  ∀ free_function: constrained by the capability required by the operation — `assoc` and `can_assoc` valid only for collections satisfying `Associative`, `get` only for collections satisfying `Lookup`, and `seq` only for collections satisfying `Seqable`
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-020G(x).
  ∀ context: `assoc` is defined for `Map`, `Vector`, and `String`
  ∧ ∀ `Map`: associates a key with a value ∧ replaces the existing value when the key already exists
  ∧ ∀ `Vector`: the key is a non-boolean, non-character integer index ∧ the value is the vector's element type
  ∧ ∀ `String`: the key is a non-boolean, non-character integer content index ∧ the value is a character subject to the String character-validity policy defined by `REQ-COLL-013` through `REQ-COLL-013B` and `REQ-COLL-020Q`
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
  ∀ built-in `Associative` collection ∧ `can_assoc(collection, key, value)`: reports whether the collection-specific key/index and capacity policy permits the requested association, and whether any runtime value policy permits the supplied value
  ∧ ∀ `Vector<T, N>`: returns true for an existing index or the logical-count append index when capacity remains, provided the value has the collection's stored element type; returns false for every invalid or full-capacity append index
  {source: stakeholder_decided, decided_by: human, timestamp: 2026-10-09}

λ REQ-COLL-020M(x).
  ∀ `String<N>` ∧ `assoc(s, i, value)` at an existing logical content index `i`: produces a new string whose ASCII character at `i` is replaced with `value` ∧ the source string remains unchanged ∧ all other content characters retain their values and order ∧ the null terminator remains immediately after the resulting content
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-020N(x).
  ∀ `String<N>` ∧ `assoc(s, i, value)` where `i` equals the source string's logical content count ∧ the source string has available content capacity: appends `value` ∧ the resulting string's logical content count increases by one ∧ its null terminator remains immediately after the new content
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-020O(x).
  ∀ `String<N>` ∧ `assoc` index of any non-boolean, non-character integral type: valid only when it is representable by the normalized `std::size_t` key domain, non-negative, no greater than the source string's logical content count, and either less than the logical content count or equal to the logical content count when content capacity remains
  ∧ ∀ invalid_index (including a negative ∨ unrepresentable index ∨ an index beyond the append position ∨ an append index at full content capacity): produces an unchanged string without throwing, allocating, or mutating the source
  ∧ ∀ null_terminator: ¬∃ valid association ∨ lookup index
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-020P(x).
  ∀ `String<N>` ∧ `can_assoc(string, index, value)`: accepts any non-boolean, non-character integral index type and a `char` value ∧ normalizes only indexes representable by the `std::size_t` key domain
  ∧ returns true for an existing content index or the logical content-count append index when content capacity remains, provided `value` is a valid non-NUL ASCII character
  ∧ returns false for a negative, unrepresentable, invalid, or full-capacity append index, or an invalid character value
  {source: stakeholder_decided, decided_by: human, timestamp: 2026-10-09}

λ REQ-COLL-020Q(x).
  ∀ `String<N>`: String owns one character-validity policy for every operation that stores content
  ∧ ∀ invalid_character (NUL ∨ non-ASCII): a String content update whose invalid character is known during constant evaluation fails at compile time
  ∧ ∀ runtime String content update whose character is invalid: stores `'.'` in place of that character ∧ the replacement preserves the operation's documented index, capacity, immutability, and null-terminator semantics
  ∧ ∀ `String::assoc` ∧ `String::conj`: applies the String-owned character policy
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-020R(x).
  ∀ context: `get(const C&, const K&)` and `get(const C&, const K&, const R&)` are supported for `Lookup` or `Indexed` collections and return the same collection-specific result type as the corresponding callable lookup operation
  ∧ ∀ fallback `R`: require(SafeImplicitConversion(lookup_result_type, R)) | governed_by(`REQ-CONV-001`–`003`)
  ∧ ∀ `Map<K, V, N>`: the lookup result and `get` return type are the mapped value `V`, not `MapEntry<K, V>`
  ∧ ∀ `Set<T, N>` ∨ `Vector<T, N>`: the lookup result and `get` return type are the stored element type `T`
  ∧ ∀ `String<N>`: the lookup result and `get` return type are `char`
  | related_surface_refinements: `REQ-COLL-020V`–`020Y`
  {source: stakeholder_decided, decided_by: human, timestamp: 2026-10-15}

λ REQ-COLL-020S(x).
  ∀ context: the lifecycle classification is `requirements-backed` for `Indexed`, `Lookup`, `Associative`, and `Conjable`, and for `count`, `get`, `contains`, `conj`, `assoc`, `can_assoc`, `dissoc`, `disj`, `peek`, `pop`, `first`, and `next` over their supported collection inputs
  ∧ ∀ `rest`: lifecycle classification is `requirements-backed` over its supported collection inputs under `REQ-SEQ-001`–`009` and `REQ-SEQ-012`
  ∧ ∀ (`Seqable` ∧ `seq`): remain `deferred` until the sequence requirements are implemented for the applicable collection inputs
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-020T(x).
  ∀ context: the lifecycle classification is also `requirements-backed` for `is_empty` over its supported sequenceable collection and producer inputs, `empty`, `can_conj`, and the `full` predicate or equivalent capacity inspection over their supported bounded collection inputs
  ∧ ∀ `not_empty`: `requirements-backed` active public free function over supported owning collection inputs, with owning-value behavior governed by `REQ-BOUNDS-010D` and `REQ-FN-026`
  ∧ ∀ behavioral contracts for `into` ∧ `fits_into`: approved by Module 2 and Module 4 ∧ both operations are `requirements-backed` active public free functions over `CljonicVector` destinations and `CljonicSource` sources
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-020U(x).
  ∀ `dissoc(map, keys...)`: require(Map(map)) ∧ arity(keys) ≥ 0 ∧ ∀ key: require(SafeImplicitConversion(Map::key_type, type(key))) | governed_by(`REQ-CONV-001`–`003`)
  ∧ ∀ present key: returns a new map with that entry removed via swap-and-remove ∧ keys are applied left to right ∧ the source map remains unchanged
  ∧ ∀ absent or repeated key: removal is a no-op ∧ zero keys returns an unchanged map copy
  ∧ ∀ result: `constexpr` ∧ `noexcept` ∧ non-allocating ∧ unsupported collection and key domains are rejected at the public API boundary
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-020V(x).
  ∀ `contains(const C&, const K&) -> bool`: require(Lookup(C) ∨ Indexed(C))
  ∧ ∀ C ∈ {`Map`, `Set`}: require(SafeImplicitConversion(C::lookup_type, K)) | governed_by(`REQ-CONV-001`–`003`)
  ∧ ∀ C ∈ {`Vector`, `String`}: require(integral(K) ∧ ¬Boolean(K) ∧ ¬character(K))
  ∧ ∀ `Range`: ¬free_function_contains ∧ member_contains_governed_by(`REQ-FN-013A`)
  {source: stakeholder_decided, decided_by: human, timestamp: 2026-10-15}

λ REQ-COLL-020W(x).
  ∀ Conjable(C): signatures = {`can_conj(const C&) -> bool`, `can_conj(const C&, const V&) -> bool`, `can_conj(const C&, const V₁&, const V₂&, …) -> bool`}
  {source: stakeholder_decided, decided_by: human, timestamp: 2026-10-15}

λ REQ-COLL-020X(x).
  ∀ Associative(C): one_pair_signatures = {`assoc(const C&, const K&, const V&) -> C`, `can_assoc(const C&, const K&, const V&) -> bool`}
  ∧ V: safely_admitted_by(C::association_value_type) ∧ same_key_value_admission(assoc ∧ can_assoc ∧ corresponding_member_calls)
  ∧ String_association_value: char
  ∧ variadic_pairs: governed_by(`REQ-FN-002U`, `REQ-FN-002W`)
  {source: stakeholder_decided, decided_by: human, timestamp: 2026-10-15}

λ REQ-COLL-020Y(x).
  ∀ operation governed_by(`REQ-COLL-020R`, `REQ-COLL-020V`–`020X`): constexpr ∧ noexcept ∧ non_mutating ∧ non_allocating ∧ public_boundary_constraints(required_capability ∧ collection_specific_key_value_domains)
  {source: stakeholder_decided, decided_by: human, timestamp: 2026-10-15}

λ REQ-COLL-020Z(x).
  ∀ `peek(const C&) -> C::value_type`: admit(`Vector<T, N>` ∨ `Queue<T, N>`) only
  ∧ ∀ nonempty `Vector<T, N>`: return(last_logical_element)
  ∧ ∀ nonempty `Queue<T, N>`: return(front_logical_element)
  ∧ ∀ empty admitted collection: return(`C::value_type{}`) including zero-capacity collections
  ∧ ∀ result: return_by_value ∧ preserve(input)
  ∧ ∀ operation: constexpr ∧ noexcept ∧ non_allocating
  ∧ ∀ unsupported collection: reject_at_compile_time_with(targeted_diagnostic ∧ `REQ-DIAG-009`)
  {source: stakeholder_decided, decided_by: human, timestamp: 2026-10-21}

λ REQ-COLL-020AA(x).
  ∀ `pop(const C&) -> C`: admit(`Vector<T, N>` ∨ `Queue<T, N>`) only
  ∧ ∀ nonempty `Vector<T, N>`: remove(last_logical_element)
  ∧ ∀ nonempty `Queue<T, N>`: remove(front_logical_element)
  ∧ ∀ empty admitted collection: return(unchanged_collection_value) including zero-capacity vectors
  ∧ ∀ result: return_by_value ∧ preserve(collection_type ∧ capacity ∧ logical_order_of_retained_elements ∧ input)
  ∧ ∀ operation: constexpr ∧ noexcept ∧ non_allocating
  ∧ ∀ unsupported collection: reject_at_compile_time_with(targeted_diagnostic ∧ `REQ-DIAG-009`)
  {source: stakeholder_decided, decided_by: human, timestamp: 2026-10-21}

## Collection Equality

λ REQ-COLL-021(x).
  ∀ bounded_owning_collection_value where element, key, and value types satisfy the stable equality capability required by `REQ-NUM-007` and `REQ-CAP-010`: supports value equality through the native C++ `==` operator
  ∧ ∀ (`Vector` ∧ `String` ∧ `Queue`) equality: order-sensitive — two values are equal exactly when their logical counts are equal and their logical elements compare equal in logical order
  ∧ ∀ (`Map` ∧ `Set`) equality: order-insensitive — two values are equal exactly when their logical counts are equal and every logical entry or element of one value has a matching logical entry or element in the other under stable key/element equality, independent of storage or insertion order
  ∧ ∀ collection_equality: `constexpr` ∧ `noexcept` ∧ non-mutating ∧ non-allocating ∧ ¬∃ traversal beyond the collection's bounded logical count ∧ recursively defined so that nested owning collection values, bounded producer values, and composite variant values compare according to the same component-wise stable-equality rule
  ∧ ∀ collection whose element, key, or value type does not admit stable equality: ¬∃ provision of `==` ∧ fails at compile time when used in an equality position
  ∧ ∀ collection_equality: ¬∃ implication of Clojure cross-type numeric unification ∨ hash-based equality
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-COLL-022(x).
  ∀ `Vector<T, N>`: provides `constexpr noexcept peek() const -> T` returning the last logical element, or `T{}` when empty
  ∧ ∀ `Vector<T, N>`: provides `constexpr noexcept pop() const -> Vector<T, N>` returning an independent value with its last logical element removed, or an unchanged value when empty
  ∧ ∀ `peek(vector)` ∧ `pop(vector)`: preserve the corresponding Vector member-operation semantics and leave `vector` unchanged
  ∧ ∀ Vector stack operations: return by value ∧ non-allocating ∧ preserve Vector type, capacity, and logical order of retained elements
  {source: stakeholder_decided, decided_by: human, timestamp: 2026-10-21}

## Active Rest Remainder Operation

`first`, `next`, and `rest` are current collection APIs. The clauses in this section define the active `rest` contract. Sequence conversion (`seq`) remains deferred in the following section.

λ REQ-SEQ-001(x).
  ∀ context: the library defines sequence as a traversal behavior over immutable values in the cljonic collection family, not as a separate owning collection type
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-SEQ-002(x).
  ∀ sequence: supports determining emptiness ∧ obtaining the remainder
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-SEQ-002A(x).
  ∀ `rest`: operate on the collection's documented logical traversal order
  ∧ ∀ nonempty_sequence: `rest` produces an owning bounded result containing every logical element after the first ∧ ∀ empty_sequence: `rest` produces the documented empty owning result
  ∧ ∀ `rest`: ¬∃ derivation of semantics from `pop()` because `pop()` MAY remove a different logical end for different collection kinds, including the last element of a vector and the first element of a queue
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-SEQ-002B(x).
  ∀ `rest`: return an owning `Vector` with the source collection's element type and static capacity ∧ preserve the source collection ∧ preserve the relative logical order of all retained elements
  ∧ ∀ maps: retained elements are value-semantic `MapEntry` values ∧ ∀ (sets ∧ maps): implementation traversal order MAY be repeatable but remains semantically unordered ∧ ∀ queues: retained elements remain in FIFO order
  ∧ `constexpr` ∧ `noexcept` ∧ non-allocating ∧ non-mutating ∧ ¬∃ RTTI
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-SEQ-003(x).
  ∀ supported collection that can be traversed as a sequence: exposes `rest` directly as a generic free function
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-SEQ-004(x).
  ∀ supported collection: `rest(collection)` is available as a generic free function where the required traversal capability exists
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-SEQ-005(x).
  ∀ `rest` traversal: ¬∃ mutation of the source collection
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-SEQ-006(x).
  ∀ `rest` traversal: bounded ∧ ¬∃ dynamic allocation
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-SEQ-007(x).
  ∀ `rest`: supports vector, map, set, queue, and string input from the cljonic collection family
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-SEQ-008(x).
  ∀ `rest`: ¬∃ requirement of support for arbitrary external arrays, ranges, iterables, or user-defined container types
  ∧ ∀ supported_rest_input: belongs to the cljonic collection family
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-SEQ-009(x).
  ∀ empty collection: `rest(collection)` returns an empty owning `Vector` with the collection's element type and capacity
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-SEQ-012(x).
  ∀ `rest` of a map: each retained element is an independent value-semantic `MapEntry` containing exactly one key and its associated value
  ∧ ∀ map traversal order: semantically unordered
  {source: stakeholder_decided, decided_by: original_spec_author}

## Deferred Sequence Conversion

> **Deferred future work — non-binding.** Sequence conversion (`seq`) remains outside the current API. The clauses below record intended future behavior and MUST remain deferred until a later increment separately propagates its implementation and tests for every collection family.

λ REQ-SEQ-010(x).
  ∀ `seq` application: returns an immutable, value-semantic bounded vector containing the sequence elements of its input
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-SEQ-011(x).
  ∀ vector returned by `seq`: independently valid ∧ uses bounded automatic or static storage ∧ ¬∃ (dynamic_allocation ∨ retained dependency on the source collection)
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

### Sequence First Access (Binding)

λ REQ-FN-002A1(x).
  ∀ (`Vector` ∧ `String`): `first` returns the logical element at index zero
  ∧ ∀ `Queue`: `first` returns the front element without removal
  ∧ ∀ `Set`: `first` returns one stored element ∧ ¬∃ reliance on which element is returned because ordinary set traversal order is semantically unordered
  ∧ ∀ nonempty_map: `first` returns one value-semantic map-entry value from the map's logical traversal order ∧ ¬∃ reliance on which entry is returned
  ∧ ∀ map_entry_value: `first` returns its key
  ∧ ∀ composition: `first(first(map))` returns the key
  ∧ ∀ empty_sequenceable_collection: `first` returns the input's `DefaultElement`
  ∧ ∀ `first`: preserves the input ∧ `constexpr` ∧ `noexcept` ∧ non-mutating ∧ non-allocating
  ∧ ∀ unsupported_input: fails compilation with one targeted diagnostic naming `first`
  {source: stakeholder_decided, decided_by: human, timestamp: 2026-10-15}

### Sequence Tail Access (Binding)

λ REQ-FN-002A2(x).
  ∀ `next(collection)`: require(`Vector` ∨ `Map` ∨ `Set` ∨ `Queue` ∨ `String`)
  ∧ ∀ supported_collection `C`: return `Vector<C::value_type, C::capacity()>` containing every logical element after the first in `C`'s documented traversal order
  ∧ ∀ result: capacity equals the source collection's declared capacity ∧ independently owning ∧ value-semantic
  ∧ ∀ empty_collection: return an empty result with the same result type and capacity
  ∧ ∀ `Map` input: retain map elements as value-semantic `MapEntry` values
  ∧ ∀ (`Map` ∧ `Set`): do not make the unspecified traversal order semantically significant
  ∧ ∀ `Queue`: preserve FIFO order among retained elements
  ∧ ∀ `next`: preserve the input ∧ `constexpr` ∧ `noexcept` ∧ non-mutating ∧ non-allocating
  ∧ ∀ unsupported_input: fails compilation with one targeted diagnostic naming `next`
  {source: stakeholder_decided, decided_by: human, timestamp: 2026-10-21}

### Map and Entry Sequence Access (Deferred, Non-binding)

> **Deferred future work — non-binding.** `REQ-FN-002A`–`002B` record future map/entry behavior for `last`, `key`, and `val`; they do not establish current API support.

λ REQ-FN-002A(x).
  ∀ map_entry_value: `last` returns its value ∧ `key` returns its key ∧ `val` returns its value
  ∧ ∀ nonempty_map: `last` returns one map-entry value
  ∧ ∀ composition: `last(first(map))` returns the value
  {source: stakeholder_decided, decided_by: human, timestamp: 2026-10-15}

λ REQ-FN-002B(x).
  ∀ `last` applied to a map: MAY return the final map-entry value in the implementation's traversal order ∧ ∀ caller: ¬∃ reliance on which entry is returned because ordinary map traversal order is semantically unordered
  rationale: map traversal order is semantically unordered, so no specific entry can be a stable result
  {source: stakeholder_decided, decided_by: human, timestamp: 2026-10-15, rationale_source: origin_artifact}

### General Comparisons (Binding)

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
  ∧ ∀ sequential_comparison: equal ≡ same(observed_count ∧ ordered_elements) | compare_only(bounded_logical_observations) ∧ ¬claim(equality_beyond_observation_bound)
  ∧ ∀ sequential_comparison: order-sensitive ∧ lazy_elementwise ∧ ¬eager_materialization ∧ terminate(first_unequal_pair ∨ shorter_exhausted_logical_count)
  ∧ ∀ producer_operand: unbounded_complete_sequence → ¬reject_solely_for_unboundedness ∧ observe_only(configured_bounded_traversal)
  {source: stakeholder_decided, decided_by: human, timestamp: 2026-10-15}

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
  ∧ ∀ `can_conj(collection)`: returns true without changing or requiring capacity from the collection
  ∧ ∀ `can_conj(collection, value₁, value₂, …)`: supports two or more values and returns true only when each value's single-value preflight succeeds in left-to-right order against the collection resulting from applying the preceding values; returns false when any preflight fails
  ∧ ∀ variadic value: individually admitted under the same collection-specific value domain as the one-value form regardless of its position
  ∧ ∀ variadic form: leaves the source unchanged ∧ is `constexpr` ∧ `noexcept` ∧ non-mutating ∧ non-allocating
  ∧ ∀ (`Set` ∧ `Map`): an element or key already present returns true — insertion is a no-op or a replacement that needs no capacity — ∧ a new element or key returns true only when capacity remains
  ∧ ∀ (`Vector` ∧ `Queue`): returns true only when capacity remains; the value argument does not affect the outcome
  ∧ ∀ `String`: returns true only when capacity remains and the character is valid (non-NUL ASCII) ∧ returns false for an invalid character even when capacity remains ∧ `conj` still applies the String-owned runtime replacement policy (`REQ-COLL-020Q`)
  rationale: a `Set` element or `Map` key already present makes `conj` a successful insertion without requiring capacity; zero-value preflight mirrors `conj(collection)`, and cumulative variadic preflight answers whether every requested value can be accepted in sequence, accounting for capacity and earlier insertions or replacements
  {source: stakeholder_decided, decided_by: human, timestamp: 2026-10-10, rationale_source: origin_artifact}

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
  ∧ ∀ one-value_form: remains the existing single-value form ∧ the zero-value and variadic arities are defined by `REQ-FN-002Y`
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
  ∧ ∀ free function `character_is_valid(c_string)`: admitted when the argument is a `char` array of static extent two — a one-character C-string such as `"x"` ∧ returns true only when the second byte is the null terminator and the single character is a valid String character ∧ a `char` array of any other static extent (the empty string and multi-character strings) is rejected with one targeted compile-time diagnostic
  ∧ ∀ function form: `constexpr` ∧ `noexcept` ∧ non-mutating ∧ non-allocating
  ∧ ∀ unsupported input type (including `bool`, floating-point, enumeration, user-defined conversion types, and a non-array C-string source such as `const char*`): rejected with one targeted compile-time diagnostic
  ∧ ∀ diagnostic fallback: constrained to unsupported input types, contains a targeted dependent `static_assert`, and is never a supported call target (`REQ-DIAG-003`, `REQ-DIAG-009`)
  rationale: expose String's character-validity rule for direct checks while ensuring integer inputs are representable before conversion and rejecting implicit conversions that may lose information; the C-string form admits a one-character string literal directly and rejects any other length at compile time rather than guessing which byte was intended
  {source: stakeholder_decided, decided_by: original_spec_author, rationale_source: origin_artifact}

λ REQ-FN-002W(x).
  ∀ `can_assoc(collection, key, value)` defined for every built-in `Associative` collection: accepts the complete argument set of `assoc` and returns whether the collection-specific key/index, capacity, and value preflight permits association
  ∧ ∀ `can_assoc(collection, key₁, value₁, key₂, value₂, …)`: supports two or more key-value pairs and returns true only when each pair's single-pair preflight succeeds in left-to-right order against the collection resulting from applying the preceding pairs; returns false when any preflight fails
  ∧ ∀ variadic pair: individually admitted under the same collection-specific key/value domain as the one-pair form regardless of its position ∧ an odd trailing key without a value is rejected at compile time
  ∧ ∀ variadic form: leaves the source unchanged ∧ is `constexpr` ∧ `noexcept` ∧ non-mutating ∧ non-allocating
  ∧ ∀ `Map`: an existing key returns true regardless of remaining capacity ∧ an absent key returns true only when capacity remains
  ∧ ∀ `Vector`: an existing index returns true ∧ the logical-count append index returns true only when capacity remains ∧ the value has the Vector's stored element type
  ∧ ∀ `String`: can_assoc(string, index, value) ≡ valid_non_NUL_ASCII(value) ∧ (existing_content_index(index) ∨ (logical_count_append_index(index) ∧ available_content_capacity(string))) | governed_by(`REQ-COLL-020P`)
  ∧ ∀ invalid key/index, inadmissible value type, or invalid String character: rejected according to the collection's compile-time or runtime value domain; invalid runtime String characters return false
  rationale: a complete preflight receives every operand of `assoc`; String's character-validity policy makes the value relevant to whether the requested association is admissible, and cumulative variadic preflight answers whether every requested pair can be associated in sequence, accounting for capacity, replacements, and indexes produced by prior pairs
  {source: stakeholder_decided, decided_by: human, timestamp: 2026-10-15, rationale_source: origin_artifact}

λ REQ-FN-002X(x).
  ∀ `disj(set, values...)`: require(Set(set)) ∧ arity(values) ≥ 0 ∧ (∀ value: require(SafeImplicitConversion(Set::value_type, type(value)))) ∧ (unsupported_domain → reject_at(public_boundary))
  ∧ ∀ zero values: returns an unchanged copy of the Set
  ∧ ∀ one or more values: removes each value if present, applying removals from left to right ∧ an absent value is a no-op ∧ repeated values are permitted and subsequent removal of a now-absent value remains a no-op ∧ result is equivalent to sequential application of the existing Set removal operation
  ∧ ∀ arity: `constexpr` ∧ `noexcept` ∧ non-mutating ∧ non-allocating ∧ returns a collection value ∧ preserves the source
  ∧ ∀ unsupported first-argument or value type: fails compilation with one targeted diagnostic naming `disj` and the violated Set/value-type domain (`REQ-DIAG-003`, `REQ-DIAG-009`)
  rationale: Clojure_disj ≡ zero_arity_identity ∧ sequential_removal | safe_widening ∧ ¬narrowed_membership
  {source: stakeholder_decided, decided_by: original_spec_author, rationale_source: origin_artifact}

λ REQ-FN-002Y(x).
  ∀ context: `conj` also provides a zero-value arity `conj(collection)` and a variadic arity `conj(collection, value₁, value₂, …)` over every `Conjable` collection (`Vector`, `Set`, `Map`, `Queue`, `String`), matching Clojure's `(conj coll)` and `(conj coll x & xs)`
  ∧ ∀ zero-value arity: returns an unchanged copy of the collection ∧ preserves the source
  ∧ ∀ variadic arity: folds its values into the result left to right, so `conj(c, x₁, x₂) ≡ conj(conj(c, x₁), x₂)` ∧ the result is a distinct collection value ∧ the source is unchanged
  ∧ ∀ value: individually admitted under the same collection-specific value domain as the one-value form (`REQ-FN-002T`) ∧ a value outside that domain fails at compile time regardless of its position in the argument list
  ∧ ∀ `String` value: the character follows the one-value invalid-character policy (`REQ-COLL-020Q`) — compile-time rejection or runtime `'.'` replacement
  ∧ ∀ value whose documented result cannot be produced because the accumulator is at full capacity for an append index (a `Vector`, `Queue`, or `String`), an absent `Set` element, or a new `Map` key: a per-value no-op that leaves the accumulator unchanged ∧ does not prevent the remaining values from being applied ∧ neither a compile-time error nor a runtime exception, consistent with the total, non-throwing one-value contract
  ∧ ∀ variadic arity: requires at least two values ∧ `constexpr` ∧ `noexcept` ∧ non-mutating ∧ non-allocating ∧ constrained at the public API boundary by `Conjable` ∧ the one-value arity remains the existing single-value form (`REQ-FN-002T`)
  rationale: Clojure's `conj` accepts `(conj coll)` and `(conj coll x & xs)`; cljonic mirrors those surfaces while preserving its bounded, total, non-throwing addition semantics — an over-capacity value is a per-value no-op rather than a Clojure-style failure, and the zero-value arity mirrors the existing `disj` zero-value contract
  {source: stakeholder_decided, decided_by: human, timestamp: 2026-10-10, rationale_source: origin_artifact}

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

- **Downstream Artifact**: `Vector`, `MapEntry`, `Map`, `Set`, `Queue`, `String` class templates and core collection free functions (`character_is_valid`, `count`, `get`, `conj`, `assoc`, `can_assoc`, `dissoc`, `disj`, `peek`, `pop`, `empty`, `is_empty`, `not_empty`, `first`, `next`, `rest`, `seq`).
- **Governed REQs**: `REQ-COLL-001`, `REQ-COLL-001A`, `REQ-COLL-002`, `REQ-COLL-002A`, `REQ-COLL-004`, `REQ-COLL-004A`–`004C`, `REQ-COLL-005`, `REQ-COLL-005A`–`005B`, `REQ-COLL-006`–`007`, `REQ-COLL-007A`, `REQ-COLL-008`–`010`, `REQ-COLL-010A`–`010B`, `REQ-COLL-011`–`012`, `REQ-COLL-012A`, `REQ-COLL-013`, `REQ-COLL-013A`–`013B`, `REQ-COLL-014`–`018`, `REQ-COLL-018A`, `REQ-COLL-019`, `REQ-COLL-019A`–`019B`, `REQ-COLL-020`, `REQ-COLL-020A`–`020AA`, `REQ-COLL-021`–`022`, `REQ-SEQ-001`–`002`, `REQ-SEQ-002A`–`002B`, `REQ-SEQ-003`–`014`, `REQ-FN-001`–`002`, `REQ-FN-002A`, `REQ-FN-002A1`, `REQ-FN-002A2`, `REQ-FN-002B`–`002H`, `REQ-FN-002M`, `REQ-FN-002P`–`002Y`, `REQ-FN-003`–`008`, `REQ-FN-008A`, `REQ-FN-026`.
- **Deferred REQs**: `REQ-SEQ-010`–`011`, `REQ-SEQ-013`–`014`, `REQ-FN-002A`–`002B`.
