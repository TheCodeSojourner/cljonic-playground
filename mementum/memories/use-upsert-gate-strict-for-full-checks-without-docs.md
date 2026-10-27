---
type: Decision
symbol: 🎯
title: use-upsert-gate-fast-in-convergence-loops
related:
  - /mementum/knowledge/ai-upsert-quality-loop.md
  - /mementum/knowledge/make-target-strategy.md
---
Human decision, 2026-10-27: whenever the AI is in a convergence loop, only
run `make upsert-gate-fast` as the aggregate validation gate. Do not run
`make upsert-gate-strict` in those loops.

The strict gate is too slow and duplicates the human's final execution of
`make git` before human-guided commits. Leave that comprehensive validation
to the human; do not automatically escalate to strict or full gates.

This supersedes the earlier preference for strict validation during iterative
development and at routine session-end checkpoints. Keep the existing file
path so older references continue to resolve.