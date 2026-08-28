---
type: Reference
title: Vocabulary Curation
status: active
category: vocabulary
tags: [vocabulary, curation, architecture, specs, concepts]
related: [vocabulary.md, README.md, mementum/knowledge/cpp-concept-layer.md]
depends-on: []
---

Use `vocabulary.md` only for durable cross-layer language. Add a term when it is normative, reused across architecture/specs/tests/code/docs, or ambiguous enough that future artifacts need one canonical name. Keep canonical terms in PascalCase so they can propagate verbatim; preserve README or design-note phrasing only as deprecated synonyms when that older wording is still worth recognizing.

Reject incidental items even if they appear in source material. Tool names, external library names, one-off examples, and internal implementation knobs do not belong unless they shape durable behavior. External tools/standards (e.g. `CMake`, `CoreGuidelines`, `NL`) remain excluded.

**Developer-as-User Rule:** Because cljonic users are C++ developers, C++ concept identifiers used across the public API (e.g. `CljonicCollection`, `SequenceableCollection`, `StableEqualityComparable`) ARE first-class vocabulary terms — not hidden under Examples. Free-function names (snake_case, Clojure parity) are the only case-folded spelling variant of a canonical PascalCase vocabulary term.

Recent application: `DefaultElement` belongs because it names a user-visible semantic concept distinct from `SentinelBasedAccess`. `deep copy on write` does not replace `CopyOnModifyCollection`; it survives only as a deprecated synonym. `CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT` stays out of the canonical vocabulary because it is an implementation/configuration detail, not durable language. Concept identifiers (`Cljonic*`, `SequenceableCollection`, `StableEqualityComparable`, etc.) are included as canonical vocabulary terms under the Developer-as-User rule.