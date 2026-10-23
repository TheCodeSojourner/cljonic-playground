---
type: Decision
symbol: 🎯
title: typed-empty-factory-form-follows-collection-principles
related:
  - /mementum/memories/deferred-api-promotion-checklist.md
  - /mementum/memories/factory-consistency-for-user-createable-values.md
  - /mementum/memories/promoting-family-member-requires-narrowing-deferred-reqs.md
---
When promoting a deferred collection factory, the empty-call form is a per-type
contract decision, not a family-wide default. Derive it from the collection's
own constructor principles, then get explicit human approval because plausible
collections diverge.

- `vector<T>()` → `Vector<T, 0>`; `queue<T>()` → `Queue<T, 0>` (2026-10-23):
  both admit a zero-capacity typed empty even though the result cannot accept
  `conj`, because the deferred direction (REQ-FN-029C) names "typed empty
  construction" as a vector principle to follow.
- `hash_set()` requires explicit element type AND capacity: Set admission is
  stricter and the hash_set contract (REQ-FN-029F) chose explicit-capacity
  empties.

Guidance: state the typed-empty semantics explicitly in the proposed contract
and surface the divergence from the nearest precedent as the one open question
in the approval ask — it is the most likely substantive decision in a factory
promotion. Write the decision into the requirement clause (e.g. "following the
`vector` typed empty construction principle") so spec, diagnostics, and the
compile-fail gate can trace it.
