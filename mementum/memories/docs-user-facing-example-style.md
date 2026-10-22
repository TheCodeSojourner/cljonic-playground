---
type: Insight
symbol: 💡
title: docs-user-facing-example-style
---
User-facing cljonic doc examples should be contract-first and avoid implementation-detail terminology.

- Keep prose concise and non-speculative.
- Prefer named examples with `const auto name = Type{...};` for readability.
- Prefer the lowercase `vector(...)` factory for idiomatic Vector creation, including when an explicit element type or capacity is needed. For other cljonic collection types, prefer CTAD where deduction expresses the intended type and capacity; use explicit template arguments when deduction is unavailable or when demonstrating a specific type or capacity.
- Examples are illustrative, not exhaustive: a few representative cases that convey the basics are enough — do not enumerate every type, overload, or arity. Include a particularly unfamiliar case (e.g. a `cljonic::Variant` operand) when it aids understanding.
- In construction-only samples, avoid surfacing methods that are intended to be documented via standalone functions.
- For intentionally unused sample variables, prefer `[[maybe_unused]]` over `(void)var;`.
- Keep example include usage consistent with library guidance (use `cljonic.hpp`, the generated single-header public entry point, in every user-facing example).
- Put `using namespace cljonic;` at file scope, immediately after `#include "cljonic.hpp"` and outside `main()` — never inside `main()`. Enforced by `scripts/compile-doc-samples.py` (`make docs-examples`); applied repo-wide 2026-10-06.
- Accept formatter-native wrapping unless formatting policy is explicitly changed.