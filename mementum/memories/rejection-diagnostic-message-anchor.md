---
type: Insight
symbol: 💡
title: rejection-diagnostic-message-anchor
related: [rejection-diagnostic-fallback.md, if-consteval-trap-for-compile-time-only-rejection.md]
---
The REQ-DIAG-009 compile-fail harnesses (`scripts/check-equal-compile-failures.py`,
`check-not-equal-compile-failures.py`) assert the diagnostic message CONTENT,
not just a phrase: operation identity (`cljonic::equal:` / `cljonic::not_equal:`),
the anchor `"outside the supported equality domain"`, the rejection taxonomy
(Floating-point values, callables, pointers, unscoped enums, standard-library
range and container types, the standard-library variant), and per-case rule
substrings (e.g. `sequential [Vector, Queue, and all producers]`,
`With three or more operands`, `mutually comparable cljonic family pair`).
`not_equal` also pins its `shares with cljonic::equal` nod. Matching is
whitespace-normalized so compiler line-wrapping cannot cause false failures.

Consequence:
- Drift in the listed content now FAILS the gate — editing a message requires
  updating the harness constants. (2026-10-03: strengthened from anchor-only.)
- Residual unasserted surface: per-arity nuance beyond the pinned substrings
  (e.g. exact wording of `mixed cljonic/non-cljonic pairs`), which is free.

When refining a diagnostic, keep the anchor phrase intact, state the rule
positively (admissible forms) plus the rejection taxonomy, and make each arity's
message self-contained rather than cross-referencing another arity's message.
