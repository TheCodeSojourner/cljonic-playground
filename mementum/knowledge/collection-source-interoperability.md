---
type: Reference
title: Collection Source Interoperability
status: designing
description: Approved bounded source-construction policy and open cross-collection materialization decisions.
tags: [collections, interoperability, construction, materialization, bounded-resources]
related: [collection-api-surface-discipline.md, cljonic-next-agenda.md]
depends-on: []
---

# Collection Source Interoperability

## Approved policy

Direct source construction is a total, bounded, non-allocating, non-throwing operation. For Vector, a compatible `std::span` is copied into owned storage. A static-extent source larger than the destination capacity is rejected at compile time. A runtime-sized source larger than capacity materializes the bounded prefix.

Complete-fit semantics belong to the preflight/materialization pair, not to constructors:

- `fits_into(destination, source)` reports whether the complete source fits and is valid.
- `into(destination, source)` returns an owning destination value and applies bounded-prefix materialization when the complete source does not fit.
- A checked constructor is not planned; constructors do not gain an optional or result error channel.

Source views remain read-only observations. Imports copy values and do not retain source lifetime or mutate the source or destination.

## Vector baseline

Vector span construction is `constexpr`, `noexcept`, and heap-free. It requires nothrow element construction and implicit convertibility, supports CTAD, and exposes active storage through a read-only `view()`.

## Open design work

Generalize the policy across Map, Set, Queue, and String. Specify accepted span, string-view, and range sources; String byte validation; Queue logical order; complete-fit predicates; and the `into` result-capacity contract. Propagate approved behavior through Allium specifications, architecture, tests, traceability, and implementation in that order.
