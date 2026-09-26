---
type: Mistake
symbol: ❌
title: helper-return-semantics-inversion
related: [equal-family-classification.md, concepts-grow-from-tested-boundaries.md]
---
During the `equal` slice (2026-09-26), a helper named `advance_if_equal` returned `(it != end) || (it != end)` ("walk continues"), but callers read the bool as "pair was equal". When the final pair matched and both sides exhausted, the helper returned false and the walk wrongly reported unequal — every cross-type sequential comparison failed. Rule: extract only stateless predicates with truth-naming semantics (e.g. `both_exhausted(...)`); never return a "loop should continue" control flag from a helper whose name states a condition. Extract final-exhaustion predicates instead of loop-control booleans; this also keeps CCN ≤ 4 under the complexity gate.
