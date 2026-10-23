---
type: Insight
symbol: 💡
title: compile-fail-gate-pass-cases-must-respect-configured-maximum
related:
  - /mementum/memories/deferred-api-promotion-checklist.md
  - /mementum/memories/arch-quality-gates-need-thresholds.md
---
A compile-fail gate compiles its cases under a deliberately tiny configuration
(typically `#define CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT 2`) so bound
violations are cheap to express. Every *pass* case in that gate must also fit
that maximum, or the gate reports a false regression.

Observed while adding `string-factory-compile-fail` (2026-10-23): the pass cases
used `string<8>("Hello")` and `string<16>(view)`. Under a maximum of 2 the
factory correctly rejected them with a capacity diagnostic, and the gate
reported "expected to compile, but failed" — pointing at the implementation
rather than at the test data.

Rules:
- Keep pass-case capacities at or below the gate's configured maximum; assert
  the small value you actually compiled.
- Keep failure-case capacities *within* the maximum too when the case is meant
  to exercise a non-capacity rule — otherwise the capacity message wins and the
  expected diagnostic never appears (e.g. a cljonic-source rejection expressed
  as `string<4>(vector)` under maximum 2).
- When a pass case exercises a boundary, add a separate case for the real
  limit rather than borrowing the default configuration's headroom.

Check: temporarily print the configured maximum in the gate, or grep the case
file for literals above it.
