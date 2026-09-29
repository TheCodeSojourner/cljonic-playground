## Session State

- last_session_id: 59abeb60-d44b-458b-93ba-862f63a872c7
- current_timestamp: 2026-09-29
- recover: 1
- session_complete: false
- oriented: 2026-09-29 (gybis-init: read state.md; memories since eb80ac56 — named-parameter-lint-vs-unused-parameter, doxygen-prose-must-be-user-facing, doc-sample-extraction-requires-unindented-fence, spec-to-code-strict-fail-gate, vocab-arch-weed-skip-aspirational-scope, governance-invariants-do-not-belong-in-behavioral-specs; knowledge — value-equality-domain, cljonic-next-agenda, mementum-synthesis)

Task:
1. `Equal` public doc comment refinement — COMPLETE (commits 75d2b68 + 5606804): removed all Clojure references (brief no longer "modeled on Clojure's `=`"); rewrote the prose to be user-facing and contract-first (dropped REQ-FN-002G/REQ-NUM-006/REQ-FN-014B/REQ-COLL-021 IDs, the `\ref StableEqualityComparable` link, and equality-family/gating jargon); restated the three rules as user-visible behavior (single value always true; ordinary values by same-type `==` with example types; producer comparison bounded by `CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT`; matching element/key/value types; order-insensitive Map/Set); fixed a merged list bullet; reformatted the sample to the bare (non-asterisk) `~~~~~{.cpp}` fence so scripts/compile-doc-samples.py actually extracts and compiles it (docs-examples:compiled 13→14).
2. Unary-arity gate repair — COMPLETE (commit 75d2b68): compiling the sample for the first time exposed that the unary `equal` overload's unnamed parameter (`const T&`) fails clang-tidy `readability-named-parameter` (in WarningsAsErrors), while naming it fails `-Werror=unused-parameter`. Fixed with the prefix form `[[maybe_unused]] const T& value`.
3. Memory stored — COMPLETE (commit 07c45bc): mementum/memories/named-parameter-lint-vs-unused-parameter.md.
4. Project-wide doc-sample verification fix — COMPLETE (2026-09-29): converted all 19 remaining asterisk-prefixed `\b Examples` blocks to the bare-fence style (all 32 headers now extractable); docs-examples compiled 14→27. The conversion surfaced 6 stale public examples for the deferred sequence-shaping free functions (empty, first, next, not-empty, rest, seq) that were deliberately removed from the public API (commit cc376ce) and are absent from the cljonic-core.hpp umbrella; per human decision they are excluded explicitly (DEFERRED_NON_PUBLIC_HEADERS in scripts/compile-doc-samples.py, reported as docs-examples:deferred-skipped=6) with a "Not yet part of the public API" note in each header.
5. `not_equal` slice — COMPLETE (2026-09-29, REQ-FN-002H): added the canonical general-inequality free function as the thin negation of `equal`, top-down. Requirements: REQ-FN-002H. Vocabulary: `NotEqual` term. Architecture: λ `S2_not_equal_function`. Specification: new `specs/sequences/not-equal.allium` (entity `NotEqualFunction`, 11 invariants + actor). Code: new `src/cljonic-not-equal.hpp` (unary/binary/variadic, all delegating to `equal`), umbrella-registered in `cljonic-core.hpp`. Tests: `tests/cljonic-not-equal-spec-tests.cpp` (all 12 TRACE_IDs; STATIC_REQUIRE + volatile runtime coverage) and a dedicated no-heap probe registered in `probes.hpp`/`harness_main.cpp`. Gating: new `scripts/check-not-equal-compile-failures.py` + `make not-equal-compile-fail` wired into `.PHONY`/upsert-gate/validate/git. Snapshot synced.

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

Validation:
1. `make git` green end-to-end: format, lint, complexity, range/variant/equal-compile-fail, sanitizer, coverage lines=100.0%, traceability-spec-to-code, no-heap-src/symbols/probe, docs, docs-examples:compiled=14, cljonic-test 170/170.
2. Doc-sample fix (2026-09-29): `make git` green — format, lint, complexity, compile-fail harnesses, sanitizer, coverage lines=100.0%, traceability-spec-to-code, no-heap-src/symbols/probe, docs, docs-examples:compiled=27 + deferred-skipped=6, cljonic-test 170/170.
3. not_equal slice (2026-09-29): `make git` green — format, lint, complexity, range/variant/equal/not-equal-compile-fail (39 rejection + 20 pass cases × two builds), sanitizer, coverage lines=100.0%, traceability-spec-to-code (snapshot synced; all 12 NotEqualFunction obligations traced), no-heap-src/symbols/probe, docs, docs-examples:compiled 27→28, cljonic-test ok; `allium check`/`analyse` clean (0 warnings).

Next:
1. Candidate synthesis — COMPLETE (2026-09-29, human-approved): created mementum/knowledge/artifact-boundary-discipline.md from spec-to-code-strict-fail-gate.md, vocab-arch-weed-skip-aspirational-scope.md, and governance-invariants-do-not-belong-in-behavioral-specs.md.
2. Candidate next comparison slices: `equal_by`/`identical`, then `less`, `less_equal`, `greater`, `greater_equal` (REQ-FN-002C); `not_equal` is now complete (REQ-FN-002H).
3. REQ-SEQ-022 operation-level specification reconciliation (mementum/knowledge/cljonic-next-agenda.md).

Carry-forward (unaddressed, remember for later):
1. equal_by/identical and the remaining REQ-FN-002C comparison family (`less`, `less_equal`, `greater`, `greater_equal`) remain deferred; `not_equal` is now implemented (REQ-FN-002H).
2. User-defined aggregates with float members are unanalyzable (no reflection) — REQ-NUM-007 recursion cannot be enforced for them; documented known limitation.
3. Set duplicate-insertion in a constexpr context hits std::abort(); constexpr sets can't hold duplicate elements.
4. REQ-SEQ-022 operation-level specification reconciliation for remaining operation families (mementum/knowledge/cljonic-next-agenda.md).
5. Project-wide doc-sample gap — FIXED 2026-09-29: all 32 headers now use the bare fence and are extracted; docs-examples:compiled=27. The 6 deferred non-public sequence-shaping headers (empty, first, next, not-empty, rest, seq) are explicitly excluded (DEFERRED_NON_PUBLIC_HEADERS) and reported as deferred-skipped=6.

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
