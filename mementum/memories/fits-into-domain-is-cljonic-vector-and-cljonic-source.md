---
type: Decision
symbol: 🎯
title: fits-into-domain-is-cljonic-vector-and-cljonic-source
related:
  - /mementum/memories/materializer-vs-preflight-terminology.md
  - /mementum/knowledge/collection-source-interoperability.md
  - /mementum/memories/deferred-api-promotion-checklist.md
---
2026-10-29, human decision (superseding the 2026-10-19 Vector-only decision):
`into` and `fits_into` admit `CljonicVector`, `CljonicSet`, `CljonicMap`,
`CljonicQueue`, and `CljonicString` destinations. The source domain is
unchanged — exactly a `CljonicCollection` or `CljonicProducer` (through
`CljonicSource`); external C++ ranges/views (`std::span`,
`std::string_view`) are never `into`/`fits_into` sources; they materialize
through direct source construction (`REQ-FN-027A`). Producer destinations are
rejected.

This supersedes the older `REQ-FN-027` text claiming `into`/`fits_into`
accepted `std::span` and `std::string_view` sources. The requirement matches
the architecture (`materialization_operations → constrain_source_by(CljonicSource)`)
and the implementation. `REQ-COLL-013A` attributes String byte replacement to
`REQ-FN-027A`.

A `cljonic::String` is a valid *source* when the destination is
`Vector<char, N>`. String `into` appends characters in traversal order,
rejects invalid characters at constant evaluation, and replaces invalid
characters with `'.'` at runtime; String `fits_into` compares finite source
count to remaining capacity without evaluating character validity.
