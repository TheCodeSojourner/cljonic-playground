---
type: Insight
symbol: 💡
title: docs-user-facing-example-style
related:
  - /mementum/memories/factory-consistency-for-user-createable-values.md
  - /mementum/memories/doxygen-prose-must-be-user-facing.md
---
User-facing cljonic doc examples should be contract-first and avoid implementation-detail terminology.

- Keep prose concise and non-speculative.
- Prefer named examples with `const auto name = Type{...};` for readability.
- Create every user-createable value through its lowercase factory: `vector`, `hash_map`, `map_entry`, `hash_set`, `queue`. Direct type construction remains only where the factory cannot express the call: range/view source import (factories construct from individual elements and are not range materializers) or a named-type contract demonstration. A CTAD/deduction-guide demonstration is NOT a valid reason for direct construction (narrowed 2026-10-23 during the `queue` promotion): inference is a factory property — the first argument's unqualified type and the argument count — so write `queue(1, 2, 3)` with a comment about inference, not `Queue{1, 2, 3}`. This supersedes the earlier "prefer CTAD for non-Vector types" rule.
- Omit integer-literal suffixes (`3U`, `2L`) in example code; plain literals suffice. Keep a suffix only when it is semantically load-bearing, e.g. `Variant<int, long>{1L}` selecting the `long` alternative.
- Do not qualify a factory as "lowercase" in prose — the identifier already shows its spelling.
- Examples are illustrative, not exhaustive: a few representative cases that convey the basics are enough — do not enumerate every type, overload, or arity. Include a particularly unfamiliar case (e.g. a `cljonic::Variant` operand) when it aids understanding.
- In construction-only samples, avoid surfacing methods that are intended to be documented via standalone functions.
- For intentionally unused sample variables, prefer `[[maybe_unused]]` over `(void)var;`.
- Keep example include usage consistent with library guidance (use `cljonic.hpp`, the generated single-header public entry point, in every user-facing example).
- Put `using namespace cljonic;` at file scope, immediately after `#include "cljonic.hpp"` and outside `main()` — never inside `main()`. Enforced by `scripts/compile-doc-samples.py` (`make docs-examples`); applied repo-wide 2026-10-06.
- Accept formatter-native wrapping unless formatting policy is explicitly changed.
