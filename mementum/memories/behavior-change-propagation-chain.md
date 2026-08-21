---
type: Insight
symbol: 💡
title: behavior-change-propagation-chain
---
When an implementation behavior change is spec-worthy (e.g. a new compile-time-vs-runtime distinction), propagate it through the full chain, not just the spec: `specs/*.allium` invariant → `cljonic-requirements.md` REQ (scope the old REQ, add a new REQ for the new case) → `architecture.md` VSM rule (mirror in both the owning layer and any layer that restates it, e.g. S3 and S1) → `vocabulary.md` term/example update. Regenerate the traceability snapshot (`make traceability-spec-to-code-update-snapshot`) after adding new spec invariants, and add a matching `TRACE_ID` in tests. If a spec obligation has no negative-compilation test harness, it's acceptable to trace it without an automated positive test (precedent: `FloatingPointBoundsAndStepsAreRejectedAtCompileTime`).
