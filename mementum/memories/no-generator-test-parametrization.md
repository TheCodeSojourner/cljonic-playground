---
type: Shift
symbol: 🔄
title: no-generator-test-parametrization
related: [/mementum/knowledge/verification-signal-discipline.md, /mementum/memories/coverage-measures-during-ai-upsert-loop.md]
---
Decision (human, 2026-10-03): do NOT adopt Catch2 generator-based test
parametrization — `GENERATE(values/table/range/random)`, tuple `GENERATE`,
`TEMPLATE_TEST_CASE`, `TEMPLATE_LIST_TEST_CASE`. This REVERSES the earlier
"Prefer Catch2 GENERATE-based parametrization" guidance that had been recorded
in `mementum/knowledge/verification-signal-discipline.md` (updated in place).

Rationale:
- No coverage gain: coverage is already lines=100.0%; generated values re-run
  the enclosing section over identical paths, so the traceability/coverage gate
  sees no new evidence.
- Recurring cost: every generated instantiation multiplies sanitiser and
  coverage wall-time, and this suite is compiled several times (normal,
  single-header, sanitizers, coverage).
- Cannot serve compile-time obligations: generated values are runtime `auto`
  and can never feed `STATIC_REQUIRE`. constexpr/noexcept obligations must stay
  explicit.
- Determinism: `random()` injects an unstated distribution assumption into
  normative behavior tests.

Write tests as explicit per-case `TEST_CASE`/`SECTION` blocks plus
`STATIC_REQUIRE` for compile-time obligations. Reconsider only if separately
approved, and only for homogeneous, non-normative cases.
