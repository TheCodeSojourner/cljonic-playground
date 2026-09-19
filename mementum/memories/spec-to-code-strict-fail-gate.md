---
type: Insight
symbol: 💡
title: spec-to-code-strict-fail-gate
---
Strict spec-to-code traceability is now part of `make upsert-gate-strict` via `make traceability-spec-to-code`. The gate requires set-scoped Allium validation (`allium check specs` + `allium analyse specs`), a synchronized committed snapshot at `spec-to-code-traceability/spec-to-code-obligation-ids.snapshot.txt`, and test trace coverage for every obligation in active, implementation-backed specifications. Specifications marked `-- lifecycle: deferred` preserve their behavioral contracts and may have contract tests, but their obligations are excluded from the strict active-surface gate until implementation begins. Traced test blocks must include at least one `CHECK*` or `REQUIRE*` assertion. Update the snapshot with `make traceability-spec-to-code-update-snapshot` when intentional active-spec obligation changes occur.