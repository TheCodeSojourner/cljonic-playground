---
type: Insight
symbol: 💡
title: same-type-constructor-pack-preference
related: [producer-parameter-equality.md, no-heap-probes-propagate-with-feature.md]
---
Clojure nesting parity (`Vector<Range<int>,N>{r}` = vector of one range) is achieved by constraining the range/view constructor with `!std::same_as<remove_cvref_t<SourceRange>, ElementType>` instead of adding API. Overload-set disambiguation via a requires-exclusion keeps the surface minimal: a single argument of the element type is pack construction; materialization of a source stays canonical in `into`/`fits_into` (invariant CompleteMaterializationUsesInto). Approved by human 2026-09-26; specified as `SingleElementTypeArgumentYieldsPackConstruction` in specs/collections/{source-construction,vector,queue}.allium. A span of producers is NOT the element type, so it still source-constructs — both intents coexist without ambiguity. String is exempt (element type `char` is never a range).