---
created: 2026-08-05
last_updated: 2026-08-05
status: draft
---

## Core Vocabulary

### Copy-on-Modify Collection
- **Definition:** A fixed-capacity value type backed by static storage that returns a modified copy instead of mutating in place. In this repo, vector, set, map, and string all follow this model.
- **Deprecated Synonyms:** bounded immutable collection, fixed-capacity collection, array-backed collection
- **Related:** Sentinel-Based Access, Keyword Catalog, Lazy Sequence, Threading Form
- **Usage:** Specification and implementation
- **Examples:** `auto xs = vector<4>{1, 2, 3}; auto ys = conj(xs, 4);`

### Sentinel-Based Access
- **Definition:** The access model where missing or invalid lookup returns a default sentinel value such as `T{}` or `char{}` instead of raising an error or returning an explicit status object.
- **Deprecated Synonyms:** sentinel access, sentinel return, default-value access
- **Related:** Copy-on-Modify Collection, Probe-First Access
- **Usage:** Specification and implementation
- **Examples:** `auto value = get(m, key);` and callers use `contains(m, key)` or `has_index(xs, i)` before relying on the result.

### Probe-First Access
- **Definition:** The required calling discipline for ambiguous domains: first ask a presence or index-validity question, then perform sentinel-based access only when the probe succeeds.
- **Deprecated Synonyms:** guard-before-get, presence check
- **Related:** Sentinel-Based Access, Copy-on-Modify Collection
- **Usage:** Specification and implementation
- **Examples:** `if (contains(m, key)) { auto value = get(m, key); }`

### Keyword Catalog
- **Definition:** The canonical finite set of global keywords represented as enum values and used wherever keyword-typed keys appear in maps or other APIs.
- **Deprecated Synonyms:** keyword enum, keyword registry
- **Related:** Copy-on-Modify Collection
- **Usage:** Specification and implementation
- **Examples:** `assoc(m, keyword::name, value)` where `keyword::name` comes from the catalog.

### Lazy Sequence
- **Definition:** A value-owning sequence view that defers element production and transformation until consumption, avoids hidden materialization, and may represent finite or infinite sources.
- **Deprecated Synonyms:** lazy view, deferred sequence
- **Related:** Sink Operation, Threading Form, Copy-on-Modify Collection
- **Usage:** Specification and implementation
- **Examples:** `auto xs = range(0, 10); auto ys = map(f, xs); auto out = into_vector<10>(ys);`

### Sink Operation
- **Definition:** An explicit operation that realizes a lazy sequence into a concrete bounded collection or terminal result. Materialization is never implicit.
- **Deprecated Synonyms:** materialization sink, eager sink, realization endpoint
- **Related:** Lazy Sequence, Copy-on-Modify Collection
- **Usage:** Specification and implementation
- **Examples:** `into_vector<16>(xs)`, reduction endpoints, or other named sink operations.

### Threading Form
- **Definition:** A composition API that improves readability by passing the prior result through a sequence of steps in a defined argument position. The canonical forms are `thread_first`, `thread_last`, `as_thread`, `cond_thread_first`, and `cond_thread_last`.
- **Deprecated Synonyms:** threading macro, pipeline form
- **Related:** Lazy Sequence, Validity Adapter
- **Usage:** Specification and implementation
- **Examples:** `thread_first(xs, step1, step2)` and `thread_last(xs, step1, step2)`.

### Validity Adapter
- **Definition:** The explicit wrapper `with_validity(x, pred)` that attaches a validity probe to a value so deferred some-threading APIs can short-circuit without relying on sentinel equality.
- **Deprecated Synonyms:** validity wrapper
- **Related:** Threading Form, Sentinel-Based Access, Probe-First Access
- **Usage:** Specification and implementation
- **Examples:** `auto wrapped = with_validity(get(m, key), [](auto const& v) { return v != 0; });`

### Canonical Comparison
- **Definition:** The canonical comparison model in which `equal`, `less`, `less_equal`, `greater`, and `greater_equal` are variadic, deep where appropriate, and reject floating-point comparison under the default policy.
- **Deprecated Synonyms:** strict comparison
- **Related:** Floating-Point Exclusion
- **Usage:** Specification and implementation
- **Examples:** `equal(a, b, c)` and `less(x, y, z)` return chained boolean results with zero-arity and one-arity calls evaluating to `true`.

### Floating-Point Exclusion
- **Definition:** The default rule that floating-point values are not part of canonical comparison or the dynamic value model; any comparison involving floats requires an explicit override API such as `_with` forms.
- **Deprecated Synonyms:** strict float policy
- **Related:** Canonical Comparison
- **Usage:** Specification and implementation
- **Examples:** `equal(1.0, 1.0)` is rejected under canonical APIs, while an explicit comparator-based override may be allowed.

## Relationship Notes

- Copy-on-Modify Collection is the foundational value model for the repo.
- Sentinel-Based Access and Probe-First Access define the canonical error-handling discipline for collection APIs.
- Keyword Catalog defines the canonical keyword identity model used by map-like structures.
- Lazy Sequence and Sink Operation define the execution boundary between deferred pipelines and concrete results.
- Threading Form and Validity Adapter define the readable composition model and the boundary for deferred some-thread semantics.
- Canonical Comparison and Floating-Point Exclusion define the canonical comparison contract.