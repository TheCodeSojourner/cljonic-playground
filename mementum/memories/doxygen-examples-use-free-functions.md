---
type: Decision
symbol: 🎯
title: doxygen-examples-use-free-functions
---
Doxygen `\b Examples` / `~~~~~{.cpp}` code blocks in `src/*.hpp` MUST demonstrate the free-function API surface (`count(x)`, `contains(x, k)`, `conj(x, v)`, `into(dest, src)`), never direct member-call syntax (`x.count()`, `x.contains(k)`). The library's idiomatic usage is free functions; member calls are implementation detail, not user-facing style. This applies even when a member call would compile — rewrite it as the equivalent free-function call instead. When adding a new type (e.g. a producer), verify every free function used in its examples is actually constrained to accept that type's concept (e.g. `count()` was `SequenceableCollection`-only and didn't support `SequenceableProducer` until this was caught while fixing a Range doc example).

Refinement: a collection/producer TYPE's own doc example (e.g. `Vector`, `Range`) must NOT call free functions at all — it demonstrates only construction syntax (CTAD, capacity/argument forms, per-type quirks like a producer's default/zero-step behavior), matching `Vector`'s existing example. Each free function's OWN doc file (`count.hpp`, `contains.hpp`, etc.) is where that function's usage is demonstrated, across every applicable collection and producer type. Don't duplicate free-function usage in a type's example; the split is: types show how to construct, functions show how to use.
