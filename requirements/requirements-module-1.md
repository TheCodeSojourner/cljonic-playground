# cljonic Requirements - Module 1: Foundation & Nominal Type System

## Purpose and Scope

This module defines the foundational resource, memory, platform, value semantics, and nominal collection recognition requirements for `cljonic`. Module 1 forms the base of the library hierarchy: all subsequent modules depend on the memory invariants, platform constraints, and nominal collection boundaries established here.

## Core Allocation & Platform Constraints

λ REQ-PLAT-001(x).
  ∀ distribution: the distributable library is header-only
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-PLAT-002(x).
  ∀ generated_single_header: usable by including one public header
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-PLAT-003(x).
  ¬∃ (heap ∨ RTTI ∨ exceptions ∨ hidden_global_initialization) ∈ library_dependencies
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-PLAT-004(x).
  ∀ collection_storage: requirements statically inspectable from the type or configuration
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-PLAT-005(x).
  ∀ small_collections: library supports efficiently enough for non-performance-critical embedded code
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-PLAT-006(x).
  ∧ preferred: implementation favors simple bounded representations over sophisticated algorithms
  rationale: resource behavior of sophisticated algorithms is difficult to audit
  {source: stakeholder_decided, decided_by: original_spec_author, rationale_source: origin_artifact}

λ REQ-PLAT-007(x).
  ∀ library: documents compiler, language-standard, and toolchain requirements separately from the behavioral contract
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-PLAT-008(x).
  ∀ library: targets C++23 as minimum supported C++ standard
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-PLAT-009(x).
  ∀ supported_configuration ∧ ∀ public_api_path: ¬∃ (dynamic_allocation ∨ dynamic_deallocation), including allocation performed indirectly by a standard-library function or other transitive dependency
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-PLAT-010(x).
  ∀ library_storage: automatic ∨ static duration ∧ ¬∃ dynamic duration introduced by the library's own code or by a dependency it invokes
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-PLAT-012(x).
  ∀ execution: supported model is single-threaded ∧ ¬∃ (thread-safety ∨ synchronization ∨ atomics ∨ thread-local_storage ∨ parallel_execution ∨ concurrent_mutation) required or provided for library-managed state
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-PLAT-013(x).
  ∃ permitted_path: cljonic used within a multithreaded application when the caller externally confines each operation and value access to one thread at a time
  ∧ ∀ contract: concurrent-access safety excluded from the library contract
  {source: stakeholder_decided, decided_by: original_spec_author}

## Fundamental Value Semantics

λ REQ-VAL-001(x).
  ∀ collection_values: immutable from the user's normal API perspective
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-VAL-002(x).
  ∀ update_operation: returns a new value ∧ leaves its input value unchanged
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-VAL-003(x).
  ∀ equal_inputs ∧ equal_arguments: collection operations have deterministic results
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-VAL-004(x).
  ¬∃ user_managed_collection_node_lifetimes required by the library
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-VAL-005(x).
  ∀ (construction ∨ update ∨ lookup ∨ traversal ∨ transformation ∨ failure_handling ∨ destruction): ¬∃ (dynamic_storage_allocation ∨ deallocation) performed, requested, or transitively invoked
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-VAL-006(x).
  ∀ operation exceeding_collection_capacity: documented ∧ testable behavior ∧ ¬∃ silent_write outside the collection's storage
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-VAL-007(x).
  ∃ permitted_path: collection element, key, and value types use the simple user-defined C++ aggregate boundary defined by `REQ-PLAT-024`, provided their required construction, copying, moving, destruction, comparison, ordering, and default-element operations perform no dynamic allocation and require no forbidden runtime services
  ∧ ∀ collection ∧ operation: required capabilities are operation-specific ∧ storage alone requires neither equality nor ordering
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-VAL-007A(x).
  ∀ user_defined_type stored as collection element ∨ map key ∨ map value: satisfies the `NothrowCollectionElement` storage contract
  ∧ ∀ (default_construction ∧ copy_construction ∧ copy_assignment ∧ destruction): non-throwing
  ∧ ∀ storage_admission_contract: independent of equality, ordering, hashing, parsing, traversal, and other operation-specific capabilities
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-VAL-008(x).
  ∀ public_semantic_operation returning an owning cljonic result: returns a self-contained owning value whose validity does not depend on the lifetime or state of another value, temporary, external storage, or hidden borrowed state
  ∧ ∀ public_semantic_operation: ¬∃ return of (`std::ranges` views ∨ `std::span` values ∨ `std::string_view` values ∨ other borrowed lazy views)
  ∧ ∀ collection_owned C++ interoperability accessor (sole view exception): const-qualified observer ∧ exposes only read-only elements or characters ∧ ¬∃ (mutable references ∨ mutable iterators) ∧ satisfies the bounded, non-mutating lifetime and invalidation requirements defined for interoperability
  ∧ ∀ explicit_producer_value (separate permitted result category, governed by producer requirements): owns its parameters ∧ ¬∃ owned materialized result storage ∧ ¬∃ (borrowed source storage ∨ hidden mutable state)
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-VAL-009(x).
  ∀ public_cljonic_operation except explicit mutable-reference operations defined by `REQ-VAL-022`: referentially transparent with respect to explicit inputs and fixed configuration
  ∧ ∀ equal_inputs ∧ arguments ∧ configuration: equivalent results ∧ ¬∃ (mutation of input values ∨ modification of hidden mutable state ∨ I/O ∨ dependence on hidden mutable state)
  ∧ ∀ atom_mutation: explicit through the Atom API ∧ ¬∃ treatment as ordinary collection mutation ∨ hidden library state
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-VAL-010(x).
  ∀ operation: purity guarantees conditional on the required operations of its user-defined element, key, and value types being pure and non-allocating
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-VAL-011(x).
  ∀ collection_type supporting literal construction ∧ ∀ explicit_capacity_literal whose content exceeds declared capacity: fails at compile time
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-VAL-012(x).
  ∀ collection_type supporting both explicit-capacity and capacity-inferred literal construction: inferred form has the same semantics and type as the explicitly sized form instantiated with the collection's required content capacity
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-VAL-013(x).
  ∀ collection_type with explicit capacity parameter ∧ ∀ declared_capacity > `CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT`: fails at compile time with a diagnostic identifying both the declared capacity and the configured maximum
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-VAL-018(x).
  ∀ public_cljonic_regex_value_type ∈ {`Regex`, `RegexMatcher`, `RegexMatch`, `RegexGroup`} (where required): self-contained ∧ immutable ∧ bounded ∧ in root `cljonic` namespace
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-VAL-019(x).
  ∀ collection_update: persistent value semantics — returns a new independently valid value ∧ leaves the prior value unchanged
  ∧ ∀ persistence: describes observable value behavior ∧ ¬∃ requirement of a particular storage algorithm
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-VAL-020(x).
  ∀ cljonic_collection_update: uses deep copying ∧ ¬∃ structural_sharing
  ∧ ∀ public_behavior: ¬∃ (exposure ∨ dependence on shared internal storage) between prior and updated values
  {source: stakeholder_decided, decided_by: original_spec_author}

## Traceability and Related Requirements

- **Downstream Artifact**: Core foundation headers, storage trait machinery (`detail::is_cljonic_collection_v`).
- **Governed REQs**: `REQ-PLAT-001`–`010`, `REQ-PLAT-012`–`013`, `REQ-VAL-001`–`013`, `REQ-VAL-018`–`020`.
- **Storage admission**: `REQ-VAL-007A` defines the global `NothrowCollectionElement` contract for user-defined stored types.