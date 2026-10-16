---
type: Mistake
symbol: ❌
title: arch-check-heuristics-vs-project-conventions
related: [architecture-lambda-notation.md, /mementum/knowledge/architecture-integrity-curation.md]
---
`gybis-arch-check`'s findings are heuristics, not defects. Two of its checks
misfire here, and reporting them as warnings overstates the result.

Nearest-`## S<n>` heading assigns each rule to a layer. `architecture.md` groups
the character-validity rules together inside the S3 span while naming each by
its conceptual layer, so `S2_character_is_valid` and
`S2_character_is_valid_c_string` look misplaced to the checker. They are
deliberately grouped, and the prefix is layer-correct: the S2 section is where
public free-function contracts live. The name is human-approved, and `state.md`
cites it verbatim.

Per-rule `traces_to(...)` coverage. The repo's convention is "policy and its
enforcement live in one rule per concern" (`architecture-lambda-notation.md`),
enforced by the 2026-09-26 convergence sweep. A concern names its policy once —
`S3_quality` names `S5_change_policy` — and the sub-gates inherit it. Adding a
trace per sub-rule would restate it and invite the drift that sweep removed.

So read the conventions and the introducing commit before assigning severity.
`architecture.md` has no gate, so nothing catches over-reporting.
