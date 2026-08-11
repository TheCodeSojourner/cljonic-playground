## Session State

- last_session_id: 2026-08-11-vocab-session
- current_timestamp: 2026-08-11T23:59:59Z
- recover: 0

## Task

**Session focus: Vocabulary curation and drift detection**
- Added 9 collection type vocabulary entries (Range, Repeat, Cycle, Iterate, Repeatedly, StoredCollection, GeneratedCollection) following established vocabulary style.
- Updated cross-references in Vector, Map, Set, LazySequence to link to StoredCollection/GeneratedCollection taxonomy.
- Initiated gybis-vocab-weed workflow; identified vocabulary drift in architecture.md and specs/*.allium (lambda notation PascalCase vs snake_case; collection_maximum_element_count vs CollectionMaximumElementCount).
- Determined pros/cons of format-doc-samples.pl: should remain separate script (modularity, testability, maintainability outweigh inlining).

## Questions

- Should lambda notation in architecture.md (S3, S4, S5 constraints) use PascalCase canonical term names (e.g., `CopyOnModifyCollection`) or remain snake_case as predicates (e.g., `copy_on_modify_collection`)?
- Should `profile_gates` in architecture.md (lines 27, 31) be promoted to a formal vocabulary term (BuildProfile or ProfileGates)?
- Should specs/*.allium files use `CollectionMaximumElementCount` (canonical) or `collection_maximum_element_count` (allium style)?
- How to resolve terminology: does gybis-vocab-weed require fixing all divergences immediately, or should some remain pending for next session?

## Decisions

**From prior session (still active):**
- All cljonic collections are finite; CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT serves as system-wide infinity for unbound forms.
- Complete collection type set: Vector, Range, Repeat, Cycle, Iterate, Repeatedly, Set, String, Map.
- Range: integer types only; Clojure-compatible 4-form overloads; step=0 produces MAX copies of start.
- Cycle and Iterate are collection types (referentially transparent element access).
- Repeatedly is a collection type; purity of the supplied function is a caller precondition.
- *By rule: every free function using element equality or ordering internally gets a *By variant; predicate-taking functions do not.
- Generated collections do not support mutation; transforms applied to generated collections return Vector.
- String<N> always allocates N+1 code units and maintains a null terminator at all times.

**New (this session):**
- format-doc-samples.pl should remain a separate script; inlining trades off modularity/testability for minor convenience.
- Vocabulary entries for all 9 collection types + 2 taxonomy terms are durable cross-layer language layer.

## Next

- Complete gybis-vocab-weed resolution: fix architecture.md and specs/*.allium divergences (pending style decision on lambda notation PascalCase vs snake_case).
- Answer Open Questions in DESIGN-NOTES.md before implementing new collections (Collection concept, _M suffix, mixed-collection return types, Into semantics, float in stored collections, count for unbound collections, String transform return types).
- Implement Range as the first new collection after Open Questions are resolved.
- Apply consistent patterns (specs, tests, docs, gates) across all new collection implementations.
- Keep strict gate runs (make git) as the first checkpoint after each source upsert.



