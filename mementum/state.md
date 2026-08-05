## Session State

- last_session_id: 2026-08-05T11:32:07-06:00
- current_timestamp: 2026-08-05T11:34:04-06:00
- recover: Distill a starting architecture from DESIGN-NOTES.md using vocabulary.md as the canonical term set.

## Task

- Tend and validate the initial canonical vocabulary for the cljonic C++26 header-only library using README.md as an additional source.

## Questions

- Whether any further durable vocabulary additions are needed before architecture distillation.
- Whether future low-level raw code-unit APIs should exist alongside the canonical character-indexed string APIs.

## Decisions

- Canonical vocabulary terms use PascalCase and should propagate verbatim into architecture, specs, tests, code, and docs.
- Vocabulary includes durable cross-layer concepts and excludes incidental examples or one-off implementation details.
- Concrete cljonic collection nouns are explicit: Vector, Map, Set, and String.
- Keyword representation is modeled as `cljonic::Keyword` with ergonomic use-site aliasing such as `kw::name`; keywords are global-only in the domain sense, not Clojure-style namespaced identifiers.
- Text configuration is modeled as `StringEncoding` with only `AsciiEncoding` and `Utf8Encoding`.
- String uses the character-capacity worst-case model: `String<N>` means `N` logical characters, with UTF-8 reserving worst-case code-unit storage.
- In UTF-8 mode, canonical string semantics operate on UnicodeScalarValue units.
- String `count` is logical character count; canonical `get` and `assoc` are character-indexed; slicing is character-range-based and preserves UTF-8 boundaries.
- Canonical UTF-8 strings must be valid UTF-8; compile-time-known invalid UTF-8 is a compile-time error, and runtime invalid UTF-8 is rejected from canonical string semantics.
- Bounded numeric semantics are locked through ClosedNumericDomain, NumericPromotionPolicy, CommonTypeLattice, StaticallyBoundedResult, and DeterministicOverflowPolicy, without bigint support.
- `DefaultElement` is a canonical vocabulary term because it names a user-visible semantic concept distinct from the broader `SentinelBasedAccess` model.
- README phrasing such as `deep copy on write` is preserved only as deprecated synonym material under `CopyOnModifyCollection`, not as a new canonical term.
- `CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT` and similar README-specific configuration knobs are incidental implementation details and stay out of `vocabulary.md`.
- `vocabulary.md` currently passes structural, completeness, and relationship validation with zero issues.

## Next

- Orientation startup gate completed for this session at 2026-08-05T11:34:04-06:00.
- Distill a starting architecture from DESIGN-NOTES.md constrained by vocabulary.md.
- Use the architecture pass to organize the durable concerns: embedded constraints, bounded numeric policy, text encoding model, regex profiles, namespace roadmap, and verification gates.
- Decide later whether to introduce explicit raw code-unit string APIs or keep only canonical character-indexed APIs in the first specification pass.

