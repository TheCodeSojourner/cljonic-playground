---
type: Pattern
symbol: 🔁
title: convergence-sweep-corrections
related: [vocab-arch-weed-skip-aspirational-scope.md, behavior-change-propagation-chain.md, use-mementum-not-copilot-memory.md]
---
The six-command convergence sweep (vocab-check, arch-check, spec-check, vocab-weed, arch-weed, spec-weed) is a divergence *detector*, and its corrections follow a fixed shape: vocab-weed surfaces a vocabulary term with no downstream anchor → propagate the term into the owning spec file and the governing REQ text (not code identifiers); arch-weed surfaces a new free function with no architecture anchor → add a `λ S2_{function}_contract` rule mirroring the spec invariants and tracing to the governing REQ IDs. Then re-run the sweep to zero before committing. Both correction shapes occurred 2026-09-26 for `equal` (`SequentialEquality` anchored into equal.allium + REQ-FN-002G; `λ S2_general_equality_function` added) and committed as one convergence commit. Run sweeps *after* the implementation commits so corrections are a separate reviewable change.