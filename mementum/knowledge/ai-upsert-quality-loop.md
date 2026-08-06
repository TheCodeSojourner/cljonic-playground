---
title: AI Upsert Quality Loop
status: active
category: workflow
tags: [quality, workflow, upsert, lint, sanitizer, coverage]
related: [Makefile, mementum/state.md]
depends-on: []
---

The canonical AI upsert workflow for this repo is `make upsert-gate`.

Loop contract:
- Run it after each source or test change.
- It fails fast in this order: `lint` -> `sanitizer-cli` -> `coverage-cli COVERAGE_FILE=$(UPSERT_COVERAGE_FILE)`.
- Keep `UPSERT_COVERAGE_FILE` aligned with the header under active development so coverage feedback stays narrow and deterministic.

Quality gate intent:
- `lint` auto-runs formatting first, then checks project-path diagnostics only (`src/**`, `tests/**`). Analyzer `note:` output is suppressed to keep loop output high-signal.
- `sanitizer-cli` is ASan+UBSan only. TSan was removed because the repo currently exposes no meaningful threading surface, so TSan was low-signal and added environment/tooling noise.
- `coverage-cli` reports bare line coverage percentage and fails below threshold. `COVERAGE_FILE` narrows measurement to a single production header.

Operational guidance:
- Prefer `make upsert-gate` over manually chaining `lint`, `sanitizer-cli`, and `coverage-cli`.
- If a gate fails, repair that failure before rerunning later gates.
- Full-set or dependency-aware spec checks still belong at checkpoint moments outside the tight inner loop.
