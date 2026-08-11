## Session State

- last_session_id: 2026-08-11-vocab-cleanup-session
- current_timestamp: 2026-08-11T23:59:59Z
- recover: 0

## Task

**Session focus: Vocabulary cleanup and cross-layer consistency verification**

**Prior session results (2026-08-11-vocab-session):**
- Added 9 collection type vocabulary entries (Range, Repeat, Cycle, Iterate, Repeatedly, StoredCollection, GeneratedCollection) following established vocabulary style.
- Updated cross-references in Vector, Map, Set, LazySequence to link to StoredCollection/GeneratedCollection taxonomy.
- Identified vocabulary drift in architecture.md and specs/*.allium (lambda notation PascalCase vs snake_case; collection_maximum_element_count vs CollectionMaximumElementCount).

**This session (continuation):**
- Resolved redundancy analysis: BuildProfile and profile_gates are redundant with gybis layer ordering (arch > spec > tests > code) + violation detection.
- Removed BuildProfile vocabulary entry entirely (12+ cross-references cleaned).
- Removed profile_gates lambdas from architecture.md (S4_adaptation, S4_disruption, redundant with gybis governance).
- Verified three-layer consistency: gybis-arch-weed (architecture ↔ specs), gybis-spec-weed (specs ↔ code), gybis-vocab-check (vocabulary semantics).
- Fixed vocabulary semantic issues: removed 4 violations (3 self-references, 1 undefined reference).
- Committed all changes with passing quality gates (make git).
- Learned make target strategy: use granular targets (test, lint, sanitizer-cli, etc.) for iteration; reserve `make git` for final validation only.

## Questions

- ✅ **RESOLVED:** Should `profile_gates` be promoted to vocabulary term? **No.** Gybis layer ordering already enforces capability gating and prevents drift. Redundant with existing governance.
- ~~Should lambda notation in architecture.md use PascalCase canonical term names?~~ **RESOLVED:** Yes, architecture.md now uses canonical PascalCase (commit d754650).
- ~~Should specs/*.allium use CollectionMaximumElementCount?~~ **RESOLVED:** Yes, specs now use canonical PascalCase (commit d754650).

## Decisions

**From prior sessions (still active):**
- All cljonic collections are finite; CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT serves as system-wide infinity for unbound forms.
- Complete collection type set: Vector, Range, Repeat, Cycle, Iterate, Repeatedly, Set, String, Map.
- Range: integer types only; Clojure-compatible 4-form overloads; step=0 produces MAX copies of start.
- Cycle and Iterate are collection types (referentially transparent element access).
- Repeatedly is a collection type; purity of the supplied function is a caller precondition.
- *By rule: every free function using element equality or ordering internally gets a *By variant; predicate-taking functions do not.
- Generated collections do not support mutation; transforms applied to generated collections return Vector.
- String<N> always allocates N+1 code units and maintains a null terminator at all times.
- format-doc-samples.pl should remain a separate script; inlining trades off modularity/testability for minor convenience.
- Vocabulary entries for all 9 collection types + 2 taxonomy terms are durable cross-layer language layer.

**New (this session):**
- BuildProfile and profile_gates are redundant with gybis governance → remove both (committed 3b8469a, 5845fe3).
- Vocabulary self-references and undefined references violate semantic model → fix immediately (committed 5845fe3).
- Make target strategy: use granular targets during iteration; final `make git` only (documented in mementum/make-target-strategy.md).

## Next

- Answer Open Questions in DESIGN-NOTES.md before implementing new collections (Collection concept, _M suffix, mixed-collection return types, Into semantics, float in stored collections, count for unbound collections, String transform return types).
- Implement Range as the first new collection after Open Questions are resolved.
- Apply consistent patterns (specs, tests, docs, gates) across all new collection implementations.
- Use granular make targets (test, lint, no-heap-src, coverage-cli) during development; final `make git` only before committing.



