## Session State

- last_session_id: c1ef7071-fad8-4316-ae67-95a4f24685e7
- current_timestamp: 2026-10-03
- recover: 1
- session_complete: true

Task:
1. gybis-init orientation — COMPLETE: oriented on state.md, recent memories, and the value-equality-domain / cljonic-next-agenda / artifact-boundary-discipline knowledge pages.
2. REQ-DIAG-009 diagnostic message refinement — COMPLETE (uncommitted): the binary fallback now names the sequential family structurally as `sequential [Vector, Queue, and all producers]` (mirrors `equal_family_of_v`'s two admission paths; drift-resistant, unlike enumerating all seven producer types); the variadic fallback is now self-contained, stating the positive rule and rejection taxonomy inline instead of deferring to "the same domain rules as the two-operand form". Text-only: no behavior, req/vocab/arch/spec change.
3. Memory stored — COMPLETE: `mementum/memories/rejection-diagnostic-message-anchor.md` (harnesses assert only the anchor phrase; wording is refinable but has no drift protection).
4. `not_equal` diagnostic parity — COMPLETE (uncommitted): applied Option A to all three `not_equal` fallback messages in `src/cljonic-not-equal.hpp` so each is self-contained (states the shared-with-`equal` domain, the positive rule, and the rejection taxonomy) instead of cross-referencing `cljonic::equal`; reused the structural naming `sequential [Vector, Queue, and all producers]`. Added `DIAGNOSTIC_CASES` + `diagnostic_message_reported` to `scripts/check-not-equal-compile-failures.py`, asserting the same `"outside the supported equality domain"` anchor as the equal harness (unary, binary, variadic cases).
5. Doxygen bullet reflow fix — COMPLETE (uncommitted): `scripts/format-doc-samples.pl`'s `format_doxygen_block` treated every `* ` line as paragraph text, so consecutive one-line `* - bullet` items were joined into a single bullet during reflow (reproduced minimally; also observed as `equal`'s first two bullets fusing). Added a list-marker branch that flushes the current paragraph and starts a new one on `^[ \t]*\*\s?-\s`, preserving list structure; verified bullet-preserving and idempotent, and re-applied the fused `equal` bullet split (now formatter-stable).
6. Memory stored — COMPLETE: `mementum/memories/doxygen-reflow-must-respect-list-markers.md` (the doc-sample formatter is the only prose-reflowing tool; list markers are paragraph boundaries).
7. Diagnostic content assertion — COMPLETE (uncommitted): the REQ-DIAG-009 harnesses previously asserted only the anchor phrase `"outside the supported equality domain"`, leaving the rest of the message without drift protection. Both `scripts/check-equal-compile-failures.py` and `scripts/check-not-equal-compile-failures.py` now assert content with whitespace-normalized matching: operation identity (`cljonic::equal:` / `cljonic::not_equal:`), the anchor, the rejection taxonomy (six categories), per-case rule substrings (`sequential [Vector, Queue, and all producers]`, `With three or more operands`, `mutually comparable cljonic family pair`), and (for not_equal) the `shares with cljonic::equal` nod. Drift detection verified with a negative simulation. `rejection-diagnostic-message-anchor.md` memory updated: content drift now FAILS the gate; only per-arity nuance beyond pinned substrings remains unasserted.

Questions:
1. None blocking. Slice B (`cljonic::Variant` free-function API) awaits selection.

Decisions:
1. Diagnostic wording (human, 2026-10-03): keep the family-level abstraction rather than a bare type dump; name the sequential members structurally as `Vector, Queue, and all producers` (human preferred "and all producers" over "and the producer families"); ASCII brackets, no em dash. The variadic message must be self-contained (state the rule and taxonomy, not merely cross-reference another arity).
2. Message text is REQ-DIAG-009 governance, not a behavioral-spec invariant; changes are approved per-write and are not gated by traceability.
3. `not_equal` parity (human, 2026-10-03): Option A — mirror `equal`'s self-contained wording (REQ-DIAG-009/004 name both functions and require the message to state the violated rule), keep a short nod to the shared domain for the `\ref Equal` relationship, and add matching `DIAGNOSTIC_CASES` to the not-equal harness. REJECTED for now: Option B (shared message macro) and Option C (hybrid delegating the taxonomy).
4. Doxygen reflow (2026-10-03): the doc-sample formatter must treat a list marker (`* - `) as a paragraph boundary; bullet integrity is a formatter responsibility in this repo, not a hand-authoring invariant. `clang-format` does not reflow comment prose, so this lives in `format-doc-samples.pl`.
5. Diagnostic assertions (2026-10-03): REQ-DIAG-009 requires the fallback to name the operation and the violated domain rule; the harnesses assert that content with whitespace-normalized matching so compiler line-wrapping cannot cause false failures. Drift in asserted content is now a gate failure, not a silent free text.

Validation:
1. `make upsert-gate-strict` EXIT=0: lint:ok, complexity:ok, range/variant/equal/not-equal compile-fail:ok (equal includes the diagnostic-message assertions), header-guards:ok, sanitizer:ok, coverage:lines=100.0%, traceability-spec-to-code:ok, no-heap-src/symbols:ok.
2. `make cljonic` regenerated the amalgamated header; `clang-format --dry-run --Werror src/cljonic-equal.hpp` clean.
3. docs/ not regenerated (deferred to final git per defer-doc-regeneration-until-final-git).
4. `not_equal` parity: `make not-equal-compile-fail:ok` (with the new diagnostic-message assertions) and `make upsert-gate-strict` EXIT=0.
5. Doxygen reflow fix: `scripts/format-doc-samples.pl` bullet-preservation verified by minimal repro (bullets preserved, second run no-op); repo-wide `make format` clean; `equal-compile-fail:ok`, `not-equal-compile-fail:ok`, `docs-examples:compiled=29` (+6 deferred).
6. Diagnostic content assertion: both harnesses `:ok` with the strengthened checks; drift-detection proven by a negative simulation (a mutated taxonomy token fails the assertion).

Next:
1. Slice B — `cljonic::Variant` free-function API (index, holds, get, get_if, emplace, swap, visit, variant_size, variant_alternative); remove `lifecycle: deferred` from `specs/capabilities/variant-api.allium` when implemented.
2. Extend the REQ-DIAG-009 rejection-diagnostic policy to other closed-domain public free functions when they gain a fallback.
3. Deferred comparison family (`equal_by`/`identical`, `less`, `less_equal`, `greater`, `greater_equal`) and `REQ-SEQ-022` operation-level reconciliation remain candidates.
4. Diagnostic message changes are uncommitted; `/gybis-fini` owns the commit (Mementum-only by default). Human declined fini for this session; tree left for review.
5. Candidate synthesis (proposed 2026-10-03, awaiting human approval): the doc-tooling memory family now exceeds the ≥3 threshold — a `mementum/knowledge/doxygen-doc-tooling.md` page from doxygen-reflow-must-respect-list-markers, doc-sample-extraction-requires-unindented-fence, doxygen-legacy-blocks-require-explicit-change, docs-examples-must-be-runnable-programs, and defer-doc-regeneration-until-final-git.

Carry-forward (unaddressed, remember for later):
1. `equal_by`/`identical` and the remaining REQ-FN-002C comparison family remain deferred.
2. User-defined aggregates with float members are unanalyzable (no reflection) — REQ-NUM-007 recursion cannot be enforced for them.
3. Set duplicate-insertion in a constexpr context hits std::abort().
4. REQ-SEQ-022 operation-level specification reconciliation (mementum/knowledge/cljonic-next-agenda.md).

## Previous Session State

- last_session_id: 171c64d2-e025-4eb9-9ee1-732dd7840fa5
- current_timestamp: 2026-10-02
- recover: 1
- session_complete: true

Task:
1. Nothrow/throwing equality contract — COMPLETE: added `concepts::NothrowEqualityComparable` (StableEqualityComparable ∧ `concepts_detail::nothrow_equality_v`) and excluded pointers/unscoped enums from `equal`'s fallthrough domain (`is_arithmetic_v || is_scoped_enum_v`); added recursive `contains_standard_range` rejection. Applied across REQ-FN-002G, vocabulary, architecture, specs, tests, and code.
2. cljonic::Variant composite type (Slice A) — COMPLETE: added `src/cljonic-variant.hpp` (storage admission `NothrowVariantAlternative`, conditional alternative-strict equality/ordering via `ComparableVariantAlternative`/`TotallyOrdered`, no valueless state); retired `std::variant` from the cljonic value domain (rejected as `equal` operand and as Map key / Set element; permitted only as the internal backend); mirrored the walkers via `cljonic_variant_traits`; removed all `std::variant` walker specializations.
3. Equality diagnostics policy — COMPLETE: `REQ-DIAG-009` rejection-diagnostic fallback (per-arity, negated admission gate, instantiation-dependent `static_assert` naming operation, operand types, and violated rule) across req/vocab/arch/spec/tests/code.
4. Mementum refresh + gybis-fini closeout — COMPLETE: refreshed `value-equality-domain.md` and three stale memories; state upsert and Mementum-only commit.

Questions:
1. None blocking. Slice B (`cljonic::Variant` free-function API) awaits selection.

Decisions:
1. Add `cljonic::Variant`; retire `std::variant` from the cljonic value domain. Storage admission = `NothrowVariantAlternative` (`NothrowCollectionElement`); equality admission = `ComparableVariantAlternative` (`NothrowStableEqualityComparable && NothrowEqualityComparable`); ordering requires `TotallyOrdered`.
2. Reject throwing equality at the concept layer (`NothrowEqualityComparable`) rather than dropping `noexcept`; exclude pointers and unscoped enums from the fallthrough domain; recursively reject nested standard ranges in composites.
3. Informative diagnostics outrank callability detection (REQ-DIAG-009): per-arity diagnostic fallback on the negated gate with a `dependent_false` `static_assert`. Domain detection uses the admission concepts, not `requires { call(...) }`; consequently a domain-rejected argument may satisfy callability, and that callability is not a supported interface.
4. Split `cljonic::Variant` into slice A (type + storage/equality admission + walkers + std::variant retirement) and slice B (free-function API), parking `VariantFreeFunctionApi` in `specs/capabilities/variant-api.allium` under `lifecycle: deferred`.
5. Keep `concepts_detail` equality concepts visible in generated docs.
6. No auto-commit during API slices; `/gybis-fini` retains its default commit step (Mementum-only).

Validation:
1. `make upsert-gate-fast:ok` and `make upsert-gate-strict` EXIT=0: format/lint/complexity, range/variant/equal/not-equal compile-fail (now with targeted diagnostic-message assertions), header-guards, sanitizer, coverage lines=100.0%, traceability (snapshot synced), no-heap-src/symbols/probe.
2. `make git:ok` (user-run, post-commit): docs:ok, docs-examples:compiled=29 (6 deferred skipped), cljonic-test:ok.
3. New tests: `tests/cljonic-variant-spec-tests.cpp` (with runtime coverage case) and `tests/cljonic-diagnostics-spec-tests.cpp`; new `tests/no_heap/cljonic-variant-probes.cpp`.

Next:
1. Slice B — `cljonic::Variant` free-function API (index, holds, get, get_if, emplace, swap, visit, variant_size, variant_alternative); remove `lifecycle: deferred` from `specs/capabilities/variant-api.allium` when implemented.
2. Extend the REQ-DIAG-009 rejection-diagnostic policy to other closed-domain public free functions when they gain a fallback.
3. Deferred comparison family (`equal_by`/`identical`, `less`, `less_equal`, `greater`, `greater_equal`) and `REQ-SEQ-022` operation-level reconciliation remain candidates.

## Older Session State

- last_session_id: fd3522cd-6d0a-4782-ae03-2be453291d76
- current_timestamp: 2026-10-02
- recover: 1
- session_complete: true

Task:
1. Equality constraint refactor — COMPLETE: added five private family concepts and `EqualPairAdmissible` in `src/cljonic-equal.hpp`; binary overloads and shared unary/binary/variadic gates use the named concepts. Updated `src/cljonic-not-equal.hpp` unary/binary gates to use `EqualPairAdmissible`. No runtime equality semantics changed.
2. C++23 review — COMPLETE: confirmed `StableEqualityComparable` admits a potentially-throwing `operator==`, while `equal` is declared `noexcept`; a throwing comparison therefore terminates. Also found that Doxygen's `EXTRACT_ALL=YES` publishes the `concepts_detail` concepts. Scalar-domain breadth (`std::is_scalar_v`, including unscoped enums and pointers) remains to be reconciled against intent.
3. gybis-fini closeout — COMPLETE: state upsert and Mementum-only commit.

Questions:
1. No human disposition received while user unavailable. Before changing behavior, decide whether throwing equality is rejected by contract, remains a caller precondition under `noexcept`, or makes `equal` potentially throwing; inspect recursive composite/variant implications.
2. Decide whether `concepts_detail` equality concepts should remain visible in generated docs or be hidden consistently with internal-detail policy.
3. Clarify whether unscoped enums and pointer scalars belong to `equal`'s non-cljonic fallthrough domain.
4. Proposed memory `noexcept-equality-requires-nothrow-comparator` was not approved; no memory file created.

Decisions:
1. Keep equality-family gates in `concepts_detail`; do not move them to `src/cljonic-concepts.hpp` or introduce ordering semantics.
2. Convert the shared pair admission gate into `EqualPairAdmissible`; preserve the current accepted domain and `noexcept` signatures pending a requirements/spec decision.
3. Leave Doxygen visibility and scalar-domain policy unchanged pending human disposition.
4. Keep the code and generated-doc changes uncommitted; fini commit is limited to Mementum state.

Validation:
1. Final `make git` passed after re-reading the user-touched `src/cljonic-equal.hpp`: format, lint, complexity, all compile-failure harnesses, header guards, sanitizers, 100.0% coverage, traceability, no-heap, docs, 28 compiled doc examples (6 deferred skipped), and full tests.
2. `make equal-compile-fail`, `make not-equal-compile-fail`, `make cljonic-test`, and `make upsert-gate-strict UPSERT_COVERAGE_FILE=cljonic-equal.hpp` passed during the refactor.

Next:
1. Reconcile the `noexcept` versus potentially-throwing equality contract at requirements/spec level before changing the supported domain.
2. Resolve internal-concept documentation visibility and scalar-domain scope.
3. Refresh `mementum/knowledge/value-equality-domain.md`, which still refers to `equal_pair_admissible_v` rather than the new `EqualPairAdmissible` concept.
4. Continue deferred comparison-family or `REQ-SEQ-022` work only when selected.

## Earlier Session State

- last_session_id: 55f47254-5bfa-42c5-be15-e3b9289e0a3a
- current_timestamp: 2026-10-01
- recover: 1
- session_complete: true

Task:
1. Requirements description under the revised gybis-req-describe contract — CANCELED per human direction (2026-10-01): do not restore or carry forward the requested root-level `requirements-description.md`.
2. gybis-fini closeout — COMPLETE: updated this state record and committed Mementum.

Questions:
1. No new blocking questions. The previously proposed artifact-boundary knowledge synthesis remains awaiting explicit human approval.

Decisions:
1. Requirements-description deliverable: canceled per human direction (2026-10-01); no restoration is expected.

Decisions:
1. Requirements-description scope: all seven dependency-ordered modules; output mode: default repository-root Markdown file, explicitly selected by the human.
2. Revised requirements-description contract: connected stakeholder prose only, no requirement designators or per-requirement bullets; supplied rationales use `because:` and analysis-derived rationales are attributed.
3. The fini workflow writes only `mementum/state.md`; record the absent output file as the next recovery item rather than expanding the closeout write boundary.

Validation:
1. Requirements description during authoring: 289 source clauses; 31 source rationales and 31 rendered `because:` statements; four analysis-derived rationales; seven module headings; two deferred sections; no requirement designators or clause bullets; `git diff --check` clean.
2. Closeout inspection: branch and worktree were clean; `requirements-description.md` was not present. No code or test gates were run.

Next:
1. Resume deferred comparison-family work (`equal_by`/`identical`, then the ordering comparisons) when selected.
2. Reconcile operation-level specifications for `REQ-SEQ-022` per `mementum/knowledge/cljonic-next-agenda.md`.
3. Ask for approval before creating the pending artifact-boundary knowledge synthesis.

Task:
1. gybis-init orientation — COMPLETE.
2. Layer-order adoption — COMPLETE (uncommitted): durability order is now `req > vocab > arch > spec > tests > code` (human decision). GYBIS-DEV-WORKFLOW.md upserted: loop line updated, new step 2 "Honor Requirements", steps renumbered 1–9, checklist item added. Human decision: NO S6 VSM layer — requirements are a document lane, not a VSM layer; VSM.md (nucleus) remains the basis of architecture.md with five layers (S5>S4>S3>S2>S1); requirements enter through S3/S5 gating (S3_domain_boundary → explicit_approved_requirement).

Decisions:
1. Layer order (human, 2026-09-29): `req > vocab > arch > spec > tests > code`. The req layer is an authority ordering between documents, not a VSM layer; no S6 added.

Task:
1. `Equal` public doc comment refinement — COMPLETE (commits 75d2b68 + 5606804): removed all Clojure references (brief no longer "modeled on Clojure's `=`"); rewrote the prose to be user-facing and contract-first (dropped REQ-FN-002G/REQ-NUM-006/REQ-FN-014B/REQ-COLL-021 IDs, the `\ref StableEqualityComparable` link, and equality-family/gating jargon); restated the three rules as user-visible behavior (single value always true; ordinary values by same-type `==` with example types; producer comparison bounded by `CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT`; matching element/key/value types; order-insensitive Map/Set); fixed a merged list bullet; reformatted the sample to the bare (non-asterisk) `~~~~~{.cpp}` fence so scripts/compile-doc-samples.py actually extracts and compiles it (docs-examples:compiled 13→14).
2. Unary-arity gate repair — COMPLETE (commit 75d2b68): compiling the sample for the first time exposed that the unary `equal` overload's unnamed parameter (`const T&`) fails clang-tidy `readability-named-parameter` (in WarningsAsErrors), while naming it fails `-Werror=unused-parameter`. Fixed with the prefix form `[[maybe_unused]] const T& value`.
3. Memory stored — COMPLETE (commit 07c45bc): mementum/memories/named-parameter-lint-vs-unused-parameter.md.
4. Project-wide doc-sample verification fix — COMPLETE (2026-09-29): converted all 19 remaining asterisk-prefixed `\b Examples` blocks to the bare-fence style (all 32 headers now extractable); docs-examples compiled 14→27. The conversion surfaced 6 stale public examples for the deferred sequence-shaping free functions (empty, first, next, not-empty, rest, seq) that were deliberately removed from the public API (commit cc376ce) and are absent from the cljonic-core.hpp umbrella; per human decision they are excluded explicitly (DEFERRED_NON_PUBLIC_HEADERS in scripts/compile-doc-samples.py, reported as docs-examples:deferred-skipped=6) with a "Not yet part of the public API" note in each header.
5. `not_equal` slice — COMPLETE (2026-09-29, REQ-FN-002H): added the canonical general-inequality free function as the thin negation of `equal`, top-down. Requirements: REQ-FN-002H. Vocabulary: `NotEqual` term. Architecture: λ `S2_not_equal_function`. Specification: new `specs/sequences/not-equal.allium` (entity `NotEqualFunction`, 11 invariants + actor). Code: new `src/cljonic-not-equal.hpp` (unary/binary/variadic, all delegating to `equal`), umbrella-registered in `cljonic-core.hpp`. Tests: `tests/cljonic-not-equal-spec-tests.cpp` (all 12 TRACE_IDs; STATIC_REQUIRE + volatile runtime coverage) and a dedicated no-heap probe registered in `probes.hpp`/`harness_main.cpp`. Gating: new `scripts/check-not-equal-compile-failures.py` + `make not-equal-compile-fail` wired into `.PHONY`/upsert-gate/validate/git. Snapshot synced.
6. `not_equal` mainpage index gap — COMPLETE (2026-09-29): the Doxygen `\mainpage` cheatsheet in `src/cljonic-core.hpp` did not list `not_equal` (the `user-facing-apis-require-mainpage-index-entry` pattern was missed in item 5). Added `\ref NotEqual "not_equal"` to the N group of Core Functions; regenerated docs; `docs/index.html` now links `not_equal` → `namespacecljonic.html#NotEqual`.
7. Header-guard standardization — COMPLETE (2026-09-29): `src/` was split 23 include-guards vs 14 `#pragma once` with no documented convention or gate. Converted all 23 include-guard headers to `#pragma once` (guards were unreferenced outside their own header; all had a single `#endif`). Added `scripts/check-header-guards.py` + `make header-guards` (wired into `.PHONY`/help/upsert-gate/validate/git), and recorded decision mementum/memories/header-guard-convention.md.

Questions:
1. None blocking.

Decisions:
1. Doc style (human, 2026-09-29): the public Doxygen prose is user-facing and contract-first — no Clojure references, requirement IDs, or capability-model jargon (per doxygen-prose-must-be-user-facing).
2. Sample style (human, 2026-09-29): use the bare (non-asterisk-prefixed) `~~~~~{.cpp}` fence so the example is actually compiled, not silently skipped (per doc-sample-extraction-requires-unindented-fence).
3. Unused-parameter idiom (human, 2026-09-29): named + `[[maybe_unused]]` in prefix position, since both gates cannot otherwise be satisfied at once.
4. Anchor capitalization (human, 2026-09-29): `\anchor Equal` stays PascalCase (the doc-page label); the callable is written `equal` in code style within prose.
5. Deferred doc-example exclusion (human, 2026-09-29): the 6 deferred non-public sequence-shaping headers are excluded from docs-examples via an explicit, reported skip list (mirroring traceability's lifecycle:deferred exemption); do not promote them into the umbrella without arch/spec work.
6. Requirements approach for not_equal (human, 2026-09-29): Option 2 — add REQ-FN-002H spelling out the three-arity negation semantics and the identical compile-time gating, so the requirement layer is self-contained rather than tracing only to REQ-FN-002C.
7. not_equal semantics (human, 2026-09-29): D1 arities mirror `equal`; D2 compile-time gating identical to `equal` (reuse `equal_pair_admissible_v`/`all_adjacent_pairs_admissible_v`); D3 thin negation delegating to `equal` (no separate comparison logic), distinct from `operator!=`.
8. not_equal unary arity (human, 2026-09-29): include `not_equal(x)` returning false for an admitted operand (Clojure `(not= x)` parity), still compile-time gated.
9. not_equal organization (human, 2026-09-29): own header/spec/tests/probe/compile-fail harness as a sibling of `equal` (per free-function-addition-organization and header-addition-and-verification-lifecycle).
10. Header-guard style (human, 2026-09-29): standardize on `#pragma once` for all `src/` and `tests/` headers (option A). Rationale: self-maintaining, removes the missing/mismatched-`#endif` bug class, already used by the flagship public types and all test headers. Core Guidelines SF.8 include-guard portability was considered and not chosen. Enforced by `make header-guards`.
11. Commit workflow (human, 2026-09-29): during free-function/API slices, do not auto-commit — step through requirements → vocabulary → architecture → specification → tests → code one layer at a time, pausing for human verification between layers; commit only on explicit request. `/gybis-fini` retains its default commit step. (Recorded in mementum/memories/gybis-auto-commit-preference.md.)

Validation:
1. `make git` green end-to-end: format, lint, complexity, range/variant/equal-compile-fail, sanitizer, coverage lines=100.0%, traceability-spec-to-code, no-heap-src/symbols/probe, docs, docs-examples:compiled=14, cljonic-test 170/170.
2. Doc-sample fix (2026-09-29): `make git` green — format, lint, complexity, compile-fail harnesses, sanitizer, coverage lines=100.0%, traceability-spec-to-code, no-heap-src/symbols/probe, docs, docs-examples:compiled=27 + deferred-skipped=6, cljonic-test 170/170.
3. not_equal slice (2026-09-29): `make git` green — format, lint, complexity, range/variant/equal/not-equal-compile-fail (39 rejection + 20 pass cases × two builds), sanitizer, coverage lines=100.0%, traceability-spec-to-code (snapshot synced; all 12 NotEqualFunction obligations traced), no-heap-src/symbols/probe, docs, docs-examples:compiled 27→28, cljonic-test ok; `allium check`/`analyse` clean (0 warnings).
4. Header-guard standardization (2026-09-29): `make git` green — format, lint, complexity, all compile-fail harnesses, `header-guards:ok`, sanitizer, coverage lines=100.0%, traceability-spec-to-code, no-heap, docs, docs-examples:compiled=28, cljonic-test ok; generated `cljonic.hpp` has 31 `#pragma once` and 0 include guards.

Next:
1. Candidate next comparison slices: `equal_by`/`identical`, then `less`, `less_equal`, `greater`, `greater_equal` (REQ-FN-002C); `not_equal` is now complete (REQ-FN-002H).
2. REQ-SEQ-022 operation-level specification reconciliation (mementum/knowledge/cljonic-next-agenda.md).
3. Candidate synthesis (proposed 2026-09-29, awaiting human approval): a gate-addition/enforcement knowledge page from header-guard-convention.md, allium-entity-needs-actor-reference.md, doc-sample-extraction-requires-unindented-fence.md, and spec-to-code-strict-fail-gate.md — may overlap verification-signal-discipline.md; request decision before writing.

Carry-forward (unaddressed, remember for later):
1. equal_by/identical and the remaining REQ-FN-002C comparison family (`less`, `less_equal`, `greater`, `greater_equal`) remain deferred; `not_equal` is now implemented (REQ-FN-002H).
2. User-defined aggregates with float members are unanalyzable (no reflection) — REQ-NUM-007 recursion cannot be enforced for them; documented known limitation.
3. Set duplicate-insertion in a constexpr context hits std::abort(); constexpr sets can't hold duplicate elements.
4. REQ-SEQ-022 operation-level specification reconciliation for remaining operation families (mementum/knowledge/cljonic-next-agenda.md).
5. Project-wide doc-sample gap — FIXED 2026-09-29: all 32 headers use the bare fence and are extracted; docs-examples:compiled=28; 6 deferred non-public sequence-shaping headers (empty, first, next, not-empty, rest, seq) explicitly excluded (DEFERRED_NON_PUBLIC_HEADERS, deferred-skipped=6). No longer open.

Task:
1. gybis-init orientation — COMPLETE (commit 4a1fd1e): oriented on state.md, recent memories, value-equality-domain and cljonic-next-agenda knowledge.
2. Convergence sweep from arch-check/vocab-check recommendations (human-approved) — COMPLETE (commit db8885e): consolidated S3_result_contract_guidance + S3_result_contract_enforcement into one S3_result_contract rule; stripped five empty `λ expressions:` filler lines; tended ProducerKind vocabulary to admit Repeatedly and added missing CljonicIterate/CljonicRepeatedly vocabulary terms (vocab-check undefined-reference gate caught the gap). Re-validation clean (0 issues; 156 vocab terms).
3. gybis-vocab-check — COMPLETE: PASS, 0 syntax/completeness/semantic issues across 154→156 terms.
4. Vocab-weed sweep (interactive, human-dispositioned) — COMPLETE, no file changes (commit f11a721 = state record): IFn kept as deliberate Clojure-concept parenthetical (skip); 12 aspirational Module 6/7 terms, 6 predicate-naming terms, 17 foundational umbrella terms dispositioned keep; ~590 spec-local contract-phrase identifiers dispositioned skip. Zero actionable divergences; standing memory vocab-arch-weed-skip-aspirational-scope confirmed as working default.
5. Arch-weed sweep (interactive, human chose spec-side corrections) — COMPLETE (commit fa12697): added producer nominal-concept entities (CljonicProducer + CljonicRange/Repeat/Cycle/Iterate/Repeatedly with kind-identity and external-container-rejection invariants) and ApiLifecycleGate entity to specs/capabilities/concepts.allium with matching actors; 526 fine-grained spec invariants accepted as architecture-principle-level granularity (sample-verified semantic coverage).
6. Spec-weed sweep (interactive, human-dispositioned) — COMPLETE (commit 2119087): the new spec entities triggered traceability-spec-to-code drift (33 uncovered obligations). Producer-concept obligations (26) covered with executable test evidence in tests/cljonic-concepts-spec-tests.cpp (ProducerLike/ExternalProducerLike scaffolding, producer_traits specialization, six TEST_CASEs, all 26 TRACE_IDs); ApiLifecycleGate entity REVERTED per human decision (governance-process invariants are not executable behavior; deferred-labeled functions like seq/first legitimately exist in code). Snapshot updated; traceability-spec-to-code:ok; 170/170 tests pass both builds; no-heap gates ok.
7. Approved memory stored (commit 69e35a0): mementum/memories/governance-invariants-do-not-belong-in-behavioral-specs.md.

Questions:
1. None blocking.

Decisions:
1. Convergence sweep (human, 2026-09-26): all three arch-check recommendations approved and executed as proposed.
2. Vocab-weed dispositions (human, 2026-09-26): implemented-only scope per standing repo default; IFn skip; aspirational, naming-policy, and umbrella term groups keep; spec-local contract identifiers skip.
3. Arch-weed corrections (human, 2026-09-26): spec-side for both divergences (producer concepts and lifecycle gate); D2 spec-granularity accepted — architecture governs at principle level.
4. Spec-weed resolutions (human, 2026-09-26): producer-concept obligations get concepts-spec test evidence; ApiLifecycleGate entity reverted — governance invariants belong in the process gates, not behavioral specs (now encoded as a stored memory).
5. Standing decisions 2026-09-25/26 from the equal arity slice remain in force (alternative-strict variant equality final; cross-type numeric unification retired; equal compile-time gated to the closed value domain; bounded-prefix termination across all arities).

Validation:
1. Final state: allium check/analyse clean on 31 specs; traceability-spec-to-code:ok (849 obligations, full TRACE_ID coverage); 170/170 tests pass in both builds; no-heap-src/symbols/probe ok; VSM coherence S5>S4>S3>S2>S1 intact; zero divergences across vocab/arch/spec/code layers.

Next:
1. Candidate synthesis (proposed 2026-09-26, awaiting human approval): ≥3-memory threshold met for the artifact-boundary family — synthesize mementum/knowledge/artifact-boundary-discipline.md from spec-to-code-strict-fail-gate.md, vocab-arch-weed-skip-aspirational-scope.md, and governance-invariants-do-not-belong-in-behavioral-specs.md on approval.
2. Candidate next slice: `not_equal` (trivially !equal, REQ-FN-002C) or `equal_by`; complete the equality-domain table.
3. REQ-SEQ-022 operation-level specification reconciliation for remaining operation families (mementum/knowledge/cljonic-next-agenda.md).

Carry-forward (unaddressed, remember for later):
1. equal_by/identical and the rest of REQ-FN-002C remain deferred.
2. User-defined aggregates with float members are unanalyzable (no reflection) — REQ-NUM-007 recursion cannot be enforced for them; documented known limitation.
3. Set duplicate-insertion in a constexpr context hits std::abort(); constexpr sets can't hold duplicate elements.
4. REQ-SEQ-022 operation-level specification reconciliation for remaining operation families (mementum/knowledge/cljonic-next-agenda.md).

Task:
1. Variadic `equal` arity slice — COMPLETE and committed (2026-09-26, commit 93231aa): REQ-FN-002G amended to the three Clojure `=` arities (unary returns true for a domain-admitted operand; variadic conjoins adjacent-pair equality left to right with short-circuit; every adjacent pair compile-time gated), S2_general_equality_function arch rule extended, five new EqualFunction invariants in equal.allium, SFINAE-safe equal_pair_admissible_v / all_adjacent_pairs_admissible predicates in src, spec tests + 13 compile-fail + 7 compile-pass arity cases in both builds, arity no-heap probe, Equal vocabulary term anchored, and the unused-<utility> include hygiene fix folded in.
2. Mementum migration — COMPLETE (commit 5e80ab9): migrated helper-return-semantics-inversion and pipefail-masks-build-failures out of the forbidden Copilot repo-memory store; validation-cadence duplicate discarded; store now empty.

Questions:
1. None blocking.

Decisions:
1. Arity semantics (human, 2026-09-26): unary `equal(x)` returns true but the operand must be admitted by the same compile-time domain gating as the binary form; floating-point and standard-library range/container operands remain rejected (REQ-NUM-006 uniform across arities). Variadic form: adjacent-pair conjunction, left-to-right with short-circuit at the first unequal pair; every adjacent pair individually compile-time gated so position never bypasses rejection. Binary overloads unchanged; the variadic form bottoms out in the existing five binary overloads via the equal_pair_admissible_v mirror, kept in lockstep by construction.
2. Termination contract (human, 2026-09-26): bounded-prefix compare; unbounded pairs compare the configured observable traversal cap; equal(Repeat{1}, Repeat{1}) is true; a finite value vs an agreeing unbounded producer is false; every call terminates — now across all arities.
3. Element-type policy (human, 2026-09-26): identical element types for cross-category sequential comparison; same-type sequential equal coincides with REQ-COLL-021 operator==.
4. Cross-family pairs (human, 2026-09-26): compile-time reject; Map↔Map, Set↔Set, String↔String only; family compatibility is a compile-time concept.
5. Nested producer elements (human, 2026-09-26): producer parameter equality (REQ-FN-014B) for nested producer components; REQ-SEQ-016/017 unchanged; sequential prefix equality exists only on the named `equal` function.
5a. Mixed pairs (human, 2026-09-26, approved): equal(Vector{1}, 1) and any cljonic↔non-cljonic pair is compile-time rejected, mirroring AlternativeStrictEquality — including every adjacent pair in the variadic form.
6. Non-cljonic fallthrough domain (human, 2026-09-26, approved option 1): `equal` is NOT part of the C++ interoperability surface; standard-library range/container types are compile-time rejected; fallthrough is the closed value domain only (scalars, scoped enums, aggregate-like structs with stable ==, std::variant composites).
7. Standing decisions 2026-09-25: alternative-strict variant equality FINAL and exclusive; cross-type numeric unification permanently out of scope (Stream D retired). NoHeap probe rule: every behavior change needs a dedicated probe in both builds.

Validation:
1. Arity slice: upsert-gate-strict green (lint, complexity, cljonic.hpp regen, range/variant/equal-compile-fail both builds, sanitizer, coverage lines=100.0%, traceability-spec-to-code with synced snapshot — drift diff was exactly the five new invariants, no-heap-src/symbols/probe ok). allium check/analyse 0 findings on equal.allium and the full 31-file spec set.
2. Convergence corrections: Equal vocabulary term anchored the three arities (definition + examples); traceability snapshot synced.

Next:
1. Candidate next slice: `not_equal` (trivially !equal, REQ-FN-002C) or `equal_by`; complete the equality-domain table.
2. REQ-SEQ-022 operation-level specification reconciliation for remaining operation families (mementum/knowledge/cljonic-next-agenda.md).

Carry-forward (unaddressed, remember for later):
1. equal_by/identical and the rest of REQ-FN-002C remain deferred.
2. User-defined aggregates with float members are unanalyzable (no reflection) — REQ-NUM-007 recursion cannot be enforced for them; documented known limitation.
3. Set duplicate-insertion in a constexpr context hits std::abort(); constexpr sets can't hold duplicate elements.
4. REQ-SEQ-022 operation-level specification reconciliation for remaining operation families (mementum/knowledge/cljonic-next-agenda.md).
