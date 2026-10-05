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

Correct collection-top phrasing (mirrors Vector):
"A non-cljonic C++ range or view source is copied into the X; a cljonic
collection or producer is not a source -- X{X{...}} encloses it as a single
element. To materialize one, use into; fits_into is the preflight predicate for
whether the whole source fits."

Applied 2026-10-05 repo-wide: src/cljonic-{vector,map,queue,set}.hpp top +
internal `(REQ-FN-027A)` comments, vocabulary.md, architecture.md,
requirements-index/module-2/module-4, and the mementum notes. Do not regress to
"into or fits_into to materialize".
