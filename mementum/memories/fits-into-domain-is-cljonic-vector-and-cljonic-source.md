---
type: Decision
symbol: 🎯
title: fits-into-domain-is-cljonic-vector-and-cljonic-source
related:
  - /mementum/memories/materializer-vs-preflight-terminology.md
  - /mementum/knowledge/collection-source-interoperability.md
  - /mementum/memories/deferred-api-promotion-checklist.md
---
2026-10-19, human decision: `fits_into` admits exactly a `CljonicVector`
destination and exactly a `CljonicCollection` or `CljonicProducer` source
(through `CljonicSource`). External C++ ranges/views (`std::span`,
`std::string_view`) are never `fits_into` sources; they materialize through
direct source construction (`REQ-FN-027A`). Non-Vector destinations (`String`,
`Map`, `Set`, `Queue`) are rejected.

This supersedes the older `REQ-FN-027` text claiming `into`/`fits_into`
accepted `std::span` and `std::string_view`, including `String<N>`
destinations. The requirement now matches the architecture
(`materialization_operations → constrain_source_by(CljonicSource)`) and the
implementation. `REQ-COLL-013A` attributes String byte replacement to
`REQ-FN-027A`.

A `cljonic::String` is a valid *source* when the destination is
`Vector<char, N>`; only a String *destination* is rejected.
