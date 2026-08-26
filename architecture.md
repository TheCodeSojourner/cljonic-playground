# cljonic - System Architecture

## Current Scope

The current implementation realizes only the Vector slice of this architecture.
The retained rules for Map, Set, Queue, String, producers, and other operations
describe future approved expansion and must not be treated as implemented
behavior. A future module becomes active only after its requirements,
vocabulary, specification, tests, and code are propagated together.

## S5 - Identity

cljonic is an embedded-first HeaderOnlyDistribution C++23 functional collection system whose identity is bounded, deterministic, and semantically clear behavior. The system remains itself only while CopyOnModifyCollection semantics, SentinelBasedAccess with ProbeFirstAccess discipline, and strict resource determinism are preserved.

λ expressions:

λ S5_identity(x). embedded_first(x) ∧ DeterministicBehavior(x) ∧ fixed_capacity_value_semantics(x)
  | no_hidden_allocation(x) ∧ no_exceptions(x)
  | canonical_vocabulary_governs(x)

λ S5_failure(x). violate(NoHeapConstraint) ∨ violate(NoExceptionConstraint)
  | violate(DeterministicBehavior)
  | violate(CopyOnModifyCollection)
  → not_cljonic_identity(x)

λ S5_values(x). clarity(x) ∧ predictability(x) ∧ safety(x)
  | clojure_parity_when_constraints_allow(x)

## S4 - Intelligence

cljonic adapts through profile-gated evolution while preserving a stable public vocabulary and backward-compatible semantic contracts. New capabilities are introduced as explicit modes, not silent behavior drift.

λ expressions:

λ S4_learning(x). detect(assumption_break) → capture_decision ∧ refine_policy
  | prefer_explicit_deprecation_windows(x)
  | stable_handle_model(x)

λ stable_handle_model(x). profile_change(x) → preserve(public_api_names)
  ∧ preserve(argument_shapes)
  ∧ preserve(result_contracts)
  | capability_variance(x) → select_by_profile_not_renaming(x)

## S3 - Control

Control enforces hard constraints: no heap, no exceptions, deterministic sentinel and overflow behavior, and strict quality gates. Resource usage and policy outcomes are bounded and auditable.

λ expressions:

λ S3_constraints(x). NoHeapConstraint(x) ∧ NoExceptionConstraint(x)
  | DeterministicOverflowPolicy(x) ∧ SentinelBasedAccess(x)
  | enforce_in_all_profiles(x)

λ S3_quality(x). require(quality_gates(x))
  | require(tests)
  | require(sanitizers_host_profiles)
  | require(clang_tidy)
  | require(clang_format)
  | require(documentation_checks)

λ quality_gates(x). tests_pass(x)
  ∧ lint_passes(x)
  ∧ docs_checks_pass(x)
  ∧ quality_gate_thresholds_met(x)
  | host_profiles(x) → sanitizers_pass(x)
  | no_heap_claim(x) → no_heap_verification_passes(x)

λ quality_gate_thresholds_met(x). host_profiles(x) → coverage_core_mvp_line_percent(x) ≡ 100
  | host_profiles(x) → asan_enabled(x) ∧ ubsan_enabled(x)
  | documentation_samples(x) → compile_and_test_pass(x)
  | docs_site(x) → doxygen_html_generated(x)

λ quality_gate_enforcement(x). any_quality_gate_fails(x) → reject_change(x)

λ S3_resource_policy(x). static_storage_only(x)
  | collection_maximum_element_count(x) ≡ CollectionMaximumElementCount
  | closed_numeric_domain(x)
  | reject_out_of_policy_inputs(x)

λ S3_result_contract_policy(x). operation_result(x) → classify_as(CompleteResult ∨ BoundedPrefixResult ∨ DefaultReturningResult ∨ CheckedFailureResult ∨ ProducerOnlyResult)
  | may_fail_complete_result(x) → require(PreflightPredicate ∨ CheckedFailureResult)
  | preflight_and_operation(x) → require(semantic_equivalence_on_success_and_failure_conditions)
  | semantically_infinite_producer(x) → bound_synthesis_by(CollectionMaximumElementCount)
  | oversized_finite_producer(x) → classify_as(BoundedPrefixResult) ∧ bound_synthesis_by(CollectionMaximumElementCount)
  | oversized_finite_producer_at_constant_evaluation(x) → classify_as(CompileTimeFailure) ∧ reject(x)
  | synthesis_cap(x) → classify_as(CollectionMaximumElementCount) ∧ not(TrueCardinality)
  | effective_size(x) → authoritative_for(ProducerIteration ∧ ProducerMaterialization)
  | effective_endpoint(x) → normalized_to(EffectiveBoundedPrefixBoundary)
  | producer_materialization(x) → require(ProducerMaterialization) ∧ enforce_synthesis_cap(x)

λ S3_domain_boundary(x). first_class_value_domain(x) ≡ Vector ∧ Map ∧ Set ∧ Queue ∧ String
  | producer_domain(x) ≡ explicit_producers
  | text_matching_domain(x) ≡ bounded_regex_values_and_match_results
  | symbolic_key_domain(x) ≡ supported_scoped_enumerations
  | domain_expansion(x) → require(explicit_approved_requirement)

λ S3_vocabulary_enforcement(x). vocabulary_or_public_surface_change(x) → require(vocabulary_consistency_gate(x))
  | vocabulary_consistency_gate(x) → enforce(canonical_vocabulary_governs) ∧ traces_to(S5_identity)
  | vocabulary_consistency_gate_fails(x) → reject_change(x)

## S2 - Coordination

Coordination is driven by a shared canonical vocabulary and explicit interaction protocols. Components remain consistent through ProbeFirstAccess discipline, stable handle contracts across profiles, and preference for Clojure-semantic consistency when embedded constraints permit.

λ expressions:

λ S2_protocol(x). canonical_vocabulary_controls_interfaces(x)
  ∧ canonical_vocabulary_controls_docs(x)
  | probe_first_access_before_sentinel_reads(x)
  | semantic_predicate_names(x) → enforce(StatePredicate ∧ VerbPredicate ∧ CapabilityPredicate)
  | api_surface_status(x) → enforce(LifecycleClassification)

λ S2_api_lifecycle_gate(x). public_function(x) → classify_as(RequirementsBacked ∨ CandidateStatus ∨ DeferredStatus ∨ ExcludedStatus)
  | classify_as(RequirementsBacked) → requires(approved_behavioral_contracts)
  | classify_as(CandidateStatus ∨ DeferredStatus ∨ ExcludedStatus) → not_supported_behavior(x)
  | relational_ops(index ∨ project ∨ rename ∨ join) → require(RelationModel) before RequirementsBacked
  | relation_model_minimum(x) → require(row_representation ∧ key_value_capabilities ∧ duplicate_row_key_semantics ∧ nested_result_representation ∧ traversal_order_semantics ∧ capacity_arithmetic ∧ complete_result_preflight ∧ bounded_failure_behavior)

λ S2_profile_coherence(x). profile_change(x) → preserve(stable_handle_model(x))
  | preserve(api_shape)
  | preserve(core_semantics)

λ S2_semantic_alignment(x). prefer(clojure_parity)
  | compatible_with(EmbeddedConstraint ∧ DeterministicBehavior)

λ S2_concept_bootstrap(x). concepts_gate_free_functions(x)
  | minimal_concept_requirements(x) → only_what_current_functions_need(x)
  | future_requirements(x) → add_when_first_function_needs_them(x)
  | concept(Collection) ≡ { c.size() returns exactly std::size_t }
  | yagni_evolution(x) → preserve(backward_compat) ∧ prevent(feature_creep)

λ S2_operation_vocabulary(x). canonical_collection_operations(x) ≡ none_currently
  | future_operation(x) → require(explicit_requirement_and_specification(x))
  | preserve(clojure_like_names_and_semantics_by_default(x))

## S1 - Operations

Operations are C++23, FP-oriented, and HeaderOnlyDistribution. Development uses CMake for build orchestration, Catch2 v3 for testing, and deterministic quality tooling. Distribution is a single AmalgamatedHeader artifact.

λ expressions:

λ S1_language(x). programming_language_version(x) ≡ C++23
  | paradigm_preference(x) ≡ FP_oriented

λ S1_test(x). test_framework(x) ≡ Catch2_v3
  | compatible_with(C++23)

λ S1_build(x). build_system(x) ≡ CMake
  | compatible_with(C++23)

λ S1_package_manager(x). package_manager(x) ≡ none_vendored_only
  | optional_dev_dependency_manager(x) ≡ none

λ S1_delivery(x). deployment_method(x) ≡ single_amalgamated_header_release_artifact

λ S1_ci_cd(x). ci_cd(x) ≡ local_makefile_target_workflow
  | remote_ci_pipeline(x) ≡ not_required_for_current_scope

λ S1_quality_tools(x). linter_formatter(x) ≡ clang_tidy ∧ clang_format
  | static_analysis(x) ≡ cppcheck ∧ include_what_you_use
  | docs(x) ≡ doxygen_html_site

λ S1_interfaces(x). interface_types(x) ≡ header_only_free_functions
  ∧ template_concept_constrained_apis
  | required_callable_member_adapters(x) → permitted_when(explicitly_specified_and_behaviorally_equivalent(x))
  | optional_member_wrappers(x) ≡ non_canonical

λ S1_value_and_view_model(x). value_returns(x) → prefer(OwningValue)
  | read_only_observation(x) → use(NonOwningView ∧ StandardViewType)
  | view_lifetime(x) → source_lifetime_bounded(x)

The following sequence and materialization rules are retained as future design
guidance; no producer or free-function sequence operation is currently active.

λ S1_sequence_materialization_model(x). unbounded_sequences(x) → represent_as(UnboundedProducer) ∧ attach_synthesis_cap(CollectionMaximumElementCount)
  | producer_family(range ∧ repeat ∧ cycle ∧ iterate ∧ repeatedly) → classify_as(ProducerOnlyResult) ∧ preserve_semantic_infinity(x) ∧ normalize_effective_bounds(x) ∧ before_materialization(x)
  | semantically_infinite_producer(x) → materialize_at_most(CollectionMaximumElementCount)
  | oversized_finite_producer(x) → materialize_as(BoundedPrefixResult) ∧ adjust_effective_endpoint(x)
  | oversized_finite_producer_at_constant_evaluation(x) → reject_as(CompileTimeFailure)
  | synthesis_cap(x) → not_interpret_as(TrueCardinality)
  | effective_size(x) → authoritative_for(free_function_observation ∧ producer_iteration ∧ producer_materialization)
  | range_slice_contract(x) → free_function_observation_is_canonical(x) ∧ get_lookup_is_excluded(x) ∧ valid_index_authoritative_for_indexed_access(x) ∧ effective_endpoint_normalized_before_iteration(x)
  | range_member_accessors(start ∧ end ∧ step) → classify_as(non_canonical)
  | bounded_collection_results(x) → require(explicit ProducerMaterialization)
  | implicit_unbounded_nested_materialization(x) → reject(x)

λ S1_construction(x). collection_construction(x) ≡ CapacityConstruction
  | literal_deduced(x) → ctad_deduction_guides(x)
  | explicit_capacity(x) → empty_default_construction_valid(x)
  | oversized_initializer(x) → compile_time_failure(x)

λ S1_type_expression_policy(x). public_template_constraints(x) → prefer(concepts)
  | domain_constraints(x) → encode_as(repo_specific_concepts)
  | traits_usage(x) → allowed_only_if(no_clear_or_portable_concept_form(x))

λ S1_user_clarity_policy(x). public_api_constraints(x) → prefer(named_cljonic_concepts)
  | trait_mechanics(x) → hide_inside(cljonic_concepts_namespace)
  | likely_user_error_paths(x) → require(explicit_compile_time_messages)
  | collection_header_examples(x) → focus_on(collection_construction)
  | user_facing_names(x) → prefer(stl_familiar_terms_like_size_and_capacity)
  | cross_cutting_free_operations(x) → document_in(operation_specific_headers)

λ S1_pattern(x). architectural_pattern(x) ≡ functional_core_with_profile_gated_capabilities
  ∧ layered_bounded_value_collection_model

## Coherence Constraints (S5 → S1)

λ coherence_identity(x). S1 ∧ S2 ∧ S3 ∧ S4 → must_preserve(S5_identity)

λ coherence_control(x). operational_choice(x) → reject_if(violates_S3_constraints)

λ coherence_adaptation(x). adaptation(x) → valid_only_if(backward_compatible_vocabulary ∧ DeterministicBehavior)

λ coherence_type_expression(x). implementation_expression(x) → reject_if(uses_traits_where_clear_concept_exists)

λ coherence_operation_surface(x). collection_operation_change(x) → reject_if(renames_or_redefines_canonical_operation_without_constraint_rationale)

λ coherence_result_semantics(x). architecture_change(x) → reject_if(omits_result_status_classification_or_preflight_alignment)

λ coherence_lifecycle_governance(x). api_scope_change(x) → reject_if(missing_LifecycleClassification_or_missing_RelationModel_gate)
