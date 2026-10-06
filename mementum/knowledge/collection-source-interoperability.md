---
type: Reference
title: Collection Source Interoperability
status: active
description: Bounded source-construction policy for cljonic collections — direct source construction vs. same-type pack vs. CTAD enclosure, and the into/fits_into materialization pair.
tags: [collections, interoperability, construction, materialization, bounded-resources, concepts]
related:
  - /mementum/memories/materializer-vs-preflight-terminology.md
  - /mementum/memories/ctad-copy-deduction-vs-enclosure-guides.md
  - /mementum/memories/source-construction-diagnostic-fallback.md
  - /mementum/memories/same-type-constructor-pack-preference.md
  - /mementum/knowledge/collection-api-surface-discipline.md
  - /mementum/knowledge/cljonic-next-agenda.md
depends-on: []
---

# Collection Source Interoperability

Direct source construction is total, bounded, non-allocating, and non-throwing.
It copies source elements into owned bounded storage, leaves the source
unchanged, and is independent of the source lifetime. A checked constructor is
not planned: constructors do not gain an optional, result, exception, or other
error channel.

## Three admission paths (do not conflate)

A collection constructor with a single non-scalar argument resolves through
exactly one of three mechanisms:

1. **SourceConstruction** — an external, non-cljonic C++ range or view source is
   copied into owned bounded storage. The direct source constructor admits
   *only* external non-cljonic ranges/views; a cljonic collection or producer is
   never a source argument (`CljonicSourceIsNotConstructorSource`,
   REQ-FN-027A).
2. **SameTypeArgumentIsOneElement** — a single argument whose type is exactly
   the element type is pack construction of one element, never source
   materialization. Example: `Vector<Range<int>, 4>{r}` is a one-element vector.
   A cljonic collection/producer that is *not* exactly the element type is
   rejected as a source argument.
3. **EnclosureConstruction** — the CTAD form (no explicit template argument
   list) whose sole argument is a cljonic collection or producer deduces the
   element type from the argument and a capacity of one, producing a one-element
   value that encloses the argument. It mirrors a Clojure collection literal
   (`[v]`, `#{v}`) and performs neither a copy nor element materialization.
   Same-type copy construction still exists via an explicit template argument
   list naming the argument's own type and capacity.

## Materialization pair: `into` vs `fits_into`

- `into` is the **materializer**. Its source domain is
  `concepts::CljonicSource` = `(CljonicCollection ∨ CljonicProducer) ∧
  NothrowConstInputRange` — cljonic collections/producers only, never an
  external C++ range or view.
- `fits_into` is the non-throwing, non-allocating **completeness preflight
  predicate** (returns `bool`); it never materializes.
- Complete materialization ≡ preflight `fits_into`, then apply `into`.

Do not write "use `into` (or `fits_into`) to materialize a range, view, or
producer": external ranges/views materialize through the direct source
constructor, not `into`.

## Bounds and overflow

- A static-extent source larger than destination capacity is rejected at compile
  time (a diagnostic, not a result status).
- A runtime/unknown extent that exceeds capacity materializes the deterministic
  bounded prefix (`BoundedPrefixResult`), preserving source traversal order.
- `fits_into` compares source count ≤ remaining capacity with overflow-safe
  arithmetic, and is `false` for an unbounded producer regardless of its capped
  observable count.
- `Map`, `Set`, and `Queue` first fold their pack arguments through their
  primitive (`Map`: `assoc`; `Set`/`Queue`: `conj`) in order; a `Map` pack
  element must be nominally `MapEntry<K,V>`.
- `String<N>` span import applies the invalid-byte replacement policy (every
  byte outside `0x01`–`0x7F`, including `0x00`, becomes `'.'`) *before* the
  bounded prefix is selected; `fits_into` is false if any byte is invalid or the
  converted content does not fit; a constant-evaluation invalid or overlong
  source is a compile-time diagnostic.

## Rejection diagnostic (REQ-DIAG-010)

Primary admission is expressed through named constraints. A rejected cljonic
collection/producer argument is caught by an additional constructor overload
constrained on the negation of the source-admission rule, emitting one targeted
`static_assert` (`SourceConstructionDiagnostic`) that names the collection, the
violated rule, and both alternatives: `into` for materialization (preflighted by
`fits_into`), or the enclosure form for a single element. The fallback exists to
explain rejection, not to accept it — it never produces a value and is never a
supported call target. The harness asserts the message anchor "is not a
constructor source" (whitespace-normalized), so wording drift fails the gate.

## Vector baseline

Vector span construction is `constexpr`, `noexcept`, and heap-free. It requires
nothrow element construction and implicit convertibility, supports CTAD, and
exposes active storage through a read-only `view()`.

## Open design work

- Enclosure as an outer type is implemented only where the element model allows
  (Vector, Set, Queue). `Map` (element is `MapEntry`) and `String` (element is
  `char`, never a range) remain deferred.
- Extend the REQ-DIAG-010 constructor-diagnostic policy to any other public
  constructor whose source domain is closed.
- Converge the cross-collection source contract (accepted span/string-view/range
  sources, String byte validation, Queue logical FIFO order, complete-fit
  predicates, and the `into` result-capacity contract) through Allium
  specifications, architecture, tests, traceability, and implementation in that
  order.
