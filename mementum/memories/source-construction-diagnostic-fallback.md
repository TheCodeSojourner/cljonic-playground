---
type: Insight
symbol: 💡
title: source-construction-diagnostic-fallback
related: [/mementum/knowledge/collection-source-interoperability.md, /mementum/memories/ctad-copy-deduction-vs-enclosure-guides.md, /mementum/memories/rejection-diagnostic-fallback.md]
---
REQ-DIAG-010 extends the rejection-diagnostic policy from public free functions
(REQ-DIAG-009, `equal`/`not_equal`) to public **constructors** whose source
domain is closed.

Why: the narrowed source constructor
(`requires(!is_cljonic_collection_v && !is_cljonic_producer_v && …)`) is a
silent gate. When a cljonic collection/producer argument is excluded, overload
resolution falls through to the variadic pack constructor, whose generic
`static_assert` says "arguments convertible to T … could not convert
'Range<int>' to 'int'". That misleads the caller, who never intended an element
conversion.

Mechanism (verified on GCC 16.2 + clang 22.1, real headers):
- Add a single-argument constructor constrained on the rejected case
  (`(is_cljonic_collection_v || is_cljonic_producer_v) && !same_as<…, value_type>`),
  body `static_assert(dependent_false<SourceValue>, "…is not a constructor
  source. Use into … (preflighted by fits_into), or write X{…} to enclose it as
  one element.")`.
- It is more specialized than the variadic pack constructor, so it wins exactly
  the rejected case and never affects accepted paths (enclosure, element pack,
  scalars, span interop all still compile).
- Its implicit CTAD guide is non-viable (return params non-deducible), so it
  does not perturb the enclosure guides.
- Name the parameter `[[maybe_unused]]` (repo idiom) — unnamed trips
  readability-named-parameter; named-but-unused trips -Wunused-parameter.

Applied to Vector/Set/Queue/Map/String. Harness asserts the message anchor
"you cannot construct a" (whitespace-normalized), so wording drift fails
the gate. The anchor was reworded on 2026-10-09 from "is not a construction
source" under the user-facing terminology decision — see
user-facing-diagnostic-messages.md.
