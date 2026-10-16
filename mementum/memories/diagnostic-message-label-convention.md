---
type: Decision
symbol: 🎯
title: diagnostic-message-label-convention
related: [/mementum/knowledge/rejection-diagnostics.md, rejection-diagnostic-message-anchor.md, user-facing-diagnostic-messages.md]
---
Every targeted compile-time diagnostic message in `src/` opens with
`cljonic::<entity>: ` — the free-function name or the class-template name.
Human-decided 2026-10-16; codified as `REQ-DIAG-011`, architecture
`S3_diagnostic_message_label`, vocabulary `DiagnosticMessageLabel`, gate
`make diagnostic-message-labels`.

The trigger: the class-internal collection assertions (capacity, initializer
count, constructor, range/view, static extent) used a bare `Vector `/`Map `
prefix with no delimiter, while the REQ-DIAG-009/010 fallbacks used
`cljonic::Vector:`. One class emitted two conventions; all 24 were relabelled.
No test or script pinned them, but the Doxygen source listings under `docs/`
contain them and refresh at `make git`.

The gate parses the trailing message argument structurally (paren-matched
statement, last top-level comma, first string literal), so doc-example asserts
and literal-bearing conditions are ignored. Unlabelled messages are fatal, a
parked backlog is non-fatal, and relabelling a parked entry is reported
**stale**, so the backlog cannot outlive the fix. Wired into `upsert-gate`,
`upsert-gate-fast`, `validate`, and `git`.

Label and language are separate axes: these messages still name internal
template parameters (`ElementType`, `T`); that pass is open.
