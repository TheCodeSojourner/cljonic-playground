🎯 Use `make upsert-gate-strict` as the default full validation gate during iterative development when docs generation is not required.

Rationale:
- Covers lint, complexity, sanitizer, coverage, strict spec-to-code traceability, and no-heap verification.
- Excludes Doxygen/docs generation, making it faster and less noisy than `make git` for routine loops.

Escalation rule:
- Use `make git` when a session explicitly needs docs generation included in the final gate.