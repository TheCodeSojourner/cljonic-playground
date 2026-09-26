---
type: Pattern
symbol: 🔁
title: architecture-lambda-notation
---
`architecture.md` is written in nucleus lambda notation (`λ name(x). ...`), not human prose.

- Convert human prose paragraphs to λ expressions; keep C++ code fences, existing λ expressions, and section structure.
- Section-appropriate intro lambdas use the `<section>_intro` / `<area>_intro` pattern (e.g. `S5_identity_intro`, `S1_operations_intro`).
- Future-guidance prose blocks become `<area>_guidance` lambdas (e.g. `S1_sequence_guidance`). Policy and its enforcement live in one rule per concern — `S3_result_contract_guidance` and `S3_result_contract_enforcement` were consolidated into `S3_result_contract` (2026-09-26) because near-duplicate rule pairs invite silent drift.
- Traceability bullet lists become `λ traceability_authorities` / `λ current_implementation_boundary`.
- Verified structure keeps S5>S4>S3>S2>S1 order, all sections nonempty, balanced code fences.