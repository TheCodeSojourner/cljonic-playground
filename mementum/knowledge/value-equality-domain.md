---
type: Reference
title: Value Equality Domain
status: active
description: The unified stable-equality architecture across collections, composite values, and producers; admission contracts and the sequence-equality prohibition.
tags: [equality, producers, collections, composite-values, concepts, requirements]
related:
  - /mementum/knowledge/cpp-concept-layer.md
  - /mementum/knowledge/collection-api-surface-discipline.md
  - /mementum/memories/producer-parameter-equality.md
  - /mementum/memories/nothrow-stable-equality-comparable-concept.md
  - /mementum/memories/cljonic-design-notes.md
depends-on: [requirements/cljonic-requirements-module-2.md, requirements/cljonic-requirements-module-3.md, requirements/cljonic-requirements-module-4.md]
---

# Value Equality Domain

This synthesis consolidates the durable equality architecture completed across
Streams A, B, and C (composite values, collection equality, producer storage,
producer parameter equality). It is written for future AI sessions: read this
before re-deriving how equality works in cljonic.

## The one equality engine

`StableEqualityComparable<T>` is the single recursive admission gate
(REQ-CAP-010, REQ-NUM-007):

```
std::equality_comparable<T>
&& !concepts_detail::contains_floating_point_v<T>
&& !concepts_detail::contains_callable_v<T>
```

- Floats are rejected as a class (NaN/precision instability); storability
  (REQ-NUM-001) never implies equality.
- Callables are rejected as a class, including function pointers (a function
  pointer has `operator==` but admits only address-as-key semantics).
- The walkers (`contains_floating_point` / `contains_callable`) are specialized
  per header and recurse through `std::variant` alternatives, cljonic
  collections, `MapEntry`, and producer stored parameters.
- `NothrowStableEqualityComparable = StableEqualityComparable &&
  NothrowCollectionElement` is the map-key / set-element admission contract.

## Three equality surfaces, one rule shape

| Surface | operator== semantics | Named operation | Constraint |
|---|---|---|---|
| Vector/String/Queue | order-sensitive (count + logical order) | — (planned `equal` family deferred) | stable element equality |
| Map/Set | order-insensitive (count + matching entries) | — | stable key/value equality |
| Range/Repeat/Cycle | producer parameter equality (stored parameters only) | `parameters_equal` | stable parameter equality |
| Iterate/Repeatedly | none | none | callable step → excluded at type boundary |

Producer parameter equality (REQ-FN-014B) is normative for every producer whose
stored parameters all admit stable equality: provide both `==` and
`parameters_equal` with identical semantics, O(1), constexpr, noexcept,
non-allocating, never traversing the produced sequence. Distinct parameters
compare unequal even when produced sequences coincide
(`Range{0,5,0} != Range{0,7,0}`; `Repeat{7} != Repeat{7,0U}`).

## The sequence-equality prohibition (REQ-SEQ-016 / REQ-FN-014A)

Statically-unbounded-capable values MUST NOT provide sequence equality —
equality that compares a materialized or observed sequence fails at compile
time. The prohibition covers produced sequences only; parameter-structural
producer equality is explicitly exempt because it is O(1) and cannot hang.

## Composite values (std::variant)

Alternative-strict equality: same alternative AND that alternative's values
compare equal. No cross-type numeric unification (`variant<int,long>{1}` is not
`variant<int,long>{1L}`), no hash-based equality. The composite key/element
domain includes scalars, scoped enums, collections, and producers with stable
parameters, all subject to recursive component analysis.

## Known limitations (do not rediscover these)

1. User-defined aggregates with float members are unanalyzable (no reflection);
   the walker cannot enforce recursion for them. Documented limitation, revisit
   with the aggregate story.
2. Set duplicate insertion at compile time aborts (`std::abort()` is not a
   constant expression); verify duplicate no-op behavior at runtime, use
   distinct elements in constexpr contexts.
3. Single range-typed constructor argument selects the source constructor, not
   the pack constructor; tests use multi-element packs (pending design
   decision, see state.md carry-forward).
4. `equal` / `equal_by` / `identical` are indexed in docs but unimplemented
   (REQ-FN-002E deferred, separate slice).
