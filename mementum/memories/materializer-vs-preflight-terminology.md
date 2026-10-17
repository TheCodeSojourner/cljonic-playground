---
type: Insight
symbol: 💡
title: materializer-vs-preflight-terminology
related: [/mementum/knowledge/collection-source-interoperability.md, /mementum/memories/source-construction-diagnostic-fallback.md, /mementum/memories/same-type-constructor-pack-preference.md]
---
`into` is the materializer; `fits_into` is a non-throwing, non-allocating
completeness **preflight predicate** (returns bool), never a materializer.

`into`'s source domain is `concepts::CljonicSource` =
`(CljonicCollection || CljonicProducer) && NothrowConstInputRange` — it CANNOT
take an external C++ range/view. External C++ ranges/views materialize via the
direct **source/interop constructor** (SourceConstruction). So "use `into` to
materialize a range, view, or producer" is WRONG: only cljonic
collections/producers go through `into`.

Correct collection-top phrasing:
"A non-cljonic C++ range or view constructor parameter is copied into the X; a
cljonic collection or producer is not a constructor source -- X{X{...}} encloses
it as a single element. To materialize one into a destination Vector, use into;
fits_into checks whether the complete source fits in that Vector's remaining
capacity."

REQ-DIAG-010 fallback guidance should name Vector: `into` and `fits_into`
accept only `CljonicVector` destinations. Never suggest materializing into Map,
Queue, Set, or String.

2026-10-17, human-approved: "source" alone spanned two domains; constructor
prose now says "constructor parameter" / "constructor source", while the
`into`/`fits_into` argument remains "source". Applied to Vector, Map, Queue,
Set overviews and all five collection fallback diagnostics. Do not regress to
"into or fits_into to materialize".
