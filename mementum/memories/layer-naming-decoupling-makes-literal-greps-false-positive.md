---
type: Insight
symbol: 💡
title: layer-naming-decoupling-makes-literal-greps-false-positive
related: [convergence-sweep-corrections.md, vocab-arch-weed-skip-aspirational-scope.md, two-level-concept-model.md]
---
Divergence tools (vocab-weed, arch-weed, req-weed) extract terms by
name and compare across layers. Those comparisons produce many false
positives because each layer names things in its own vocabulary:

- Architecture uses its own rule names (`S2_*`, `S3_*`, `coherence_*`),
  not spec entity names.
- Implementation uses C++ identifiers: `cljonic_collection`,
  `std::totally_ordered`, `dependent_false`.
- Specs/code carry Clojure references and free-function spellings
  (`can_conj`, `can_assoc`) that look non-canonical to a naive matcher.
- Descriptive English ("header-only", "deferred", "sequence traversal")
  matches deprecated synonyms.

Rule: judge divergence **semantically**, never by literal grep. A term
that differs in spelling but denotes the same concept is **not** drift;
mechanically "fixing" it would corrupt code/spec text. Only genuine term
drift is actionable (e.g. a deprecated synonym used as a capability
name, like `IFn` for `CallableLookup`).

Observed 2026-10-06: a ~33-hit "non-canonical term" scan reduced to 1
genuine fix; the 24 `unused_canonical_term` hits were all `keep`.
