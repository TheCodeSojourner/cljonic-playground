---
type: Reference
title: AI Upsert Quality Loop
status: active
category: workflow
tags: [quality, workflow, upsert, lint, complexity, sanitizer, coverage]
related: [Makefile, mementum/state.md]
depends-on: []
---

The canonical AI upsert workflow for this repo is:
- AI convergence loop: only `make upsert-gate-fast` as the aggregate gate.
- Final comprehensive validation: the human runs `make git` before human-guided commits.
- Do not automatically run `make upsert-gate-strict` or `make git` during convergence or at routine session-end checkpoints (human decision, 2026-10-27).

Strict gate reference (not an instruction to run it):
- `upsert-gate-strict` runs `upsert-gate` first, then `no-heap`.
- `upsert-gate-strict` also enforces strict spec-to-code traceability via `traceability-spec-to-code`.
- Documentation example compilation against generated `cljonic.hpp` is enforced in `make git` through `make docs-examples`, not in `make test`.
- `no-heap` runs the fast source scan (`no-heap-src`) and builds the dedicated no-heap harness target (`cljonic_no_heap_probe`).
- No-heap harness files live under `tests/no_heap/` and are organized by feature-family probes (for example vector, then future set/map/range/cycle and closure-core probes).
- The no-heap harness target is excluded from default test runs (`make all` and `make test`) and only runs through strict policy targets.

Loop contract:
- Use `make upsert-gate-fast` for convergence-loop aggregate validation.
- Do not substitute `make upsert-gate` or strict/full gates for the fast gate.
- When coverage is explicitly requested outside the loop, keep `UPSERT_COVERAGE_FILE` aligned with the header under active development so feedback stays narrow and deterministic.
- Keep `COMPLEXITY_PATH` at `src` for mandatory loop enforcement; narrow it only for local diagnosis when investigating a specific file.

Quality gate intent:
- `lint` auto-runs formatting first, then checks project-path diagnostics only (`src/**`, `tests/**`). Analyzer `note:` output is suppressed to keep loop output high-signal.
- `complexity-cli` enforces source-level complexity boundaries via lizard (`CYCLOMATIC_COMPLEXITY_THRESHOLD` and `FUNCTION_LENGTH_THRESHOLD`) to keep AI-authored functions easy to read and upsert safely.
- `sanitizer-cli` is ASan+UBSan only. TSan was removed because the repo currently exposes no meaningful threading surface, so TSan was low-signal and added environment/tooling noise.
- `coverage-cli` emits `coverage:lines=<pct>` on stdout and fails below threshold. `COVERAGE_FILE` narrows measurement to a single production header.

Operational guidance:
- Prefer `make upsert-gate-fast` over manually recreating an aggregate gate.
- Leave final `make git` execution to the human; do not duplicate its expensive validation with `make upsert-gate-strict`.
- Use `make docs-examples` directly when iterating on Doxygen sample code or public include behavior without paying full `make git` cost.
- If a gate fails, repair that failure before rerunning later gates.
- Full-set or dependency-aware spec checks still belong at checkpoint moments outside the tight inner loop.
