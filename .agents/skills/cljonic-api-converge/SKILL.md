---
name: cljonic-api-converge
description: "Use for /cljonic-api-converge <function>."
argument-hint: "<function> [canonical API name] [targeted|full]"
user-invocable: true
disable-model-invocation: true
---

λ cljonic-api-converge(function, canonical?, mode?).
  purpose: final cross-layer convergence of one free-function API
  | gate: explicit_user_invocation("/cljonic-api-converge") ∧ function ∃
  | canonical ≔ supplied ∨ derive(repository_artifacts) | ambiguous(target) → ask_user
  | mode ∈ {targeted, full} | default ≔ targeted
  | sequence: startup → recall → requested_passes → evidence_reconciliation → handoff
  | notation: ≔ definition | ∧ all | ∨ alternatives | ¬ prohibition | → consequence/order
  | current(evidence) ≡ inputs_match_final_versions ∧ contract_mapping_valid

λ boundaries().
  forbid(commit ∨ stage ∨ revert ∨ discard_user_edits ∨ run("make git", direct ∨ indirect))
  | preserve(unrelated ∧ preexisting_edits) ∧ ¬attribute_to_this_API
  | coordinator ≡ read_only(product) except evidence_repair_scope
  | evidence_repair_scope ≔ target_no_heap_probes ∪ necessary_harness_build_registration
      ∪ owning_header_Doxygen_prose_example ∪ corresponding_amalgamation_docs
  | exception ≠ permission(change_API_behavior ∨ ordinary_tests ∨ req ∨ vocab ∨ arch ∨ spec)
  | other_changes → owning_gybis_scope_and_approval_gates
  | invoke(required_gybis, skill_tool) ∧ ¬hand_slash_execution_back_to_user
  | allowed_skills ≔ matching_check_weed ∪ explicitly_required_internal_support
  | substantive_choice ∨ requested_approval → ask_user → wait → resume
  | weed_resolution ≡ human_decision | investigate ∨ skip → unresolved ¬converged

λ startup().
  inspect("git status --short") → map(API_related_paths,
    {requirements, vocabulary, architecture, specs, tests, implementation, docs, generated})
  | ledger ≔ {command, reported_status, inputs_known, subsequent_edits, current_invalidated_unresolved}
  | after(each_gybis_invocation): inspect(status ∧ relevant_diffs) → update(ledger)
  | evidence ≔ concrete_artifacts ∧ actual_skill_reports ¬inferred_success
  | missing_or_ambiguous_skill_result → inspect(transcript_or_output_if_available)
    → if_still_missing(ask_user_for_status_findings_modified_paths)
  | resume_or_context_loss → compare(checkpoint, status, diffs)
    | ¬assume(prior_skill_ran ∨ result_current, conversation_order)
  | build(evidence_table) ∧ inspect(existing_probe_bodies ∧ source_example)
    even_if(empty_diff ∨ diagnostic_only)

λ recall().
  read(current_section([state](../../../mementum/state.md)))
  | read([probe propagation](../../../mementum/memories/no-heap-probes-propagate-with-feature.md),
         [Doxygen tooling](../../../mementum/knowledge/doxygen-doc-tooling.md),
         [header lifecycle](../../../mementum/knowledge/header-addition-and-verification-lifecycle.md))
  | search(mementum/memories/ ∪ mementum/knowledge/, {target, no_heap, examples, verification})
  | follow(relevant_related_links) → ledger(read_paths ∧ conflicts)
  | mandatory even_if(guidance_already_inline)
  | memory ≡ context ¬authority_over(current_contract ∨ human_decision)
  | [representative examples](../../../mementum/memories/docs-user-facing-example-style.md)
      > lifecycle_page_demand(every_collection_in_example)
  | [defer HTML](../../../mementum/memories/defer-doc-regeneration-until-final-git.md) ≡ default
  | verify(historical_paths ∧ inventories ∧ gate_descriptions, current_artifacts)
  | remaining_substantive_conflict → surface ∧ ask_user

λ commands().
  allium ≔ {"allium check <file-or-directory>", "allium analyse <file-or-directory>", "allium plan <file>"}
  | use("analyse") ∧ ¬guess(commands) ∧ ¬probe("--help")
  | uncertain(command) → read(Makefile ∨ owning_skill_guidance)
  | changed(req ∨ vocab) → prefer("make requirements-structure" ∨ "make vocabulary-structure")
  | focused_trace ≔ 'make trace-id-check SPEC=<spec.allium> TESTS="<test.cpp> [shared.cpp...]"'
  | add("EXACT=1") only_if(all_supplied_tests_dedicated_to_one_spec)
  | authoritative_trace ≔ "make traceability-spec-to-code"
  | checker_self_tests ≔ "make audit-scripts"

λ targeted().
  scope ≔ API_changed_surfaces ∧ relevant_cross_layer_edges ¬unrelated_content
  | invoke_in_order:
    1. "/gybis-req-check" if(requirements_changed ∨ downstream_readiness_not_current)
       | readiness_probes(vocab ∧ arch ∧ Allium) → ¬duplicate unless(detail_needed ∨ inputs_changed)
    2. "/gybis-spec-check" if(active_spec_changed ∨ spec_check_not_current)
       | may_repair(specs) → invalidate_and_rerun(affected_readiness_checks) before weeds
    3. applicable({"/gybis-req-weed", "/gybis-vocab-weed", "/gybis-arch-weed", "/gybis-spec-weed"})
       in_dependency_order
       | req_weed ≠ vocabulary_only | spec_weed includes(strict_trace ∧ possible_tests)
  | skip(layer) only_if(unchanged_artifacts ∧ current_evidence)
  | after_each_invocation → ledger_update ∧ dependent_invalidation
  | continue_noninteractive ∧ ask_user_only_for_decision
  | finish_passes → evidence_reconciliation mandatory

λ full().
  gate: user_requests(full)
  | explain(spec_check_may_repair ∧ weeds_may_modify_overlapping_layers
            ∧ spec_weed_runs(strict_trace ∧ test_suite)) before start
  | invoke_exact_order:
    ["/gybis-req-check", "/gybis-vocab-check", "/gybis-arch-check", "/gybis-spec-check",
     "/gybis-req-weed", "/gybis-vocab-weed", "/gybis-arch-weed", "/gybis-spec-weed"]
  | after_each → inspect(status ∧ diffs) → ledger_update
  | before_first_weed:
      compare(worktree, pre_check_baseline) → invalidate(earlier_results_depending_on_repairs)
      → rerun(only_affected_checks)
      | downstream_readiness_predates_spec_repair → rerun("/gybis-req-check") if_needed
  | weed_allowed only_if(required_owning_checks_current_and_passing)
  | user_decision → ask_user → resume
  | ¬duplicate_full_suite unless(later_edit_invalidates)
  | finish_passes → evidence_reconciliation mandatory even_if(all_gybis_green)

λ evidence_table().
  contract ≔ final(requirements ∧ vocabulary ∧ architecture ∧ active_spec)
  | columns ≔ {clause_or_obligation, ordinary_assertion_or_rejection_test,
               runtime_no_heap_probe_assertion, Doxygen_demonstration, validation_status}
  | derive_rows(contract) ¬derive_from(existing_tests)
  | rows_cover(admitted_domains, results, applicable_empty_missing_full_capacity_boundaries,
               safe_conversions, input_preservation, resource_guarantees)
  | nonapplicable(cell) → contract_based_reason
  | examples ≡ representative ¬exhaustive | rejected_calls → compile_fail ¬runnable_examples
  | inspect(always) ∧ upsert(if_deficient) ∧ unchanged(surface) only_if(case_level_mapping)
  | exists(file) ∨ present(TRACE_ID) ∨ pass(weed ∨ gate) ≠ sufficient_evidence

λ evidence_reconciliation().
  read(actual_probe_bodies ∧ owning_source_Doxygen_program)
  | no_heap_evidence() → example_evidence() → generated_outputs() → final_validation()
  | repair within evidence_repair_scope → apply ∧ ledger_update
  | outside_scope ∨ substantive_contract_decision → owning_skill_approval ∧ keep_unresolved
  | later_upstream_repair → reopen(affected_rows) → reconcile_again

λ no_heap_evidence().
  upsert(existing_target_or_family_probe under tests/no_heap/) | ¬parallel_harness
  | no_suitable_probe → add(current_naming ∧ registration(probes.hpp, harness_main.cpp, build_targets))
  | ∀ new_or_changed_admitted_behavior:
      direct_API_call ∧ observable_result_assertions ∈ dedicated_runtime_no_heap_harness
  | runtime_derived_inputs where_needed(prevent_constant_folding)
    | volatile_input → plain_local → API(const_T_reference)
  | representative(source_operand_families) ∧ meaningful_boundaries ∧ required_input_preservation
  | rejected_form_without_admitted_runtime_counterpart → explain(table) ∧ retain(compile_fail)
  | noexcept ∨ static_assert ∨ source_scan ∨ symbol_scan ∨ ordinary_tests ≠ runtime_probe_coverage

λ example_evidence().
  upsert(owning_source_header.Doxygen_contract_and_program) ¬generated_HTML_or_scratch_only
  | match(final_domain, canonical_terms, results, boundaries, conversions, preservation)
  | program ≔ '#include "cljonic.hpp"' → file_scope("using namespace cljonic;") → "main()"
  | C++23 ∧ public_free_functions ∧ representative_named_values ∧ plain_integer_literals
  | prefer(CTAD) if(deduction_expresses(intended_type ∧ intended_capacity))
    | explicit_template_arguments if(deduction_unavailable ∨ demonstrating(specific_type ∨ specific_capacity))
  | ¬unnecessary_U_suffix ∧ ¬implementation_detail_member_calls
  | constexpr_results → static_assert | runtime_derived_inputs → checked_results
  | normal_case ∧ applicable_important_boundary_fallback_or_prefix ∧ required_input_preservation
  | runtime_failure → nonzero_exit | unused_result ∨ unconditional_success ≠ evidence
  | bare_fences("~~~~~{.cpp}", "~~~~~") ∧ ¬asterisk_prefix_inside_sample
  | Doxygen_block immediately_before(public_declaration)
  | concise_user_prose ∧ link(shared_conversion_rules) ∧ fallback_term("default value")
  | ¬capability_model_jargon ∧ preserve(unrelated_legacy_mainpage_narrative)
  | source_scan_includes_comments → avoid(forbidden_allocator_container_spellings ∨ prose_token("new "))
  | format_twice(scoped_clang_format ∧ "scripts/format-doc-samples.pl <header>")
    → verify(second_pass_changes_nothing ∧ intact_bullets_and_fences)
  | promotion → verify(curated_source_cheatsheet ∧ umbrella_include ∧ extraction_registration)
    | gap_outside_exception → owning_skill_approval

λ generated_outputs().
  before(any_generator): inspect(status ∧ diff, every_output)
  | preexisting_or_ambiguous_output_edits → ask_user before overwrite
  | ¬restore_discard_user_edits
  | final_source_formatting → "make cljonic" overwrites(cljonic.hpp)
  | "make docs-examples" also_invokes("make cljonic")
  | default: defer(docs/ HTML, human_final_gate) ∧ report(deferred ¬rendered_current)
  | explicit_user_request(generation ∨ rendered_verification) → inspect_outputs → "make docs"
    | warning: "make docs" removes(docs/) before regenerate
  | deferred_HTML ≠ missing_source_example_evidence

λ final_validation().
  final(implementation, probes, harness, amalgamation) → "make no-heap"
  | require(source_scan ∧ symbol_checks ∧ executed_successfully(modular ∧ single_header))
  | safe_final_amalgamation → "make docs-examples"
    → verify(compiled_count ∧ deferred_exclusions ∧ target_extracted_and_compiled_this_invocation)
  | extracted ≔ configured_build_dir/docs-examples/target_translation_unit
    | default_build_dir ≔ build/
  | fresh(extracted) ∧ ¬stale_previous_file ∧ ¬hand_copied_substitute
  | link(extracted, repository_C++23_compiler_settings) → execute(extracted_program) → require(exit=0)
  | docs_examples_compile_only("-c") ≠ link_or_runtime_validation
  | reuse(result) only_if(current(result) ∧ mapped_behaviors_covered)
  | failed ∨ unavailable(validation) → blocked ¬converged

λ invalidation(changed).
  req → requirements_check ∧ requirements_to_downstream_comparisons
  | vocab → vocabulary_check ∧ vocabulary_to_downstream_comparisons
  | arch → architecture_check ∧ architecture_to_spec_code_comparisons
  | spec → spec_check ∧ Allium ∧ traceability ∧ spec_comparisons
  | req ∨ vocab ∨ arch ∨ spec → affected_probe_example_mappings even_if(unchanged_probe_example)
  | tests ∨ implementation → relevant_tests ∧ spec_to_code
  | tests/no_heap/ → no_heap | compile_fail_inputs → matching_compile_fail_gate
  | TRACE_IDs ∨ spec → strict_trace_against_snapshot
  | executable_public_header → modular_and_single_header_tests ∧ compile_fail ∧ no_heap
      ∧ amalgamation ∧ docs_examples
  | Doxygen_only → formatting ∧ amalgamation ∧ example_extract_compile_link_run
      ∧ comment_sensitive_no_heap_source_scan ∧ HTML
      | ¬invalidate(unchanged_runtime_behavior_evidence)
  | output_current only_if(regenerated_from_final_source) | explicitly_deferred_HTML ¬blocks(source_convergence)
  | after_final_edits → rerun(only_invalidated_owning_checks)
  | reuse(gybis_builtin_validation) only_if(explicit_report_covers_final_inputs)

λ handoff().
  converged only_if(no_new_edits_since_last_relevant_pass ∧ required_checks_current_and_passing
                    ∧ evidence_table_satisfied ∧ no_unresolved_divergence)
  | missing_probe_example_evidence → ¬converged even_if(all_gybis_green)
  | otherwise: identify(invalidated_edge ∧ next_needed_action)
  | final: evidence_table ∧ probe_example_paths_and_cases(added ∨ verified_unchanged)
    ∧ modular_and_single_header_no_heap_results ∧ exact_example_compile_link_run_results
    ∧ unresolved_items ∧ HTML_deferral_if_any
    ∧ state("make git and commits not performed; user-owned")
  | every_pause → checkpoint(concise, copy_pasteable)

```text
WORKFLOW=converge
API=
MODE_OR_LAYER=
LAST_COMMAND_STATUS=
COMPLETED=
MODIFIED_PATHS=
CURRENT_VALIDATIONS=
NO_HEAP_EVIDENCE=
DOXYGEN_EXAMPLE_EVIDENCE=
INVALIDATED=
OPEN_DECISIONS=
NEXT_ACTION=
```
