---
created: 2026-08-05
last_updated: 2026-08-05
status: draft
---

## Core Vocabulary

### CopyOnModifyCollection
- **Definition:** A fixed-capacity value type backed by static storage that returns a modified copy instead of mutating in place. In this repo, vector, set, map, and string all follow this model.
- **Deprecated Synonyms:** Copy-on-Modify Collection, bounded immutable collection, fixed-capacity collection, array-backed collection
- **Related:** SentinelBasedAccess, KeywordCatalog, LazySequence, ThreadingForm
- **Usage:** Specification and implementation
- **Examples:** `auto xs = vector<4>{1, 2, 3}; auto ys = conj(xs, 4);`

### SentinelBasedAccess
- **Definition:** The access model where missing or invalid lookup returns a default sentinel value such as `T{}` or `char{}` instead of raising an error or returning an explicit status object.
- **Deprecated Synonyms:** Sentinel-Based Access, sentinel access, sentinel return, default-value access
- **Related:** CopyOnModifyCollection, ProbeFirstAccess
- **Usage:** Specification and implementation
- **Examples:** `auto value = get(m, key);` and callers use `contains(m, key)` or `has_index(xs, i)` before relying on the result.

### ProbeFirstAccess
- **Definition:** The required calling discipline for ambiguous domains: first ask a presence or index-validity question, then perform sentinel-based access only when the probe succeeds.
- **Deprecated Synonyms:** Probe-First Access, guard-before-get, presence check
- **Related:** SentinelBasedAccess, CopyOnModifyCollection
- **Usage:** Specification and implementation
- **Examples:** `if (contains(m, key)) { auto value = get(m, key); }`

### KeywordCatalog
- **Definition:** The canonical finite set of global keywords represented as enum values and used wherever keyword-typed keys appear in maps or other APIs.
- **Deprecated Synonyms:** Keyword Catalog, keyword enum, keyword registry
- **Related:** CopyOnModifyCollection
- **Usage:** Specification and implementation
- **Examples:** `assoc(m, keyword::name, value)` where `keyword::name` comes from the catalog.

### LazySequence
- **Definition:** A value-owning sequence view that defers element production and transformation until consumption, avoids hidden materialization, and may represent finite or infinite sources.
- **Deprecated Synonyms:** Lazy Sequence, lazy view, deferred sequence
- **Related:** SinkOperation, ThreadingForm, CopyOnModifyCollection
- **Usage:** Specification and implementation
- **Examples:** `auto xs = range(0, 10); auto ys = map(f, xs); auto out = into_vector<10>(ys);`

### SinkOperation
- **Definition:** An explicit operation that realizes a lazy sequence into a concrete bounded collection or terminal result. Materialization is never implicit.
- **Deprecated Synonyms:** Sink Operation, materialization sink, eager sink, realization endpoint
- **Related:** LazySequence, CopyOnModifyCollection
- **Usage:** Specification and implementation
- **Examples:** `into_vector<16>(xs)`, reduction endpoints, or other named sink operations.

### ThreadingForm
- **Definition:** A composition API that improves readability by passing the prior result through a sequence of steps in a defined argument position. The canonical forms are `thread_first`, `thread_last`, `as_thread`, `cond_thread_first`, and `cond_thread_last`.
- **Deprecated Synonyms:** Threading Form, threading macro, pipeline form
- **Related:** LazySequence, ValidityAdapter
- **Usage:** Specification and implementation
- **Examples:** `thread_first(xs, step1, step2)` and `thread_last(xs, step1, step2)`.

### ValidityAdapter
- **Definition:** The explicit wrapper `with_validity(x, pred)` that attaches a validity probe to a value so deferred some-threading APIs can short-circuit without relying on sentinel equality.
- **Deprecated Synonyms:** Validity Adapter, validity wrapper
- **Related:** ThreadingForm, SentinelBasedAccess, ProbeFirstAccess
- **Usage:** Specification and implementation
- **Examples:** `auto wrapped = with_validity(get(m, key), [](auto const& v) { return v != 0; });`

### CanonicalComparison
- **Definition:** The canonical comparison model in which `equal`, `less`, `less_equal`, `greater`, and `greater_equal` are variadic, deep where appropriate, and reject floating-point comparison under the default policy.
- **Deprecated Synonyms:** Canonical Comparison, strict comparison
- **Related:** FloatingPointExclusion
- **Usage:** Specification and implementation
- **Examples:** `equal(a, b, c)` and `less(x, y, z)` return chained boolean results with zero-arity and one-arity calls evaluating to `true`.

### FloatingPointExclusion
- **Definition:** The default rule that floating-point values are not part of canonical comparison or the dynamic value model; any comparison involving floats requires an explicit override API such as `_with` forms.
- **Deprecated Synonyms:** Floating-Point Exclusion, strict float policy
- **Related:** CanonicalComparison
- **Usage:** Specification and implementation
- **Examples:** `equal(1.0, 1.0)` is rejected under canonical APIs, while an explicit comparator-based override may be allowed.

## Supporting Vocabulary

### ClojureParity
- **Definition:** The design goal of matching Clojure semantics where feasible without violating the repo's embedded, bounded, and deterministic constraints.
- **Deprecated Synonyms:** Clojure parity, Clojure-feel
- **Related:** LazySequence, CanonicalComparison, FunctionalStyle
- **Usage:** Architecture, specification, and documentation
- **Examples:** Zero-arity comparison returns `true`, lazy sequence behavior is preferred by default, and threading forms are adopted for readable composition.

### EmbeddedConstraint
- **Definition:** The platform constraint set that assumes embedded targets with bounded resources and therefore prioritizes fixed capacity, predictable execution, and explicit profiles.
- **Deprecated Synonyms:** embedded constraint, embedded systems constraint
- **Related:** NoHeapConstraint, NoExceptionConstraint, DeterministicBehavior, BuildProfile
- **Usage:** Architecture, specification, implementation, and documentation
- **Examples:** Collection APIs use fixed-capacity storage and avoid runtime allocation on strict profiles.

### NoHeapConstraint
- **Definition:** The rule that strict profiles must not perform heap allocation anywhere in the library's behavior or infrastructure.
- **Deprecated Synonyms:** no heap allocation, no-heap rule
- **Related:** EmbeddedConstraint, StaticStorage, DeterministicBehavior, BuildProfile
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** Verification gates prohibit forbidden allocation APIs and bounded collections store data without dynamic allocation.

### NoExceptionConstraint
- **Definition:** The rule that library behavior is non-throwing and does not depend on exception-based control flow.
- **Deprecated Synonyms:** no exceptions, non-throwing rule
- **Related:** EmbeddedConstraint, SentinelBasedAccess, ContractPolicy, DeterministicBehavior
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** Missing access returns sentinel values instead of throwing, and contract failures route through explicit policy hooks.

### StaticStorage
- **Definition:** Storage with compile-time-bounded capacity and no dependency on dynamic allocation, typically realized with array-backed layouts.
- **Deprecated Synonyms:** static storage, array-backed storage
- **Related:** CopyOnModifyCollection, NoHeapConstraint, EmbeddedConstraint
- **Usage:** Architecture, specification, implementation, and documentation
- **Examples:** Vector, set, map, and string values use fixed-capacity array-backed storage.

### DeterministicBehavior
- **Definition:** The requirement that behavior, failure modes, and profile-selected semantics remain predictable and stable for the same inputs and configuration.
- **Deprecated Synonyms:** deterministic semantics, deterministic execution
- **Related:** EmbeddedConstraint, NoHeapConstraint, NoExceptionConstraint, ContractPolicy, BuildProfile
- **Usage:** Architecture, specification, tests, and documentation
- **Examples:** Full-capacity `conj` returns the unchanged value deterministically, and invalid runtime regex compilation returns a stable invalid-pattern sentinel.

### ContractPolicy
- **Definition:** The project-wide contract model governing public API boundaries, invariant checks, and the deterministic policy hook used when contract violations occur.
- **Deprecated Synonyms:** contract, contract boundary, contract handling
- **Related:** NoExceptionConstraint, DeterministicBehavior, BuildProfile
- **Usage:** Architecture, specification, implementation, and documentation
- **Examples:** Public API preconditions are expressed as contracts and violations route through a selected terminate, trap, or assert policy.

### SemanticConcept
- **Definition:** A small, composable compile-time concept used to encode semantic API constraints and reject invalid combinations with clear diagnostics.
- **Deprecated Synonyms:** concept, semantic constraint
- **Related:** CanonicalComparison, ThreadingForm, BuildProfile
- **Usage:** Specification, implementation, tests, and documentation
- **Examples:** Strict comparison excludes floating-point types through concept constraints, and deferred some-threading uses `probe_validatable` as a concept gate.

### FunctionalStyle
- **Definition:** The preferred programming style for the library: pure free functions, immutable return values, explicit sinks, and minimal hidden side effects.
- **Deprecated Synonyms:** functional style, immutable style
- **Related:** CopyOnModifyCollection, LazySequence, ThreadingForm, ClojureParity
- **Usage:** Architecture, specification, implementation, and documentation
- **Examples:** Canonical APIs are free-function-first, transforms remain lazy by default, and eager materialization is explicit.

### BuildProfile
- **Definition:** A named configuration mode that selects capability and verification differences without changing the core API vocabulary.
- **Deprecated Synonyms:** profile, build profile
- **Related:** EmbeddedConstraint, DeterministicBehavior, ContractPolicy, HeaderOnlyDistribution
- **Usage:** Architecture, specification, implementation, tests, and documentation
- **Examples:** Strict host profiles enable sanitizers and coverage gates, while constrained embedded profiles may disable optional regex support.

### HeaderOnlyDistribution
- **Definition:** The packaging model in which the library is delivered as headers only, with development sources organized separately from the generated distribution artifact.
- **Deprecated Synonyms:** header-only, header-only library
- **Related:** AmalgamatedHeader, StaticStorage
- **Usage:** Architecture, implementation, build, and documentation
- **Examples:** Production implementation units live as headers and are combined into a distributable single-header output.

### AmalgamatedHeader
- **Definition:** The generated single-header distribution artifact assembled from the per-function production headers that remain the source of truth during development.
- **Deprecated Synonyms:** amalgamated header, single-header distribution
- **Related:** HeaderOnlyDistribution
- **Usage:** Implementation, build, tests, and documentation
- **Examples:** Build automation regenerates the distributable header from the production header tree.

## Relationship Notes

- CopyOnModifyCollection is the foundational value model for the repo.
- SentinelBasedAccess and ProbeFirstAccess define the canonical error-handling discipline for collection APIs.
- KeywordCatalog defines the canonical keyword identity model used by map-like structures.
- LazySequence and SinkOperation define the execution boundary between deferred pipelines and concrete results.
- ThreadingForm and ValidityAdapter define the readable composition model and the boundary for deferred some-thread semantics.
- CanonicalComparison and FloatingPointExclusion define the canonical comparison contract.
- ClojureParity and FunctionalStyle explain where semantics are intentionally borrowed from Clojure.
- EmbeddedConstraint, NoHeapConstraint, NoExceptionConstraint, StaticStorage, and DeterministicBehavior define the platform and execution constraints.
- ContractPolicy, SemanticConcept, and BuildProfile define how correctness and portability constraints are expressed across layers.
- HeaderOnlyDistribution and AmalgamatedHeader define the packaging vocabulary for build and user documentation.