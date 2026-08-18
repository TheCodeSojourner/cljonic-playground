# cljonic Requirements

## Purpose

cljonic is a header-only C++ library for embedded-system development. It provides a broad Clojure-inspired functional programming vocabulary, bounded value-oriented collections, sequence operations, and free functions without using dynamic storage. The public API is intended to provide useful Clojure-like convenience beyond a minimal embedded collection subset, while every supported operation remains subject to cljonic's bounded-resource, value-semantic, and auditable execution constraints.

This document is the behavioral starting point for cljonic. Vocabulary, architecture, specifications, tests, and code must remain consistent with these requirements. Implementation choices are replaceable when they preserve the requirements.

## Scope

cljonic is intended as a broad convenience library for code that benefits from:

- immutable-by-default values;
- concise functional operations;
- predictable bounded memory use;
- no heap allocation anywhere in cljonic or in any transitive dependency used by cljonic;
- no required exceptions, error codes, or global mutable error state for ordinary collection operations;
- default elements for convenience access, with documented pre-access predicates when absence or failure must be distinguished;
- resource use and capacity limits that are visible and auditable from the types or configuration;
- operation without required RTTI, hidden global initialization, or other heavyweight runtime services;
- predictable behavior at empty, full, and over-capacity boundaries;
- small average collection sizes and non-performance-critical application code;
- a single-header distribution that can be integrated into an embedded build;
- compile-time evaluation where practical; and
- simple, auditable behavior for embedded developers.

The breadth of the convenience surface MUST NOT be achieved by weakening the
library's bounded-resource, no-allocation, no-exception, value-semantic,
single-threaded, or compile-time-capability constraints. Public functions and
value types remain part of a closed, intentionally selected cljonic vocabulary;
the broad-surface goal does not require reproducing all Clojure runtime
features or supporting arbitrary external C++ types.

The first-class cljonic value and data-structure domain MUST consist of bounded
`Vector`, `Map`, `Set`, `Queue`, and `String` values, explicit producer values,
bounded regex values and match results where required by the text surface, and
application-defined scoped enumeration values when used as supported map keys
or set elements. This closed domain defines the required inputs and outputs of
the public convenience API; arbitrary external containers, runtime type
extension, and protocol-style participation are not required.

Result representation MUST follow a bounded-result versus producer policy:
operations SHOULD return an owning bounded cljonic value when a useful finite
capacity can be derived from the operation and its inputs, and SHOULD use an
explicit producer when the result is unbounded or no useful finite capacity is
available without imposing an arbitrary caller-independent limit. A runtime-
varying element count alone MUST NOT require producer semantics when a safe
compile-time capacity bound exists.

String support MUST be a first-class bounded convenience layer rather than
being limited to parsing, regex, and debug formatting. The initial text layer
MUST provide useful ASCII-preserving and ASCII-transforming operations as
owning `String` results, while operations that can increase result length or
produce multiple strings MUST expose destination capacity and follow the
universal bounded-result and preflight policy.

Set support MUST be a first-class bounded algebra layer rather than being
limited to membership and mutation-like value construction. The initial set
algebra MUST include bounded owning `union`, `intersection`, and `difference`
results plus bounded `subset` and `superset` predicates, all using stable
equality, unordered set semantics, explicit capacity behavior, and the
universal bounded-result and preflight policy where a complete result can fail
to fit. This decision does not approve relational map operations such as
`index`, `project`, `rename`, or `join`.

Relational map/set operations MUST require an explicit relation model before
they become supported public operations. The model MUST define the relation row
representation, key and value capabilities, duplicate-row and duplicate-key
semantics, nested result representation, traversal-order semantics, capacity
arithmetic, complete-result preflight, and bounded failure behavior for
`index`, `project`, `rename`, and `join`. Feasibility of their Clojure
vocabulary alone MUST NOT promote them to the supported API.

Numeric support MUST include a broader convenience family rather than only the
numeric behavior incidental to collection indexing and capacity arithmetic.
The family MUST cover checked fixed-width arithmetic, scalar comparison and
selection, constrained numeric predicates, explicit numeric conversions, and
fixed-width bitwise operations. This broader vocabulary MUST remain governed by
the explicit numeric policy for representability, overflow, floating-point
stability, conversion loss, division by zero, and preflight behavior.

Callable support MUST include a bounded convenience family for constructing,
adapting, composing, and applying ordinary element functions. The family MUST
remain free-function-first and compile-time checked, and any returned callable
MUST satisfy the existing bounded closure storage, copying, invocation,
destruction, and non-allocation requirements. Callable convenience MUST NOT
introduce transducers, hidden mutable caching, runtime type dispatch, or
implicit callback retention by collection results.

Collection-shaping and traversal support MUST include a broad first-pass family
for bounded prefix, suffix, stride, predicate, duplicate, grouping,
partitioning, flattening, tree-traversal, mapping, filtering, and ordering
operations. The family MUST use owning bounded results or explicit producers
according to the result-representation policy, MUST define nested-result and
capacity behavior, and MUST preserve input values, callback constraints, and
unordered map/set semantics.

Every function considered during API-surface review MUST have an explicit
classification lifecycle. A function MAY be a candidate while feasibility is
being assessed, deferred when its value or contract is not yet approved,
excluded when it conflicts with the project boundary, or requirements-backed
only after its behavior, capacity, failure, diagnostic, and test obligations
are defined in this document. Function vocabulary or apparent C++
implementability alone MUST NOT establish supported behavior.

Every supported operation whose complete result can fail because of capacity,
representability, cardinality, matching, filtering, transformation, or other
runtime conditions MUST define a bounded-result policy and a corresponding
non-throwing, non-allocating preflight predicate when callers need to
distinguish complete success from a default, bounded, or partial result. The
predicate MUST measure the same result and failure conditions as the operation;
operation-specific predicates MAY use names other than `fits_into` when the
operation is not destination materialization.

cljonic is not required to reproduce Clojure's JVM implementation, runtime, compiler, or structural-sharing algorithms.

## Requirement Language

- MUST means a requirement.
- MUST NOT means a prohibition.
- SHOULD means a strong preference that requires an explicit decision to deviate from.
- MAY means an optional capability.
- A `MUST` or `MUST NOT` clause is locked behavior. A `SHOULD` clause is a locked preference with an allowed, documented deviation. A `MAY` clause permits a capability but does not require it; it MUST NOT be summarized as supported behavior unless another clause or an explicit decision enables it.

## Locked Design Summary

This summary is a navigation aid for human readers and future AI distillation. The numbered `REQ-*` clauses remain authoritative; this summary MUST NOT introduce behavior that is not stated in those clauses.

- cljonic is a closed C++ vocabulary of bounded collections and free functions for embedded development, not a general C++ extension framework.
- The public API is a broad Clojure-inspired convenience surface rather than a minimal embedded collection subset; breadth MUST remain subordinate to the locked resource, value, execution, and capability constraints.
- The first-class value and data-structure domain is closed: it includes bounded `Vector`, `Map`, `Set`, `Queue`, and `String` values, explicit producers, required bounded regex values and match results, and supported application-defined scoped enumeration values. Arbitrary external containers and protocol-style extension are not required.
- Result representation follows a bounded-result versus producer policy: owning bounded results are preferred when a useful finite capacity can be derived, while explicit producers represent unbounded results or results without a useful finite capacity absent an arbitrary caller-independent limit. Runtime-varying cardinality alone does not require a producer when a safe compile-time capacity bound exists.
- String support is a first-class bounded convenience layer, not only parsing, regex, and debug formatting. ASCII-preserving and ASCII-transforming operations return owning `String` values; operations that can increase result length or produce multiple strings expose destination capacity and the applicable bounded-result/preflight behavior.
- Set support is a first-class bounded algebra layer, including owning `union`, `intersection`, and `difference` results plus `subset` and `superset` predicates. Set algebra uses stable equality, preserves unordered set semantics, exposes explicit capacity behavior, and does not by itself approve relational map operations.
- Relational map/set operations require an explicit relation model before support: `index`, `project`, `rename`, and `join` need defined row representation, capabilities, duplicate semantics, nested results, traversal order, capacity arithmetic, complete-result preflight, and bounded failure behavior.
- Numeric support includes a broader convenience family of checked fixed-width arithmetic, scalar comparison and selection, constrained numeric predicates, explicit conversions, and fixed-width bitwise operations, all governed by the explicit numeric policy for representability, overflow, floating-point stability, conversion loss, division by zero, and preflight behavior.
- Callable support includes a bounded free-function convenience family for constructing, adapting, composing, and applying ordinary element functions, with compile-time invocation checks and the existing bounded closure storage and non-allocation constraints. It does not introduce transducers, hidden mutable caching, runtime type dispatch, or implicit callback retention by collection results.
- Collection-shaping and traversal support includes a broad first-pass family for bounded prefix, suffix, stride, predicate, duplicate, grouping, partitioning, flattening, tree-traversal, mapping, filtering, and ordering operations. Results use owning bounded values or explicit producers according to the result policy, with defined nested-result and capacity behavior, preserved inputs, constrained callbacks, and unordered map/set semantics.
- API functions follow an explicit classification lifecycle: candidate during feasibility review, deferred when value or contract approval is incomplete, excluded when incompatible with the project boundary, and requirements-backed only after behavior, capacity, failure, diagnostics, and tests are defined. Vocabulary presence or apparent C++ implementability alone does not establish support.
- Every operation whose complete result can fail because of capacity, representability, cardinality, matching, filtering, transformation, or another runtime condition has a documented bounded-result policy and a corresponding non-throwing, non-allocating preflight predicate when callers need to distinguish complete success from a default, bounded, or partial result. The predicate uses the same result and failure semantics as its operation; `fits_into` remains the materialization predicate, while other operations may use operation-specific names.
- The supported collection family is flat, contiguous, and bounded. Map and set operations use linear scans; unsorted map and set removal uses swap-and-remove.
- Collection persistence is semantic and implemented by deep copying; structural sharing is not used.
- Producers are explicit values and materialize through `into`; `fits_into` is the only completeness preflight, and no hidden completeness state is carried by collections.
- `comp` supports zero-or-more ordinary right-to-left function composition; transducers are not supported.
- Stored closures are supported as collection elements and map values when their storage capabilities are satisfied, but they do not gain equality or ordering capabilities.
- A bounded single-threaded `Atom<T>` provides `deref`, `reset`, and exactly-once synchronous `swap` with compile-time type checking.
- Application `enum class` types provide keyword-like map keys; optional `KeywordEnumNameMap` values provide explicit human-readable debug names without changing enum identity or map storage; runtime symbols, metadata, reader/EDN, macros, reflection, and runtime namespace resolution are not supported.
- `view(collection)` returns standard read-only views for the contiguous cljonic collections, and supported operations work correctly for all their documented inputs.

## Canonical Type and Result Model

This section defines the canonical semantics that apply to later requirements. It is a normative vocabulary section and MUST be used to interpret the meaning of result, view, producer, default, partial, bounded, and failure terms elsewhere in this document.

- An owning value is a self-contained cljonic value whose validity does not depend on an external source lifetime, hidden borrowed state, or a hidden result cache. Collection values, string values, regex values, map-entry values, and producer parameters are examples of owning values.
- A non-owning view is a read-only observation of an existing cljonic value. A view MUST NOT own storage, MUST NOT extend the lifetime of its source, and MUST NOT permit mutation of the source value.
- A bounded result is an owning cljonic value whose capacity is known and finite under the operation's documented constraints.
- A bounded-prefix result is a bounded result that is intentionally smaller than the complete result because the complete result could not fit within the declared capacity or the operation's documented result policy. This is the canonical term used throughout this document; the phrase "partial result" is a descriptive synonym and SHOULD be avoided in normative text unless it is explicitly referring to older narrative wording.
- A default-returning result is the documented default value produced by a convenience operation when the requested access or lookup cannot produce a valid value.
- A checked-failure result is a documented non-throwing, non-allocating result indicating that an operation could not complete successfully without violating the library's failure model.
- A producer is an explicit value that represents a sequence or materialization source without owning the materialized result storage. Producers MAY be finite or unbounded, but they MUST NOT hide an arbitrary capacity limit or a materialized result cache.
- A complete result is the full result of an operation as defined by the relevant requirement. A caller MAY require a preflight predicate to determine whether the complete result fits before materialization.
- Every public operation MUST document whether it returns a complete result, a bounded-prefix result, a default value, or a checked failure. When the complete result may fail to fit, the operation MUST expose a corresponding non-throwing, non-allocating preflight predicate that measures the same result and failure conditions as the operation.
- The lifetime of a view is limited to the lifetime of its source value and any documented invalidation rules. A view MUST NOT extend the lifetime of the source collection or provide mutable access to it.
- Stable equality and total ordering are capabilities, not assumptions implied by storage. A type is eligible for equality or ordering only when the applicable requirement or capability contract explicitly permits it.
- This model is normative for the rest of the requirements. When a later clause refers to a "result," "default," "bounded-prefix result," "producer," or "view," it MUST be interpreted in terms of this section.

## Result Status Classification

This section defines the required outcome classification for public cljonic operations. It is normative and MUST be used to interpret result, failure, producer, and default semantics throughout the document.

- Result status is orthogonal to value kind. Every public operation MUST declare its outcome classification as one of the following:
  - complete result: the operation produced the full result defined by the requirement and the result is valid as a bounded owning cljonic value;
  - bounded-prefix result: the operation produced a deterministic prefix or reduced result because the complete result could not fit within the documented capacity or policy;
  - default-returning result: the operation produced a documented default value because the requested value was unavailable, missing, or invalid;
  - checked-failure result: the operation produced a documented non-throwing, non-allocating failure outcome indicating that the requested result could not be completed under the library's failure model;
  - producer-only result: the operation returned an explicit producer value that represents a materialization source or stream without owning the final materialized storage.
- An operation MUST document which status it returns, which preflight predicate (if any) governs completion, and which failure or default semantics apply when the operation does not produce a complete result. A result-status decision is part of the operation's contract and MUST be consistent with the capacity, lifetime, and value-semantics rules used elsewhere in this document.
- A complete result and a bounded-prefix result are not alternative value types; they are two possible status outcomes for the same operation under different capacity or representability conditions. The status MUST be determined by the operation's contract, not by caller-side guessing.
- For operations whose complete result may fail to fit or may be invalid under runtime conditions, the operation MUST provide a corresponding non-throwing, non-allocating preflight predicate that measures the same success and failure conditions as the operation itself. The predicate MUST be the authoritative test for whether a complete result is available.
- A default-returning result and a checked-failure result are distinct: the former is a documented convenience value for an absent or invalid access, while the latter is an explicit signal that the operation could not complete successfully under the required model.

## Naming and Predicate Policy

This section defines the required naming and predicate policy for public cljonic operations. It is normative and MUST be used to interpret the meaning of operation names and preflight predicates throughout the document.

- Naming and predicate policy MUST be stable, explicit, and outcome-oriented.
- Predicate names MUST read as questions about the operation's precondition or completion condition, not as access operations. Examples include `empty`, `full`, `valid_index`, `contains_key`, and `fits_into`.
- A predicate MUST be non-throwing, non-allocating, and MUST test the same domain and failure conditions as the corresponding operation.
- A predicate MUST never hide a default-returning access behind a truthy check; a `true` result means the operation can return a valid value under the documented contract, and a `false` result means the operation must follow its documented default, bounded-prefix, or checked-failure behavior.
- If an operation has no canonical named predicate, it MAY define an operation-specific predicate only if the name clearly states the tested condition and the same conditions are used by the operation itself.
- Predicate naming MUST prefer stable semantic wording over implementation wording. Names that imply mutation, allocation, exception behavior, or hidden state are not allowed.
- The canonical predicate for materialization completeness remains `fits_into`; other operation-specific predicates MAY be used only when they are not just aliases for the same materialization-completion check.
- The requirement grammar MUST remain consistent: operations return results, predicates answer whether a valid complete result is available, and default-returning or checked-failure outcomes remain explicit rather than inferred from truthiness or value equality.

## Fixed-Width Numeric Contract

This section defines the required numeric semantics for public cljonic arithmetic, conversion, and predicate operations. It is normative and MUST be used to interpret numeric representability, overflow, conversion loss, division-by-zero, and checked arithmetic behavior throughout the document.

- Numeric support MUST use explicit fixed-width semantics and MUST not rely on implicit C++ promotions or unchecked arithmetic.
- Every numeric operation MUST declare whether it is checked or unchecked, and checked arithmetic MUST return a documented checked-failure result or a bounded default when the result cannot be represented in the destination type.
- Integer overflow MUST be defined as a checked arithmetic failure; it MUST NOT silently wrap, truncate, or reinterpret the result.
- Signed and unsigned conversion MUST be explicit and MUST not silently change the value domain. Converting from a wider type to a narrower type MUST be governed by a documented representability check; conversion loss MUST be treated as a checked failure unless the operation explicitly defines a saturating or lossy conversion mode.
- Division by zero MUST be a checked-failure result or a documented default-returning result, never an undefined arithmetic outcome.
- Floating-point operations MUST define the required stability and rounding policy for comparisons, conversions, and scalar selection. A float or double result MUST not be treated as exact unless the requirement explicitly states exactness.
- Numeric predicates MUST distinguish representability from value comparison; a predicate that checks "fits in" MUST be independent of equality with a default value and MUST be non-throwing and non-allocating.
- Exact-result operations and lossy-conversion operations MUST be separate capabilities. A requirement may allow a lossy conversion only when it explicitly names the conversion policy and its failure semantics.
- Numeric capability gates MUST be compile-time visible: if an operation requires a wider or narrower representation, the type or capability contract must make that requirement explicit.
- The numeric family is bounded by representability and explicit conversion policy. Unsupported arithmetic or conversion cases MUST return a documented checked-failure or default result, not hidden undefined behavior.

## String, View, and Lifetime Alignment

This section defines the required alignment among string behavior, view semantics, and lifetime rules. It is normative and MUST be used to interpret later text, access, and result clauses throughout the document.

- A string is a first-class bounded owning value, not a convenience wrapper around raw text or parser state.
- String operations that preserve content and fit in a declared destination capacity MUST return an owning bounded string result.
- String operations that can increase result length, produce multiple strings, or require dynamic output shape MUST use an explicit destination capacity or an explicit producer result, and MUST follow the universal bounded-result and preflight policy.
- A string view is a non-owning read-only observation of an existing string value. It MUST NOT own storage, MUST NOT extend the lifetime of the source, and MUST NOT permit mutation of the source.
- The lifetime of any string view is limited to the lifetime of the underlying string value and any explicit invalidation rules. A view MUST remain valid only while its source remains valid.
- String operations MUST distinguish complete bounded result, bounded-prefix result, default-returning result, checked-failure result, and producer-only result as defined by the result-status clause.
- ASCII-preserving and ASCII-transforming operations MUST remain bounded and deterministic under the document's no-allocation and no-exception constraints.
- Any string operation whose complete output may exceed the destination capacity MUST expose the corresponding non-throwing, non-allocating preflight predicate before materialization.
- String views and collection views share the same lifetime semantics: they are observations only, and they cannot outlive their source or mutate it.
- The string layer is therefore a bounded value layer with explicit capacity and lifetime rules, not an unbounded or externally borrowed text model.

## Requirements Traceability

This map identifies the primary downstream artifact for each requirement family. Architecture, specifications, tests, and code MUST preserve traceability back to the originating `REQ-*` clauses.

| Requirement family | Primary downstream artifact                   |
| ------------------ | --------------------------------------------- |
| `REQ-VAL-*`        | Value semantics specification                 |
| `REQ-BOUNDS-*`     | Bounds and preflight specification            |
| `REQ-COLL-*`       | Collection architecture and specification     |
| `REQ-SEQ-*`        | Sequence and producer specification           |
| `REQ-FN-*`         | Free-function specification                   |
| `REQ-DIAG-*`       | Diagnostics specification                     |
| `REQ-CONST-*`      | Compile-time evaluation specification         |
| `REQ-ERR-*`        | Failure and totality specification            |
| `REQ-PLAT-*`       | Platform and interoperability architecture    |
| `REQ-NUM-*`        | Numeric policy specification                  |
| `REQ-VOCAB-*`      | Vocabulary document                           |
| `REQ-TEST-*`       | Test traceability matrix and executable tests |

## Behavioral Requirements

### Value Semantics

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

REQ-VAL-014. A materialization operation whose maximum possible result cardinality is not statically guaranteed to fit MUST use an explicit bounded destination collection supplied at the call site. Producer values MUST NOT own result storage or require a result-capacity template parameter.

REQ-VAL-015. The destination supplied to `into` MUST encode the result capacity and result collection type. `into` MUST return a new destination-typed collection and MUST leave both the destination value and producer/input values unchanged.

REQ-VAL-016. `count` MUST return the exact element count for a materialized collection and MUST return a conservative maximum materialization count for a producer or composed producer without traversing an unbounded producer.

REQ-VAL-017. Compile-time cardinality composition MUST use saturating arithmetic: every composed producer cardinality MUST be at most `CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT`.

REQ-VAL-018. Public cljonic regex value types MUST be self-contained, immutable, bounded values in the root `cljonic` namespace, including `Regex`, `RegexMatcher`, `RegexMatch`, and `RegexGroup` where those representations are required.

REQ-VAL-019. cljonic MUST provide persistent value semantics for its collection values: an update MUST return a new independently valid value and MUST leave the prior value unchanged. Persistence describes observable value behavior and MUST NOT require a particular storage algorithm.

REQ-VAL-020. cljonic collection updates MUST use deep copying and MUST NOT use structural sharing. Public behavior MUST NOT expose or depend on shared internal storage between the prior and updated values.

REQ-VAL-021. Stored closures MUST be supported as collection elements and map values in the supported collection contract when their closure types satisfy the required bounded representation, construction, copying, moving, destruction, and non-allocation capabilities. Storing a closure MUST NOT grant it equality, ordering, map-key, set-element, or serialization capabilities.

REQ-VAL-022. A bounded single-threaded `Atom<T>` MUST be supported as an explicit mutable reference to one cljonic value. The atom MUST use automatic or static storage, MUST not provide thread-safety or concurrent mutation, and MUST preserve the stored value's ordinary value semantics.

### Bounds and Default Elements

REQ-BOUNDS-001. Collection access MUST be bounds-checked.

REQ-BOUNDS-002. Every collection MUST define a default element of its value type, normally produced by value-initialization such as `T{}`.

REQ-BOUNDS-003. An access that cannot return a valid element MUST return the collection's default element through the default-returning API.

REQ-BOUNDS-004. The API MUST make it possible to determine before access whether an operation can return a valid element without relying on exceptions or inspecting the value for equality with the default element.

REQ-BOUNDS-005. The library MUST document every operation that may return a default element.

REQ-BOUNDS-006. Bounds failures, capacity failures, and missing-key results MUST have distinct documented semantics, even when more than one uses a default-returning convenience API.

REQ-BOUNDS-007. For every default-returning access operation, the API MUST provide a non-throwing, non-allocating way to determine before access whether the operation can return a valid element.

REQ-BOUNDS-008. Pre-access predicates MUST be defined in terms of the operation's domain and MUST distinguish empty collections or sequences, invalid indexes, missing keys, and full-collection capacity state where those conditions apply.

REQ-BOUNDS-009. For an immutable collection value, a successful pre-access predicate and the corresponding access operation MUST agree: if the predicate reports that a valid element is available, the access MUST return that element; otherwise the access MUST follow its documented default-element semantics.

REQ-BOUNDS-010. The API MUST provide `empty` for collection and sequence underflow checks, `full` or an equivalent capacity inspection for bounded insertion operations, `valid_index` for indexed access, and membership or key-presence checks for key-based access where those capabilities apply.

REQ-BOUNDS-011. A pre-access predicate MUST NOT inspect or compare the accessed value, rely on equality with `T{}`, perform an ambiguous default-returning access, throw, or allocate.

REQ-BOUNDS-012. For every `into` operation whose maximum possible result cardinality can exceed the destination collection's capacity, the API MUST provide the non-throwing, non-allocating preflight predicate `fits_into(destination, producer)`, which determines whether the complete result fits within that destination.

REQ-BOUNDS-013. A result-capacity preflight predicate and the corresponding materializing operation MUST have compatible semantics for immutable inputs. If the predicate reports that the complete result fits, the operation MUST produce the complete result. If the predicate reports that the complete result does not fit, the operation MUST follow its documented bounded-result behavior, which MAY return a deterministic prefix limited to its compile-time capacity.

REQ-BOUNDS-014. Result-capacity preflight predicates MUST measure the same result quantity and apply the same matching, filtering, transformation, or overflow semantics as their corresponding materializing operations.

REQ-BOUNDS-015. An `into` operation MUST document whether its result is complete or may be a bounded prefix when the destination capacity is insufficient. If it may be a bounded prefix, it MUST preserve the defined result order and MUST provide a preflight predicate that determines whether the complete result fits.

REQ-BOUNDS-016. `into` MUST return the destination collection type directly. Ordinary collection values MUST NOT carry hidden completeness state, and cljonic MUST NOT require a materialization-result wrapper for partial-prefix status; callers MUST use `fits_into` before `into` when completeness matters.

REQ-BOUNDS-017. Every supported operation whose complete result can fail because of capacity, representability, cardinality, matching, filtering, transformation, or another documented runtime condition MUST define its bounded-result, default-result, or failure policy. When callers need to distinguish complete success from that default, bounded, partial, or failed result, the operation MUST provide a corresponding non-throwing, non-allocating preflight predicate. The predicate MUST measure the same result quantity and apply the same capacity, representability, cardinality, matching, filtering, transformation, and failure semantics as the operation. `fits_into` MUST remain the canonical preflight for complete producer materialization into an explicit destination; operation-specific predicates MAY be used for other operations.

### Collections

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

REQ-COLL-014. The public C++ string type MUST be named `cljonic::String<N>`, where `N` is the maximum content length in bytes and excludes the null terminator.

REQ-COLL-015. The API MUST support explicit-capacity string-literal construction such that `cljonic::String<N>{literal}` is valid only when the literal's content length is no greater than `N`.

REQ-COLL-016. An explicit-capacity string-literal construction whose literal content length exceeds `N` MUST fail at compile time.

REQ-COLL-017. The API MUST support capacity-inferred string-literal construction such that `cljonic::String{literal}` has the same semantics and type as `cljonic::String<content_length>{literal}`, where `content_length` excludes the literal's null terminator.

### Sequences and Traversal

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

REQ-SEQ-015. Equality between finite sequenceable values MAY compare their elements in sequence order when the element types satisfy deep equality. Equality MUST NOT begin an unbounded traversal.

REQ-SEQ-016. A sequenceable value that is statically known to be unbounded MUST NOT satisfy the default deep-equality capability. Ordinary equality involving such a value MUST fail at compile time rather than execute indefinitely.

REQ-SEQ-017. Deep equality for a nested value MUST require finite-observation capability recursively at every nested level. A finite outer collection MUST NOT acquire deep equality when any nested value requires unbounded traversal for equality.

REQ-SEQ-018. Finite nesting of bounded owning collections and producer values MUST be supported when each value satisfies its storage and capability requirements. The existence of nested producer values MUST NOT cause implicit materialization of those producers.

REQ-SEQ-019. No operation MUST implicitly recursively materialize or traverse an unbounded producer nested inside an owning collection or another producer. Bounded inspection or materialization of nested producers MUST require explicit caller-selected bounds at each materialized level.

REQ-SEQ-020. The library MUST distinguish finite nesting depth from unbounded cardinality. The bounded owning value model MUST NOT require support for genuinely self-referential infinite structural nesting; such support would require a separately approved indirection or lazy-reference capability.

REQ-SEQ-021. Operations that produce sequenceable results SHOULD return an owning bounded cljonic value when a useful finite result capacity can be derived from the operation and its inputs. Operations whose results are unbounded, or for which no useful finite capacity can be derived without imposing an arbitrary caller-independent limit, SHOULD return or expose an explicit producer value instead. A runtime-varying result count alone MUST NOT require producer semantics when a safe compile-time capacity bound exists. Producer values MUST NOT own materialized result storage or hide an arbitrary result-capacity limit; complete materialization MUST use an explicit bounded destination through `into`, with `fits_into` providing the completeness preflight where required.

REQ-SEQ-022. The first-pass collection-shaping and traversal family MUST include bounded operations corresponding to `take`, `drop`, `take_while`, `drop_while`, `take_last`, `drop_last`, `take_nth`, `nth`, `nthnext`, `nthrest`, `butlast`, `map_indexed`, `rseq`, `second`, `ffirst`, `fnext`, `nfirst`, `nnext`, `some`, `is_every`, `not_any`, `not_every`, `distinct`, `dedupe`, `frequencies`, `reductions`, `split_at`, `split_with`, `mapcat`, `interleave`, `interpose`, `partition`, `partition_all`, `partition_by`, `partitionv`, `partitionv_all`, `group_by`, `flatten`, `tree_seq`, `keep`, `keep_indexed`, `remove`, `replace`, `mapv`, `filterv`, `subvec`, `find`, `reduce_kv`, `sort`, and `sort_by`. Each operation MUST define its callback, termination, equality, ordering, nested-result, producer, capacity, typed-absence, and bounded-result behavior as applicable. Operations MUST preserve inputs and obey the applicable stable-equality, total-order, callback-purity, unordered map/set, and `REQ-BOUNDS-017` requirements. Transducer-only arities and hidden lazy-sequence machinery remain unsupported.

### Free Functions and Functional Operations

REQ-FN-001. The primary user-facing operations MUST be free functions rather than requiring users to learn collection-specific member APIs.

REQ-FN-002. The supported free-function vocabulary MUST include at least the functions listed in the API Vocabulary Inventories, subject to each entry's lifecycle status and the capabilities of each input. Only entries classified as `requirements-backed` constitute supported behavior; `candidate`, `deferred`, and `excluded` entries MUST NOT be presented as supported functions.

REQ-FN-002A. For a nonempty map, `first` MUST return one map-entry value. For a map-entry value, `first` MUST return its key, `last` MUST return its value, `key` MUST return its key, and `val` MUST return its value. These operations MUST compose so that `first(first(map))` returns the key and `last(first(map))` returns the value.

REQ-FN-002B. `last` applied to a map MAY return the final map-entry value in the implementation's traversal order, but callers MUST NOT rely on which entry is returned because ordinary map traversal order is semantically unordered.

REQ-FN-002C. The canonical named comparison functions MUST use full descriptive names: `equal`, `not_equal`, `less`, `less_equal`, `greater`, and `greater_equal`. Short aliases such as `eq`, `neq`, `lt`, `lte`, `gt`, or `gte` MUST NOT be required by the supported API.

REQ-FN-002D. Where the semantics and capabilities permit, binary comparisons SHOULD also be exposed through the corresponding native C++ operators `==`, `!=`, `<`, `<=`, `>`, and `>=`. Named functions MUST remain available for generic, constrained, or variadic use.

REQ-FN-002E. `equal` MUST represent general value equality, including recursively defined finite collection equality. Numeric equality MUST be a separately specified capability or operation and MUST NOT be inferred solely from the existence of general value equality.

REQ-FN-002F. Clojure's `=` MUST map conceptually to cljonic general equality, while Clojure's numeric `==` MUST map conceptually to a separately specified numeric-equality operation. The C++ spelling `=` MUST NOT be introduced as a cljonic function because it is assignment syntax.
REQ-FN-002G. `map` MUST accept its transforming function as the first argument, followed by one or more compatible source collections. Multi-source `map` MUST invoke the function with corresponding values from the source collections according to its documented termination and capacity policy.

REQ-FN-002H. `comp` MUST accept zero or more compatible element functions and MUST compose them into one callable value. Zero functions MUST produce an identity callable, and one function MUST produce an equivalent callable. For functions supplied as `comp(f, g, h, ...)`, compile-time checking MUST verify from right to left that each function accepts the return type of the function to its right; the composed function MUST have the rightmost function's input type and the leftmost function's return type. The resulting callable MUST apply the functions from right to left, representing `f(g(h(value)))` for three functions. `comp` MUST reject incompatible parameter and return types at compile time and MUST NOT provide transducer composition or destination-independent collection-processing semantics.

REQ-FN-002I. `deref` MUST read the current value of an `Atom<T>`, `reset` MUST replace it with a value assignable to `T`, and `swap` MUST synchronously invoke its update function with the current `T` value and replace the atom with a result assignable to `T`. The `swap` function MUST be checked at compile time for compatible parameter and return types, MUST be evaluated exactly once per call, and MUST NOT use retry or compare-and-set semantics.

REQ-FN-002J. Runtime text parsing MUST provide `can_parse_int` and `parse_int` for the supported fixed-width integer target, `can_parse_float` and `parse_float` for the supported floating-point target, and `can_parse_bool` and `parse_bool` for documented boolean spellings. Each parser MUST be non-throwing and non-allocating; each `can_parse_*` predicate MUST agree with its parser, which MUST return its documented default value when the predicate is false.

REQ-FN-002K. `fits_print(destination, value, keyword_enum_name_maps...)` MUST report whether the complete debug representation of a supported value fits in the destination, and `print_to(destination, value, keyword_enum_name_maps...)` MUST return the destination collection type directly. Both functions MUST accept zero or more `KeywordEnumNameMap` values, MUST match supplied maps by enum type rather than argument position, and MUST apply the supplied maps recursively while formatting nested supported values. Zero maps MUST preserve default formatting. If `fits_print` is false, `print_to` MUST follow its documented bounded-prefix behavior; ordinary values MUST NOT carry hidden formatting-completeness state.

REQ-FN-002L. The public `view(collection)` free function MUST return the appropriate non-owning, read-only standard view for every supported collection type: `std::span<const T>` for `Vector<T, N>`, `Set<T, N>`, and `Queue<T, N>`; `std::span<const MapEntry<K, V>>` for `Map<K, V, N>`; and `std::string_view` for `String<N>`. The view MUST expose the active logical elements and MUST preserve the source collection's traversal semantics.

REQ-FN-002M. `can_conj(collection, value)` MUST return true when `conj` can produce its documented result without capacity failure, including when a set already contains the value. `can_assoc(map, key, value)` MUST return true when the key already exists because `assoc` replaces its value without consuming capacity, and MUST return true for a new key only when capacity is available.

REQ-FN-002N. The supported free-function vocabulary MUST include `can_add`, `can_subtract`, and `can_multiply` for checked fixed-width arithmetic preflight. The corresponding arithmetic functions MUST use the documented checked default behavior; any wrapping or saturating alternatives MUST use distinct names.

REQ-FN-002O. `KeywordEnumNameContext` MAY bundle zero or more `KeywordEnumNameMap` values for reuse with `fits_print` and `print_to`, but it MUST preserve the same matching, fallback, capacity, allocation, and output semantics as passing the maps directly. A context MUST be type-safe, MUST NOT require dynamic storage or type erasure, and MUST reject duplicate maps for the same enum type at compile time where the duplication is knowable from the types.

REQ-FN-002P. The callable `Map<K, V, N>` lookup forms specified by `REQ-COLL-004B` MUST be equivalent to the corresponding `get` overloads for the same map, key, value, and fallback arguments. `operator[]` MUST NOT be required or provided as the map lookup syntax because its conventional insertion semantics conflict with cljonic's immutable bounded-map contract.

REQ-FN-002Q. The callable `Vector<T, N>` lookup forms specified by `REQ-COLL-002A` MUST be equivalent to the corresponding `get` overloads for the same vector, index, element type, and fallback arguments. `valid_index(vector, index)` MUST be non-throwing, non-allocating, and consistent with both callable lookup forms and indexed access. `operator[]` MUST NOT be required or provided as the vector lookup syntax because its conventional unchecked-access semantics conflict with cljonic's bounds-checked contract.

REQ-FN-002R. The callable `Set<T, N>` lookup forms specified by `REQ-COLL-005B` MUST be equivalent to the corresponding `get` overloads for the same set, value, and fallback arguments. Set callable lookup MUST use the same stable equality capability and bounded linear scan as `contains`; it MUST NOT provide a boolean-returning `operator()` overload because `contains(set, value)` is the canonical membership predicate.

REQ-FN-003. Generic free functions MUST be constrained by explicit concepts or equivalent compile-time requirements.

REQ-FN-004. Unsupported operations MUST fail at compile time with useful diagnostics where the limitation is knowable from the types.

REQ-FN-005. Functional operations MUST preserve input values.

REQ-FN-006. `map`, `filter`, and similar transformations MUST have a documented capacity policy when the result can exceed its target capacity.

REQ-FN-007. Functions MUST be composable across compatible collection and sequence types.

REQ-FN-008. `map`, `filter`, `reduce`, and similar higher-order operations MUST preserve the purity and input-preservation guarantees of the library when provided callbacks that are themselves pure and non-allocating. The library MUST NOT introduce side effects or hidden mutable state independently of those callbacks.

REQ-FN-009. `cycle`, `iterate`, `range`, `repeat`, and `repeatedly` MUST be standalone producer values or producer operations that do not own materialized result storage or require a result-capacity template parameter. Their values MUST be materialized through `into` into an explicit bounded destination. Unbounded forms MUST produce at most the destination capacity and MAY return a deterministic prefix; finite forms MUST produce their complete result when it fits the destination.

REQ-FN-010. For `cycle`, `iterate`, `range` without a finite end, `repeat` without a count, and `repeatedly` without a count, complete-result preflight MUST report that the result does not fit unless the operation terminates before reaching its unbounded behavior. Counted `repeat` and `repeatedly`, and finite `range`, MUST use their runtime result count for preflight and bounded `into` materialization.

REQ-FN-011. `Range`, `Repeat`, `Cycle`, `Iterate`, and `Repeatedly` producer values MUST own their parameters and MUST remain valid independently of other values. They MUST NOT allocate result storage or retain borrowed dependencies on source collections, callbacks, or input values.

REQ-FN-012. `range` MUST use an inclusive start and exclusive end, default start `0`, default step `1`, and MUST produce an infinite repetition of `start` when `step` is zero. A nonzero step that moves away from the end MUST produce an empty finite range, and equal start and end MUST produce an empty range when the step is nonzero.

REQ-FN-013. For `range` with both equal start and end and a zero step, zero-step infinite repetition MUST take precedence over empty-range behavior.

REQ-FN-014. Callbacks supplied to `iterate` and `repeatedly` MUST be pure and non-allocating. Counted forms MUST invoke a callback exactly once per produced element during each `into` call when materialized completely; uncounted forms MUST be treated as potentially infinite producers.

REQ-FN-014C. Producer construction MUST NOT evaluate producer callbacks. During each `into` call, an element-producing callback MUST be evaluated as needed for each produced element, and repeated `into` calls over the same producer MUST repeat those callback evaluations rather than reuse a hidden realization cache.

REQ-FN-014A. The unbounded-equality restriction MUST apply to every producer form that can produce an unbounded sequence, including open-ended `range`, `cycle`, uncounted `repeat`, uncounted `repeatedly`, and unbounded `iterate`, regardless of whether two such producers currently appear to produce the same values.

REQ-FN-014B. A separate explicitly named structural-comparison operation MAY compare the parameters of unbounded producers, but structural comparison MUST NOT be exposed as ordinary sequence equality and MUST NOT imply that two producers have equal materialized sequences.

REQ-FN-015. Single-match regex functions corresponding to `re_find` and `re_matches` MUST return an owning `String` and MUST return the empty `String` when no match exists.

REQ-FN-016. Single-match regex functions MUST provide corresponding non-throwing, non-allocating preflight predicates, such as `has_re_find` and `has_re_matches`, that determine whether a match exists. A valid zero-length match MUST be distinguishable from absence only through the corresponding predicate.

REQ-FN-017. Compile-time regex construction MUST use the root `cljonic::Regex` type with braced literal syntax such as `cljonic::Regex{"^[A-Z]+$"}`. The pattern MUST be parsed and validated during constant evaluation, and invalid compile-time patterns MUST fail with an actionable diagnostic.

REQ-FN-018. Runtime regex construction MUST be provided through `cljonic::core::re_pattern` using bounded cljonic string input and MUST use fixed bounded storage, require no dynamic allocation or exceptions, and follow the same documented regex API surface and matching semantics as compile-time patterns.

REQ-FN-019. Clojure core regex functions MUST be provided in `cljonic::core` with hyphens translated to underscores, including `re_find`, `re_seq`, `re_matches`, `re_pattern`, `re_matcher`, and `re_groups`.

REQ-FN-020. Regex capture results MUST be owning bounded values. A multi-group match MUST expose its full match and captures through a bounded `Vector` of owning `String` values, and MUST NOT return views or references into the input string.

REQ-FN-021. Functions corresponding to Clojure nil-returning operations MUST use typed cljonic absence semantics: single element or value results return their documented default value; collection results return an empty bounded collection where appropriate; boolean predicates such as `some` return `false` when no result exists; and single-match regex functions return an empty owning `String` with presence determined by their preflight predicates.

REQ-FN-022. The first-class bounded string convenience layer MUST include ASCII-preserving or ASCII-transforming operations corresponding to `is_blank`, `capitalize`, `ends_with`, `escape`, `includes`, `index_of`, `last_index_of`, `lower_case`, `re_quote_replacement`, `reverse`, `starts_with`, `trim`, `trim_newline`, `triml`, `trimr`, and `upper_case`, subject to the documented `String` capacity and ASCII constraints. These operations MUST return owning bounded values where they produce text, MUST preserve input values, and MUST use deterministic non-throwing behavior. Operations corresponding to `join`, `replace`, `replace_first`, `split`, and `split_lines` MUST also be supported as bounded string convenience operations when their destination string or collection capacities are explicit and their complete-result preflight behavior is documented. Any result-growing or multi-result operation MUST obey `REQ-BOUNDS-017`.

REQ-FN-023. The first-class bounded set algebra layer MUST include binary `union`, `intersection`, and `difference` operations over compatible `Set<T, N>` and `Set<T, M>` inputs, returning owning bounded `Set<T, max(N, M)>` values, plus `is_subset` and `is_superset` predicates returning `bool`. The derived result capacity MUST be a compile-time value and MUST remain within `CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT`; a statically invalid derived capacity MUST fail at compile time. Set algebra MUST require the stable equality capability applicable to set membership, MUST preserve the semantic unorderedness of set traversal, and MUST leave all input sets unchanged.

`fits_set_algebra(left, right)` MUST be the non-throwing, non-allocating complete-result preflight for the result-producing set algebra operations. `intersection` and `difference` MUST produce complete results within the derived capacity. When `union` has more distinct values than the derived capacity, `union` MUST return a deterministic bounded prefix containing the first `max(N, M)` distinct values encountered by its defined operand traversal, and `fits_set_algebra` MUST return `false`; the returned set remains semantically unordered and no completeness state may be stored in it. Result-producing algebra operations MUST otherwise obey `REQ-BOUNDS-017`; relational map operations such as `index`, `project`, `rename`, and `join` remain outside this requirement until an explicit relation model is approved.

REQ-FN-024. The relational map/set operations `index`, `project`, `rename`, and `join` MUST NOT be treated as supported public operations until an explicit relation model is approved. That model MUST define the relation row representation, key and value capabilities, duplicate-row and duplicate-key semantics, nested result representation, traversal-order semantics for unordered inputs and results, capacity arithmetic, complete-result preflight, and bounded failure behavior. A function's correspondence to a Clojure namespace symbol or its apparent implementability in C++ MUST NOT by itself satisfy this gate.

REQ-FN-025. The first-pass callable convenience family MUST include `identity`, `constantly`, `complement`, `partial`, `fnil`, `juxt`, `every_pred`, `some_fn`, and `apply`, in addition to the `comp` behavior defined by this document. Each callable constructor or adapter MUST express compatible parameter and return types through concepts or equivalent compile-time constraints, MUST return a bounded callable representation when it returns a callable, and MUST preserve the invocation and non-allocation requirements of its captured functions and values. `juxt`, `every_pred`, and `some_fn` MUST define their short-circuit, result-shape, and invocation-order behavior; `partial` and `fnil` MUST define their captured-argument and default-argument behavior; and `apply` MUST define its supported final sequence argument and capacity behavior. These functions MUST remain ordinary callable operations and MUST NOT provide transducer semantics, runtime type dispatch, hidden mutable caching, or implicit callback retention by collection results.

### Compile-Time Diagnostics

REQ-DIAG-001. Public templates and free functions MUST express knowable compile-time capability requirements through named concepts or equivalent constraints.

REQ-DIAG-002. Compile-time-invalid uses MUST fail at the public API boundary with diagnostics that identify the violated cljonic capability, capacity, or value constraint.

REQ-DIAG-003. The library SHOULD use targeted `static_assert` diagnostics for context-dependent compile-time failures that cannot be expressed clearly through concepts or equivalent constraints.

REQ-DIAG-004. Diagnostic requirements MUST specify the meaning and relevant constraint of a diagnostic rather than depend on compiler-specific wording.

REQ-DIAG-005. Public concept names MUST be precise, capability-oriented, and understandable when rendered in compiler diagnostics. Concept names MUST identify the required capability or constraint and MUST NOT rely on vague names such as `Valid`, `Supported`, or `Allowed` without further qualification.

REQ-DIAG-006. Materialization APIs with runtime-unknown result cardinality SHOULD provide an implementation-defined compiler warning or equivalent tooling diagnostic when the caller uses the default maximum capacity, recommending an explicit call-site capacity. Such a warning MUST NOT replace the required capacity parameter or depend on non-portable diagnostics for correctness.

### Compile-Time Evaluation

REQ-CONST-001. Collection construction and non-allocating operations SHOULD be `constexpr` when the value types and compiler permit it.

REQ-CONST-002. Operations that are required to be compile-time evaluable MUST be tested in constant expressions.

REQ-CONST-003. `consteval` MUST be used only where compile-time execution is semantically required and does not unnecessarily restrict valid embedded use.

REQ-CONST-004. Compile-time and runtime evaluation MUST produce equivalent observable results.

### Error and Failure Policy

REQ-ERR-001. Supported collection operations MUST NOT require exceptions.

REQ-ERR-002. The library MUST NOT require error codes or global mutable error state for ordinary collection access.

REQ-ERR-003. Default-returning convenience functions MUST have documented pre-access predicates that allow callers to determine whether the operation can return a valid element or complete successfully.

REQ-ERR-004. Failure behavior MUST be deterministic and documented for empty collections, missing keys, invalid indexes, full collections, duplicate set insertion, and duplicate map keys. Duplicate set insertion MUST be a successful no-op, and duplicate map-key association MUST replace the existing value. Any pre-access predicate or capacity query provided for these conditions MUST have semantics consistent with the corresponding access or update operation.

REQ-ERR-005. The library MUST NOT invoke undefined behavior for invalid collection access within its documented API.

REQ-ERR-006. Every supported operation whose success depends on runtime values, collection state, capacity, representability, cardinality, or producer behavior MUST comply with `REQ-BOUNDS-017`. When callers need to distinguish complete success from a default, bounded, partial, or failed result, the operation MUST provide a corresponding non-throwing, non-allocating preflight predicate unless a non-throwing, non-allocating checked result directly communicates that distinction. Any preflight predicate or checked result MUST agree with the operation for immutable inputs, and a false or failed outcome MUST produce the operation's documented default, bounded, partial, or failure result.

REQ-ERR-007. Supported cljonic operations MUST be defined for all their documented inputs. They MUST NOT throw, invoke undefined behavior, access storage out of bounds, allocate unexpectedly, silently corrupt state, terminate the process, or depend on hidden mutable error state. Runtime-detectable unsuccessful conditions MUST use the operation-specific documented mechanism: a preflight predicate with a default or bounded result, a checked result, or a compile-time rejection when the condition is knowable from the types or constant expressions.

REQ-ERR-008. Overflow policy MUST be operation-specific but MUST follow one general contract: compile-time-known capacity or representability overflow MUST be rejected at compile time; runtime-detectable overflow MUST provide a non-throwing, non-allocating preflight predicate; the corresponding operation MUST leave its input unchanged when the predicate is false and MUST return its documented default or bounded result. `into` is the explicit partial-prefix exception and MUST use `fits_into` as specified elsewhere.

## Resource and Platform Requirements

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

REQ-PLAT-011. Compile-time regex support MUST use CTRE or an equivalent compile-time regular-expression implementation whose exact integration is audited for compatibility with cljonic's no-allocation, no-exception, bounded-storage, diagnostic, and licensing requirements.

REQ-PLAT-012. The supported execution model MUST be single-threaded. The library MUST NOT require or provide thread-safety, synchronization, atomics, thread-local storage, parallel execution, or concurrent mutation of library-managed state.

REQ-PLAT-013. Use of cljonic within a multithreaded application MAY be possible when the caller externally confines each operation and value access to one thread at a time, but concurrent-access safety MUST NOT be part of the library contract.

REQ-PLAT-014. The platform-interoperability capability MUST support the simple user-defined struct boundary defined by `REQ-PLAT-024` for collection elements and map values.

REQ-PLAT-015. A user-defined aggregate-like struct MUST be eligible as a map key or set element only when it explicitly or otherwise demonstrably provides stable equality required by that operation. Struct storage MUST NOT implicitly make equality available.

REQ-PLAT-016. A user-defined aggregate-like struct MUST be eligible for sorting or ordered traversal only when it explicitly or otherwise demonstrably provides the total-order capability required by that operation. Struct storage MUST NOT implicitly make total ordering available.

REQ-PLAT-017. The platform-interoperability capability MAY provide an API that exposes a cljonic `String` as `std::string_view`. Such a view MUST be non-owning, MUST perform no dynamic allocation, MUST expose exactly the string's stored ASCII content excluding its null terminator, and MUST remain valid only while the source `String` object remains alive and unmodified.

REQ-PLAT-018. A `std::string_view` returned for a cljonic `String` MUST NOT be documented as null-terminated, and callers MUST NOT retain it after the source string's lifetime or after an operation that replaces the source string value.

REQ-PLAT-019. The platform-interoperability capability MUST provide the public `view(collection)` free function for each supported collection type: `Vector`, `Map`, `Set`, `Queue`, and `String`. The function MUST return a non-owning, read-only standard view without dynamic allocation or mutation of the source collection.

REQ-PLAT-020. `view(Vector<T, N>)`, `view(Set<T, N>)`, and `view(Queue<T, N>)` MUST return `std::span<const T>`; `view(Map<K, V, N>)` MUST return `std::span<const MapEntry<K, V>>`; and `view(String<N>)` MUST return `std::string_view`.

REQ-PLAT-021. Standard views returned by `view(collection)` MUST be non-owning and read-only, MUST expose only the active logical range, MUST perform no dynamic allocation, and MUST NOT extend the source collection's lifetime.

REQ-PLAT-022. Every collection view MUST remain valid only while its source collection remains alive and unmodified. Copying a view MUST copy only its bounded view state and MUST NOT extend the source collection's lifetime.

REQ-PLAT-023. A view of a map or set MUST preserve the source collection's semantic iteration contract: repeatable implementation order MAY be exposed, but callers MUST NOT rely on any particular order. A view MUST NOT turn an unordered collection into an ordered one.

REQ-PLAT-024. The supported collection contract MUST support simple user-defined C++ structs with public data members, no user-defined methods, no private data, and non-allocating value operations as collection elements and map values. Arbitrary user-defined classes, class hierarchies, and general external containers MUST NOT be required to participate in cljonic operations.

REQ-PLAT-025. Destructuring of supported tuple-like values and simple aggregate structs MUST use ordinary C++ structured bindings or equivalent language-supported positional decomposition. The binding layout MUST be determined from the value type at compile time, while value reads follow normal C++ evaluation rules.

REQ-PLAT-026. cljonic MUST NOT provide a separate Clojure-style map-key destructuring syntax, macro, or code-generation facility. Map destructuring MUST use explicit cljonic access functions such as `get`, `contains`, and the applicable pre-access predicates; defaults and absence MUST follow the existing typed absence rules.

REQ-PLAT-027. cljonic MUST NOT provide runtime `Symbol` values. C++ namespaces, cljonic namespaces, and statically named free functions MUST provide the supported naming model; runtime symbols, runtime namespace resolution, reader integration, macro integration, and evaluator integration are outside the library boundary.

REQ-PLAT-028. Application-defined scoped enumeration types declared with `enum class` MUST be supported as cljonic map keys and set elements when their fixed underlying representation provides stable equality. A dedicated cljonic `Keyword` type or keyword concept MUST NOT be required.

REQ-PLAT-029. Application guidance SHOULD recommend a globally scoped `enum class Keywords` when an application has one shared vocabulary of configuration or data keys. The enumerators MUST define the application's supported keyword set at compile time; cljonic MUST NOT provide runtime keyword registration, parsing, interning, or namespace resolution.

REQ-PLAT-030. cljonic MUST NOT provide runtime or attached metadata for collection or scalar values. Metadata is outside the embedded collection-and-free-function scope because it adds storage, copying, equality, and interoperability complexity without providing a required embedded capability.

REQ-PLAT-031. cljonic MUST be a C++ header-only library and MUST NOT provide a Clojure reader, EDN parser, reader conditionals, macro system, syntax-quoting system, evaluator, or runtime namespace-resolution system. Clojure-like data and operations MUST be expressed through the supported C++ API and compile-time language facilities.

REQ-PLAT-032. cljonic MUST NOT provide runtime type inspection, reflection, runtime class or hierarchy queries, or runtime dynamic dispatch based on type identity. Supported type capabilities MUST be expressed through compile-time concepts, traits, and constraints without requiring RTTI.

REQ-PLAT-033. cljonic MUST provide bounded human-readable debug formatting for supported built-in values through an explicit fixed-capacity destination. Formatting MUST NOT allocate dynamically, require iostreams, throw exceptions, depend on RTTI, or use hidden mutable state.

REQ-PLAT-034. Formatting of supported built-in collections SHOULD use readable Clojure-like delimiters where practical, but it MUST be treated as debug output rather than stable serialization. Map and set formatting MUST NOT promise a particular element order.

REQ-PLAT-035. Application-defined enum keys MUST be formatted using their underlying numeric values by default. cljonic MUST NOT infer enum names through reflection or provide runtime symbolic-name registration, parsing, interning, namespace resolution, or serialization. An explicitly supplied `KeywordEnumNameMap` MAY provide bounded human-readable debug names for selected enum values without changing enum identity or map-key semantics.

REQ-PLAT-036. The supported cljonic API MUST NOT require direct support for external standard-library types such as `std::array`, `std::tuple`, `std::pair`, `std::optional`, or `std::variant` as collection or free-function inputs or outputs. A supported simple aggregate struct MAY contain fields of those types and MUST be storable as a cljonic value when all required field operations are bounded, non-allocating, and satisfy the applicable construction, copying, moving, destruction, equality, and ordering constraints.

REQ-PLAT-037. Every C++ standard-library facility and transitive execution path actually used by cljonic MUST satisfy the applicable cljonic requirements for the supported configuration, including bounded storage, no dynamic allocation, no required exceptions, no RTTI, no hidden global initialization, and single-threaded behavior. Unused portions of the standard library and unrelated application-level standard-library use are outside this requirement.

REQ-PLAT-038. `KeywordEnumNameEntry` MUST represent one application-defined enum value and one bounded ASCII display name for debug formatting. The display name MUST NOT participate in enum equality or map-key identity.

REQ-PLAT-039. `KeywordEnumNameMap<Enum, EntryCount, NameCapacity>` MUST associate one scoped application enum type with a bounded collection of `KeywordEnumNameEntry` values and one bounded ASCII display name for the enum type. `EntryCount` MAY be zero; a zero-entry map MUST be a valid type-name-only formatting map that provides enum-type fallback output without any enum-to-name entries. The map MUST support `constexpr` construction where the contained values permit it, MUST perform no dynamic allocation, MUST require no exceptions or RTTI, and MUST provide bounded lookup by enum value when entries exist.

REQ-PLAT-040. `make_keyword_enum_name_map` MUST provide a convenience construction path for `KeywordEnumNameMap` and MUST validate entry count, duplicate enum values, name capacity, and documented name-character constraints without dynamic allocation or exceptions. It MUST accept an empty entry collection and construct a valid type-name-only map. Compile-time-known invalid constant construction MUST fail at compile time; runtime construction MUST provide the applicable non-throwing preflight and documented failure behavior.

REQ-PLAT-041. When a supplied `KeywordEnumNameMap` contains a display name for an enum value, debug formatting MUST emit that name with a leading `:`. When a supplied map covers the enum type but has no entry for the value, debug formatting MUST emit the enum type display name followed by the underlying value in the form `EnumTypeName(value)`. When no map covers the enum type, debug formatting MUST use the underlying numeric value. These representations are debug output and MUST NOT be treated as stable serialization.

REQ-PLAT-042. `KeywordEnumNameContext` MUST be capable of bundling zero or more `KeywordEnumNameMap` values without dynamic storage or type erasure. A context MUST be usable anywhere the variadic map arguments to `fits_print` and `print_to` are accepted, and direct variadic map arguments and a context containing the same maps MUST produce equivalent output and capacity results.

## Numeric Domain Requirements

REQ-NUM-000. Raw floating-point calculations and comparisons MUST be treated as potentially non-deterministic across values, compiler settings, evaluation modes, and platforms because of rounding, representation, NaN, infinity, signed zero, and other IEEE-754 effects. Raw floating-point behavior MUST NOT be used for a stable semantic contract unless an explicit numeric policy defines and tests that behavior.

REQ-NUM-001. Raw floating-point types MAY be stored as scalar values, vector elements, or map values when the selected operation does not require semantic equality or ordering, because those storage operations do not claim a stable floating-point result or identity contract.

REQ-NUM-002. Raw floating-point types MUST NOT be accepted as map keys or set elements because rounding, representation, NaN, infinity, and signed-zero behavior can make equality unsuitable as a stable identity across supported environments.

REQ-NUM-003. `range` MUST require an integral or otherwise explicitly discrete numeric type. Raw floating-point bounds or steps MUST be rejected at compile time because rounding and evaluation differences can change termination and produced values.

REQ-NUM-004. Any operation that requires equality or ordering MUST express that requirement through a named capability or equivalent compile-time constraint. Raw floating-point types MUST NOT satisfy the default stable-equality or total-order capabilities because their results and comparisons may vary across supported environments.

REQ-NUM-005. Sorting or other ordered traversal of raw floating-point values MUST fail at compile time because NaN, signed zero, rounding, and platform-dependent evaluation can prevent a stable total order. A future explicitly configured total-order numeric wrapper MAY provide the required capability, but raw `float` and `double` MUST remain unsupported by the default policy.

REQ-NUM-006. Equality, distinctness, frequency counting, or other identity operations over raw floating-point values MUST be rejected unless an explicitly approved numeric policy provides the required semantics, because rounding and special-value behavior can make those results non-deterministic. No implicit epsilon comparison or NaN normalization policy is provided.

REQ-NUM-007. Collection operations that compare values MUST apply the same numeric capability constraints recursively to their element, key, and value types. In particular, equality of collections containing raw floating-point values MUST be rejected by default even when the collection itself can store those values, because nested floating-point comparisons may not be stable across supported environments.

REQ-NUM-008. Numeric conversion operations MUST have an explicit target type and MUST NOT select their result type through a global configuration constant. A cljonic equivalent of Clojure `int` MUST return a fixed-width signed 32-bit integer type when the conversion succeeds.

REQ-NUM-009. A default numeric conversion MUST reject at compile time any statically known conversion that loses range, precision, or accuracy. A statically known conversion that is provably exact and representable MAY be accepted in a constant expression.

REQ-NUM-010. When conversion safety cannot be determined at compile time, the library MUST provide an explicitly checked, non-throwing runtime conversion whose result communicates success or failure without dynamic allocation or global mutable error state. The checked result type and failure states MUST be documented before implementation.

REQ-NUM-011. Potentially lossy conversion from a runtime floating-point value to an integer MUST NOT be performed by an implicitly narrowing overload because rounding and representation differences can change the converted result. It MUST require an explicitly checked or explicitly lossy conversion operation named to make the policy visible at the call site.

REQ-NUM-012. Explicitly lossy numeric conversions MAY be provided as separate operations, but their truncation, saturation, wrapping, NaN, infinity, out-of-range, and signedness behavior MUST be documented and tested. They MUST NOT be the default conversion behavior.

REQ-NUM-013. Numeric conversion and text parsing MUST be separate operations. A cljonic equivalent of Clojure `int` MUST accept numeric or character inputs according to its documented source capabilities and MUST NOT implicitly parse a `String`; string-to-integer conversion MUST use a separately named parsing operation with deterministic failure behavior.

REQ-NUM-014. `parse_float` MUST document its accepted syntax, sign, decimal, exponent, infinity, NaN, range, rounding, and accuracy behavior. The parser MUST NOT make raw floating-point parsing suitable for stable equality, ordering, or identity contracts without the explicit numeric policy required elsewhere.

REQ-NUM-015. Fixed-width arithmetic MUST use checked behavior by default. Runtime arithmetic operations MUST provide corresponding preflight predicates such as `can_add`, `can_subtract`, and `can_multiply`; when the predicate is false, the operation MUST return its documented default and leave its inputs unchanged. Compile-time-known arithmetic overflow MUST be rejected during constant evaluation.

REQ-NUM-016. Wrapping and saturating arithmetic, if provided, MUST be separately named operations and MUST NOT be implicit behavior of the default arithmetic operations.

REQ-NUM-017. The first-pass numeric convenience family MUST include checked fixed-width arithmetic corresponding to `add`, `sub`, `mult`, `div`, `quot`, `rem`, and `mod`, with `can_add`, `can_subtract`, and `can_multiply` preflights; scalar comparison and selection corresponding to `compare`, `min`, `max`, `min_key`, and `max_key`; integer-step operations corresponding to `inc`, `dec`, and `negate`; constrained numeric predicates corresponding to `is_zero`, `is_positive`, `is_negative`, `is_even`, and `is_odd`; explicit conversions corresponding to `to_int`, `to_long`, `to_float`, `to_double`, `to_num`, `to_char`, `to_byte`, and `to_short`; and fixed-width bitwise operations `bit_not`, `bit_and`, `bit_or`, `bit_xor`, `bit_and_not`, `bit_clear`, `bit_set`, `bit_flip`, `bit_test`, `bit_shift_left`, `bit_shift_right`, and `unsigned_bit_shift_right`. These operations MUST obey the applicable compile-time capability, representability, overflow, division-by-zero, shift-count, floating-point, and preflight requirements. Raw floating-point values MUST remain excluded from operations that require the stable equality, identity, or total-order capabilities defined by this document.

## API and Vocabulary Requirements

REQ-VOCAB-001. The canonical terms MUST include collection, sequence, sequenceable, traversal, vector, map, set, queue, string, capacity, default element, valid index, `valid_index`, persistent value, free function, bounded storage, platform interoperability, aggregate-like struct, stable equality, total order, discrete numeric type, numeric policy, owning value, non-owning view, standard view type, bounded result, partial result, preflight predicate, exact conversion, checked conversion, lossy conversion, parsing, finite observation, finite deep equality, bounded inspection, unbounded producer, producer materialization, relation model, `MapEntry`, general equality, numeric equality, semantic predicate name, state predicate, verb predicate, capability predicate, `is_` predicate prefix, `can_` predicate prefix, `has_` predicate prefix, `valid_` predicate prefix, lifecycle classification, `candidate`, `deferred`, `excluded`, `requirements-backed`, `KeywordEnumNameEntry`, `KeywordEnumNameMap`, `KeywordEnumNameContext`, and keyword enum name mapping.

REQ-VOCAB-002. Each canonical term MUST have one meaning in public documentation, requirements, tests, and code.

REQ-VOCAB-003. Clojure-inspired names MAY be used when their behavior is documented in C++ terms.

REQ-VOCAB-004. The public API MUST prefer a small set of orthogonal capabilities over a deep hierarchy of collection-specific interfaces.

REQ-VOCAB-005. The public API MUST expose capacity and failure policy clearly enough that a caller can reason about resource use from source code.

REQ-VOCAB-006. Public C++ collection type names MUST use PascalCase, including `Vector`, `Map`, `Set`, `Queue`, and `String`.

REQ-VOCAB-007. Clojure-inspired public C++ function names MUST replace hyphens with underscores, so a name such as `last-index-of` is exposed as `last_index_of`. A terminal Clojure question mark MUST be expressed semantically: state or adjectival predicates MUST use an `is_` prefix, while readable verb predicates MUST retain their direct verb form. Capability and preflight predicates MUST retain established `can_`, `has_`, and `valid_` prefixes. Thus `empty?`, `zero?`, `pos?`, `neg?`, `even?`, `odd?`, `blank?`, `subset?`, and `superset?` map to `is_empty`, `is_zero`, `is_positive`, `is_negative`, `is_even`, `is_odd`, `is_blank`, `is_subset`, and `is_superset`; `contains?`, `starts-with?`, `ends-with?`, and `includes?` map to `contains`, `starts_with`, `ends_with`, and `includes`. Namespace layout is a separate API design decision.

REQ-VOCAB-008. Public cljonic namespace names SHOULD mirror the corresponding Clojure namespace names using lowercase C++ namespace identifiers and underscores where separators are needed, so `clojure.core` maps conceptually to `cljonic::core` and `clojure.string` maps conceptually to `cljonic::string`.

REQ-VOCAB-009. The public cljonic API MUST be developed as a broad Clojure-inspired convenience surface rather than as a minimal embedded collection subset. The broad-surface goal MUST remain subordinate to the library's bounded-resource, no-allocation, no-exception, value-semantic, single-threaded, and compile-time-capability requirements. It MUST NOT require reproducing Clojure runtime features, supporting arbitrary external C++ types, or expanding the closed cljonic vocabulary without an approved requirement.

REQ-VOCAB-010. The first-class cljonic value and data-structure domain MUST be closed to the supported bounded `Vector`, `Map`, `Set`, `Queue`, and `String` values, explicit producer values, bounded regex values and match results required by the text surface, and application-defined scoped enumeration values when used as supported map keys or set elements. Public operations MUST NOT be required to accept or return arbitrary external containers, runtime type extensions, or protocol-style user-defined collection participation. Simple user-defined aggregate-like structs MAY participate where the applicable platform, storage, equality, ordering, and other capability requirements explicitly permit them; such participation MUST NOT expand the collection vocabulary.

REQ-VOCAB-011. Every public function considered during API-surface review MUST have one explicit lifecycle classification: `candidate`, `deferred`, `excluded`, or `requirements-backed`. A `candidate` classification MUST indicate feasibility under review and MUST NOT imply approved API scope. A `deferred` classification MUST identify a missing scope approval or governing requirements boundary where practical. An `excluded` classification MUST identify the incompatible project boundary or semantic model. A function MAY be classified as `requirements-backed` when its public scope, governing behavior, and resource constraints are approved by stable requirements; architecture and detailed specification work MAY still define overloads, concrete capacity formulas, diagnostics, and edge-case mechanics, provided they preserve those requirements. The requirements document MUST remain authoritative for supported behavior; API-surface classifications MUST NOT silently add or alter requirements.

## API Vocabulary Inventories

These inventories are the authoritative cljonic vocabulary selected for the
core, string, and set namespaces. They record canonical C++ spellings,
lifecycle status, and the requirement family that governs each selected group.
This requirements document is authoritative for all selected vocabulary and
behavior; external namespace inventories or source-vocabulary baselines are
not required to interpret it, and completeness of any external vocabulary does
not imply cljonic support for every symbol.

The status values have the following meaning:

- `requirements-backed`: included in the supported cljonic vocabulary under
	the referenced requirements; architecture and detailed specification work
	MAY remain pending but MUST preserve the approved contract.
- `candidate`: a feasible function or family still under scope or contract
	review; it is not supported behavior.
- `deferred`: intentionally retained for later consideration because a required
	model or contract is missing.
- `excluded`: outside the cljonic boundary or incompatible with its semantic
	model.

### Core Inventory

| Clojure functions or family                                                                              | Canonical cljonic names                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          | Status                | Requirements                                                                                                      |
| -------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ | --------------------- | ----------------------------------------------------------------------------------------------------------------- |
| `seq`, `first`, `last`, `next`, `rest`, `count`, `empty`                                                 | `seq`, `first`, `last`, `next`, `rest`, `count`, `empty`                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         | `requirements-backed` | `REQ-SEQ-001` through `REQ-SEQ-014`, `REQ-FN-002`                                                                 |
| `get`, `assoc`, `dissoc`, `contains`, `key`, `val`                                                       | same names                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       | `requirements-backed` | `REQ-COLL-*`, `REQ-FN-002`                                                                                        |
| `conj`, `peek`, `pop`                                                                                    | same names                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       | `requirements-backed` | `REQ-COLL-*`, `REQ-FN-002`                                                                                        |
| `comp`, `map`, `filter`, `reduce`, `into`                                                                | same names                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       | `requirements-backed` | `REQ-FN-002`, `REQ-FN-002G` through `REQ-FN-009`                                                                  |
| `cycle`, `iterate`, `range`, `repeat`, `repeatedly`                                                      | same names                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       | `requirements-backed` | `REQ-FN-009` through `REQ-FN-014C`                                                                                |
| `deref`, `reset`, `swap`                                                                                 | same names                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       | `requirements-backed` | `REQ-VAL-022`, `REQ-FN-002I`                                                                                      |
| `atom`                                                                                                   | `atom`                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           | `requirements-backed` | `REQ-VAL-022`, `REQ-FN-002`                                                                                       |
| cljonic preflight and views                                                                              | `valid_index`, `can_conj`, `can_assoc`, `fits_into`, `view`                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      | `requirements-backed` | `REQ-BOUNDS-*`, `REQ-FN-002L` and `REQ-FN-002M`, `REQ-PLAT-017` through `REQ-PLAT-023`                            |
| core regex operations                                                                                    | `re_find`, `re_seq`, `re_matches`, `re_pattern`, `re_matcher`, `re_groups`                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       | `requirements-backed` | `REQ-VAL-018`, `REQ-FN-015` through `REQ-FN-020`, `REQ-PLAT-011`                                                  |
| collection-shaping and traversal family                                                                  | `take`, `drop`, `take_while`, `drop_while`, `take_last`, `drop_last`, `take_nth`, `nth`, `nthnext`, `nthrest`, `butlast`, `map_indexed`, `rseq`, `second`, `ffirst`, `fnext`, `nfirst`, `nnext`, `some`, `is_every`, `not_any`, `not_every`, `distinct`, `dedupe`, `frequencies`, `reductions`, `split_at`, `split_with`, `mapcat`, `interleave`, `interpose`, `partition`, `partition_all`, `partition_by`, `partitionv`, `partitionv_all`, `group_by`, `flatten`, `tree_seq`, `keep`, `keep_indexed`, `remove`, `replace`, `mapv`, `filterv`, `subvec`, `find`, `reduce_kv`, `sort`, `sort_by` | `requirements-backed` | `REQ-SEQ-021`, `REQ-SEQ-022`, `REQ-BOUNDS-017`; detailed architecture/specification pending                       |
| callable convenience family                                                                              | `identity`, `constantly`, `complement`, `partial`, `fnil`, `juxt`, `every_pred`, `some_fn`, `apply`                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              | `requirements-backed` | `REQ-FN-025`, `REQ-VAL-021`, `REQ-DIAG-*`; detailed architecture/specification pending                            |
| numeric convenience family                                                                               | `add`, `sub`, `mult`, `div`, `quot`, `rem`, `mod`, `can_add`, `can_subtract`, `can_multiply`, `compare`, `min`, `max`, `min_key`, `max_key`, `inc`, `dec`, `negate`, `is_zero`, `is_positive`, `is_negative`, `is_even`, `is_odd`, `to_int`, `to_long`, `to_float`, `to_double`, `to_num`, `to_char`, `to_byte`, `to_short`, `bit_not`, `bit_and`, `bit_or`, `bit_xor`, `bit_and_not`, `bit_clear`, `bit_set`, `bit_flip`, `bit_test`, `bit_shift_left`, `bit_shift_right`, `unsigned_bit_shift_right`                                                                                           | `requirements-backed` | `REQ-NUM-000` through `REQ-NUM-017`, `REQ-FN-002N`, `REQ-BOUNDS-017`; detailed architecture/specification pending |
| additional core candidates                                                                               | `get_in`, `assoc_in`, `update`, `update_in`, `merge`, `merge_with`, `select_keys`, `keys`, `vals`, `zipmap`, `update_keys`, `update_vals`, `vector`, `vec`, `hash_map`, `hash_set`, `is_empty`, `swap_vals`, `reset_vals`, `str`, `pr_str`, `prn_str`, `replicate`                                                                                                                                                                                                                                                                                                                               | `candidate`           | Scope identified; function contracts remain to be added                                                           |
| excluded runtime, macro, transducer, random, concurrency, I/O, reflection, and type-inspection functions | Clojure spellings are not part of the cljonic vocabulary                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         | `excluded`            | `REQ-PLAT-027`, `REQ-PLAT-031`, `REQ-PLAT-032`, `REQ-PLAT-033`                                                    |

The core inventory uses `is_empty`, `swap_vals`, and `reset_vals` as the
canonical C++ spellings for the Clojure names `empty?`, `swap-vals!`, and
`reset-vals!`; all other names follow the semantic predicate naming rule in
`REQ-VOCAB-007`. The requirements MUST not treat the candidate row as
supported until the lifecycle gate in `REQ-VOCAB-011` is satisfied.

### String Inventory

| Clojure functions or family                                | Canonical cljonic names                                                                                                                                                                                          | Status                | Requirements                                                                                |
| ---------------------------------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | --------------------- | ------------------------------------------------------------------------------------------- |
| bounded string value and literal construction              | `String<N>`, `String{literal}`                                                                                                                                                                                   | `requirements-backed` | `REQ-COLL-012` through `REQ-COLL-017`                                                       |
| parsing and parser preflight                               | `parse_int`, `can_parse_int`, `parse_float`, `can_parse_float`, `parse_bool`, `can_parse_bool`                                                                                                                   | `requirements-backed` | `REQ-FN-002J`, `REQ-NUM-013`, `REQ-NUM-014`                                                 |
| bounded formatting                                         | `fits_print`, `print_to`                                                                                                                                                                                         | `requirements-backed` | `REQ-PLAT-033` through `REQ-PLAT-042`, `REQ-FN-002K`                                        |
| regex and match preflight                                  | `re_find`, `re_matches`, `re_seq`, `re_pattern`, `re_matcher`, `re_groups`, `has_re_find`, `has_re_matches`                                                                                                      | `requirements-backed` | `REQ-FN-015` through `REQ-FN-020`, `REQ-PLAT-011`                                           |
| first-class bounded string convenience                     | `is_blank`, `capitalize`, `ends_with`, `escape`, `includes`, `index_of`, `last_index_of`, `lower_case`, `re_quote_replacement`, `reverse`, `starts_with`, `trim`, `trim_newline`, `triml`, `trimr`, `upper_case` | `requirements-backed` | `REQ-FN-022`, `REQ-BOUNDS-017`; detailed architecture/specification pending                 |
| bounded result-growing and multi-result text operations    | `join`, `replace`, `replace_first`, `split`, `split_lines`                                                                                                                                                       | `requirements-backed` | `REQ-FN-022`, `REQ-BOUNDS-017`, `REQ-TEST-075`; detailed architecture/specification pending |
| broader text operations requiring an unapproved text model | remaining `clojure.string` symbols                                                                                                                                                                               | `deferred`            | `REQ-VOCAB-011`; text model or result contract missing                                      |

`escape` and `re_quote_replacement` remain bounded text transformations, not
general serialization or replacement-language runtimes. All text results MUST
remain owning bounded ASCII `String` values or explicitly bounded collections
of those values.

### Set Inventory

| Clojure functions or family                                   | Canonical cljonic names                                            | Status                | Requirements                                                                |
| ------------------------------------------------------------- | ------------------------------------------------------------------ | --------------------- | --------------------------------------------------------------------------- |
| bounded set membership and value operations                   | `contains`, `conj`, `disj`, `count`, `seq`, traversal operations   | `requirements-backed` | `REQ-COLL-005` through `REQ-COLL-005B`, `REQ-SEQ-007` through `REQ-SEQ-014` |
| set algebra                                                   | `difference`, `intersection`, `union`, `is_subset`, `is_superset`  | `requirements-backed` | `REQ-FN-023`, `REQ-BOUNDS-017`                                              |
| map-oriented non-relational conveniences                      | `map_invert`, `rename_keys`, `select`                              | `candidate`           | Scope identified; map-result and capacity contracts remain to be added      |
| relational map/set operations                                 | `index`, `project`, `rename`, `join`                               | `deferred`            | `REQ-FN-024`; explicit relation model required                              |
| remaining `clojure.set` symbols outside the selected families | Clojure spellings are not part of the supported cljonic vocabulary | `excluded`            | `REQ-VOCAB-010`, `REQ-VOCAB-011`                                            |

Set algebra results MUST be owning bounded `Set` values with the derived
capacity and documented bounded-prefix behavior above, MUST preserve semantic
unorderedness, and MUST obey the stable-equality and capacity policies. The
relational row model is not implied by the presence of the set namespace or by
map/set membership support.

## Verification Requirements

REQ-TEST-001. Every behavioral requirement MUST be traceable to at least one test or an explicitly recorded reason why it is not directly testable.

REQ-TEST-002. Tests MUST verify that update operations leave their inputs unchanged.

REQ-TEST-003. Tests MUST cover empty, singleton, boundary-capacity, full-capacity, and over-capacity cases for every collection.

REQ-TEST-004. Tests MUST cover valid and invalid indexes, present and missing keys, duplicate keys with value replacement, duplicate set values with successful no-op insertion, and queue underflow/overflow.

REQ-TEST-004A. Tests MUST verify that `can_conj` returns true for an already-present set value and that `can_assoc` returns true for an existing map key even when the map is full, while `can_assoc` returns false for a new key when the map is full.

REQ-TEST-004B. Tests MUST verify that a map is callable with one and two arguments, that present keys return their associated values, that missing keys return `V{}` or the supplied fallback respectively, that a present key storing `V{}` is distinguishable from a missing key through `contains`, and that callable lookup preserves map immutability, capacity, traversal state, and allocation guarantees. Tests MUST verify equivalence between callable lookup and the corresponding `get` overloads and MUST verify that `operator[]` is not part of the required map lookup API.

REQ-TEST-004C. Tests MUST verify that a vector is callable with one and two arguments, that valid indexes return their elements, that invalid indexes return `T{}` or the supplied fallback respectively, that a valid index storing `T{}` is distinguishable from an invalid index through `valid_index`, and that negative indexes are invalid when the accepted index type can represent them. Tests MUST verify equivalence between callable lookup and the corresponding `get` overloads, bounds-checking, vector immutability, order and traversal preservation, absence of allocation, and that `operator[]` is not part of the required vector lookup API.

REQ-TEST-004D. Tests MUST verify that a set is callable with one and two arguments, that present values return the matching stored element, that absent values return `T{}` or the supplied fallback respectively, and that a present `T{}` value is distinguishable from absence through `contains`. Tests MUST verify equivalence between callable lookup and the corresponding `get` overloads, stable-equality membership behavior, set immutability, membership preservation, traversal-order preservation, absence of allocation, and the absence of a boolean-returning `operator()` overload.

REQ-TEST-005. Tests MUST verify sequence equivalence across vector, map, set, queue, and string inputs.

REQ-TEST-005A. Tests MUST verify that sequencing a map produces independently valid map-entry values, that a map-entry value has count two, that its first and last values are its key and value, and that `key` and `val` agree with those results.

REQ-TEST-005B. Tests MUST verify the composed behavior of `first(first(map))` and `last(first(map))` without depending on which entry an unordered map returns first.

REQ-TEST-005C. Tests MUST verify that unsorted map and set removal preserves the remaining membership and associations, permits traversal order to change, and does not alter the original value returned by the immutable update operation. Tests MUST verify that vector order and queue FIFO behavior remain unaffected by this removal strategy.

REQ-TEST-006. Tests MUST include compile-time checks for the operations designated `constexpr`.

REQ-TEST-007. Tests MUST verify that all library paths compile and run with dynamic storage unavailable and that no allocation or deallocation facility is required.

REQ-TEST-008. Tests MUST verify that the amalgamated single-header distribution behaves equivalently to the modular source headers.

REQ-TEST-009. Tests MUST include compile-failure checks for every requirement that mandates compile-time rejection, including unsupported operations, concept violations, invalid capacities, unavailable capabilities, statically invalid conversions, invalid equality or ordering uses, and other type- or constant-expression-known violations. Each compile-failure check MUST verify the intended rejection boundary and the diagnostic meaning required by the applicable `REQ-DIAG-*` requirement; tests MAY avoid depending on compiler-specific wording.

REQ-TEST-010. Tests MUST not depend on any particular iteration order for maps or sets.

REQ-TEST-011. The test infrastructure MUST install an allocation trap or equivalent allocator instrumentation that detects direct and indirect calls from cljonic or its transitive dependencies to all relevant C and C++ allocation and deallocation entry points.

REQ-TEST-012. The allocation tests MUST fail the build or test run on any dynamic allocation or deallocation reached through cljonic, whether or not the allocation is ultimately used.

REQ-TEST-013. The allocation tests MUST exercise construction, copying, moving, updating, lookup, traversal, transformation, failure and pre-access predicate handling, and destruction for every public collection and free-function family.

REQ-TEST-014. The allocation tests MUST run against both modular headers and the generated single-header distribution.

REQ-TEST-015. Allocation tests MUST use non-allocating representative element, key, and value types so that detected allocations are attributable to cljonic rather than user-defined type behavior.

REQ-TEST-016. Tests MUST verify that pure collection operations produce equivalent results for equal inputs and fixed configuration, do not mutate their inputs, and do not modify library-managed mutable state.

REQ-TEST-017. Tests for higher-order operations MUST use pure, non-allocating callbacks when verifying the library's purity guarantees.

REQ-TEST-018. Tests MUST cover ASCII string construction, null-terminator maintenance, byte counting excluding the terminator, valid and invalid byte input, indexed access, traversal, sequence conversion, capacity boundaries, and over-capacity behavior.

REQ-TEST-019. Tests MUST verify explicit-capacity string-literal construction, capacity-inferred construction, equivalence of `String{literal}` and `String<content_length>{literal}`, and compile-time rejection when a literal exceeds explicit capacity.

REQ-TEST-020. Tests MUST verify compile-time rejection of over-capacity literals for every collection type that supports literal construction, including vector, map, set, queue, and string where applicable.

REQ-TEST-021. Tests MUST verify explicit-capacity construction, capacity-inferred construction, and equivalence of the inferred form and the corresponding explicitly sized form for every non-string collection type that supports those forms. String-specific construction coverage is defined by `REQ-TEST-019`, while cross-collection over-capacity literal rejection is defined by `REQ-TEST-020`.

REQ-TEST-022. Tests MUST verify compile-time rejection and diagnostic content for an explicit capacity greater than `CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT` across every collection type with an explicit capacity parameter.

REQ-TEST-023. Tests MUST verify result-capacity preflight predicates for representative unknown-cardinality operations, including a fitting result, an over-capacity result, complete materialization when the predicate succeeds, and the documented bounded-result behavior when the predicate reports that the complete result does not fit.

REQ-TEST-024. Tests MUST cover bounded `cycle`, `iterate`, `range`, `repeat`, and `repeatedly`, including finite results, unbounded-prefix results, empty inputs where applicable, capacity boundaries, and deterministic termination behavior.

REQ-TEST-025. Tests MUST verify that `count` does not traverse unbounded producers, returns exact counts for finite producers, returns the configured maximum materialization count for unbounded producers, and computes composed producer counts with saturating arithmetic.

REQ-TEST-026. Tests MUST cover `range` default arguments, inclusive-start and exclusive-end behavior, positive and negative steps, steps moving away from the endpoint, zero-step infinite repetition, equal start and end, and the zero-step/equal-end precedence rule.

REQ-TEST-027. Tests MUST verify that `iterate` and `repeatedly` require pure, non-allocating callbacks and that counted forms perform exactly the requested number of callback evaluations when completely materialized.

REQ-TEST-028. Tests MUST verify that single-match regex functions return an empty owning `String` when no match exists, that their preflight predicates report absence, and that a valid zero-length match is distinguished from absence by the predicate.

REQ-TEST-029. Tests MUST verify compile-time `cljonic::Regex` construction through braced string-literal syntax, successful matching, invalid-pattern compile-time diagnostics, and compatibility with the CTRE integration.

REQ-TEST-030. Tests MUST verify bounded runtime regex construction through `cljonic::core::re_pattern`, including valid patterns, invalid patterns, pattern-capacity boundaries, deterministic failure behavior, and absence of dynamic allocation or exceptions.

REQ-TEST-031. Tests MUST verify that regex capture results own their strings, that full matches and capture groups are represented by bounded values, and that returned results remain valid independently of the input string lifetime.

REQ-TEST-032. Tests MUST verify typed absence replacement semantics for representative `first`, `next`, `get`, `find`, collection transformation, boolean predicate, and regex operations.

REQ-TEST-033. Tests MUST verify that the library's supported behavior does not require threads, synchronization, atomics, thread-local storage, or parallel execution, and MUST document concurrent-access behavior as outside the supported contract.

REQ-TEST-034. Tests MUST verify that raw floating-point values are accepted in permitted scalar, vector-element, and map-value positions when no comparison capability is required.

REQ-TEST-035. Tests MUST verify compile-time rejection of raw floating-point map keys, set elements, `range` bounds or steps, sorting, ordered traversal, distinctness, frequency counting, and default equality operations.

REQ-TEST-036. Tests MUST verify that equality constraints propagate through collection element, key, and value types, including rejection of equality for collections containing raw floating-point values unless an explicitly approved numeric policy is supplied.

REQ-TEST-037. Tests MUST verify that a non-allocating simple user-defined aggregate-like struct can be stored, copied, moved, traversed, and used as a non-comparison vector element or map value.

REQ-TEST-038. Tests MUST verify compile-time rejection of an aggregate-like struct as a map key, set element, or ordered value when the struct does not provide the required stable equality or total-order capability, and acceptance when those capabilities are explicitly provided.

REQ-TEST-039. Tests MUST verify that a `std::string_view` interoperability result exposes the exact ASCII content and size of a source `String`, performs no allocation, excludes the null terminator, is not treated as null-terminated, and has a documented lifetime limited to the source `String` object.

REQ-TEST-040. Tests MUST verify that each supported collection type provides a bounded, non-owning, read-only view path without dynamic allocation or source mutation.

REQ-TEST-041. Tests MUST verify standard-view interoperability where applicable, including `std::span<const T>` for a contiguous vector view and `std::string_view` for a `String`, and MUST verify cljonic-defined view behavior for map, set, and queue traversal.

REQ-TEST-042. Tests MUST verify that collection views do not extend source lifetimes, do not expose mutable references, document invalidation after source lifetime ends or replacement without dereferencing an expired view, and do not impose an ordering contract on map or set traversal.

REQ-TEST-043. Tests MUST verify that the cljonic equivalent of `int` has a fixed signed 32-bit result type and that its result type does not change with global configuration.

REQ-TEST-044. Tests MUST verify compile-time rejection of statically known lossy or out-of-range numeric conversions, acceptance of statically known exact representable conversions where supported, and deterministic behavior for checked runtime conversion success and failure.

REQ-TEST-045. Tests MUST verify that runtime floating-point to integer conversion requires an explicitly checked or explicitly lossy operation, that the default conversion does not narrow implicitly, and that every explicit lossy policy handles truncation, saturation or rejection, NaN, infinity, and out-of-range values as documented.

REQ-TEST-046. Tests MUST verify that numeric conversion does not parse strings implicitly and that separately named string-to-integer parsing reports malformed, empty, out-of-range, and valid inputs deterministically without exceptions or dynamic allocation.

REQ-TEST-047. Tests MUST verify that ordinary equality supports finite sequenceable comparisons without unbounded traversal and rejects statically known unbounded `range`, `cycle`, uncounted `repeat`, uncounted `repeatedly`, and unbounded `iterate` values at compile time.

REQ-TEST-048. Tests MUST verify that an unbounded producer can be stored when storage capabilities permit it, while a containing collection does not acquire default deep equality merely by storing that producer.

REQ-TEST-049. Tests MUST verify deep equality for finite nested vectors, sets, and maps, including recursive equality and order-independent set and map comparison where applicable.

REQ-TEST-050. Tests MUST verify that a finite outer collection containing an unbounded producer remains storable but is not default-deep-equality-comparable, and that equality does not begin an unbounded nested traversal.

REQ-TEST-051. Tests MUST verify that nested producer materialization requires explicit bounds at each materialized level and that no operation implicitly materializes an unbounded nested producer.

REQ-TEST-052. Tests MUST verify the canonical full-word comparison functions, native C++ operator forms where supported, rejection of unsupported comparison capabilities, and the absence of any requirement for abbreviated aliases.

REQ-TEST-053. Tests MUST distinguish general recursive value equality from numeric equality, including Clojure `=`-equivalent collection comparisons and separately constrained numeric comparisons.

REQ-TEST-054. Tests MUST verify persistent value semantics by retaining an original collection across an update, confirming that the original is unchanged, confirming that the updated value is independently valid, and confirming that neither value depends on shared mutable storage.

REQ-TEST-055. Tests MUST verify that `fits_into(destination, producer)` reports whether the complete result fits, that `into` uses the supplied destination's collection type and capacity, leaves the destination and producer unchanged, returns a bounded prefix when `fits_into` is false, and produces a complete result when `fits_into` is true.

REQ-TEST-056. Tests MUST verify that producer construction does not evaluate callbacks, that element-producing callbacks are evaluated once per produced element during each `into` call, and that repeated `into` calls repeat callback evaluation rather than use a realization cache.

REQ-TEST-057. Tests MUST verify that `map` accepts the transforming function before its source collection arguments, supports the approved multi-source form, and applies the documented termination and capacity behavior.

REQ-TEST-058. Tests MUST verify that a closure satisfying the storage capabilities can be stored as a vector element and map value, and MUST verify compile-time rejection of closure use as a map key or set element when stable equality is unavailable.

REQ-TEST-059. Tests MUST verify positional structured binding for supported tuple-like values and simple aggregate structs, nested positional access where supported, and explicit map access through `get`, `contains`, and pre-access predicates rather than a special map-destructuring syntax.

REQ-TEST-060. Tests MUST verify that runtime `Symbol` values and runtime symbol-based namespace resolution are not part of the cljonic API, while supported C++ and cljonic namespace names and statically named free functions remain available.

REQ-TEST-061. Tests MUST verify that an application-defined scoped `enum class` can serve as a map key and set element without a dedicated cljonic keyword type or keyword concept, and MUST verify compile-time rejection of unsupported key operations. Tests MUST also verify `KeywordEnumNameEntry`, nonempty and zero-entry `KeywordEnumNameMap`, `make_keyword_enum_name_map` with an empty entry collection, and `KeywordEnumNameContext` construction and capability constraints.

REQ-TEST-062. Tests MUST verify that cljonic values and collection types provide no metadata attachment or metadata lookup API.

REQ-TEST-063. Tests MUST verify that the cljonic API provides no reader, EDN parser, reader conditional, macro, syntax-quoting, evaluator, or runtime namespace-resolution facility, and that supported collection construction and free-function calls remain usable through ordinary C++ syntax.

REQ-TEST-064. Tests MUST verify zero-function identity, one-function equivalence, variadic `comp` with multiple compatible element functions, right-to-left application, use of the composed function with `map`, compile-time checking of each adjacent parameter and return type from right to left, compile-time rejection of incompatible compositions, and the resulting input and return types. Tests MUST verify that no transducer API is provided.

REQ-TEST-065. Tests MUST verify bounded single-threaded `Atom<T>` construction, `deref`, `reset`, and synchronous `swap`, including compile-time rejection of `swap` functions with incompatible parameter or return types, exactly-once update-function evaluation, input-value preservation, and absence of retry or compare-and-set behavior.

REQ-TEST-066. Tests MUST verify `can_parse_int`/`parse_int`, `can_parse_float`/`parse_float`, and `can_parse_bool`/`parse_bool` for valid, empty, malformed, out-of-range, special-value, and default-result cases according to each parser's documented syntax and policy.

REQ-TEST-067. Tests MUST verify totality across representative supported operations and runtime failure conditions, including absence, invalid indexes, capacity limits, invalid runtime regex patterns, invalid conversions, malformed parser input, bounded producer prefixes, and incompatible compile-time uses. Tests MUST verify absence of exceptions, undefined behavior, out-of-bounds access, unexpected allocation, silent state corruption, process termination, and hidden mutable error state.

REQ-TEST-068. Tests MUST verify that cljonic provides no runtime type inspection, reflection, runtime class or hierarchy queries, or runtime type-identity dispatch, and that supported capability selection is compile-time based.

REQ-TEST-069. Tests MUST verify bounded debug formatting for supported scalar values, strings, vectors, queues, map entries, maps, sets, and application enum keys where formatting is supported. Tests MUST verify zero, one, and multiple variadic `KeywordEnumNameMap` arguments, including a zero-entry type-name-only map, equivalent `KeywordEnumNameContext` use, matching by enum type independent of map argument order, mapped `:name` output, `EnumTypeName(value)` output for unmapped values of a covered enum and for every value covered by a zero-entry map, numeric fallback for uncovered enums, `fits_print` and `print_to` capacity agreement, no allocation, source immutability, Clojure-like delimiters where specified, unordered map/set output, and numeric default formatting of enum keys.

REQ-TEST-070. Tests MUST verify that formatting is not stable serialization, that output order for maps and sets is not prescribed, and that closures, atoms, and arbitrary user-defined classes are not required to provide default formatting.

REQ-TEST-071. Tests MUST verify that `view(Vector<T, N>)`, `view(Set<T, N>)`, and `view(Queue<T, N>)` return read-only `std::span<const T>` views, that `view(Map<K, V, N>)` returns a read-only span of map-entry values, and that `view(String<N>)` returns a `std::string_view` excluding the null terminator. Tests MUST verify active-range contents, no allocation, source immutability, and documented source-lifetime validity.

REQ-TEST-072. Tests MUST verify that direct use of unsupported external standard-library types is not required, while simple aggregate structs containing bounded, non-allocating fields of such types can be stored when their required operations satisfy the applicable cljonic constraints.

REQ-TEST-073. Tests MUST verify compile-time rejection of known capacity and arithmetic overflow, runtime preflight and unchanged-input behavior for collection, string, conversion, parsing, regex, and checked arithmetic overflow, and the documented bounded-prefix exception for `into`.

REQ-TEST-074. Tests MUST verify that keyword enum display names and enum type names obey the bounded ASCII string rules, that duplicate enum entries and duplicate maps for one enum type are rejected as specified, that empty entry collections produce valid type-name-only maps, that missing names fall back deterministically, and that direct variadic map arguments and an equivalent `KeywordEnumNameContext` produce identical output and preflight results.

REQ-TEST-075. Tests MUST cover the first-class bounded string convenience layer, including ASCII-preserving and ASCII-transforming operations, empty and singleton inputs, boundary-capacity inputs, invalid or unsupported ASCII bytes, source immutability, deterministic results, and absence of allocation or exceptions. Tests MUST verify explicit destination capacity and matching preflight behavior for result-growing and multi-result operations corresponding to `join`, `replace`, `replace_first`, `split`, and `split_lines`, including complete, over-capacity, empty-result, and partial-result cases where partial results are permitted.

REQ-TEST-076. Tests MUST cover bounded set algebra for empty, singleton, overlapping, disjoint, duplicate, full-capacity, and over-capacity inputs. Tests MUST verify that compatible `Set<T, N>` and `Set<T, M>` inputs produce `Set<T, max(N, M)>`, that statically invalid derived capacities are rejected, and that `fits_set_algebra` agrees with complete-result production. Tests MUST verify complete `intersection` and `difference` results, bounded-prefix `union` results when distinct values exceed the derived capacity, correct boolean results for `subset` and `superset`, stable-equality capability constraints, unchanged inputs, semantic unorderedness, absence of allocation and exceptions, and no hidden completeness state. Tests MUST separately verify that relational map operations remain unsupported until their relation model is approved.

REQ-TEST-077. Tests MUST verify that `index`, `project`, `rename`, and `join` are not exposed as supported operations before an approved relation model defines their row representation, capabilities, duplicate semantics, nested result representation, traversal order, capacity arithmetic, preflight, and bounded failure behavior. The tests MUST verify that vocabulary inventory or apparent C++ implementability does not bypass this requirement gate.

REQ-TEST-078. Tests MUST cover the first-pass numeric convenience family for valid, boundary, zero, negative, division-by-zero, overflow, underflow, invalid shift-count, representability, and conversion-loss cases as applicable. Tests MUST verify checked preflight agreement, documented default or failure results, unchanged inputs, compile-time rejection of statically invalid operations, explicit conversion behavior, raw floating-point capability restrictions, absence of allocation and exceptions, and distinct naming and behavior for any wrapping or saturating alternatives.

REQ-TEST-079. Tests MUST cover the first-pass callable convenience family, including zero-argument and ordinary invocation, compile-time rejection of incompatible parameter and return types, bounded closure representation, copy/move/destruction behavior, absence of allocation and exceptions, and input preservation. Tests MUST verify identity, constant return, complement, partial application, `fnil` defaults, `juxt` result shape and invocation order, `every_pred` and `some_fn` short-circuit behavior, and `apply` final-sequence handling and capacity behavior. Tests MUST verify that callable operations do not provide transducer semantics, hidden mutable caching, runtime type dispatch, or implicit callback retention by collection results.

REQ-TEST-080. Tests MUST cover the first-pass collection-shaping and traversal family across empty, singleton, finite, producer, boundary-capacity, over-capacity, nested-result, duplicate, unordered, callback, and ordering cases as applicable. Tests MUST verify prefix, suffix, stride, predicate, duplicate, grouping, partitioning, flattening, tree-traversal, mapping, filtering, and sorting behavior; typed absence; input preservation; producer termination; stable-equality and total-order constraints; callback purity and invocation rules; matching preflight behavior; absence of allocation and exceptions; and rejection of transducer-only arities and hidden lazy-sequence machinery.

REQ-TEST-081. Documentation and compile-time/API-surface checks MUST verify that representative functions remain correctly classified as `candidate`, `deferred`, `excluded`, or `requirements-backed`, and that a function is not presented as supported before its applicable behavior, capacity, failure, diagnostic, and test requirements exist. Checks MUST verify that API-surface vocabulary does not silently override the authoritative requirements document.

## Implementation Constraints

These constraints guide architecture and code but remain subordinate to the behavioral requirements:

- Prefer fixed-capacity storage and explicit capacity parameters.
- Prefer flat bounded representations for the current implementation.
- Use deep copying on update; structural sharing is not part of the cljonic implementation or public contract.
- Use swap-and-remove for removal from unsorted maps and sets after linear lookup; do not use it for ordered vectors or FIFO queues.
- Keep traversal operations separate from collection-specific APIs while using bounded owning values for public sequence results.
- Use concepts to express capabilities such as traversable, countable, associative, stack-like, and conjable.
- Keep the public free-function layer independent of concrete storage classes.
- Generate the single header from modular source files rather than treating the generated file as the source of truth.
- Avoid implementation features that obscure allocation, copying, or failure costs.

## Compatibility and Scope

This section gives the current compatibility map between major Clojure capabilities and cljonic. It is a scope guide, not a claim of complete Clojure compatibility. Each capability is classified as semantically supported, deliberately replaced by bounded or static behavior, available only through an explicit capability, or out of scope. This table is explanatory and does not cite specific requirements; the numbered `REQ-*` clauses in earlier sections are authoritative for all behaviors.

| Clojure capability                     | Clojure Capability Description                                                                                                                                                                 | cljonic mapping                                                                                                                                                                                                                                                                                                                                                                                                                         | Compatibility classification                                                              |
| -------------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ----------------------------------------------------------------------------------------- |
| Immutable and persistent collections   | Collections are not changed in place; updates return values that preserve the prior version; persistent implementations commonly use structural sharing so versions can reuse unchanged parts. | Bounded immutable-by-default value collections; updates use deep copying and do not use structural sharing.                                                                                                                                                                                                                                                                                                                             | Semantically supported with a different implementation                                    |
| Vector, map, set, queue, and string    | Core collection types for ordered values, key/value associations, unique values, FIFO access, and text.                                                                                        | Supported bounded collection family with flat contiguous storage, explicit capacities, linear scans for map/set operations, deterministic failure behavior, and no dynamic storage.                                                                                                                                                                                                                                                     | Semantically supported with bounded storage                                               |
| Maps as functions of keys              | A map can be invoked with a key to retrieve its associated value, with an optional default when the key is absent.                                                                             | `Map<K, V, N>` provides callable `operator()(key)` and `operator()(key, fallback)` forms equivalent to `get(map, key)` and `get(map, key, fallback)`. The one-argument form returns `V{}` for a missing key; `contains(map, key)` distinguishes absence from a stored default value. `operator[]` is not provided because it suggests insertion.                                                                                        | Supported through C++ callable syntax                                                     |
| Vectors as functions of indexes        | A vector can be invoked with an index to retrieve its element, returning `nil` when the index is invalid.                                                                                      | `Vector<T, N>` provides callable `operator()(index)` and `operator()(index, fallback)` forms equivalent to `get(vector, index)` and `get(vector, index, fallback)`. Invalid indexes, including representable negative indexes, return `T{}` or the supplied fallback; `valid_index(vector, index)` distinguishes invalid access from a valid default-valued element. `operator[]` is not provided because it suggests unchecked access. | Supported through C++ callable syntax with typed bounds behavior                          |
| Sets as functions of values            | A set can be invoked with a value to return that value when it is a member, or `nil` when it is absent.                                                                                        | `Set<T, N>` provides callable `operator()(value)` and `operator()(value, fallback)` forms equivalent to `get(set, value)` and `get(set, value, fallback)`. A present value returns the matching stored element; an absent value returns `T{}` or the supplied fallback. `contains(set, value)` remains the boolean membership predicate.                                                                                                | Supported through C++ callable syntax with typed membership behavior                      |
| Sequence traversal                     | A common way to view collections as values with a first element and a remainder.                                                                                                               | `seq`, `first`, `last`, `next`, `rest`, and `count` over supported collections; `seq` returns an owning bounded vector.                                                                                                                                                                                                                                                                                                                 | Semantically supported with a different implementation                                    |
| Map entries                            | A map is traversed as key/value pairs, with each pair behaving like a two-element sequence.                                                                                                    | Map sequencing returns owning two-component map-entry values that are themselves sequenceable.                                                                                                                                                                                                                                                                                                                                          | Semantically supported with a different implementation                                    |
| Lazy sequences                         | Sequence elements are computed as they are requested, usually without storing the entire result first; realized elements may be cached for later reuse.                                        | Explicit producer values and `into` replace hidden lazy realization and implicit storage; each `into` call evaluates the producer as needed and does not use a realization cache.                                                                                                                                                                                                                                                       | Deliberately replaced by bounded static behavior                                          |
| Infinite sequences                     | Some sequence producers have no endpoint and can keep yielding values indefinitely.                                                                                                            | Unbounded producers such as open-ended `range`, `cycle`, `repeat`, `repeatedly`, and `iterate`; observation requires explicit capacity and equality must not traverse indefinitely.                                                                                                                                                                                                                                                     | Deliberately replaced by bounded observation                                              |
| Higher-order functions                 | Functions such as `comp`, `map`, `filter`, and `reduce` accept functions as inputs to compose, transform, or consume values.                                                                   | `comp` provides ordinary right-to-left element-function composition; `map(transform, values...)`, `filter`, `reduce`, and producer callbacks remain subject to purity, copying, invocation, and non-allocation capabilities.                                                                                                                                                                                                            | Supported through explicit static capabilities                                            |
| Closures and function values           | A function can be passed around as a value and can capture surrounding local values.                                                                                                           | Callbacks may be passed to functions without being retained by their results; stored closures are supported as elements and map values when their bounded storage capabilities are satisfied, but they have no default equality, ordering, map-key, set-element, or serialization capabilities.                                                                                                                                         | Supported when the closure satisfies the operation’s compile-time capability requirements |
| Protocols and polymorphism             | Protocols define named operations that unrelated types can implement.                                                                                                                          | cljonic uses a closed vocabulary of built-in collection types and free functions; concepts and traits constrain those types internally, while simple aggregate structs are supported as values without becoming general protocol extensions.                                                                                                                                                                                            | Deliberately replaced by a closed static vocabulary                                       |
| Numeric tower                          | Numbers can use several representations, including integers, ratios, floating point, arbitrary-precision integers, and decimals, with promotion between some types.                            | Fixed-width numeric types, checked arithmetic by default, explicit conversion and parser policies, and raw floating-point restrictions where rounding, representation, and special-value behavior could make equality, ordering, or discrete stepping surprising.                                                                                                                                                                       | Deliberately replaced by static numeric capabilities                                      |
| Transients                             | Temporary mutable variants of persistent collections for bulk mutations during construction, often used as a performance optimization to amortize copying during repeated updates.             | Not applicable; cljonic uses immutable-by-default deep-copied values without a mutable transient phase. Collection mutations always return new independent values; no temporary mutable view is provided.                                                                                                                                                                                                                               | Out of scope                                                                              |
| Arbitrary-precision integers           | Integer values can grow beyond fixed machine-word limits.                                                                                                                                      | cljonic uses fixed-width numeric types because storage must remain bounded and statically auditable.                                                                                                                                                                                                                                                                                                                                    | Not supported                                                                             |
| Ratios and exact fractional numbers    | A ratio stores an exact numerator and denominator instead of an approximate decimal result.                                                                                                    | cljonic has no ratio type or automatic numeric promotion because numeric representations and conversion policies are explicit and bounded.                                                                                                                                                                                                                                                                                              | Not supported                                                                             |
| Arbitrary-precision decimals           | Decimal values can retain more precision than a fixed floating-point type, whose rounding and representation can vary across supported environments.                                           | cljonic has no arbitrary-precision decimal type because numeric storage must remain bounded and statically auditable.                                                                                                                                                                                                                                                                                                                   | Not supported                                                                             |
| General equality                       | `=` compares values, including nested collections, rather than asking whether two references are identical.                                                                                    | Full-word `equal` and applicable native C++ operators; finite nested values compare recursively, while unbounded producers are excluded.                                                                                                                                                                                                                                                                                                | Semantically supported with termination constraints                                       |
| Numeric equality                       | `==` compares numeric values under Clojure's numeric semantics and is distinct from general value equality.                                                                                    | Separate from general equality and subject to numeric capabilities; raw floating-point equality is restricted because rounding and special values can make results surprising.                                                                                                                                                                                                                                                          | Supported through explicit numeric capability                                             |
| Ordering and sorting                   | Comparison functions and sorting arrange values when a meaningful ordering exists.                                                                                                             | Full-word comparison functions and native operators where total-order capabilities exist; raw floating point is rejected by default because NaN, signed zero, rounding, and evaluation differences can make ordering surprising.                                                                                                                                                                                                        | Supported through explicit static capabilities                                            |
| Keywords                               | Self-identifying names commonly used as map keys and lightweight identifiers.                                                                                                                  | Application-defined `enum class` types provide the supported keyword-like keys; a globally scoped `enum class Keywords` may define one application-wide vocabulary, with compile-time equality and no runtime keyword object. Optional `KeywordEnumNameMap` values can provide explicit `:name` debug output without changing enum identity.                                                                                            | Deliberately replaced by application-defined static keys                                  |
| Symbols and namespaces                 | Symbols represent names, while namespaces organize and qualify vars and other names.                                                                                                           | cljonic has no runtime `Symbol` values or symbol-based namespace resolution; C++ namespaces and statically named cljonic free functions provide the supported naming model.                                                                                                                                                                                                                                                             | Not supported                                                                             |
| Metadata                               | Values can carry an associated map of descriptive information without changing their main use.                                                                                                 | cljonic does not attach metadata because it is not needed for embedded collection and free-function development and would add storage, copying, equality, and interoperability complexity.                                                                                                                                                                                                                                              | Not supported                                                                             |
| Reader literals and EDN                | The reader turns text and literals into Clojure data; EDN is a portable data notation.                                                                                                         | cljonic provides no reader or EDN parser; collections and values are constructed through ordinary C++ syntax.                                                                                                                                                                                                                                                                                                                           | Not supported                                                                             |
| Macros and syntax quoting              | Macros transform code before evaluation, and syntax quoting helps construct code with namespace-aware symbols.                                                                                 | cljonic provides no macro or syntax-quoting system; compile-time templates and ordinary C++ language facilities define the supported construction model.                                                                                                                                                                                                                                                                                | Not supported                                                                             |
| Destructuring                          | Bindings can extract values from vectors, maps, and other structured data in one expression.                                                                                                   | Positional decomposition uses C++ structured bindings for supported tuple-like values and simple aggregate structs; map access uses `get`, `contains`, and pre-access predicates, with no special map-destructuring syntax.                                                                                                                                                                                                             | Deliberately replaced by C++ binding and access forms                                     |
| Transducers                            | Composable transformation descriptions can be applied independently of the destination collection.                                                                                             | cljonic does not provide transducers; ordinary function composition through `comp` covers compatible element-to-element transformation pipelines.                                                                                                                                                                                                                                                                                       | Not supported                                                                             |
| Regex                                  | Regular-expression patterns can find, match, and capture text.                                                                                                                                 | Compile-time `Regex`, bounded runtime patterns, owning bounded captures, and typed absence semantics.                                                                                                                                                                                                                                                                                                                                   | Semantically supported with bounded static behavior                                       |
| Nil and absence                        | `nil` represents absence and is accepted by many functions and collection operations.                                                                                                          | Typed defaults, empty bounded collections, boolean false, and preflight predicates replace untyped `nil` where required.                                                                                                                                                                                                                                                                                                                | Deliberately replaced by typed static behavior                                            |
| Exceptions and errors                  | Invalid operations commonly report errors through exceptions or other runtime failures.                                                                                                        | Supported cljonic operations work correctly for all their documented inputs; cljonic does not use or require exceptions, and uses compile-time diagnostics, preflight predicates, checked results, defaults, and deterministic bounded results instead.                                                                                                                                                                                 | Deliberately replaced by explicit static/checked behavior                                 |
| Atoms, agents, refs, watches, and STM  | Mutable reference types coordinate state changes, observation, and in some cases transactions.                                                                                                 | A bounded single-threaded `Atom<T>` is supported with `deref`, `reset`, and synchronous type-checked `swap`; agents, refs, watches, validators, and STM are not supported.                                                                                                                                                                                                                                                              | Atom supported; remaining reference features not supported                                |
| Parallel reducers and concurrency      | Some facilities coordinate work across threads or parallelize reductions.                                                                                                                      | No parallel execution, synchronization, atomics, or thread-safety contract.                                                                                                                                                                                                                                                                                                                                                             | Not supported                                                                             |
| Runtime type inspection and reflection | Programs can inspect runtime types, classes, hierarchy, and reflective metadata.                                                                                                               | cljonic provides no runtime type inspection, reflection, class or hierarchy queries, or type-identity dispatch; concepts, traits, and constraints select capabilities at compile time.                                                                                                                                                                                                                                                  | Not supported                                                                             |
| Printing and reading                   | Values can be rendered as readable text and read back as data in supported formats.                                                                                                            | cljonic provides bounded Clojure-like debug formatting through variadic `fits_print` and `print_to`; optional `KeywordEnumNameMap` values provide explicit enum names and enum-type fallback output. It provides no stable serialization, reader, EDN parser, or round-trip text interchange.                                                                                                                                           | Debug formatting supported; reading and serialization not supported                       |
| C++ interoperability                   | Clojure can call Java and use Java values.                                                                                                                                                     | Direct external standard-library types are not required as cljonic collection or free-function inputs or outputs; simple aggregate structs containing bounded, non-allocating standard-library fields may be stored, and `view(collection)` returns the locked standard read-only spans or string view.                                                                                                                                 | Supported through explicit platform capabilities                                          |

### Compatibility Classification

Future decisions should classify each Clojure characteristic as one of:

- semantically supported with a different implementation;
- deliberately replaced by a bounded or static cljonic behavior;
- supported only through an explicit opt-in capability; or
- out of scope.

The compatibility table is explanatory prose intended for eventual user-facing documentation. It MUST be regenerated or checked against the numbered requirements when it is moved out of this document; the table MUST NOT become an independent source of behavior.

## Downstream Use

- Vocabulary work MUST extract and define the canonical terms in this document.
- Architecture work MUST assign these requirements to durable capability and collection layers.
- Specification work MUST convert each `REQ-*` item into observable behavior.
- Test work MUST maintain traceability from `REQ-*` identifiers to test cases.
- Code work MUST implement only behavior that is supported by an approved requirement or a recorded architecture decision.
- Any conflict between this document and a lower layer MUST be surfaced as drift rather than silently resolved in code.
