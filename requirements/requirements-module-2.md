# cljonic Requirements - Module 2: Capability Concepts & Preflight Infrastructure

## Purpose and Scope

This module establishes the C++20 concept capability framework, result status outcome classification model, preflight predicate policies, compile-time diagnostic rules, and vocabulary conventions for `cljonic`. Module 2 defines how the type system enforces safety, communicates failure, and governs function preconditions without runtime exceptions or dynamic memory allocations.

## Collection Capability Definitions

λ REQ-CAP-001(x).
  ∀ context: `Indexed` means a capability for efficient (constant-time, non-traversing), integer-indexed access to a collection's logical elements
  ∧ ∀ Indexed_collection: defines a bounded logical index domain ∧ provides non-mutating access for an index in that domain ∧ provides a non-throwing, non-allocating predicate that determines whether an index is in that domain
  ∧ ∀ negative_index (representable by the accepted index type): outside the domain
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-CAP-002(x).
  ∀ context: `Lookup` means a capability for non-mutating access through a collection-defined lookup domain
  ∧ ∀ Lookup_collection: defines its lookup key type ∧ provides access returning the associated value for a present key or the documented default or fallback result when the key is absent ∧ provides a non-throwing, non-allocating predicate distinguishing a present key from an absent key without inspecting the returned value
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-CAP-003(x).
  ∀ context: `Seqable` means a capability for producing the collection's documented logical traversal representation
  ∧ ∀ collection: satisfies `Seqable` only when its sequence conversion is available under the lifecycle rules for that collection
  ∧ ∀ Seqable_collection: provides sequence conversion without mutating the source, dynamic allocation, or exceptions ∧ sequence result is an independently valid bounded value whose elements and order follow the collection's documented traversal semantics
  ∧ ∀ sequence_traversal: deferred until the requirements in `REQ-SEQ-001` through `REQ-SEQ-014` become implementation-backed
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-CAP-004(x).
  ∀ context: `Associative` means a capability for non-mutating association of a key and value into a collection, producing a new collection value
  ∧ ∀ Associative_collection: defines its key and value types ∧ defines the valid-key and capacity policy for association ∧ preserves the source collection ∧ provides a non-throwing, non-allocating preflight predicate that agrees with the association operation for immutable inputs
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-CAP-005(x).
  ∀ context: `Associative` does not imply that the key domain is string-based, map-specific, or non-indexed
  ∧ ∃ permitted_path: vector ∨ string satisfies `Associative` by using integer indexes as keys, subject to the collection-specific replacement, append, capacity, and invalid-index requirements defined by Module 3
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-CAP-006(x).
  ∀ capability_satisfaction: compositional ∧ operation-specific
  ∧ ∀ capabilities ∈ {`Indexed`, `Lookup`, `Seqable`, `Associative`, `Conjable`}: distinct ∧ satisfying one does not imply another unless an explicit requirement states that relationship
  ∧ ∀ public_operation: requires only the capability or combination of capabilities needed by its documented behavior
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-CAP-007(x).
  ∀ context: `Indexed` refines `Lookup` for the integer lookup domain — every `Indexed` collection also satisfies `Lookup` with an integer key type ∧ indexed access and lookup agree for the same valid index
  ∧ ∀ context: `Associative` does not generally imply `Indexed` ∨ `Lookup`; its lookup-domain relationship is stated by the concrete collection requirement
  ∧ ∀ context: `Seqable` remains independent of the other four capabilities (`Indexed`, `Lookup`, `Associative`, and `Conjable`)
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-CAP-008(x).
  ∀ supported_collection_family: required capability participation is — `Vector` satisfies `Indexed`, `Lookup`, `Associative`, and `Conjable`; `Map` satisfies `Lookup`, `Associative`, and `Conjable`; `Set` satisfies `Lookup` and `Conjable`; `String` satisfies `Indexed`, `Lookup`, `Associative`, and `Conjable`; `Queue` satisfies `Conjable` and none of `Indexed`, `Lookup`, or `Associative` by default
  ∧ ∀ collection: satisfies `Seqable` only when the deferred sequence requirements are implemented for that collection
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-CAP-009(x).
  ∀ public_capability_name ∈ {`Indexed`, `Lookup`, `Seqable`, `Associative`, `Conjable`}: identifies the corresponding semantic capability in requirements, architecture, specifications, tests, source concepts, and generated documentation
  ∧ ∀ context: `Seqable` remains lifecycle-independent from `Indexed`, `Lookup`, `Associative`, and `Conjable`
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-CAP-010(x).
  ∀ supported map-key ∧ set-element domain: includes composite values formed by `cljonic::Variant` over scalar C++ literals, scoped enumerations, supported cljonic collections, supported producers whose stored parameters admit stable equality, and `MapEntry` values, subject to the storage, stable-equality, and component-recursion requirements of this module and Modules 3 and 4
  ∧ ∀ producer admitted as map key ∨ set element (directly ∨ as composite component): compares by producer parameter equality as governed by `REQ-FN-014B` — equality compares only the producer's bounded stored parameters ∧ ¬∃ traversal ∨ materialization of the produced sequence ∧ ¬∃ implication that equal parameters yield equal materialized sequences
  ∧ ∀ composite_value: satisfies the same admission contract as non-composite values — every stored component admits stable equality ∧ recursion applies to all nested components ∧ callable components are rejected because callables do not admit a stable value-equality comparison in the supported domain
  ∧ ∀ `cljonic::Variant` used as map key ∨ set element: uses alternative-strict equality — two variant values are equal only when they hold the same alternative and that alternative's values compare equal ∧ variants holding different alternatives compare unequal even when the alternative values would compare conventionally equal
  ∧ ∀ composite_domain_extension: does not expand the closed cljonic vocabulary beyond the standard composition operator ∧ does not reproduce Clojure runtime features, cross-type numeric unification, or hash-based equality
  rationale: callable components cannot participate in the stable value-equality comparison the composite domain requires
  {source: stakeholder_decided, decided_by: original_spec_author, rationale_source: origin_artifact}

λ REQ-CAP-011(x).
  ∀ context: `cljonic::Variant<Alternatives...>` is the nominal cljonic composite value type holding exactly one alternative at a time; it replaces `std::variant` as the supported composite in the cljonic value domain
  ∧ ∀ alternative: satisfies the `NothrowCollectionElement` storage contract — nothrow default construction, copy or move construction, copy assignment, and destruction — so a `Variant` is admissible as a map value, a vector element, and a queue element
  ∧ ∀ admitted_alternative: arithmetic scalar ∨ scoped enumeration ∨ cljonic collection ∨ producer with stable parameters ∨ `MapEntry` ∨ nested `cljonic::Variant`; floating-point and callable alternatives are admissible for storage exactly where `NothrowCollectionElement` admits them, even though they are not equality-admissible
  ∧ ∀ cljonic::Variant: provides alternative-strict value equality through the native `==` operator and the `equal` free function exactly when every alternative satisfies `NothrowStableEqualityComparable` and provides a non-throwing comparison; otherwise it provides no `==` and any use in an equality position fails at compile time, mirroring a storable-but-not-comparable collection element such as `Vector<float, N>`
  ∧ ∀ equality_semantics: two `Variant` values are equal exactly when they hold the same alternative and that alternative's values compare equal ∧ variants holding different alternatives compare unequal even when their alternative values would compare conventionally equal ∧ ¬∃ cross-type numeric unification ∧ ¬∃ hash-based equality
  ∧ ∀ ordering: `Variant` provides the native ordering operators exactly when every alternative satisfies `TotallyOrdered`
  ∧ ∀ cljonic::Variant: has no valueless state ∧ ¬∃ `valueless_by_exception` ∧ because every alternative is nothrow-storable, no operation leaves the value without an active alternative
  ∧ ∀ operation: `constexpr` ∧ `noexcept` ∧ non-mutating for const operations ∧ non-allocating ∧ ¬∃ RTTI ∧ ¬∃ throwing access path
  ∧ ∀ cljonic::Variant: is a component of the closed equality value domain — nestable inside cljonic collections, producers with stable parameters, `MapEntry`, and other `cljonic::Variant` values — and is subject to the same recursive component analysis as every other composite
  ∧ ∀ standard_library `std::variant`: is not a supported cljonic value type ∧ fails at compile time as an `equal` operand ∧ fails at compile time as a map key ∨ set element ∧ MAY be used only as an internal implementation detail of `cljonic::Variant`
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-CAP-012(x).
  ∀ cljonic::Variant capability: exposed as a cljonic free function ∧ backed by a `cljonic::Variant` member method, consistent with the `std::variant` capability set filtered by cljonic embedded constraints
  ∧ ∀ required_capability: active-index query (`index`); alternative-membership predicate by type or index (`holds`); checked access to the active alternative by type or index (`get`, precondition: the queried alternative is active); checked access returning a pointer or null when the alternative is not active (`get_if`); non-throwing in-place rebinding of the active alternative by type or index (`emplace`); swap (`swap`); single-value visit (`visit`); equality comparison (`equal` and the native `==`); alternative count and alternative type queries (`variant_size`, `variant_alternative`)
  ∧ ∀ access ∨ rebinding: non-throwing ∧ ¬∃ `bad_variant_access` ∧ rebinding leaves no valueless state
  ∧ ∀ free_function: has the same behavior as its backing member method ∧ non-allocating ∧ non-mutating for query operations
  ∧ ∀ excluded_std_variant_capability ∈ {`valueless_by_exception`, exception-based `get`, multi-variant `visit`}: not provided
  ∧ ∀ class_template_argument_deduction: a single-alternative deduction guide is provided, consistent with the library's constructor-pack preference
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-CAP-013(x).
  ∀ context: `Conjable` means a capability for non-mutating insertion of one value into a collection, producing a new collection value
  ∧ ∀ Conjable_collection: defines its value type and capacity policy ∧ provides `conj` and a non-throwing, non-allocating `can_conj` preflight that agrees with `conj` for immutable inputs ∧ preserves its source collection
  ∧ ∀ context: `Conjable` does not by itself imply `Associative`, `Indexed`, or `Lookup`
  {source: stakeholder_decided, decided_by: original_spec_author}

## Canonical Type, Result, and Status Model

- An owning value is a self-contained cljonic value whose validity does not depend on an external source lifetime, hidden borrowed state, or a hidden result cache.
- A non-owning view is a read-only observation of an existing cljonic value. A view MUST NOT own storage, MUST NOT extend the lifetime of its source, and MUST NOT permit mutation of the source value.
- A bounded result is an owning cljonic value whose capacity is known and finite under the operation's documented constraints.
- A bounded-prefix result is a bounded result that is intentionally smaller than the complete result because the complete result could not fit within the declared capacity or the operation's documented result policy.
- A default-returning result is the documented default value produced by a convenience operation when the requested access or lookup cannot produce a valid value.
- A checked-failure result is a documented non-throwing, non-allocating result indicating that an operation could not complete successfully without violating the library's failure model.
- A producer is an explicit, self-contained value that represents a sequence or materialization source without owning the materialized result storage. A producer MUST own its parameters and MUST NOT borrow source storage, retain hidden mutable state, or depend on a source lifetime. A producer is distinct from both an owning materialized result and a collection-owned C++ interoperability view.
- A producer-only result is a public operation result whose value is a self-contained producer rather than an owning materialized collection. A producer-only result owns producer parameters, does not own materialized result storage, and does not borrow source storage or hidden mutable state.
- A complete result is the full result of an operation as defined by the relevant requirement.
- Every public operation MUST document whether it returns a complete result, a bounded-prefix result, a default value, or a checked failure. When the complete result may fail to fit, the operation MUST expose a corresponding non-throwing, non-allocating preflight predicate that measures the same result and failure conditions as the operation.
- Stable equality and total ordering are capabilities, not assumptions implied by storage. A type is eligible for equality or ordering only when the applicable requirement or capability contract explicitly permits it.

## Result Status Classification Rules

- Result status is orthogonal to value kind. Every public operation MUST declare its outcome classification as one of:
  - complete result
  - bounded-prefix result
  - default-returning result
  - checked-failure result
  - `ProducerOnlyResult`
- An operation MUST document which status it returns, which preflight predicate (if any) governs completion, and which failure or default semantics apply when the operation does not produce a complete result.
- For operations whose complete result may fail to fit or may be invalid under runtime conditions, the operation MUST provide a corresponding non-throwing, non-allocating preflight predicate that measures the same success and failure conditions as the operation itself.
- A default-returning result and a checked-failure result are distinct: the former is a documented convenience value for an absent or invalid access, while the latter is an explicit signal that the operation could not complete successfully under the required model.

## Naming and Predicate Policy

- Naming and predicate policy MUST be stable, explicit, and outcome-oriented.
- Predicate names MUST read as questions about the operation's precondition or completion condition, not as access operations. Examples include `empty`, `full`, `contains`, `contains_key`, and `fits_into`.
- A predicate MUST be non-throwing, non-allocating, and MUST test the same domain and failure conditions as the corresponding operation.
- A predicate MUST never hide a default-returning access behind a truthy check; a `true` result means the operation can return a valid value under the documented contract, and a `false` result means the operation must follow its documented default, bounded-prefix, or checked-failure behavior.
- The canonical predicate for materialization completeness remains `fits_into`; other operation-specific predicates MAY be used only when they are not just aliases for the same materialization-completion check.

## Bounds and Default Elements Requirements

λ REQ-BOUNDS-001(x).
  ∀ collection_access: bounds-checked
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-BOUNDS-002(x).
  ∀ collection: defines a default element of its value type, normally produced by value-initialization such as `T{}`
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-BOUNDS-003(x).
  ∀ access that cannot return a valid element: returns the collection's default element through the default-returning API
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-BOUNDS-004(x).
  ∀ context: the API makes it possible to determine before access whether an operation can return a valid element without relying on exceptions or inspecting the value for equality with the default element
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-BOUNDS-005(x).
  ∀ operation that may return a default element: documented by the library
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-BOUNDS-006(x).
  ∀ (bounds_failure ∨ capacity_failure ∨ missing_key_result): distinct documented semantics, even when more than one uses a default-returning convenience API
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-BOUNDS-007(x).
  ∀ default_returning_access_operation: the API provides a non-throwing, non-allocating way to determine before access whether the operation can return a valid element
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-BOUNDS-008(x).
  ∀ pre_access_predicate: defined in terms of the operation's domain ∧ distinguishes empty collections or sequences, invalid indexes, missing keys, and full-collection capacity state where those conditions apply
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-BOUNDS-009(x).
  ∀ immutable_collection_value: a successful pre-access predicate and the corresponding access operation agree — if the predicate reports that a valid element is available, the access returns that element; otherwise the access follows its documented default-element semantics
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-BOUNDS-010(x).
  ∀ context: the API provides `contains` as the canonical lookup-domain membership predicate across all collection kinds — for maps it tests key presence, for sets element presence, and for vector/string indexed collections it tests index-in-range (mirroring Clojure's `contains?`)
  ∧ ∃ permitted_path: separately approved producer requirements extend `contains` to producer bounded-observation positions while preserving the same non-throwing, non-allocating pre-access predicate discipline; this does not make a producer `Indexed` or provide positional value retrieval
  ∧ ∀ future free-function surface: provides `empty` for collection and sequence underflow checks ∧ `full` or an equivalent capacity inspection remains part of the current bounded insertion surface, along with membership or key-presence checks for key-based access where those capabilities apply
  rationale: mirrors Clojure's `contains?` so the canonical membership predicate reads consistently across collection kinds
  {source: stakeholder_decided, decided_by: original_spec_author, rationale_source: origin_artifact}

λ REQ-BOUNDS-010A(x).
  ∀ sequenceable_value ∈ (SequenceableCollection ∨ SequenceableProducer): `is_empty(sequenceable_value)` is a non-throwing, non-allocating boolean predicate returning `true` exactly when `count(sequenceable_value) == 0`
  ∧ ∀ unbounded_producer: `count(sequenceable_value)` is the configured observation cap rather than complete cardinality, and `is_empty` follows that count observation
  ∧ ∀ supported owning collection: exposes `is_empty` only as a free function ∧ ¬∃ `is_empty()` member requirement
  ∧ ∀ unsupported input outside (SequenceableCollection ∨ SequenceableProducer): rejects with one targeted compile-time diagnostic naming the argument and required sequenceable domain (`REQ-DIAG-009`)
  ∧ ∀ future traversal_work: defines the corresponding sequence behavior
  ∧ ∀ deferred `empty` operation: returns an empty owning value of the same supported collection type as its input, preserving the input's capacity type where that capacity is part of the collection type
  ∧ ∀ deferred `not_empty` operation: returns an owning value of the same supported collection type as its input — an independently valid copy of the input when the input contains one or more elements and the corresponding empty value when the input contains zero elements ∧ ¬∃ boolean alias for `is_empty`
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-BOUNDS-011(x).
  ∀ pre_access_predicate: ¬∃ (inspection ∨ comparison of the accessed value ∨ reliance on equality with `T{}` ∨ ambiguous default-returning access ∨ throw ∨ allocation)
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-BOUNDS-012(x).
  ∀ `into` operation whose maximum possible result cardinality can exceed the destination collection's capacity: the API provides the non-throwing, non-allocating preflight predicate `fits_into(destination, producer)`, which determines whether the complete result fits within that destination
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-BOUNDS-013(x).
  ∀ immutable_inputs: a result-capacity preflight predicate and the corresponding materializing operation have compatible semantics — if the predicate reports that the complete result fits, the operation produces the complete result; if the predicate reports that the complete result does not fit, the operation follows its documented bounded-result behavior, which MAY be a deterministic prefix limited to its compile-time capacity
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-BOUNDS-014(x).
  ∀ result_capacity_preflight_predicate: measures the same result quantity ∧ applies the same matching, filtering, transformation, or overflow semantics as its corresponding materializing operation
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-BOUNDS-015(x).
  ∀ `into` operation: documents whether its result is complete or may be a bounded prefix when the destination capacity is insufficient
  ∧ ∀ bounded_prefix possibility: preserves the defined result order ∧ provides a preflight predicate that determines whether the complete result fits
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-BOUNDS-016(x).
  ∀ `into` operation: returns the destination collection type directly
  ∧ ∀ ordinary_collection_value: ¬∃ hidden completeness state ∧ cljonic does not require a materialization-result wrapper for partial-prefix status ∧ callers use `fits_into` before `into` when completeness matters
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-BOUNDS-017(x).
  ∀ supported operation whose complete result can fail because of (capacity ∨ representability ∨ cardinality ∨ matching ∨ filtering ∨ transformation ∨ another documented runtime condition): defines its bounded-result, default-result, or failure policy
  ∧ ∀ caller needing to distinguish complete success from that default, bounded, partial, or failed result: the operation provides a corresponding non-throwing, non-allocating preflight predicate that measures the same result quantity and applies the same capacity, representability, cardinality, matching, filtering, transformation, and failure semantics as the operation
  ∧ ∀ context: `fits_into` remains the canonical preflight for complete producer materialization into an explicit destination ∧ operation-specific predicates MAY be used for other operations
  rationale: callers must be able to distinguish complete success from default, bounded, partial, or failed results without exceptions
  {source: stakeholder_decided, decided_by: original_spec_author, rationale_source: origin_artifact}

## Error and Failure Policy Requirements

λ REQ-ERR-001(x).
  ∀ supported_collection_operation: ¬∃ requirement of exceptions
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-ERR-002(x).
  ∀ ordinary_collection_access: ¬∃ requirement of error codes ∨ global mutable error state
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-ERR-003(x).
  ∀ default_returning_convenience_function: documented pre-access predicates allowing callers to determine whether the operation can return a valid element or complete successfully
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-ERR-004(x).
  ∀ (empty_collection ∨ missing_key ∨ invalid_index ∨ full_collection ∨ duplicate_set_insertion ∨ duplicate_map_key): failure behavior deterministic ∧ documented
  ∧ ∀ duplicate_set_insertion: successful no-op ∧ ∀ duplicate_map_key_association: replaces the existing value
  ∧ ∀ pre_access_predicate ∨ capacity_query provided for these conditions: semantics consistent with the corresponding access or update operation
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-ERR-005(x).
  ∀ invalid_collection_access within the documented API: ¬∃ undefined_behavior invoked by the library
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-ERR-006(x).
  ∀ supported operation whose success depends on (runtime values ∨ collection state ∨ capacity ∨ representability ∨ cardinality ∨ producer behavior): complies with `REQ-BOUNDS-017`
  ∧ ∀ caller needing to distinguish complete success from a default, bounded, partial, or failed result: the operation provides a corresponding non-throwing, non-allocating preflight predicate unless a non-throwing, non-allocating checked result directly communicates that distinction
  ∧ ∀ preflight_predicate ∨ checked_result: agrees with the operation for immutable inputs ∧ a false or failed outcome produces the operation's documented default, bounded, partial, or failure result
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-ERR-007(x).
  ∀ supported_cljonic_operation: defined for all its documented inputs
  ∧ ∀ operation: ¬∃ (throw ∨ undefined_behavior ∨ out-of-bounds_storage_access ∨ unexpected_allocation ∨ silent_state_corruption ∨ process_termination ∨ dependence on hidden mutable error state)
  ∧ ∀ runtime_detectable_unsuccessful_condition: uses the operation-specific documented mechanism — a preflight predicate with a default or bounded result, a checked result, or a compile-time rejection when the condition is knowable from the types or constant expressions
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-ERR-008(x).
  ∀ overflow_policy: operation-specific but follows one general contract — compile-time-known capacity or representability overflow rejected at compile time ∧ runtime-detectable overflow provides a non-throwing, non-allocating preflight predicate ∧ the corresponding operation leaves its input unchanged when the predicate is false ∧ returns its documented default or bounded result
  ∧ ∀ context: `into` is the explicit partial-prefix exception and uses `fits_into` as specified elsewhere
  ∧ ∀ direct_source_constructor whose documented operation is bounded-prefix construction: not a complete materialization operation ∧ does not require a separate preflight predicate; complete source materialization remains governed by `into`, preflighted by `fits_into`
  {source: stakeholder_decided, decided_by: original_spec_author}

## Compile-Time Diagnostics

λ REQ-DIAG-001(x).
  ∀ public_template ∧ free_function with knowable compile-time capability requirements: expresses them through named concepts or equivalent constraints
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-DIAG-002(x).
  ∀ compile_time_invalid_use: fails at the public API boundary with diagnostics that identify the violated cljonic capability, capacity, or value constraint
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-DIAG-003(x).
  ∧ preferred: the library uses targeted `static_assert` diagnostics for context-dependent compile-time failures that cannot be expressed clearly through concepts or equivalent constraints
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-DIAG-004(x).
  ∀ diagnostic_requirement: specifies the meaning and relevant constraint of a diagnostic rather than depending on compiler-specific wording
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-DIAG-005(x).
  ∀ public_concept_name: precise ∧ capability-oriented ∧ understandable when rendered in compiler diagnostics ∧ identifies the required capability or constraint ∧ ¬∃ reliance on vague names such as `Valid`, `Supported`, or `Allowed` without further qualification
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-DIAG-006(x).
  ∀ materialization API with runtime-unknown result cardinality: ∧ preferred — provides an implementation-defined compiler warning or equivalent tooling diagnostic when the caller uses the default maximum capacity, recommending an explicit call-site capacity
  ∧ ∀ such_warning: ¬∃ replacement of the required capacity parameter ∧ ¬∃ dependence on non-portable diagnostics for correctness
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-DIAG-007(x).
  ∀ callable_requirement: distinguishes compile-time-checkable constraints from behavioral policy constraints
  ∧ ∀ named_concept ∨ equivalent_constraint: ∧ preferred — enforces invocability, argument compatibility, result compatibility, required `noexcept` behavior, and any required `constexpr` capability at the public API boundary
  ∧ ∀ (allocation behavior ∨ I/O ∨ input mutation ∨ hidden mutable-state dependence ∨ semantic determinism): explicit contract obligations verified through implementation review, focused tests, resource checks, or other applicable quality gates ∧ ¬∃ representation of a C++ concept as proving those properties unless it actually can
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-DIAG-008(x).
  ∀ requirement_designator: globally unique ∧ stable after publication so that traceability survives requirement refinement
  ∧ ∀ designator: form `REQ-<FAMILY>-<NUMBER>` with an optional uppercase alphabetic suffix reserved for a refinement of the immediately preceding numeric requirement
  ∧ ∀ contiguous family-specific requirement list: requirements presented and referenced in numeric order, with a suffixed refinement immediately following its base requirement
  ∧ ∃ permitted_path: thematic module sections group related requirements from different families ∧ requirements distributed across dependency-ordered modules
  ∧ ∀ existing_designator: ¬∃ renumbering to fill gaps
  rationale: traceability must survive requirement refinement across downstream annotations
  {source: stakeholder_decided, decided_by: original_spec_author, rationale_source: origin_artifact}

λ REQ-DIAG-009(x).
  ∀ public_free_function, present ∨ added_in_future, whose supported domain is closed and expressible through named capability concepts (illustrative, non-narrowing examples: `equal`, `not_equal`, the producer factories `repeat`, `cycle`, `iterate`, and `repeatedly`, and the collection primitives `count`, `get`, `contains`, `conj`, `can_conj`, `assoc`, `can_assoc`, `dissoc`, `disj`, `peek`, `pop`, `is_empty`, `into`, and `fits_into`):
  ∧ primary admission: expressed through named capability concepts at the public API boundary (`REQ-DIAG-001`, `REQ-DIAG-002`, `REQ-DIAG-005`)
  ∧ preferred diagnostic fallback: a diagnostic overload constrained on the negation of the admission gate, provided for each supported arity, so that an argument outside the supported domain fails with one targeted `static_assert` that names the operation, the rejected operand types, and the violated domain rule (`REQ-DIAG-003`)
  ∧ the diagnostic fallback exists to explain rejection, not to accept it: it never returns a value and is never a supported call target
  ∧ ∀ diagnostic_fallback message: states the meaning and the violated constraint in plain user-facing terms rather than relying on capability-concept or internal-implementation jargon or on compiler-specific wording (`REQ-DIAG-004`)
  ∧ detectability contract: compile-time detection of domain support uses the named admission concepts and their `*_admissible_v` predicates, ¬∃ callability detection such as `requires { call(...) }`; a domain-rejected argument MAY satisfy callability once a diagnostic fallback is provided, and that callability is not a supported interface
  ∧ whenever_possible: a diagnostic fallback is required for a public free function wherever a distinct overload can be formed whose constraints are strictly less preferred than the admitted overload for every valid call and viable only for a rejected call; a public free function whose rejection surface admits no such overload (its domain is gated by a class-template constraint before any call candidate is considered, or no sibling candidate exists to compete with) carries a documented exclusion — recorded in the requirement and the function's header — and its single named constraint diagnostic is the accepted outcome
  ∧ forward_binding: the obligation binds every public free function added in the future, not only those named here; a public free function whose rejection surface is neither admitted-gated and fallback-taught nor explicitly excluded is a conformance defect surfaced by the diagnostic-coverage gate
  ∧ ∀ producer_factory_free_function ∈ {`repeat`, `cycle`, `iterate`, `repeatedly`}: primary admission is expressed through its product-specific named capability concept (`NothrowCollectionElement` for `repeat`; `CljonicSource` for `cycle`; `IterateStep` for `iterate`; `RepeatedlyStep` for `repeatedly`) ∧ an argument outside the closed producer domain fails with one targeted `static_assert` naming the operation, the violated producer-admission rule, and the correct usage, replacing the compiler's list of rejected concept candidates
  ∧ ∀ `Range`: excluded — its element domain is a class-template constraint (`signed_integral`) that already fails with a single named constraint diagnostic before any constructor is considered, so no diagnostic fallback is required; a `Range` diagnostic fallback is for a future `range()` factory function, not the type's constructors
  rationale: maximally informative compile-time diagnostics are a global library goal that applies to every public free function, present and future; a single targeted diagnostic is more accurate to a reader than the compiler's list of rejected concept candidates, so a fallback is added wherever one can be formed and an explicit, justified exclusion is recorded wherever it cannot
  {source: stakeholder_decided, decided_by: original_spec_author, rationale_source: origin_artifact}

λ REQ-DIAG-010(x).
  ∀ public_constructor whose source domain is closed and expressible through named constraints (for example a cljonic collection's direct source constructor, which admits only non-cljonic C++ range or view sources — `REQ-FN-027A`):
  ∧ primary admission: expressed through named constraints at the constructor boundary, so a cljonic collection or producer argument that is not the destination's element type is excluded from source construction
  ∧ preferred diagnostic fallback: an additional constructor overload constrained on the negation of the source-admission rule, so that a rejected cljonic collection or producer argument fails with one targeted `static_assert` that names the collection, states the violated source-construction rule, and directs the caller to `into` for materialization (preflighted by `fits_into`) or to the enclosure form for a single element (`REQ-DIAG-003`)
  ∧ the diagnostic fallback exists to explain rejection, not to accept it: it never produces a value and is never a supported call target
  ∧ ∀ diagnostic_fallback message: states the meaning and the violated constraint in user-facing terms rather than relying on a generic element-conversion message, and does not depend on compiler-specific wording (`REQ-DIAG-004`)
  ∧ ∀ constructor whose rejection occurs at class instantiation (a type argument gated by the class-template `requires`: `Range`, `MapEntry`, `Repeat`, `Cycle`, `Iterate`, `Repeatedly`, `Variant`): excluded — no constructor is reached, so a constructor fallback is unreachable and no diagnostic fallback is required; the class-template constraint already yields one named constraint diagnostic
  ∧ ∀ constructor-level `requires` used as an availability guard rather than a source-domain rejection (`Iterate` and `Repeatedly` default constructors): excluded — it is not a closed source-domain rejection
  ∧ ∀ aggregate-like struct (`MapEntry`): excluded — its domain is type-argument gated, and any constructor added to host a fallback would destroy aggregate status
  rationale: without the fallback the rejected argument falls through to the pack constructor's generic conversion diagnostic (for example "could not convert a producer to the element type"), which misleads the caller because no element conversion was intended; one targeted, educational diagnostic is more accurate and keeps the rejection reason discoverable
  {source: stakeholder_decided, decided_by: original_spec_author, rationale_source: origin_artifact}

## Compile-Time Evaluation

λ REQ-CONST-001(x).
  ∀ (collection_construction ∧ non-allocating_operation): ∧ preferred — `constexpr` when the value types and compiler permit it
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-CONST-002(x).
  ∀ operation required to be compile-time evaluable: tested in constant expressions
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-CONST-003(x).
  ∀ `consteval` use: only where compile-time execution is semantically required ∧ does not unnecessarily restrict valid embedded use
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-CONST-004(x).
  ∀ valid_inputs: compile-time and runtime evaluation produce equivalent observable results
  ∧ ∃ permitted_path: a requirement defines a deliberate invalid-input distinction in which a compile-time-known invalid value is rejected during constant evaluation while the corresponding runtime value follows a documented deterministic replacement or failure policy
  {source: stakeholder_decided, decided_by: original_spec_author}

## Vocabulary and Naming Conventions

λ REQ-VOCAB-001(x).
  ∀ context: the canonical terms include collection, sequence, sequenceable, traversal, `Indexed`, `Lookup`, `Seqable`, `Associative`, `Conjable`, `ConstRangeTraversal`, `LogicalTraversalOrder`, `ReadOnlyInteropAccessor`, `ContiguousConstView`, vector, map, set, queue, string, capacity, default element, `contains`, persistent value, free function, bounded storage, platform interoperability, aggregate-like struct, stable equality, total order, alternative-strict equality, discrete numeric type, numeric policy, owning value, non-owning view, standard view type, bounded result, partial result, preflight predicate, exact conversion, checked conversion, lossy conversion, parsing, finite observation, finite deep equality, bounded inspection, unbounded producer, `ProducerOnlyResult`, producer materialization, `NoMutationConstraint`, relation model, `MapEntry`, general equality, numeric equality, sequential equality, `equal`, semantic predicate name, state predicate, verb predicate, capability predicate, `IsPredicatePrefix`, `CanPredicatePrefix`, `HasPredicatePrefix`, `ValidPredicatePrefix`, lifecycle classification, `CandidateStatus`, `DeferredStatus`, `ExcludedStatus`, `RequirementsBacked`, `KeywordEnumNameEntry`, `KeywordEnumNameMap`, `KeywordEnumNameContext`, and keyword enum name mapping
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-VOCAB-002(x).
  ∀ canonical_term: one meaning in public documentation, requirements, tests, and code
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-VOCAB-003(x).
  ∃ permitted_path: Clojure-inspired names used when their behavior is documented in C++ terms
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-VOCAB-004(x).
  ∀ public_API: prefers a small set of orthogonal capabilities over a deep hierarchy of collection-specific interfaces
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-VOCAB-005(x).
  ∀ public_API: exposes capacity and failure policy clearly enough that a caller can reason about resource use from source code
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-VOCAB-006(x).
  ∀ public C++ collection type name ∈ {`Vector`, `Map`, `Set`, `Queue`, `String`}: PascalCase
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-VOCAB-007(x).
  ∀ Clojure_inspired public C++ function name: replaces hyphens with underscores, so a name such as `last-index-of` is exposed as `last_index_of`
  ∧ ∀ terminal Clojure question mark: expressed semantically — state or adjectival predicates use an `is_` prefix ∧ readable verb predicates retain their direct verb form
  ∧ ∀ capability ∧ preflight predicate: retain established `can_`, `has_`, and `valid_` prefixes
  ∧ mapping: `empty?`, `zero?`, `pos?`, `neg?`, `even?`, `odd?`, `blank?`, `subset?`, `superset?` map to `is_empty`, `is_zero`, `is_positive`, `is_negative`, `is_even`, `is_odd`, `is_blank`, `is_subset`, `is_superset`; `contains?`, `starts-with?`, `ends-with?`, `includes?` map to `contains`, `starts_with`, `ends_with`, `includes`
  ∧ ∀ context: namespace layout is a separate API design decision
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-VOCAB-008(x).
  ∀ public_cljonic_namespace_name: ∧ preferred — mirrors the corresponding Clojure namespace names using lowercase C++ namespace identifiers and underscores where separators are needed, so `clojure.core` maps conceptually to `cljonic::core` and `clojure.string` maps conceptually to `cljonic::string`
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-VOCAB-009(x).
  ∀ public_cljonic_API: developed as a broad Clojure-inspired convenience surface rather than as a minimal embedded collection subset
  ∧ ∀ broad_surface_goal: subordinate to the library's bounded-resource, no-allocation, no-exception, value-semantic, single-threaded, and compile-time-capability requirements
  ∧ ∀ context: ¬∃ requirement to reproduce Clojure runtime features, support arbitrary external C++ types, or expand the closed cljonic vocabulary without an approved requirement
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-VOCAB-010(x).
  ∀ first_class cljonic value ∧ data-structure domain: closed to the supported bounded `Vector`, `Map`, `Set`, `Queue`, and `String` values, explicit producer values, bounded regex values and match results required by the text surface, and application-defined scoped enumeration values when used as supported map keys or set elements
  ∧ ∀ public_operation: ¬∃ requirement to accept or return arbitrary external containers, runtime type extensions, or protocol-style user-defined collection participation
  ∧ ∃ permitted_path: simple user-defined aggregate-like structs participate where the applicable platform, storage, equality, ordering, and other capability requirements explicitly permit them; such participation does not expand the collection vocabulary
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-VOCAB-011(x).
  ∀ public_function considered during API-surface review: one explicit lifecycle classification ∈ {`candidate`, `deferred`, `excluded`, `requirements-backed`}
  {source: stakeholder_decided, decided_by: original_spec_author}

## Traceability and Related Requirements

- **Downstream Artifact**: C++20 concepts, preflight predicates, compile-time assertions, and result status type definitions.
- **Governed REQs**: `REQ-CAP-001`–`013`, `REQ-BOUNDS-001`–`017`, `REQ-ERR-001`–`008`, `REQ-DIAG-001`–`010`, `REQ-CONST-001`–`004`, `REQ-VOCAB-001`–`011`.