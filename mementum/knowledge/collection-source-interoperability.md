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

For ordinary Vector creation, use the lowercase `vector(...)` factory at
runtime and compile time, with `vector<T>(...)` or
`vector<T, Capacity>(...)` when the element type or capacity is explicit.
`Vector<T, Capacity>` remains a public type; its direct construction from an
external C++ range or view is the deliberate source-import exception.

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
   materialization. Example: `vector<Range<int>, 4>(r)` is a one-element vector.
   A cljonic collection/producer that is *not* exactly the element type is
   rejected as a source argument.
3. **EnclosureConstruction** — the CTAD form (no explicit template argument
   list) whose sole argument is a cljonic collection or producer deduces the
   element type from the argument and a capacity of one, producing a one-element
   value that encloses the argument. For Vector, use the lowercase
   `vector(value)` factory as the idiomatic form; CTAD remains available. It
   mirrors a Clojure collection literal (`[v]`, `#{v}`) and performs neither a
   copy nor element materialization. Same-type copying uses ordinary copy
   initialization, such as `auto copy = v`.

## Materialization pair: `into` vs `fits_into`

- `into` is the **materializer**. Its source domain is
  `concepts::CljonicSource` = `(CljonicCollection ∨ CljonicProducer) ∧
  NothrowConstInputRange` — cljonic collections/producers only, never an
  external C++ range or view.
- `fits_into` is the non-throwing, non-allocating **completeness preflight
  predicate** (returns `bool`); it never materializes.
- Complete materialization ≡ preflight `fits_into`, then apply `into`.
- Both operations use the same `MaterializableSource` admission, including safe,
  non-throwing construction from the actual source reference. A completeness
  preflight cannot accept an element conversion the materializer rejects.

Do not write "use `into` (or `fits_into`) to materialize a range, view, or
producer": external ranges/views materialize through the direct source
constructor, not `into`. For a Vector, express that import with
`Vector<T, Capacity>{external_range}`; use the lowercase factory for ordinary
element-pack creation.

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
supported call target. The harness asserts the message anchor "you cannot
construct a" (whitespace-normalized), so wording drift fails the gate.

## Vector baseline

Vector span construction is `constexpr`, `noexcept`, and heap-free. Like Set,
Map, and Queue imports, it checks the actual `range_reference_t`, not merely
the advertised `range_value_t`, through `NothrowRangeConstruction`. Traversal
and element construction must be non-throwing. Known standard ref/transform
views are checked through their base traversal, iterator copies/moves, and callbacks
because their wrappers do not consistently propagate `noexcept`.

`SafeImplicitConversion<Target, Argument>` admits identical unqualified types
and arithmetic conversions that preserve the entire source-type value domain.
Boolean, character, enum, pointer, and user-defined domains do not implicitly
change types. Safe integer-to-floating widening uses constrained direct
initialization: brace initialization alone would reject even exactly
representable integer domains. Vector retains CTAD and a read-only `view()`,
but its lowercase factory is the idiomatic creation surface.

MapEntry has safe key/value constructors and is no longer a C++ aggregate.
Map packs and entry updates still require the exact declared MapEntry type.
String admits exact string_view, character arrays, and safe character ranges;
it does not import arbitrary implicit string_view conversions or raw pointers.

## Open design work

- Enclosure as an outer type is implemented only where the element model allows
  (Vector, Set, Queue). `Map` (element is `MapEntry`) and `String` (element is
  `char`, never a range) remain deferred.
- Extend the REQ-DIAG-010 constructor-diagnostic policy to any other public
  constructor whose source domain is closed.
- Further source-contract changes follow the required order: requirements,
  vocabulary, architecture, specs, tests, implementation, documentation.
