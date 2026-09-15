---
type: Insight
symbol: ⚠️
title: development-order-spec-first
---
The repo's required progression is strict: requirements → vocabulary → architecture → specs → tests → implementation.

When architecture changes are the last approved step, we must not jump ahead into source or tests before propagating the change into the spec layer. This was a corrective miss: the last step was architecture work, and source/test edits were made before the spec propagation step.

Rule: before any source or test edits, update the relevant behavioral specification(s) and traceability first, then tests, then implementation. Preserve the ordered lifecycle and visible approval chain.

This is a project-level guardrail for future sessions and must be treated as a hard-order constraint, not a suggestion.
