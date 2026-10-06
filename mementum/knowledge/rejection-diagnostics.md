---
type: Reference
title: Rejection Diagnostics
status: active
description: The cljonic rejection-diagnostic policy — a gate is not a teacher. Why closed-domain rejections get a targeted static_assert fallback, how the fallback is built, which domain axis decides whether one is needed, and the detectability contract that replaces callability probes.
tags: [diagnostics, concepts, compile-time, constructors, free-functions, verification]
related:
  - /mementum/memories/rejection-diagnostic-fallback.md
  - /mementum/memories/source-construction-diagnostic-fallback.md
  - /mementum/memories/diagnostic-fallback-tracks-constraint-axis.md
  - /mementum/memories/requires-expression-constrained-template-sfinae-limitation.md
  - /mementum/memories/rejection-diagnostic-message-anchor.md
  - /mementum/knowledge/collection-source-interoperability.md
  - /mementum/knowledge/value-equality-domain.md
depends-on: []
---

# Rejection Diagnostics

A named constraint at a public API boundary is a **gate, not a teacher**. It
decides whether a call is admitted; it does not explain the rejection. When a
call is rejected, overload resolution does not stop — it falls through to the
next viable candidate, whose message describes a different problem than the
caller actually has. The cljonic rejection-diagnostic policy installs a
**diagnostic fallback** that wins exactly the rejected case and emits one
targeted message.

## The misleading fall-through (why this exists)

- A `Vector`/`Set`/`Queue`/`Map`/`String` **source constructor** is gated
  `!is_cljonic_collection_v && !is_cljonic_producer_v`. A rejected cljonic
  argument falls through to the **variadic pack constructor**, whose generic
  `static_assert` says *"could not convert a producer to the element type"* —
  but no element conversion was intended, so the message misleads.
- `equal`/`not_equal` and the producer factories degrade into a **list of
  rejected concept candidates** instead of naming the violated domain rule.

## Mechanism (REQ-DIAG-003)

For each supported arity, add an overload **constrained on the negation of the
admission gate** whose body is
`static_assert(concepts_detail::dependent_false<...>, "...")`. Properties:

- It is more specialized than the sibling it competes with, so it wins exactly
  the rejected case and never affects an accepted path.
- `dependent_false` makes the assertion fire only at instantiation.
- It **never returns a value** (or returns a concrete sentinel, e.g.
  `RejectedProducerFactory`, so `auto` deduction stays well-formed and the
  `static_assert` is the sole diagnostic) and is **never a supported call
  target**.
- The message names the operation, the rejected operand type(s), and the
  violated domain rule, and states the correct alternative — in user-facing
  terms, never compiler-specific wording (REQ-DIAG-004).

## Two governing requirements

- **REQ-DIAG-009 — public free functions.** `equal`, `not_equal`, and the
  producer factory functions `repeat`, `cycle`, `iterate`, `repeatedly`.
  Producer admission uses the product-specific concept: `NothrowCollectionElement`
  (`repeat`), `CljonicSource` (`cycle`), `IterateStep` (`iterate`),
  `RepeatedlyStep` (`repeatedly`).
- **REQ-DIAG-010 — public source constructors.** The five collections' direct
  source constructors. The message directs the caller to `into` for
  materialization (preflighted by `fits_into`) or to the enclosure form for a
  single element.

## SFINAE-friendly capability concepts

A capability concept that a free function is constrained on must itself be
SFINAE-friendly. Naming a member type in the requires-expression **parameter
list** breaks that: for a type lacking the member, substitution hard-errors
(`no type named 'lookup_type'`) instead of yielding `false`. Put the member type
in the requires-expression **body** and reference it through `std::declval`:

```cpp
template <typename C>
concept LookupCollection = CljonicCollection<C> && requires(const C& c) {
    typename C::lookup_type;
    { c(std::declval<const typename C::lookup_type&>()) } noexcept;
    { c.contains(std::declval<const typename C::lookup_type&>()) } noexcept -> std::same_as<bool>;
};
```

`AssociativeCollection` additionally must use `association_value_type` (not
`value_type`) — for `Map`, `value_type = MapEntry<K,V>` but `assoc` takes the
mapped `V`. `ConjableCollection` follows the same body form. This is what lets
`assoc`/`get`/`conj`/`can_assoc`/`can_conj` be gated on the named concept *and*
produce a clean rejection. See
`/mementum/memories/capability-concept-member-types-belong-in-body.md`.

## Which axis decides whether a fallback is needed

Fallback necessity tracks **which axis** the closed domain is gated on — **not**
whether the type carries a class-template constraint. Most cljonic types
(`Vector`/`Set`/`Queue`/`Map`/`String`, `MapEntry`) are class-template-gated,
yet `Vector`/`Set` still need and carry a REQ-DIAG-010 fallback.

- **Axis A — class-instantiation (type-argument) constraint.** `Range`'s
  `signed_integral`, `MapEntry`'s `NothrowStableEqualityComparable`,
  `NothrowCollectionElement`. Rejection happens at instantiation, **before any
  constructor is considered**, and C++ already emits one named constraint
  diagnostic. A constructor fallback is **unreachable and impossible** — a
  constructor parameter cannot repair an already-failed class instantiation.
- **Axis B — constructor-call (call-argument) constraint.** Rejection falls
  through overload resolution to a **sibling** constructor with a misleading
  message. A fallback is **needed and possible** (REQ-DIAG-010). `Vector`/`Set`
  sit on **both** axes: class-template-gated on the element type, and
  constructor-gated on the source argument.

Third case: **`Variant`** is Axis B (its converting constructor is gated on
`matching_alternative_count_v == 1`) but has **no sibling constructor**, so
rejection already reads accurately as "no matching constructor" — no fallback.

## Deliberate carve-outs (REQ-DIAG-010 exclusions clause)

| Category                           | Members                                                                                  | Why exempt                                                                                                                                     |
| ---------------------------------- | ---------------------------------------------------------------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------- |
| Axis A (type-argument) gated       | `Range`, `MapEntry`, `Repeat`, `Cycle`, `Iterate`, `Repeatedly`, `Variant`               | No constructor is reached; the class-template constraint already yields one named diagnostic                                                   |
| Availability-guard ctor `requires` | `Iterate()` / `Repeatedly()` default ctors (`requires std::default_initializable<Step>`) | An availability guard (REQ-VAL-017D), not a closed source-domain rejection                                                                     |
| Aggregate-like struct              | `MapEntry`                                                                               | Type-argument gated; any constructor added to host a fallback would destroy aggregate status (structured bindings, `MapEntry<int,int>{1,100}`) |

The 2026-10-06 constructor sweep confirmed **nothing else to add**: every public
constructor is either covered or correctly carved out.

## Detectability contract (do not probe callability)

Compile-time detection of domain support must use the **named admission
concepts** and their `*_admissible_v` predicates — **never**
`requires { call(...) }`. Once a fallback exists, a domain-rejected argument
**may satisfy callability** (the fallback overload is viable); that callability
is not a supported interface. Rejection tests must therefore assert the
underlying admission concept directly. (This bit 9 probes in the
producer-diagnostic slice.)

## Gotchas

- GCC `-Wtemplate-body` errors on "no return statement in constexpr function
  returning non-void" — add an unreachable trailing `return false;` after the
  `static_assert`.
- `dependent_false` is required so the assertion fires only at instantiation.
- Name the parameter `[[maybe_unused]]` (repo idiom): unnamed trips
  readability-named-parameter; named-but-unused trips `-Wunused-parameter`.
- The REQ-DIAG-010 fallback's implicit CTAD guide is non-viable (return params
  non-deducible), so it does not perturb the enclosure guides.
- The compile-fail harnesses assert a **message anchor** (e.g. `"is not a
  construction source"`, whitespace-normalized) so wording drift fails the gate.
- GCC can hard-error on a `requires { expr; }` simple-requirement against a
  constrained template selected via brace-init, instead of yielding `false` —
  assert traits/concepts directly instead of wrapping in `requires`.

## Traceability

`REQ-DIAG-009` ∧ `REQ-DIAG-010` ∧ `REQ-DIAG-001` ∧ `REQ-DIAG-003` →
architecture `S3_rejection_diagnostic` → `specs/capabilities/diagnostics.allium`
(entity `RejectionDiagnostic`). Message wording, the instantiation-dependent
`static_assert` mechanism, and the "never a supported call target" obligation
are **governance** properties held in requirements/architecture — the Allium
entity records only observable contract, so the requirement-level carve-out
needs no spec/arch propagation.

## Open work

- Slice B — the `cljonic::Variant` free-function API — would add new public
  entry points requiring their own REQ-DIAG-009/010 disposition.
- A future `range()` factory function would give `Range` a REQ-DIAG-009
  diagnostic fallback (currently excluded; the fallback belongs to the factory,
  not the type's constructors).
