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

**Decision:** Minimize doc generation by using granular make targets for intermediate checks, `make upsert-gate-strict` after a coherent implementation or policy slice, and `make git` only for final comprehensive validation.

**Why:** `make git` invokes Doxygen (expensive operation) as its final step. Running it multiple times during a session unnecessarily regenerates HTML docs.

**Available Granular Targets (no doc generation):**
- `make test` — Build + test cycle
- `make lint` — Format + clang-tidy checks
- `make no-heap-src` — Source-level heap-allocation checks
- `make no-heap-symbols` — Binary-level allocator symbol scan
- `make complexity-cli` — Complexity checks (quiet mode)
- `make coverage-cli` — Coverage check with percentage output
- `make sanitizer-cli` — ASan/UBSan run (quiet mode)
- `make traceability-spec-to-code` — Spec-to-code obligation validation

**Coherent-Slice Gate (without docs):**
- `make upsert-gate-fast` — lint, complexity, active strict traceability, and no-heap verification

**Pre-Final Gate (without docs):**
- `make upsert-gate-strict` — fast gate plus range compile-fail, sanitizers, and focused coverage

**Comprehensive Gate (with docs):**
- `make git` — format, lint, complexity-cli, sanitizer-cli, coverage-cli, traceability-spec-to-code, no-heap, **docs**

**Workflow:**
1. During development/iteration → use focused targets above
2. After a coherent implementation or policy slice → run `make upsert-gate-fast`
3. Before final milestone validation → run `make upsert-gate-strict`
4. At milestone, handoff, or release validation → run `make git` once
5. To explicitly regenerate docs → `make docs`

**Session lesson (2026-08-11):** Ran `make git` twice unnecessarily during BuildProfile/vocabulary cleanup. Should have used intermediate targets and final `make git` only.