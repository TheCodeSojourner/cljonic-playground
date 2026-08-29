# cljonic Requirements - Module 2: Capability Concepts & Preflight Infrastructure

## Purpose and Scope

This module establishes the C++20 concept capability framework, result status outcome classification model, preflight predicate policies, compile-time diagnostic rules, and vocabulary conventions for `cljonic`. Module 2 defines how the type system enforces safety, communicates failure, and governs function preconditions without runtime exceptions or dynamic memory allocations.

## Canonical Type, Result, and Status Model

- An owning value is a self-contained cljonic value whose validity does not depend on an external source lifetime, hidden borrowed state, or a hidden result cache.
- A non-owning view is a read-only observation of an existing cljonic value. A view MUST NOT own storage, MUST NOT extend the lifetime of its source, and MUST NOT permit mutation of the source value.
- A bounded result is an owning cljonic value whose capacity is known and finite under the operation's documented constraints.
- A bounded-prefix result is a bounded result that is intentionally smaller than the complete result because the complete result could not fit within the declared capacity or the operation's documented result policy.
- A default-returning result is the documented default value produced by a convenience operation when the requested access or lookup cannot produce a valid value.
- A checked-failure result is a documented non-throwing, non-allocating result indicating that an operation could not complete successfully without violating the library's failure model.
- A producer is an explicit value that represents a sequence or materialization source without owning the materialized result storage.
- A complete result is the full result of an operation as defined by the relevant requirement.
- Every public operation MUST document whether it returns a complete result, a bounded-prefix result, a default value, or a checked failure. When the complete result may fail to fit, the operation MUST expose a corresponding non-throwing, non-allocating preflight predicate that measures the same result and failure conditions as the operation.
- Stable equality and total ordering are capabilities, not assumptions implied by storage. A type is eligible for equality or ordering only when the applicable requirement or capability contract explicitly permits it.

## Result Status Classification Rules

- Result status is orthogonal to value kind. Every public operation MUST declare its outcome classification as one of:
  - complete result
  - bounded-prefix result
  - default-returning result
  - checked-failure result
  - producer-only result
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

REQ-BOUNDS-001. Collection access MUST be bounds-checked.

REQ-BOUNDS-002. Every collection MUST define a default element of its value type, normally produced by value-initialization such as `T{}`.

REQ-BOUNDS-003. An access that cannot return a valid element MUST return the collection's default element through the default-returning API.

REQ-BOUNDS-004. The API MUST make it possible to determine before access whether an operation can return a valid element without relying on exceptions or inspecting the value for equality with the default element.

REQ-BOUNDS-005. The library MUST document every operation that may return a default element.

REQ-BOUNDS-006. Bounds failures, capacity failures, and missing-key results MUST have distinct documented semantics, even when more than one uses a default-returning convenience API.

REQ-BOUNDS-007. For every default-returning access operation, the API MUST provide a non-throwing, non-allocating way to determine before access whether the operation can return a valid element.

REQ-BOUNDS-008. Pre-access predicates MUST be defined in terms of the operation's domain and MUST distinguish empty collections or sequences, invalid indexes, missing keys, and full-collection capacity state where those conditions apply.

REQ-BOUNDS-009. For an immutable collection value, a successful pre-access predicate and the corresponding access operation MUST agree: if the predicate reports that a valid element is available, the access MUST return that element; otherwise the access MUST follow its documented default-element semantics.

REQ-BOUNDS-010. The API MUST provide `contains` as the canonical lookup-domain membership predicate across all collection kinds: for maps it MUST test key presence, for sets element presence, and for vector/string indexed collections it MUST test index-in-range (mirroring Clojure's `contains?`). The API MUST also provide `empty` for collection and sequence underflow checks, `full` or an equivalent capacity inspection for bounded insertion operations, and membership or key-presence checks for key-based access where those capabilities apply.

REQ-BOUNDS-010A. `is_empty` MUST be a non-throwing, non-allocating boolean predicate that returns `true` exactly when its supported collection or sequence input contains zero elements. `empty` MUST remain a value-producing operation and MUST return an empty owning value of the same supported collection type as its input, preserving the input's capacity type where that capacity is part of the collection type. `not_empty` MUST return an owning value of the same supported collection type as its input: it MUST return an independently valid copy of the input when the input contains one or more elements and the corresponding empty value when the input contains zero elements. `not_empty` MUST NOT be a boolean alias for `is_empty`.

REQ-BOUNDS-011. A pre-access predicate MUST NOT inspect or compare the accessed value, rely on equality with `T{}`, perform an ambiguous default-returning access, throw, or allocate.

REQ-BOUNDS-012. For every `into` operation whose maximum possible result cardinality can exceed the destination collection's capacity, the API MUST provide the non-throwing, non-allocating preflight predicate `fits_into(destination, producer)`, which determines whether the complete result fits within that destination.

REQ-BOUNDS-013. A result-capacity preflight predicate and the corresponding materializing operation MUST have compatible semantics for immutable inputs. If the predicate reports that the complete result fits, the operation MUST produce the complete result. If the predicate reports that the complete result does not fit, the operation MUST follow its documented bounded-result behavior, which MAY return a deterministic prefix limited to its compile-time capacity.

REQ-BOUNDS-014. Result-capacity preflight predicates MUST measure the same result quantity and apply the same matching, filtering, transformation, or overflow semantics as their corresponding materializing operations.

REQ-BOUNDS-015. An `into` operation MUST document whether its result is complete or may be a bounded prefix when the destination capacity is insufficient. If it may be a bounded prefix, it MUST preserve the defined result order and MUST provide a preflight predicate that determines whether the complete result fits.

REQ-BOUNDS-016. `into` MUST return the destination collection type directly. Ordinary collection values MUST NOT carry hidden completeness state, and cljonic MUST NOT require a materialization-result wrapper for partial-prefix status; callers MUST use `fits_into` before `into` when completeness matters.

REQ-BOUNDS-017. Every supported operation whose complete result can fail because of capacity, representability, cardinality, matching, filtering, transformation, or another documented runtime condition MUST define its bounded-result, default-result, or failure policy. When callers need to distinguish complete success from that default, bounded, partial, or failed result, the operation MUST provide a corresponding non-throwing, non-allocating preflight predicate. The predicate MUST measure the same result quantity and apply the same capacity, representability, cardinality, matching, filtering, transformation, and failure semantics as the operation. `fits_into` MUST remain the canonical preflight for complete producer materialization into an explicit destination; operation-specific predicates MAY be used for other operations.

## Error and Failure Policy Requirements

REQ-ERR-001. Supported collection operations MUST NOT require exceptions.

REQ-ERR-002. The library MUST NOT require error codes or global mutable error state for ordinary collection access.

REQ-ERR-003. Default-returning convenience functions MUST have documented pre-access predicates that allow callers to determine whether the operation can return a valid element or complete successfully.

REQ-ERR-004. Failure behavior MUST be deterministic and documented for empty collections, missing keys, invalid indexes, full collections, duplicate set insertion, and duplicate map keys. Duplicate set insertion MUST be a successful no-op, and duplicate map-key association MUST replace the existing value. Any pre-access predicate or capacity query provided for these conditions MUST have semantics consistent with the corresponding access or update operation.

REQ-ERR-005. The library MUST NOT invoke undefined behavior for invalid collection access within its documented API.

REQ-ERR-006. Every supported operation whose success depends on runtime values, collection state, capacity, representability, cardinality, or producer behavior MUST comply with `REQ-BOUNDS-017`. When callers need to distinguish complete success from a default, bounded, partial, or failed result, the operation MUST provide a corresponding non-throwing, non-allocating preflight predicate unless a non-throwing, non-allocating checked result directly communicates that distinction. Any preflight predicate or checked result MUST agree with the operation for immutable inputs, and a false or failed outcome MUST produce the operation's documented default, bounded, partial, or failure result.

REQ-ERR-007. Supported cljonic operations MUST be defined for all their documented inputs. They MUST NOT throw, invoke undefined behavior, access storage out of bounds, allocate unexpectedly, silently corrupt state, terminate the process, or depend on hidden mutable error state. Runtime-detectable unsuccessful conditions MUST use the operation-specific documented mechanism: a preflight predicate with a default or bounded result, a checked result, or a compile-time rejection when the condition is knowable from the types or constant expressions.

REQ-ERR-008. Overflow policy MUST be operation-specific but MUST follow one general contract: compile-time-known capacity or representability overflow MUST be rejected at compile time; runtime-detectable overflow MUST provide a non-throwing, non-allocating preflight predicate; the corresponding operation MUST leave its input unchanged when the predicate is false and MUST return its documented default or bounded result. `into` is the explicit partial-prefix exception and MUST use `fits_into` as specified elsewhere.

## Compile-Time Diagnostics

REQ-DIAG-001. Public templates and free functions MUST express knowable compile-time capability requirements through named concepts or equivalent constraints.

REQ-DIAG-002. Compile-time-invalid uses MUST fail at the public API boundary with diagnostics that identify the violated cljonic capability, capacity, or value constraint.

REQ-DIAG-003. The library SHOULD use targeted `static_assert` diagnostics for context-dependent compile-time failures that cannot be expressed clearly through concepts or equivalent constraints.

REQ-DIAG-004. Diagnostic requirements MUST specify the meaning and relevant constraint of a diagnostic rather than depend on compiler-specific wording.

REQ-DIAG-005. Public concept names MUST be precise, capability-oriented, and understandable when rendered in compiler diagnostics. Concept names MUST identify the required capability or constraint and MUST NOT rely on vague names such as `Valid`, `Supported`, or `Allowed` without further qualification.

REQ-DIAG-006. Materialization APIs with runtime-unknown result cardinality SHOULD provide an implementation-defined compiler warning or equivalent tooling diagnostic when the caller uses the default maximum capacity, recommending an explicit call-site capacity. Such a warning MUST NOT replace the required capacity parameter or depend on non-portable diagnostics for correctness.

## Compile-Time Evaluation

REQ-CONST-001. Collection construction and non-allocating operations SHOULD be `constexpr` when the value types and compiler permit it.

REQ-CONST-002. Operations that are required to be compile-time evaluable MUST be tested in constant expressions.

REQ-CONST-003. `consteval` MUST be used only where compile-time execution is semantically required and does not unnecessarily restrict valid embedded use.

REQ-CONST-004. Compile-time and runtime evaluation MUST produce equivalent observable results.

## Vocabulary and Naming Conventions

REQ-VOCAB-001. The canonical terms MUST include collection, sequence, sequenceable, traversal, vector, map, set, queue, string, capacity, default element, valid index, `contains`, persistent value, free function, bounded storage, platform interoperability, aggregate-like struct, stable equality, total order, discrete numeric type, numeric policy, owning value, non-owning view, standard view type, bounded result, partial result, preflight predicate, exact conversion, checked conversion, lossy conversion, parsing, finite observation, finite deep equality, bounded inspection, unbounded producer, producer materialization, relation model, `MapEntry`, general equality, numeric equality, semantic predicate name, state predicate, verb predicate, capability predicate, `is_` predicate prefix, `can_` predicate prefix, `has_` predicate prefix, `valid_` predicate prefix, lifecycle classification, `candidate`, `deferred`, `excluded`, `requirements-backed`, `KeywordEnumNameEntry`, `KeywordEnumNameMap`, `KeywordEnumNameContext`, and keyword enum name mapping.

REQ-VOCAB-002. Each canonical term MUST have one meaning in public documentation, requirements, tests, and code.

REQ-VOCAB-003. Clojure-inspired names MAY be used when their behavior is documented in C++ terms.

REQ-VOCAB-004. The public API MUST prefer a small set of orthogonal capabilities over a deep hierarchy of collection-specific interfaces.

REQ-VOCAB-005. The public API MUST expose capacity and failure policy clearly enough that a caller can reason about resource use from source code.

REQ-VOCAB-006. Public C++ collection type names MUST use PascalCase, including `Vector`, `Map`, `Set`, `Queue`, and `String`.

REQ-VOCAB-007. Clojure-inspired public C++ function names MUST replace hyphens with underscores, so a name such as `last-index-of` is exposed as `last_index_of`. A terminal Clojure question mark MUST be expressed semantically: state or adjectival predicates MUST use an `is_` prefix, while readable verb predicates MUST retain their direct verb form. Capability and preflight predicates MUST retain established `can_`, `has_`, and `valid_` prefixes. Thus `empty?`, `zero?`, `pos?`, `neg?`, `even?`, `odd?`, `blank?`, `subset?`, and `superset?` map to `is_empty`, `is_zero`, `is_positive`, `is_negative`, `is_even`, `is_odd`, `is_blank`, `is_subset`, and `is_superset`; `contains?`, `starts-with?`, `ends-with?`, and `includes?` map to `contains`, `starts_with`, `ends_with`, and `includes`. Namespace layout is a separate API design decision.

REQ-VOCAB-008. Public cljonic namespace names SHOULD mirror the corresponding Clojure namespace names using lowercase C++ namespace identifiers and underscores where separators are needed, so `clojure.core` maps conceptually to `cljonic::core` and `clojure.string` maps conceptually to `cljonic::string`.

REQ-VOCAB-009. The public cljonic API MUST be developed as a broad Clojure-inspired convenience surface rather than as a minimal embedded collection subset. The broad-surface goal MUST remain subordinate to the library's bounded-resource, no-allocation, no-exception, value-semantic, single-threaded, and compile-time-capability requirements. It MUST NOT require reproducing Clojure runtime features, supporting arbitrary external C++ types, or expanding the closed cljonic vocabulary without an approved requirement.

REQ-VOCAB-010. The first-class cljonic value and data-structure domain MUST be closed to the supported bounded `Vector`, `Map`, `Set`, `Queue`, and `String` values, explicit producer values, bounded regex values and match results required by the text surface, and application-defined scoped enumeration values when used as supported map keys or set elements. Public operations MUST NOT be required to accept or return arbitrary external containers, runtime type extensions, or protocol-style user-defined collection participation. Simple user-defined aggregate-like structs MAY participate where the applicable platform, storage, equality, ordering, and other capability requirements explicitly permit them; such participation MUST NOT expand the collection vocabulary.

REQ-VOCAB-011. Every public function considered during API-surface review MUST have one explicit lifecycle classification: `candidate`, `deferred`, `excluded`, or `requirements-backed`.

## Traceability and Related Requirements

- **Downstream Artifact**: C++20 concepts, preflight predicates, compile-time assertions, and result status type definitions.
- **Governed REQs**: `REQ-BOUNDS-001`–`017`, `REQ-ERR-001`–`008`, `REQ-DIAG-001`–`006`, `REQ-CONST-001`–`004`, `REQ-VOCAB-001`–`011`.
