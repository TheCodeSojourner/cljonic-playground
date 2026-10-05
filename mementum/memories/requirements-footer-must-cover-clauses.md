---
type: Insight
symbol: 💡
title: requirements-footer-must-cover-clauses
related: [/mementum/knowledge/architecture-integrity-curation.md]
---
Each `requirements/requirements-module-N.md` ends with a `**Governed REQs**`
footer that MUST cover every `λ REQ-...` clause present in that module
(derivable, not hand-drifted). Fix 2026-10-05: module 2's footer said
`REQ-CAP-001`–`010` / `REQ-DIAG-001`–`008` while clauses actually ran to
`REQ-CAP-012` / `REQ-DIAG-010` (CAP-011/012 = `cljonic::Variant` domain and
capabilities; DIAG-009/010 = free-function and source-constructor rejection
diagnostics). Repaired to `CAP-001`–`012`, `DIAG-001`–`010`; all modules then
showed `uncovered=[]`.

No committed checker enforces footer coverage — validate by expanding each
footer's ranges per domain and diffing against the module's clauses. Cross-module
and within-module non-monotone designator numbering plus number gaps (e.g.
`COLL-003`, `PLAT-011`) are intentional topical allocation: report as info, never
renumber.
