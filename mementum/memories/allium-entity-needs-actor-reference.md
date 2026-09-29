---
type: Mistake
symbol: ❌
title: allium-entity-needs-actor-reference
related: [spec-to-code-strict-fail-gate.md, artifact-boundary-discipline.md]
---
`allium check specs` emits `allium.entity.unused` ("Entity 'X' is declared but not referenced elsewhere in this specification.") as a **warning** for any entity that nothing else in its own spec file references. The traceability gate runs `allium check specs`, and the repo requires 0 warnings, so a bare entity fails `make git`.

Found 2026-09-29 adding `specs/sequences/not-equal.allium`: `entity NotEqualFunction { ... }` alone warned. Fix (as every other spec does) is a trailing actor referencing the entity:

```
actor NotEqualFunctionOperator { identified_by: NotEqualFunction }
```

Actors do NOT create traceability obligations — the snapshot regex extracts only `entity-fields.*` and `invariant.*.*` — so adding one is safe. `specs/sequences/equal.allium` ends with `actor EqualFunctionOperator { identified_by: EqualFunction }` for exactly this reason. Rule: every new spec entity needs a matching `actor { identified_by: Entity }`.
