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
- Inner loop: `make upsert-gate`
- Default full validation (no docs): `make upsert-gate-strict`
- Full validation with docs: `make git`

Strict checkpoint workflow:
- Use `make upsert-gate-strict` as the default session-end verification step after iterative upserts.
- `upsert-gate-strict` runs `upsert-gate` first, then `no-heap`.
- `upsert-gate-strict` also enforces strict spec-to-code traceability via `traceability-spec-to-code`.
- `no-heap` runs the fast source scan (`no-heap-src`) and builds the dedicated no-heap harness target (`cljonic_no_heap_probe`).
- No-heap harness files live under `tests/no_heap/` and are organized by feature-family probes (for example vector, then future set/map/range/cycle and closure-core probes).
- The no-heap harness target is excluded from default test runs (`make all` and `make test`) and only runs through strict policy targets.

Loop contract:
- Run it after each source or test change.
- It fails fast in this order: `lint` -> `complexity-cli` -> `sanitizer-cli` -> `coverage-cli COVERAGE_FILE=$(UPSERT_COVERAGE_FILE)`.
- Keep `UPSERT_COVERAGE_FILE` aligned with the header under active development so coverage feedback stays narrow and deterministic.
- Keep `COMPLEXITY_PATH` at `src` for mandatory loop enforcement; narrow it only for local diagnosis when investigating a specific file.

Quality gate intent:
- `lint` auto-runs formatting first, then checks project-path diagnostics only (`src/**`, `tests/**`). Analyzer `note:` output is suppressed to keep loop output high-signal.
- `complexity-cli` enforces source-level complexity boundaries via lizard (`CYCLOMATIC_COMPLEXITY_THRESHOLD` and `FUNCTION_LENGTH_THRESHOLD`) to keep AI-authored functions easy to read and upsert safely.
- `sanitizer-cli` is ASan+UBSan only. TSan was removed because the repo currently exposes no meaningful threading surface, so TSan was low-signal and added environment/tooling noise.
- `coverage-cli` emits `coverage:lines=<pct>` on stdout and fails below threshold. `COVERAGE_FILE` narrows measurement to a single production header.

Operational guidance:
- Prefer `make upsert-gate` over manually chaining `lint`, `complexity-cli`, `sanitizer-cli`, and `coverage-cli`.
- Prefer `make upsert-gate-strict` over `make git` for routine full checks when docs regeneration is not required.
- Escalate to `make git` only when the session explicitly requires docs generation as part of the final gate.
- If a gate fails, repair that failure before rerunning later gates.
- Full-set or dependency-aware spec checks still belong at checkpoint moments outside the tight inner loop.
