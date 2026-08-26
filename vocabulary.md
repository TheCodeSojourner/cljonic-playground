---
created: 2026-08-05
last_updated: 2026-08-26
status: draft
---

## Core Vocabulary

## Current Scope

The current implementation and tests cover only the `Vector` collection and
its direct construction and member-observation contract. Free-function
operations are not currently supported. Terms for other collections and
operations remain planned vocabulary; they do not constitute supported
behavior until an active specification, test coverage, and implementation are
added.

### CopyOnModifyCollection
- **Definition:** A fixed-capacity value type backed by static storage that returns a modified copy instead of mutating in place. In this repo, vector, set, map, and string all follow this model.
- **Deprecated Synonyms:** Copy-on-Modify Collection, bounded immutable collection, fixed-capacity collection, array-backed collection, deep copy on write, deep copying on write
- **Related:** Vector, Map, Set, String, SentinelBasedAccess
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
- **Related:** CopyOnModifyCollection, String, CapacityConstruction
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** `Vector<int, 4>{1, 2}` constructs a fixed-capacity value whose `size()`, `capacity()`, and call operator provide direct member observation.



### Map
- **Definition:** The cljonic fixed-capacity associative collection type with content-based key lookup, immutable copy-on-modify updates, and intentionally unspecified iteration order.
- **Deprecated Synonyms:** map collection, bounded map, fixed-capacity map
- **Related:** CopyOnModifyCollection, String
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** A `Map` supports `assoc`, `dissoc`, `contains`, and `get`, with duplicate-key `assoc` replacing an existing value in the returned copy.



### Set
- **Definition:** The cljonic fixed-capacity uniqueness-preserving collection type with content-based membership semantics and immutable copy-on-modify updates.
- **Deprecated Synonyms:** set collection, bounded set, fixed-capacity set
- **Related:** CopyOnModifyCollection, CapacityConstruction
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** A `Set` supports `conj`, `dissoc`, `contains`, and `count`, with equality defined by logical content rather than storage order.



### String
- **Definition:** The cljonic fixed-capacity string collection type. Its logical capacity is measured in characters, while its physical storage is measured in code units under the active StringEncoding. In UTF-8 mode, storage is sized for the worst-case number of code units needed to hold the declared character capacity.
- **Deprecated Synonyms:** StaticString, static string, fixed-capacity string, string collection
- **Related:** CopyOnModifyCollection, Vector, Map, Set, CapacityConstruction
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** A `String<32>` means room for 32 characters; in UTF-8 mode its backing storage may reserve worst-case code-unit capacity to preserve that logical character capacity.

















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
- **Definition:** A self-contained cljonic value whose validity does not depend on another value's lifetime, hidden borrowed state, or hidden result cache.
- **Deprecated Synonyms:** owning value, self-contained value
- **Related:** NonOwningView, CopyOnModifyCollection
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



### Queue
- **Definition:** The cljonic bounded first-class collection type for queue semantics within the closed value and data-structure domain.
- **Deprecated Synonyms:** queue collection, bounded queue
- **Related:** Vector, Map, Set, String
- **Usage:** Requirements, architecture, specification, implementation, tests, and documentation
- **Examples:** `Queue` is part of the required first-class bounded domain alongside `Vector`, `Map`, `Set`, and `String`.







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
- **Related:** NoHeapConstraint, NoExceptionConstraint, DeterministicBehavior
- **Usage:** Architecture, specification, implementation, and documentation
- **Examples:** Collection APIs use fixed-capacity storage and avoid runtime allocation on strict profiles.



### NoHeapConstraint
- **Definition:** The rule that strict profiles must not perform heap allocation anywhere in the library's behavior or infrastructure.
- **Deprecated Synonyms:** no heap allocation, no-heap rule
- **Related:** EmbeddedConstraint, DeterministicBehavior
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** Verification gates prohibit forbidden allocation APIs and bounded collections store data without dynamic allocation.



### NoExceptionConstraint
- **Definition:** The rule that library behavior is non-throwing and does not depend on exception-based control flow.
- **Deprecated Synonyms:** no exceptions, non-throwing rule
- **Related:** EmbeddedConstraint, SentinelBasedAccess, DeterministicBehavior
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** Missing access returns sentinel values instead of throwing, and contract failures route through explicit policy hooks.





### CollectionMaximumElementCount
- **Definition:** The compile-time configuration boundary that sets the maximum element count allowed for cljonic collections and the synthesis cap used by semantically infinite lazy producers. In current documentation this boundary is exposed via the preprocessor macro CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT.
- **Deprecated Synonyms:** collection max element count, maximum collection element count, CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT macro
- **Related:** CapacityConstruction, EmbeddedConstraint, DeterministicBehavior
- **Usage:** Architecture, specification, implementation, build, and documentation
- **Examples:** A project can raise the boundary by defining CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT before including cljonic headers or by setting it from compiler/build flags; a zero-step range may synthesize at most this many elements before truncation.



### DeterministicBehavior
- **Definition:** The requirement that behavior, failure modes, and profile-selected semantics remain predictable and stable for the same inputs and configuration.
- **Deprecated Synonyms:** deterministic semantics, deterministic execution
- **Related:** EmbeddedConstraint, NoHeapConstraint, NoExceptionConstraint
- **Usage:** Architecture, specification, tests, and documentation
- **Examples:** Full-capacity `conj` returns the unchanged value deterministically, and invalid runtime regex compilation returns a stable invalid-pattern sentinel.











### HeaderOnlyDistribution
- **Definition:** The packaging model in which the library is delivered as headers only, with development sources organized separately from the generated distribution artifact.
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
- **Definition:** The construction contract that supports both literal-deduced and explicit-capacity creation while rejecting oversized initializers at compile time.
- **Deprecated Synonyms:** collection construction pattern, explicit-capacity construction, literal-deduced construction
- **Related:** CopyOnModifyCollection
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** Explicit-capacity empty construction is valid, but an initializer count that exceeds capacity is a compile-time failure.
















## Relationship Notes

- CopyOnModifyCollection is the foundational value model for the repo.
- SentinelBasedAccess and ProbeFirstAccess define the canonical error-handling discipline for collection APIs.
- Keyword and KeywordCatalog define the canonical keyword identity model used by map-like structures.
- LazySequence and SinkOperation define the execution boundary between deferred pipelines and concrete results.
- ThreadingForm and ValidityAdapter define the readable composition model and the boundary for deferred some-thread semantics.
- CanonicalComparison and FloatingPointExclusion define the canonical comparison contract.
- ClosedNumericDomain, NumericPromotionPolicy, CommonTypeLattice, StaticallyBoundedResult, and DeterministicOverflowPolicy define the bounded numeric semantics that fit the embedded constraint model.
- OwningValue, NonOwningView, and StandardViewType define ownership and lifetime semantics for values versus views.
- CompleteResult, BoundedPrefixResult, DefaultReturningResult, CheckedFailureResult, and PreflightPredicate define canonical result-status and completion semantics.
- LifecycleClassification with RequirementsBacked, CandidateStatus, DeferredStatus, and ExcludedStatus defines API-surface governance vocabulary.
- UnboundedProducer and ProducerMaterialization define explicit producer-to-result boundaries.
- RelationModel governs when relational operations can move from deferred to requirements-backed.
- SemanticPredicateName with StatePredicate and VerbPredicate defines canonical predicate naming constraints.
- ClojureParity and FunctionalStyle explain where semantics are intentionally borrowed from Clojure.
- EmbeddedConstraint, NoHeapConstraint, NoExceptionConstraint, StaticStorage, and DeterministicBehavior define the platform and execution constraints.
- ContractPolicy and SemanticConcept define how correctness constraints are expressed across layers.
- HeaderOnlyDistribution and AmalgamatedHeader define the packaging vocabulary for build and user documentation.
- CompileTimeEvaluation, RegexProfile, and StableHandleModel define capability boundaries that architecture must preserve across compile-time and runtime modes.
- NamespaceAdoptionRoadmap, MvpNamespaceCutLine, OptionalNamespaceCutLine, and NamespacePhaseOrder define architectural scope and rollout boundaries.
- SourceLayout, QualityGate, and NoHeapVerification define the structural and verification constraints that architecture must treat as first-class.
- ComparisonArity, ContentEquality, CapacityConstruction, and CardinalityModel define observable behavior contracts that should map directly into Allium specs.
- InvalidPatternSentinel, PatternValidityProbe, StepDescriptor, UnchangedValueReturn, SwapWithLastCompaction, and ProbeValidityConcept capture edge-case and rejection semantics that behavioral specs must name explicitly.