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
- Prefer Catch2 GENERATE-based parametrization for boundary/state matrices when behavior is shared across many input combinations.

Catch2 guidance:
- Use tuple-based GENERATE patterns to replace repetitive SECTION blocks.
- Expand coverage by adding parameter tuples, not by duplicating control-flow structure.
- Keep type-incompatible scenarios in separate branches or tests when tuple unification is not possible.

This page governs how verification evidence is produced so AI loops consume high-signal diagnostics with minimal ambiguity.
