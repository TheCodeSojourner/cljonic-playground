## Session State

- last_session_id: 4c6756bf-400a-4e32-b5c4-f447a3991b06
- current_timestamp: 2026-09-25
- recover: 1
- session_complete: true

Task:
1. Resume after session 5bfdfb56 and finish its follow-through: fix the `NothrowCollectionElement` blind spot by constraining the `Iterate()`/`Repeatedly()` default ctors with `requires std::default_initializable<Step>` (REQ-VAL-017D), regenerate the stale root `cljonic.hpp`, retire Stream D (alternative-strict final), pin `NumericEquality` vocabulary (no cross-type rules), fix the stale `S3_domain_boundary` (add `Repeatedly`, retire `planned_producer_domain`), run the full six-command convergence sweep, and regenerate + verify the obligation-id snapshot with no-heap gates.
2. Prior session work carried in this working tree (validated, approval pending): Streams A+B+C — Composite Values in the Closed Cljonic Domain (`std::variant` map keys/set elements, recursive stable equality REQ-CAP-010, collection equality REQ-COLL-021), producer storage admission (REQ-VAL-017D default forms), producer parameter equality (REQ-FN-014B `Range`/`Repeat`/`Cycle` `==` + `parameters_equal`).

Questions:
1. No blocking questions remain.

Decisions:
1. `StableEqualityComparable` is now recursive: `std::equality_comparable<T> && !contains_floating_point_v<T> && !contains_callable_v<T>`.
2. `concepts_detail::contains_floating_point` / `contains_callable` walkers recurse through `std::variant` alternatives, cljonic collections, MapEntry, AND producer stored parameters (per-header specializations; producers in their own headers, MapEntry in cljonic-map-entry.hpp; `Iterate`/`Repeatedly` mark `contains_callable` true due to the stored step).
3. Callable components rejected as a class, including function pointers.
4. `MapEntry<K,V>::operator==` constrained (REQ-NUM-007 fix).
5. Collections have conditional `operator==` (REQ-COLL-021): Vector/Queue/String order-sensitive; Map/Set order-insensitive.
6. New spec `specs/collections/equality.allium`; `entity AlternativeStrictEquality` added; REQs REQ-CAP-010 / REQ-COLL-021 added.
7. Stream B: default ctors added to `Repeat` (finite-empty, `Repeat{T{},0}`), `Repeatedly` (finite-empty, `Repeatedly{Step{},0}`), `Iterate` (unbounded, `Iterate{T{},Step{}}`), `Cycle` (`= default`, unbounded over default source). `Range` already had its default (finite-empty). New REQ-VAL-017D + architecture section "Producer Storage Admission Architecture" + `entity ProducerStorageAdmission` in materialization.allium + per-producer `satisfies_nothrow_collection_element`/`DefaultFormIsValid` invariants.
8. A single range-typed argument to `Vector<C,N>{...}`/`Queue<C,N>{...}` matches the range/view constructor, not the pack constructor (source-constructor ambiguity). Tests must use multi-element pack construction or `.assoc(0U, ...)`.
9. `NothrowCollectionElement<Iterate<T, Step>>` with a non-default-constructible Step evaluates TRUE in an unevaluated requires context (member-init failures aren't visible), even though direct `Iterate<T,Step>{}` is a hard error; storage-site rejection is verified via the variant-compile-fail gate instead (FAILURE_PREAMBLE_CASES with NonDefaultStep/NonDefaultGen).
10. Stream C: producer equality is PARAMETER-STRUCTURAL (REQ-FN-014B). `Range<T>` `operator==` compares `start_/end_/step_`; `Repeat<T>` compares `value_/count_/is_finite_`; `Cycle<Source>` compares `source_/count_/is_finite_` (recursing into the owned source). All three provide a named free function `parameters_equal` with identical semantics. O(1), constexpr, noexcept, non-allocating, never traverses. Distinct parameters compare unequal even when produced sequences coincide (e.g. `Range{0,5,0} != Range{0,7,0}`; `Repeat{7} != Repeat{7,0U}`).
11. Stream C admission: `Range`/`Repeat`/`Cycle` satisfy `StableEqualityComparable` and `NothrowStableEqualityComparable` over stable components → usable as map keys, set elements, and composite components. `Iterate`/`Repeatedly` never do (callable `step_` → `contains_callable` true).
12. REQ amendments for Stream C: REQ-SEQ-016 reworded to withhold SEQUENCE equality (compile-time failure) while permitting producer parameter equality; REQ-FN-014A reworded (sequence-equality restriction only); REQ-FN-014B made normative (mandatory `==` + `parameters_equal` for all-stable-parameter producers); REQ-CAP-010 widened "supported bounded producers" → "supported producers whose stored parameters admit stable equality".
13. New spec `specs/producers/parameters-equal.allium` + `entity ProducerParameterEquality` in materialization.allium + `RecursesIntoProducerParameters` on StableEqualityComparable + `ProducerComponentsUseProducerParameterEquality` on CollectionEquality. Vocabulary gains `ProducerParameterEquality` term.
14. Compile-fail gate extended: producer-key/equality PASS cases + float/callable producer FAILURE cases (Repeat-float, Cycle-float-source, variant-iterate-callable).
15. Convergence weeds (all decisions human-approved): vocab-weed C1 arch (canonical `producer_parameter_equality`), C2 spec+code (full repo canonicalization to ProducerParameterEquality / producer parameter equality; vocabulary synonym map corrected), C3 skip (29 aspirational terms retained); arch-weed A1 arch (lambda renamed `S2_producer_parameter_equality`), A2 arch (Level-2 collection_equality recursion clause added), A3 arch (StableEqualityComparable formula corrected with `!contains_callable_v`); spec-weed converged first pass, no decisions.
16. New knowledge page `mementum/knowledge/value-equality-domain.md` synthesizing the unified equality architecture (streams A+B+C crystallized).

17. Human decision 2026-09-25: alternative-strict variant equality is FINAL and exclusive. Cross-type numeric unification (`variant<int,long>{1}` vs `variant<int,long>{1L}`) is permanently out of scope, not deferred. No Stream D work stream exists or will be created.
18. `NumericEquality` vocabulary term pinned (human-approved vocab-tend): "No cross-type comparison rules are permitted: values of different numeric types never compare equal under any cljonic equality operation; a cross-type comparison requires an explicit conversion performed by the caller under NumericPolicy." Zero usages in arch/specs, so no propagation was needed.
19. Arch-tend fix for arch-check warning W1: `S3_domain_boundary.implemented_value_domain` now includes `Repeatedly`; the single-occurrence `planned_producer_domain(x) ≡ repeatedly` line retired. All four domain statements (current_scope, module4_active_slice, S3_domain_boundary, current_implementation_boundary) now agree.
20. Obligation-id snapshot regenerated (`make traceability-spec-to-code-update-snapshot`) and verified (`make traceability-spec-to-code` ok): 782 strict-gate ids, zero drift, every obligation covered by at least one test TRACE_ID (strict `comm` gate). No-heap src/symbol gates pass.

Validation:
1. `make git`: FULL PASS (format, lint, complexity, cljonic header, range-compile-fail, variant-compile-fail, sanitizers, coverage lines=100.0%, traceability-spec-to-code, no-heap-src/symbols, docs, 13 doc examples, single-header probe, git:ok).
2. `make test`: 100% tests passed out of 154.
3. Six-command convergence sweep: vocab-check PASS (151 terms, 0 issues), arch-check PASS (0 findings after the S3 fix), spec-check PASS (30/30 files, 0 diagnostics/0 issues), vocab-weed 0 divergences (29 aspirational terms retained per standing decision C3), arch-weed 0 divergences (14 candidates all resolved as naming-form coverage), spec-weed 0 divergences (1378 obligations, strict traceability PASS, tests PASS).
4. Probe check: `NothrowCollectionElement<Iterate<int, NonDefaultStep>>` and `<Repeatedly<int, NonDefaultGen>>` now correctly evaluate false; storage-site rejection previously verified only via the variant-compile-fail gate is now visible at the concept boundary itself.

Next:
1. Human approval needed before committing the implementation work: Streams A+B+C implementation + blind-spot fix + regenerated cljonic.hpp + regenerated docs/snapshot + the NumericEquality vocab pin + the S3 domain-boundary fix are all uncommitted in the working tree (this fini commits mementum/ only, matching the established precedent).
2. Candidate next slice: `equal`/`equal_by`/`identical` documented (cljonic-core.hpp:135) but unimplemented; REQ-FN-002E deferred. Completes the equality domain table in mementum/knowledge/value-equality-domain.md.
3. RETIRED: Stream D (cross-type numeric unification). Human decision 2026-09-25: alternative-strict is final and exclusive; there is no reopen path — changing it would be a new spec-tend with full propagation, not a deferred item.
4. REQ-SEQ-022 operation-level specification reconciliation remains the older agenda item (mementum/knowledge/cljonic-next-agenda.md).

Carry-forward (unaddressed, remember for later):
1. Single-range-argument construction ambiguity: `Vector<C,N>{one_range_typed_value}` selects the source constructor, not the pack constructor (producers are ranges). Tests work around it (two-element packs / `.assoc(0U, ...)`); users hit it. Needs a decision: documented guidance, a REQ pinning constructor-preference, or a named append/emplace helper.
2. RESOLVED this session: the NothrowCollectionElement blind spot is fixed by constrained producer default ctors (`requires std::default_initializable<Step>` on Iterate/Repeatedly). Concept-level rejection now matches storage-site rejection; the variant-compile-fail storage cases remain as regression evidence.
3. `equal`/`equal_by`/`identical` documented (cljonic-core.hpp:135) but unimplemented; REQ-FN-002E deferred. Separate slice.
4. User-defined aggregates with float members are unanalyzable (no reflection) — REQ-NUM-007 recursion cannot be enforced for them; documented known limitation, revisit with the aggregate story.
5. Set duplicate-insertion in a constexpr context hits `std::abort()` (src/cljonic-set.hpp:117), so constexpr sets with duplicate elements can't be formed; the equality contract is verified with distinct producers at compile time and duplicate no-op at runtime.
