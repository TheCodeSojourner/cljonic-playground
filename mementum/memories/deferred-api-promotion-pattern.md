---
type: Insight
symbol: 💡
title: deferred-api-promotion-pattern
related: [deferred-api-promotion-checklist.md, behavior-change-propagation-chain.md, docs-user-facing-example-style.md]
---
The 2026-10-30 `seq` activation exercised the full deferred-to-binding promotion path and exposed two durable lessons.

1. Absence can be load-bearing in test concepts. The MapEntry spec test's `HasSequenceConversion` concept relied on `seq` being undeclared so a requires-expression failed vacuously; activating `seq` (even with only a diagnostic fallback overload) silently flipped the concept true and broke the build. When promoting any deferred API, audit test concepts for admission checks that depend on the function being undeclared, and rewrite them to test the non-void result form (`requires !std::same_as<decltype(fn(v)), void>`).
2. Producer results cannot derive capacity from runtime `count()`. Bounded result capacity is a compile-time constant, so a producer-materializing operation must use the observation cap (`CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE`) as its result capacity, contributing exactly `count()` elements when bounded and the full cap when unbounded. Do not promise producer results sized to runtime counts.

Human-approved semantics live in REQ-SEQ-010; the promotion sequence is requirements → vocabulary → architecture → spec → tests/probes → implementation → integration.
