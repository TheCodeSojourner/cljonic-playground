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

## Relationship Notes

- CopyOnModifyCollection is the foundational value model for the repo.
- SentinelBasedAccess and ProbeFirstAccess define the canonical error-handling discipline for collection APIs.
- KeywordCatalog defines the canonical keyword identity model used by map-like structures.
- LazySequence and SinkOperation define the execution boundary between deferred pipelines and concrete results.
- ThreadingForm and ValidityAdapter define the readable composition model and the boundary for deferred some-thread semantics.
- CanonicalComparison and FloatingPointExclusion define the canonical comparison contract.