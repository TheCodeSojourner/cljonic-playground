---
type: Reference
title: cljonic Build Target Strategy
description: Workflow for choosing granular Make checks during development and the comprehensive validation gate.
status: active
category: workflow
tags: [make, build, verification, documentation]
related: [mementum/knowledge/ai-upsert-quality-loop.md, mementum/state.md]
depends-on: []
---

**Decision (2026-10-27):** AI convergence loops use only `make upsert-gate-fast` as their aggregate gate. Do not run `make upsert-gate-strict` in those loops or as an automatic session-end escalation. The human runs `make git` for final comprehensive validation before human-guided commits.

**Why:** The strict gate is too slow for convergence and duplicates the human's final `make git`. Full validation also regenerates documentation.

**Available Granular Targets (no doc generation):**
- `make test` — Build + test cycle
- `make lint` — Format + clang-tidy checks
- `make no-heap-src` — Source-level heap-allocation checks
- `make no-heap-symbols` — Binary-level allocator symbol scan
- `make complexity-cli` — Complexity checks (quiet mode)
- `make coverage-cli` — Coverage check with percentage output
- `make sanitizer-cli` — ASan/UBSan run (quiet mode)
- `make traceability-spec-to-code` — Spec-to-code obligation validation

**Interactive-only targets (never from an agent/script context):** the plain (non-`-cli`) variants render artifacts or open a browser. `make coverage` runs `genhtml` and launches `$BROWSER` on `build-coverage/coverage-report/` — use `make coverage-cli` instead. The aggregate gates (`git`, `validate`, `upsert-gate`) already call the `-cli` variants.

**Coherent-Slice Gate (without docs):**
- `make upsert-gate-fast` — lint, complexity, active strict traceability, and no-heap verification

**Strict Gate Reference (not an AI convergence/checkpoint step):**
- `make upsert-gate-strict` — expensive validation reserved for explicit requests outside convergence loops

**Comprehensive Gate (with docs):**
- `make git` — format, lint, complexity-cli, sanitizer-cli, coverage-cli, traceability-spec-to-code, no-heap, **docs**

**Workflow:**
1. During development/iteration → use focused targets above
2. After a coherent implementation or policy slice → run `make upsert-gate-fast`
3. Before final milestone validation → do not automatically escalate to strict/full gates
4. Before human-guided commits → the human runs `make git`
5. To explicitly regenerate docs → `make docs`

**Session lesson (2026-08-11):** Ran `make git` twice unnecessarily during BuildProfile/vocabulary cleanup. Should have used intermediate targets and final `make git` only.