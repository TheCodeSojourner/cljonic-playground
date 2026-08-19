---
title: Vocabulary Curation
status: active
category: vocabulary
tags: [vocabulary, curation, architecture, specs]
related: [vocabulary.md, README.md]
depends-on: []
---

Use `vocabulary.md` only for durable cross-layer language. Add a term when it is normative, reused across architecture/specs/tests/code/docs, or ambiguous enough that future artifacts need one canonical name. Keep canonical terms in PascalCase so they can propagate verbatim; preserve README or design-note phrasing only as deprecated synonyms when that older wording is still worth recognizing.

Reject incidental items even if they appear in source material. Tool names, library names, one-off examples, and current implementation knobs do not belong unless they shape durable behavior. The practical test is whether the term constrains future architecture or specifications.

Recent application: `DefaultElement` belongs because it names a user-visible semantic concept distinct from `SentinelBasedAccess`. `deep copy on write` does not replace `CopyOnModifyCollection`; it survives only as a deprecated synonym. `CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT` stays out of the canonical vocabulary because it is an implementation/configuration detail, not durable language.