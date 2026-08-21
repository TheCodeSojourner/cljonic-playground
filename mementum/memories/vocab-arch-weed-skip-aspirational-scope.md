---
type: Insight
symbol: 💡
title: vocab-arch-weed-skip-aspirational-scope
---
`vocabulary.md` and `architecture.md` intentionally describe scope beyond the current implementation (e.g. Map/Set/Queue/String, Repeat/Cycle/Iterate/Repeatedly, view/NonOwningView, Regex, ThreadingForm are documented before any code exists for them). When running `/gybis-vocab-weed` or `/gybis-arch-weed`, a full "unused term" or "principle missing in code" sweep will surface dozens of these as false-positive divergences. Ask the user whether to scope the sweep to already-implemented areas before enumerating; this was confirmed as the preferred default in this repo.
