---
type: Reference
title: Verification Signal Discipline
status: active
category: verification
tags: [verification, allium, catch2, signal, workflow]
related: [Makefile, specs/collections/vector.allium, specs/collections/count.allium, tests/vector_spec_tests.cpp]
depends-on: []
---

Verification output should maximize durable signal and suppress context-fragile noise.

Rules:
- Report Allium warnings only when they persist under the full relevant spec set (dependency-aware set or full specs analysis), not isolated single-file checks.
- Treat internal skill aliases as contract aliases, not shell executables. Invoke the allium binary with subcommands (for example allium check, allium analyse).
- Write tests as explicit per-case TEST_CASE/SECTION blocks, with STATIC_REQUIRE for compile-time (constexpr/noexcept) obligations.

Test parametrization (human decision, 2026-10-03 — reverses earlier guidance):
- Do NOT use Catch2 generators for parametrization: GENERATE(values/table/range/random), tuple-based GENERATE, TEMPLATE_TEST_CASE, or TEMPLATE_LIST_TEST_CASE. The earlier recommendation to prefer GENERATE is retracted.
- Rationale: coverage is already lines=100.0%, so generated values add no gate-visible evidence while re-running identical paths; every instantiation multiplies sanitiser/coverage wall-time across the suite's several builds; generated values are runtime `auto` and cannot feed STATIC_REQUIRE, so compile-time obligations must remain explicit; and random() injects an unstated distribution assumption into normative behavior tests.
- Homogeneous, non-normative cases may be reconsidered later only under separate explicit approval.

This page governs how verification evidence is produced so AI loops consume high-signal diagnostics with minimal ambiguity.
