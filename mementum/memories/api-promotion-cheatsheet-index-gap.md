---
type: Mistake
symbol: ❌
title: api-promotion-cheatsheet-index-gap
related:
  - /mementum/memories/deferred-api-promotion-checklist.md
  - /mementum/knowledge/header-addition-and-verification-lifecycle.md
---
The promotion checklist omits the Core_Cheatsheet index in
`src/cljonic-core.hpp`. Promoting `first` on 2026-10-19 left it absent from the
rendered `Sequence traversal` list until the user noticed. The index is
hand-maintained, not generated from the include graph, so a promotion must add
the operation to its alphabetical bullet and then regenerate `cljonic.hpp` and
`docs/`. Check both the source bullet and the rendered `docs/index.html`.
