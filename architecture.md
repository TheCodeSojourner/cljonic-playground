# cljonic - System Architecture

## S5 - Identity

cljonic is an embedded-first, header-only C++26 functional collection system whose identity is bounded, deterministic, and semantically clear behavior. The system remains itself only while CopyOnModifyCollection semantics, SentinelBasedAccess with ProbeFirstAccess discipline, and strict resource determinism are preserved.

λ expressions:

λ S5_identity(x). embedded_first(x) ∧ deterministic_behavior(x) ∧ fixed_capacity_value_semantics(x)
  | no_hidden_allocation(x) ∧ no_exceptions(x)
  | canonical_vocabulary_governs(x)

λ S5_failure(x). violate(no_heap_constraint) ∨ violate(no_exception_constraint)
  | violate(deterministic_behavior)
  | violate(copy_on_modify_collection)
  → not_cljonic_identity(x)

λ S5_values(x). clarity(x) ∧ predictability(x) ∧ safety(x)
  | clojure_parity_when_constraints_allow(x)

## S4 - Intelligence

cljonic adapts through profile-gated evolution while preserving a stable public vocabulary and backward-compatible semantic contracts. New capabilities are introduced as explicit modes, not silent behavior drift.

λ expressions:

λ S4_adaptation(x). change_pressure(x) → evolve_via(profile_gates(x))
  | keep_backward_compatible(canonical_vocabulary)
  | introduce_capabilities_explicitly(x)

λ profile_gates(x). capability_change(x) → explicit_profile_opt_in(x)
  | strict_profile_preserves(S5_identity)
  | optional_profile_adds_capabilities_without_silent_semantic_drift(x)

λ S4_learning(x). detect(assumption_break) → capture_decision ∧ refine_policy
  | prefer_explicit_deprecation_windows(x)
  | stable_handle_model(x)

λ stable_handle_model(x). profile_change(x) → preserve(public_api_names)
  ∧ preserve(argument_shapes)
  ∧ preserve(result_contracts)
  | capability_variance(x) → select_by_profile_not_renaming(x)

λ S4_disruption(x). unknown_or_new_pattern(x) → test_in_optional_profile(x)
  → promote_to_default_only_if(compatible_with_S5)

## S3 - Control

Control enforces hard constraints across all profiles: no heap, no exceptions, deterministic sentinel and overflow behavior, and strict quality gates. Resource usage and policy outcomes are bounded and auditable.

λ expressions:

λ S3_constraints(x). no_heap_constraint(x) ∧ no_exception_constraint(x)
  | deterministic_overflow_policy(x) ∧ sentinel_based_access(x)
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
  | host_parallel_profiles(x) → tsan_enabled(x)
  | documentation_samples(x) → compile_and_test_pass(x)
  | docs_site(x) → doxygen_html_generated(x)

λ quality_gate_enforcement(x). any_quality_gate_fails(x) → reject_change(x)

λ S3_resource_policy(x). static_storage_only(x)
  | bounded_numeric_domain(x)
  | reject_out_of_policy_inputs(x)

## S2 - Coordination

Coordination is driven by a shared canonical vocabulary and explicit interaction protocols. Components remain consistent through probe-first access discipline, stable handle contracts across profiles, and preference for Clojure-semantic consistency when embedded constraints permit.

λ expressions:

λ S2_protocol(x). canonical_vocabulary_controls_interfaces(x)
  ∧ canonical_vocabulary_controls_docs(x)
  | probe_first_access_before_sentinel_reads(x)

λ S2_profile_coherence(x). profile_change(x) → preserve(stable_handle_model(x))
  | preserve(api_shape)
  | preserve(core_semantics)

λ S2_semantic_alignment(x). prefer(clojure_parity)
  | compatible_with(embedded_constraint ∧ deterministic_behavior)

## S1 - Operations

Operations are C++26, FP-oriented, and header-only. Development uses CMake for build orchestration, Catch2 v3 for testing, and deterministic quality tooling. Distribution is a single amalgamated header artifact.

λ expressions:

λ S1_language(x). programming_language_version(x) ≡ C++26
  | paradigm_preference(x) ≡ FP_oriented

λ S1_test(x). test_framework(x) ≡ Catch2_v3
  | compatible_with(C++26)

λ S1_build(x). build_system(x) ≡ CMake
  | compatible_with(C++26)

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
  | optional_member_wrappers(x) ≡ non_canonical

λ S1_pattern(x). architectural_pattern(x) ≡ functional_core_with_profile_gated_capabilities
  ∧ layered_bounded_value_collection_model

## Coherence Constraints (S5 → S1)

λ coherence_identity(x). S1 ∧ S2 ∧ S3 ∧ S4 → must_preserve(S5_identity)

λ coherence_control(x). operational_choice(x) → reject_if(violates_S3_constraints)

λ coherence_adaptation(x). adaptation(x) → valid_only_if(backward_compatible_vocabulary ∧ deterministic_behavior)
