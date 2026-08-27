# cljonic Requirements - Module 1: Foundation & Nominal Type System

## Purpose and Scope

This module defines the foundational resource, memory, platform, value semantics, and nominal collection recognition requirements for `cljonic`. Module 1 forms the base of the library hierarchy: all subsequent modules depend on the memory invariants, platform constraints, and nominal collection boundaries established here.

## Core Allocation & Platform Constraints

REQ-PLAT-001. The distributable library MUST be header-only.

REQ-PLAT-002. The generated single header MUST be usable by including one public header.

REQ-PLAT-003. The library MUST NOT depend on the heap, RTTI, exceptions, or hidden global initialization.

REQ-PLAT-004. Collection storage requirements MUST be statically inspectable from the type or configuration.

REQ-PLAT-005. The library MUST support small collections efficiently enough for non-performance-critical embedded code.

REQ-PLAT-006. The implementation SHOULD favor simple bounded representations over sophisticated algorithms whose resource behavior is difficult to audit.

REQ-PLAT-007. The library MUST document compiler, language-standard, and toolchain requirements separately from the behavioral contract.

REQ-PLAT-008. The library MUST target C++23 as its minimum supported C++ standard.

REQ-PLAT-009. The library MUST never allocate from or deallocate to dynamic storage in any supported configuration or public API path, including allocation performed indirectly by a standard-library function or other transitive dependency.

REQ-PLAT-010. All storage used by the library implementation MUST have automatic or static storage duration; the library MUST NOT introduce dynamic storage duration through its own code or through a dependency it invokes.

REQ-PLAT-012. The supported execution model MUST be single-threaded. The library MUST NOT require or provide thread-safety, synchronization, atomics, thread-local storage, parallel execution, or concurrent mutation of library-managed state.

REQ-PLAT-013. Use of cljonic within a multithreaded application MAY be possible when the caller externally confines each operation and value access to one thread at a time, but concurrent-access safety MUST NOT be part of the library contract.

## Fundamental Value Semantics

REQ-VAL-001. Collection values MUST be immutable from the user's normal API perspective.

REQ-VAL-002. An update operation MUST return a new value and MUST leave its input value unchanged.

REQ-VAL-003. Collection operations MUST have deterministic results for equal inputs and equal arguments.

REQ-VAL-004. The library MUST NOT require users to manage collection node lifetimes.

REQ-VAL-005. The library MUST NOT perform, request, or transitively invoke any dynamic storage allocation or deallocation for construction, update, lookup, traversal, transformation, failure handling, or destruction.

REQ-VAL-006. Any operation that can exceed a collection's capacity MUST have a documented, testable behavior. It MUST NOT silently write outside the collection's storage.

REQ-VAL-007. Collection element, key, and value types MAY use the simple user-defined C++ aggregate boundary defined by `REQ-PLAT-024`, provided that their required construction, copying, moving, destruction, comparison, ordering, and default-element operations do not perform dynamic allocation or require forbidden runtime services. The required capabilities depend on the collection and operation; storage alone MUST NOT require equality or ordering.

REQ-VAL-008. Every public function that returns an owning cljonic value MUST return a self-contained value whose validity does not depend on the lifetime or state of another value, temporary, external storage, or hidden borrowed state. The explicitly non-owning views returned by `view(collection)` are exempt from this owning-value requirement; their source-lifetime and invalidation rules MUST be governed by `REQ-PLAT-017` through `REQ-PLAT-023`.

REQ-VAL-009. Except for the explicit mutable-reference operations defined by `REQ-VAL-022`, public cljonic operations MUST be referentially transparent with respect to their explicit inputs and fixed configuration: for equal inputs, arguments, and configuration, they MUST produce equivalent results and MUST NOT mutate input values, modify hidden mutable state, perform I/O, or depend on hidden mutable state. Atom mutation MUST remain explicit through the Atom API and MUST NOT be treated as ordinary collection mutation or hidden library state.

REQ-VAL-010. The purity guarantees of an operation are conditional on the required operations of its user-defined element, key, and value types being pure and non-allocating.

REQ-VAL-011. For every collection type that supports literal construction, an explicit-capacity collection literal whose content exceeds its declared capacity MUST fail at compile time.

REQ-VAL-012. For every collection type that supports both explicit-capacity and capacity-inferred literal construction, the inferred form MUST have the same semantics and type as the explicitly sized form instantiated with the collection's required content capacity.

REQ-VAL-013. For every collection type with an explicit capacity parameter, a declared capacity greater than `CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT` MUST fail at compile time with a diagnostic identifying both the declared capacity and the configured maximum.

REQ-VAL-018. Public cljonic regex value types MUST be self-contained, immutable, bounded values in the root `cljonic` namespace, including `Regex`, `RegexMatcher`, `RegexMatch`, and `RegexGroup` where those representations are required.

REQ-VAL-019. cljonic MUST provide persistent value semantics for its collection values: an update MUST return a new independently valid value and MUST leave the prior value unchanged. Persistence describes observable value behavior and MUST NOT require a particular storage algorithm.

REQ-VAL-020. cljonic collection updates MUST use deep copying and MUST NOT use structural sharing. Public behavior MUST NOT expose or depend on shared internal storage between the prior and updated values.

## Traceability and Related Requirements

- **Downstream Artifact**: Core foundation headers, storage trait machinery (`detail::is_cljonic_collection_v`).
- **Governed REQs**: `REQ-PLAT-001`–`010`, `REQ-PLAT-012`–`013`, `REQ-VAL-001`–`013`, `REQ-VAL-018`–`020`.
