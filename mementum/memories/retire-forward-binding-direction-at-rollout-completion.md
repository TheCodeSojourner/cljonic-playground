---
type: Decision
symbol: 🎯
title: retire-forward-binding-direction-at-rollout-completion
related:
  - /mementum/memories/development-order-spec-first.md
  - /mementum/memories/promoting-family-member-requires-narrowing-deferred-reqs.md
  - /mementum/memories/deferred-api-promotion-checklist.md
---
Requirements written as *forward-binding direction* for a planned rollout
decay into stale constraints once the rollout completes — decide retire vs.
record at completion time, don't let them drift.

The factory family shipped exactly this way: `REQ-FN-029B`/`029C` were
approved 2026-10-22 as non-binding naming/contract direction for the then-
deferred factories. As each factory was promoted (`vector`, `hash_map`,
`map_entry`, `hash_set`, `queue`, `string`), the deferred set shrank, and the
direction rules stayed behind claiming scope over a future that no longer
existed. Each convergence pass had to keep honoring (or repairing, e.g. the
`S4_collection_factory_direction` `¬∃ deferred_factory_name` fix) rules with
zero remaining instances.

Retired 2026-10-23 after the human confirmed no future factories are planned:
removed 029B/029C and the S4 rule; the requirements index keeps a dated
breadcrumb and git preserves the full text.

Guidance:
- When the last member of a deferred family becomes active, treat the family's
  forward-binding direction clauses as retirement candidates, not permanent
  law.
- The six-convergence-pass cost is the signal: a rule with no instances that
  every pass must still reconcile is dead weight.
- If a future factory ever appears, elicit/distill its contract fresh at that
  moment — the per-factory sessions proved each contract is genuinely decided
  case-by-case (typed empty for `queue`, capacity-only for `string`), not
  derived from the direction.
- Deletion is safe here: git history carries the reasoning; keep the audit
  trail as a dated note in the index rather than a live requirement.
