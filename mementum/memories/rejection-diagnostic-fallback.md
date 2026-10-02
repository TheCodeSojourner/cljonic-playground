---
type: Decision
symbol: 🎯
title: rejection-diagnostic-fallback
related: [if-consteval-trap-for-compile-time-only-rejection.md, requires-expression-constrained-template-sfinae-limitation.md, governance-invariants-do-not-belong-in-behavioral-specs.md]
---
Informative compile-time errors outrank callability-based detection for closed-domain public free functions (`equal`, `not_equal`). Use a diagnostic overload constrained on the negation of the admission gate (`!EqualPairAdmissible<...>`), one per arity, ending in `static_assert(concepts_detail::dependent_false<...>, "...")`. It names the operation, the rejected operand types, and the violated domain rule, replacing a compiler cascade of rejected concept candidates with one message. Domain-support detection must use the named admission concepts / `*_admissible_v`, never `requires { call(...) }`: once a fallback exists, a rejected argument may satisfy callability, and that is not a supported interface.

Gotchas:
- GCC `-Wtemplate-body` errors on "no return statement in constexpr function returning non-void"; add an unreachable trailing `return false;` after the `static_assert`.
- `dependent_false` (a template-dependent false variable) is required so the assertion fires only at instantiation.
- Recorded at REQ-DIAG-009, architecture `S3_rejection_diagnostic`, and `specs/capabilities/diagnostics.allium`; message/mechanism are governance, not behavioral-spec invariants.
