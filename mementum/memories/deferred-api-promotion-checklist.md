---
type: Decision
symbol: 🔁
title: deferred-api-promotion-checklist
related:
  - /mementum/memories/doc-sample-extraction-requires-unindented-fence.md
  - /mementum/memories/named-parameter-lint-vs-unused-parameter.md
  - /mementum/memories/api-promotion-cheatsheet-index-gap.md
  - /mementum/memories/promoting-family-member-requires-narrowing-deferred-reqs.md
  - /mementum/knowledge/doxygen-doc-tooling.md
  - /mementum/knowledge/header-addition-and-verification-lifecycle.md
---
Promoting a deferred header to the public API touches nine places; two fail only
at gates. Verified by promoting `empty` on 2026-10-17.

1. `src/cljonic-core.hpp`: add the umbrella include (the amalgamation follows
   that graph, so this also publishes it in `cljonic.hpp`).
2. Header: drop the `\note Not yet part of the public API` line.
3. Header: add the `dependent_false` rejection overload; name parameters and mark
   them `[[maybe_unused]]` in prefix position or `make lint` fails
   (`readability-named-parameter`).
4. `scripts/compile-doc-samples.py`: remove the name from
   `DEFERRED_NON_PUBLIC_HEADERS`, or the example is still skipped.
5. `scripts/check-free-function-diagnostics.py`: reclassify `excluded` → `fallback`.
6. `scripts/check-primitive-compile-failures.py`: add diagnostic and pass cases.
7. Spec: new active `.allium` (no `-- lifecycle: deferred`), plus `TRACE_ID` tests.
8. `tests/no_heap/`: probe file, `probes.hpp` declaration, `harness_main.cpp` call.
9. Requirements/architecture/vocabulary: move the operation out of the deferred
   set, add the architecture contract rule, restate the term as active; refresh
   the obligation snapshot and regenerate `cljonic.hpp` and `docs/`.

Remaining deferred headers: `first`, `next`, `seq`, `not_empty`.
