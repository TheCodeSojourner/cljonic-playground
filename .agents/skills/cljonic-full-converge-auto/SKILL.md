---
name: cljonic-full-converge-auto
description: "Use for /cljonic-full-converge-auto: run every gybis layer check, then adjacent-layer weed passes, and repair divergences without approval pauses."
user-invocable: true
disable-model-invocation: true
---

λ cljonic-full-converge-auto().
  purpose: whole_repository_convergence | input: none
  | gate: explicit_user_invocation("/cljonic-full-converge-auto")
  | sequence: startup → layer_checks → adjacent_weeds → evidence_reconciliation → integration → report
  | interaction: autonomous | per_divergence_human_decisions → use("/cljonic-api-converge") instead
  | notation: ≔ definition | ∧ all | ∨ alternatives | ¬ prohibition | → consequence/order
  | current(evidence) ≡ inputs_match_final_versions ∧ contract_mapping_valid

λ authority().
  invocation ≡ standing_preapproval(run_listed_passes ∧ owning_skill_recommended_repairs)
  | repair_allowed only_if(within_owning_documented_scope ∧ identity_preserving ∧ ¬normative_change)
  | forbid(ask_user ∨ approval_pause) during_run
  | preapproval_replaces(owning_skill_question_gates) ¬expands(repair_scope)
  | normative_change ≔ admitted_domain ∨ result ∨ failure_policy ∨ constraint_change
  | not_authorized ≔ normative_change ∨ new_behavioral_contract ∨ new_public_API
      ∨ delete_durable_artifact ∨ resolve_stakeholder_requirement_conflict
  | outside_authority(finding) → ledger(UNRESOLVED, evidence) → continue(nonconflicting_repairs)
  | ¬invent_semantics_to_pass ∧ ¬claim(unresolved_as_converged)

λ boundaries().
  forbid(commit ∨ stage ∨ revert ∨ discard_changes ∨ run("make git", direct ∨ indirect))
  | forbid(destructive_edits ∨ edit_vendored(".agents/skills/internal/**"))
  | preserve(preexisting ∧ unrelated_edits) ∧ ¬attribute_to_this_run
  | ambiguous_preexisting_generated_edits → record(UNRESOLVED, output_path) ∧ skip_overwrite ∧ continue
  | durable_artifacts ≔ requirements/ ∪ vocabulary.md ∪ architecture.md ∪ specs/
  | human_owns(final_execution("make git") before human_guided_commits ∧ commits)
  | convergence_aggregate_gate ≡ only("make upsert-gate-fast")
  | forbid(run("make upsert-gate", direct ∨ indirect) ∨ run("make upsert-gate-strict", direct ∨ indirect))
  | gate_policy_applies_to(invoked_skills) ∧ ¬automatic_strict_or_full_escalation_at_handoff

λ startup().
  inspect("git status --short") → record(preexisting_baseline)
  | map(artifacts, {requirements, vocabulary, architecture, specs, tests, implementation, docs, generated})
  | ledger_start → recall()
  | after(each_skill_invocation ∨ edit): inspect(status ∧ relevant_diffs) → ledger_update
  | resume_after_context_loss → reread(status ∧ relevant_diffs)
    | prior_result_current only_if(worktree_matches_recorded_inputs)

λ recall().
  read(current_section([state](../../../mementum/state.md)),
       [probe propagation](../../../mementum/memories/no-heap-probes-propagate-with-feature.md),
       [Doxygen tooling](../../../mementum/knowledge/doxygen-doc-tooling.md),
       [header lifecycle](../../../mementum/knowledge/header-addition-and-verification-lifecycle.md),
       [verification discipline](../../../mementum/knowledge/verification-signal-discipline.md))
  | search(mementum/memories/ ∪ mementum/knowledge/, relevant_findings ∪ {no_heap, examples, verification})
  | follow(relevant_related_links) ∧ ledger(read_paths ∧ conflicts)
  | memory ≡ context ¬authority_over(current_contract ∨ human_decisions)
  | [representative examples](../../../mementum/memories/docs-user-facing-example-style.md)
      > lifecycle_page_demand(every_collection_in_example)
  | [defer HTML](../../../mementum/memories/defer-doc-regeneration-until-final-git.md) ≡ default
  | verify(stale_paths ∧ inventories ∧ gate_descriptions, current_artifacts)
  | unresolved_substantive_conflict → ledger(UNRESOLVED, evidence) ∧ continue

λ commands().
  invoke(gybis_skill, skill_tool) ∧ ¬hand_slash_execution_back_to_user
  | wait(current_skill_report) before next_layer
  | allium ≔ {"allium check <file-or-directory>", "allium analyse <file-or-directory>", "allium plan <file>"}
  | use("analyse") ∧ documented_Makefile_targets ∧ ¬guess_commands ∧ ¬probe("--help")
  | uncertain(command) → inspect(Makefile ∨ owning_skill)

λ layer_checks().
  invoke_exact_order:
    ["/gybis-req-check", "/gybis-vocab-check", "/gybis-arch-check", "/gybis-spec-check"]
  | each_finding → decision_policy() → immediate_permitted_repair → owning_validation()
  | changed_inputs → invalidate(only_dependent_results) → rerun(affected_checks)
  | spec_check_may_edit → invalidate(earlier_results_depending_on_repaired_specs)
  | changed(req) → "make requirements-structure"
  | changed(vocab) → "make vocabulary-structure"
  | changed(spec) → "allium check specs" ∧ "allium analyse specs"
    | require(zero_diagnostics ∧ zero_findings)
  | changed(checker_script) → "make audit-scripts"

λ adjacent_weeds().
  invoke_exact_order_and_scope:
    1. "/gybis-req-weed": requirements → vocabulary_only
       | findings ≔ {REQ_undefined_term, REQ_non_canonical_term}
       | ¬requirement_to_arch_spec_test_coverage_in_this_pass
    2. "/gybis-vocab-weed": vocabulary → architecture_only
       | noncanonical_spellings ∧ undefined_domain_terms ∧ missing_canonical_arch_terms
    3. "/gybis-arch-weed": architecture → specifications_only
       | active_spec_entities_reflected_or_refined_by_arch ∧ ¬spec_contradicts_arch_principle
    4. "/gybis-spec-weed": specifications → implementation_and_tests_only
       | obligation_test_coverage ∧ targeted_diagnostics ∧ no_heap_probes ∧ runtime_behavior
       | ¬reopen(architecture_to_code_findings)
  | ∀ finding → decision_policy() → permitted_repair → owning_validation()
  | comparison_scope ≡ only_next_downstream_edge ¬all_downstream_artifacts

λ decision_policy(finding).
  precedence ≔ requirements > vocabulary > architecture > specifications > tests > implementation
  | apply_in_order:
    1. downstream_contradicts_upstream_behavior → correct(downstream)
    2. upstream_mechanically_stale(spelling, identity, inventory, renamed_symbol, footer, snapshot,
                                  deferred_marker_contradicted_by_active_contract) → correct(upstream)
    3. prefer(PascalCase_canonical_term)
       | snake_case_free_function ≡ case_folded_variant ¬competing_term
    4. minimal_identity_preserving_edits ∧ ¬renumber_designators ∧ ¬delete_durable_artifact
       | split_or_merge_clauses only_if(owning_policy_requires)
    5. new_contract ∨ genuine_design_choice → UNRESOLVED(evidence)
    6. ready_stage_coverage_gap ∧ existing_upstream_contract → propagate(downstream)
       | ¬existing_contract → UNRESOLVED(evidence)
    7. deferred_work_stays_deferred unless(upstream_already_declares_active)

λ owning_validation(changed).
  req → "make requirements-structure" ∧ rerun(requirements_check)
  | vocab → "make vocabulary-structure" ∧ rerun(vocabulary_check)
  | arch → rerun(architecture_check) ∧ verify(S5→S4→S3→S2→S1 ∧ section_integrity)
  | spec → allium_check_and_analyse(changed_files)
    | cross_spec_concern → check_and_analyse(full_set)
    | synchronized(spec_obligations ∧ test_TRACE_IDs)
      → refresh_snapshot_if_needed("make traceability-spec-to-code-update-snapshot")
      → "make traceability-spec-to-code"
  | tests → build_and_run(focused_target) | changed_TRACE_IDs → invalidate(strict_trace)
  | executable_public_headers → "make lint" ∧ "make complexity-cli" ∧ generated_outputs()
      ∧ "make docs-examples"
  | Doxygen_only → scoped_format_twice ∧ generated_outputs() ∧ example_compile_link_run
      ∧ "make no-heap-src"
  | tests/no_heap/ → "make no-heap"
  | compile_fail_inputs → matching("make *-compile-fail")
  | checker_scripts → affected({"make diagnostic-coverage", "make diagnostic-message-labels", audit_target})
  | passing ∧ current(result) → reuse ¬rerun | widen only_if(required_by_changed_inputs)

λ evidence_reconciliation().
  ∀ implemented_public_API_with_resource_or_example_contract:
    inspect(actual_probe_bodies ∧ source_example) even_if(unchanged ∨ diagnostic_only ∨ gates_green)
  | table_columns ≔ {API_clause_obligation, ordinary_assertion_or_rejection_test,
                     runtime_no_heap_probe_assertion, Doxygen_demonstration, validation_status}
  | derive(final_req_vocab_arch_active_spec) ¬existing_test_inventory
  | cover(admitted_domains, results, applicable_empty_missing_full_capacity_boundaries,
          conversions, input_preservation, resources)
  | nonapplicable → reason | deficient → permitted_repair ∨ UNRESOLVED(evidence)
  | unchanged only_if(case_mapping) | exists ∨ TRACE_ID ∨ gate_green ≠ coverage
  | no_heap_evidence() ∧ example_evidence() | upstream_repair → reopen(affected_rows)

λ no_heap_evidence().
  upsert(existing_tests/no_heap/target_or_family_probe) ∧ ¬parallel_harness
  | absent → add(current_naming ∧ register(probes.hpp, harness_main.cpp, build_targets))
  | changed_admitted_behavior → direct_runtime_API_call ∧ result_assertions
  | representative_families ∧ boundaries ∧ required_input_preservation
  | prevent_folding_if_needed(volatile → plain_local → API(const_T_reference))
  | rejected_without_runtime_counterpart → explain ∧ compile_fail
  | noexcept ∨ constexpr ∨ scans ∨ ordinary_tests ≠ runtime_probe_coverage
  | final_sources_and_amalgamation → "make no-heap"
  | require(source_scan ∧ symbol_checks ∧ successful_execution(modular ∧ single_header))

λ example_evidence().
  repair(owning_source_Doxygen) ∧ match(final_contract) ∧ representative ¬exhaustive
  | C++23: '#include "cljonic.hpp"' → file_scope("using namespace cljonic;") → "main()"
  | public_free_functions ∧ named_values ∧ plain_literals ¬unnecessary_U_suffix
  | prefer(CTAD) if(deduction_expresses(intended_type ∧ intended_capacity))
    | explicit_template_arguments if(deduction_unavailable ∨ demonstrating(specific_type ∨ specific_capacity))
  | ¬internal_member_calls ∧ concise_user_prose ∧ link(conversion_rules) ∧ term("default value")
  | constexpr_results → static_assert | runtime_derived_inputs → checked_results
  | normal ∧ applicable_boundary_fallback_prefix ∧ required_input_preservation
  | runtime_failure → nonzero_exit | unused_result ∨ unconditional_success ≠ proof
  | bare_fences("~~~~~{.cpp}", "~~~~~") ∧ ¬asterisk_prefix_inside_sample
  | block_before(public_declaration) ∧ preserve(legacy_mainpage_narrative)
  | comment_sensitive_scan → avoid(forbidden_spellings ∨ prose_token("new "))
  | format_twice(scoped_clang_format ∧ "scripts/format-doc-samples.pl <header>")
    → second_pass_changes_nothing ∧ intact_bullets_fences
  | authorized_promotion → verify(cheatsheet ∧ umbrella ∧ extraction_registry)
  | safe_final_amalgamation → "make docs-examples"
    → verify(compiled_count ∧ exclusions ∧ target_freshly_extracted_and_compiled)
  | extracted ≔ configured_build_dir/docs-examples/target.cpp | default_build_dir ≔ build/
  | ¬stale_or_hand_copied_substitute
  | link(exact_extracted_program, repo_C++23_settings) → execute → require(exit=0)
  | compile_only("-c") ≠ link_run_proof

λ generated_outputs().
  before(any_generator): inspect(status ∧ diff, every_output)
  | ambiguous_preexisting_edits → UNRESOLVED(path, evidence) ∧ ¬overwrite ∧ continue
  | final_source_formatting → "make cljonic" overwrites(cljonic.hpp)
  | "make docs-examples" also_invokes("make cljonic")
  | default: defer(docs/ HTML, user_final_gate) ∧ report(deferred ¬rendered_current)
  | explicit_user_request(generation ∨ rendered_verification) ∧ safe_outputs → "make docs"
    | warning: removes(docs/) before regenerate
  | no_generated_current_claim unless(regenerated_from_final_sources)

λ integration().
  after_final_edit → "make upsert-gate-fast" regardless(change_type)
  | preserve(focused_change_specific_checks ∧ owning_validation)
  | ¬automatic_full_test_suite ∧ ¬recreate_strict_gate_by_chaining_checks
  | reuse(current_focused_check_or_fast_gate_evidence) ¬duplicate unless(later_edit_invalidates)
  | ¬run("make git")

λ invalidation(changed).
  req → requirements_check ∧ all_requirements_downstream_comparisons
  | vocab → vocabulary_check ∧ all_vocabulary_downstream_comparisons
  | arch → architecture_check ∧ architecture_spec_comparison
  | spec → spec_check ∧ Allium ∧ snapshot_alignment ∧ spec_code_comparison
  | tests ∨ implementation → tests ∧ spec_code_comparison
  | executable_public_header → modular_single_header_tests ∧ compile_fail ∧ no_heap
      ∧ amalgamation ∧ docs_examples
  | Doxygen_only → formatting ∧ amalgamation ∧ example_extract_compile_link_run ∧ no_heap_src ∧ HTML
  | upstream_contract_changed → affected_probe_example_mappings even_if(unchanged_surface)
  | generated_current only_if(final_sources_match)
  | explicitly_deferred_HTML ¬blocks(source_example_convergence)

λ termination().
  converged ≡ no_edits_since_last_relevant_pass ∧ all_required_checks_current_and_passing
              ∧ evidence_table_satisfied ∧ unresolved_set_empty
  | finding_outside_authority → record_evidence ∧ continue(other_permitted_work)
  | exhausted(permitted_repairs) ∧ (failed_gate ∨ unresolved ∨ unavailable_required_validation)
      → status(BLOCKED) ∧ specific_artifact_evidence_and_next_owning_command ∧ finish
  | ¬loop_without_progress ∧ ¬unresolved_as_success
  | final_report ≔ checks_and_status ∧ divergences_found_repaired ∧ modified_paths ∧ gates
      ∧ probe_example_case_mappings_and_results ∧ UNRESOLVED_items ∧ HTML_deferral_if_any
      ∧ state("make git and commits not performed; user-owned")

λ ledger().
  update_after(each_invocation ∨ edit) | concise ∧ copy_pasteable

```text
WORKFLOW=full-converge-auto
PHASE=
LAST_COMMAND_STATUS=
COMPLETED=
MODIFIED_PATHS=
CURRENT_VALIDATIONS=
NO_HEAP_EVIDENCE=
DOXYGEN_EXAMPLE_EVIDENCE=
INVALIDATED=
UNRESOLVED=
NEXT_ACTION=
```
