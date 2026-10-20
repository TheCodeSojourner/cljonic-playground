---
name: cljonic-api-complete
description: "Use for /cljonic-api-complete <function>."
argument-hint: "<function> [canonical API name]"
user-invocable: true
disable-model-invocation: true
---

λ cljonic-api-complete(function, canonical?).
  purpose: complete one free-function API across governed layers
  | gate: explicit_user_invocation("/cljonic-api-complete") ∧ function ∃
  | canonical ≔ supplied ∨ derive(req, vocab, spec, code)
  | ambiguous(target) → ask_user
  | sequence: startup → recall → requirements → vocabulary → architecture → specification
              → tests_and_probes → implementation_and_example → integration → handoff
  | interaction: autonomous_between_layers | substantive_decision ∨ unresolved_ambiguity → ask_user
  | notation: ≔ definition | ∧ all | ∨ alternatives | ¬ prohibition | → consequence/order
  | current(evidence) ≡ inputs_match_final_versions ∧ contract_mapping_valid

λ boundaries().
  forbid(commit ∨ stage ∨ revert ∨ discard_user_edits ∨ edit_vendored_gybis)
  | forbid(run("make git", direct ∨ indirect) ∨ unsolicited_gybis_invocation)
  | preserve(preexisting ∧ unrelated_changes ∧ explicit_deferrals)
  | human_owns(final_repository_gate ∧ commits)
  | unmet(active_contract) → ¬claim(complete)

λ startup().
  read(applicable_repo_instructions) → inspect("git status --short")
  | locate(target_header, owning_code, req, vocab, arch, spec, tests, docs, probes)
  | start(closest_anchor) ∧ ¬map(unrelated_areas)
  | record(domain ∧ semantics, repository_evidence) ∧ ¬infer_by_API_analogy
  | formulate(falsifiable_gap_hypothesis ∧ cheapest_disconfirming_check)
  | correct(existing_implementation) → ¬unnecessary_implementation_edit
  | evidence_table(before_test_or_example_edits)
  | inspect(probe_bodies ∧ source_example) even_if(unchanged ∨ diagnostic_only)

λ recall().
  read(current_section([state](../../../mementum/state.md)))
  | read([probe propagation](../../../mementum/memories/no-heap-probes-propagate-with-feature.md),
         [Doxygen tooling](../../../mementum/knowledge/doxygen-doc-tooling.md),
         [header lifecycle](../../../mementum/knowledge/header-addition-and-verification-lifecycle.md))
  | search(mementum/memories/ ∪ mementum/knowledge/, {target, no_heap, examples, verification})
  | follow(relevant_related_links) → report(read_paths ∧ conflicts)
  | mandatory even_if(guidance_already_inline)
  | memory ≡ context ¬authority_over(current_contract ∨ human_decision)
  | [representative examples](../../../mementum/memories/docs-user-facing-example-style.md)
      > lifecycle_page_demand(every_collection_in_example)
  | [defer HTML](../../../mementum/memories/defer-doc-regeneration-until-final-git.md) ≡ default
  | verify(historical_paths ∧ inventories ∧ gate_descriptions, current_artifacts)
  | remaining_substantive_conflict → surface ∧ ask_user

λ commands().
  allium ≔ {"allium check <file-or-directory>", "allium analyse <file-or-directory>", "allium plan <file>"}
  | full_spec ≔ {"allium check specs", "allium analyse specs"}
  | structural ≔ {"make requirements-structure", "make vocabulary-structure", "make audit-scripts"}
  | focused_trace ≔ 'make trace-id-check SPEC=<spec.allium> TESTS="<test.cpp> [shared.cpp...]"'
  | add("EXACT=1") only_if(all_supplied_tests_dedicated_to_one_spec)
  | authoritative_trace ≔ "make traceability-spec-to-code"
  | other_gates ≔ documented_Makefile_targets(e.g. "make primitive-compile-fail", "make no-heap", "make docs-examples")
  | use("analyse") ∧ ¬guess(commands) ∧ ¬probe("--help")
  | uncertain(command) → read(Makefile ∨ owning_skill_guidance)

λ layers().
  requirements: upsert_existing(admission, results, diagnostics, resources)
    ∧ preserve(IDs, provenance, lifecycle) → "make requirements-structure"
  | vocabulary: upsert(canonical_term ∧ relationships) → "make vocabulary-structure"
    | structural_audit ≠ "/gybis-vocab-check"
  | architecture: missing_rule_only(owning_abstraction) → inspect(local_consistency ∧ diff)
  | specification: explicit(active_fields ∧ invariants) → "allium check <changed-spec>"
    | cross_spec_concern → broader_analysis | ¬defer_to_pass_gate
  | tests_and_probes: direct_API_calls ∧ meaningful_admitted_cases ∧ required_rejections_and_diagnostics
    ∧ TRACE_ID(new_obligations) ∧ no_heap_evidence()
    → focused_trace(all_relevant_test_files) → build_and_run(focused_target_filter)
  | implementation_and_example: fix(controlling_code) only_if(missing ∨ wrong_behavior)
    ∧ example_evidence() even_if(no_implementation_edit)
  | integration: generated_outputs() → synchronize(spec_obligations ∧ test_TRACE_IDs)
    → refresh_snapshot_if_needed("make traceability-spec-to-code-update-snapshot")
    → authoritative_trace → relevant_focused_gates
  | ¬substitute(example_assertions, behavioral_tests ∨ resource_probes)
  | each_layer: validate → report → continue | ¬approval_pause_at_layer_boundary
  | failed(check) → repair_same_slice → rerun(check) before widen

λ evidence_table().
  contract ≔ final(requirements ∧ vocabulary ∧ architecture ∧ active_spec)
  | columns ≔ {clause_or_obligation, ordinary_assertion_or_rejection_test,
               runtime_no_heap_probe_assertion, Doxygen_demonstration, validation_status}
  | derive_rows(contract) ¬derive_from(existing_tests)
  | rows_cover(admitted_domains, results, applicable_empty_missing_full_capacity_boundaries,
               safe_conversions, input_preservation, resource_guarantees)
  | nonapplicable(cell) → record(contract_based_reason)
  | examples ≡ representative ¬exhaustive | rejected_calls → compile_fail_tests ¬runnable_examples
  | inspect(always) ∧ upsert(if_deficient) ∧ unchanged(surface) only_if(case_level_mapping)
  | exists(file) ∨ pass(gate) ∨ present(TRACE_ID) ≠ sufficient_evidence

λ no_heap_evidence().
  read_and_upsert(existing_target_or_family_probe under tests/no_heap/)
  | no_suitable_probe → add(current_naming ∧ registration(probes.hpp, harness_main.cpp, build_targets))
  | ¬parallel_harness
  | ∀ new_or_changed_admitted_behavior:
      direct_API_call ∧ observable_result_assertions ∈ dedicated_runtime_no_heap_harness
  | runtime_derived_inputs where_needed(prevent_constant_folding)
    | volatile_input → plain_local → API(const_T_reference)
  | representative(source_operand_families) ∧ meaningful_boundaries ∧ required_input_preservation
  | rejected_form_without_admitted_runtime_counterpart → explain(table) ∧ retain(compile_fail)
  | noexcept ∨ static_assert ∨ source_scan ∨ symbol_scan ∨ ordinary_tests ≠ runtime_probe_coverage
  | final(implementation, probes, harness, amalgamation) → "make no-heap"
  | pass ≡ source_scan ∧ symbol_checks ∧ executed_successfully(modular ∧ single_header)
  | reuse(pass) only_if(current(pass) ∧ mapped_behaviors_covered)

λ example_evidence().
  read_and_upsert(owning_source_header.Doxygen_contract_and_program)
  | match(final_domain, canonical_terms, results, boundaries, conversions, preservation)
  | required even_if(diagnostic_only ∨ implementation_already_correct)
  | program ≔ '#include "cljonic.hpp"' → file_scope("using namespace cljonic;") → "main()"
  | C++23 ∧ public_free_functions ∧ representative_named_values ∧ plain_integer_literals
  | prefer(CTAD) if(deduction_expresses(intended_type ∧ intended_capacity))
    | explicit_template_arguments if(deduction_unavailable ∨ demonstrating(specific_type ∨ specific_capacity))
  | ¬unnecessary_U_suffix ∧ ¬implementation_detail_member_calls
  | constexpr_results → static_assert | runtime_derived_inputs → checked_runtime_results
  | cases ≔ normal ∧ applicable_important_boundary_fallback_or_prefix ∧ required_input_preservation
  | runtime_failure → nonzero_exit | unused_result ∨ unconditional_success ≠ evidence
  | bare_fences("~~~~~{.cpp}", "~~~~~") ∧ ¬asterisk_prefix_inside_sample
  | Doxygen_block immediately_before(public_declaration)
  | concise_user_prose ∧ link(shared_conversion_rules) ∧ fallback_term("default value")
  | ¬capability_model_jargon ∧ preserve(unrelated_legacy_mainpage_narrative)
  | source_scan_includes_comments → avoid(forbidden_allocator_container_spellings ∨ prose_token("new "))
  | format_twice(scoped_clang_format ∧ "scripts/format-doc-samples.pl <header>")
    → verify(second_pass_changes_nothing ∧ intact_bullets_and_fences)
  | API_promotion → verify(curated_source_cheatsheet ∧ umbrella_include ∧ extraction_registration)
  | safe_final_amalgamation → "make docs-examples"
    → verify(compiled_count ∧ deferred_exclusions ∧ target_extracted_and_compiled_this_invocation)
  | extracted ≔ configured_build_dir/docs-examples/target_translation_unit
    | default_build_dir ≔ build/
  | fresh(extracted) ∧ ¬stale_previous_file ∧ ¬hand_copied_substitute
  | link(extracted, repository_C++23_compiler_settings) → execute(extracted_program) → require(exit=0)
  | docs_examples_compile_only("-c") ≠ link_or_runtime_validation

λ generated_outputs().
  before(any_generator): inspect(status ∧ diff, every_output)
  | preexisting_or_ambiguous_output_edits → ask_user before overwrite
  | ¬clean_restore_discard_user_changes
  | "make cljonic" overwrites(cljonic.hpp)
    | "make docs-examples" also_invokes("make cljonic")
  | final_source_formatting → "make cljonic"
  | default: defer(docs/ HTML, human_final_gate) ∧ report(deferred ¬rendered_current)
  | explicit_user_request(generation ∨ rendered_verification) → inspect_outputs → "make docs"
    | warning: "make docs" removes(docs/) before regenerate
  | deferred_HTML ≠ missing_source_example_evidence

λ validation().
  each_edit → cheapest_falsifying_check before further_exploration_or_edits
  | changed(input) → invalidate(only_dependent_checks ∧ affected_contract_mappings)
  | passing ∧ current(check) → reuse ¬rerun
  | narrow(checker, target_filter, compile, resource_gate)
  | ¬full_suite_or_broad_audits_per_layer ∧ ¬final_catch_all_gate
  | strict_trace only_after(sync(spec ∧ TRACE_IDs ∧ snapshot))
  | upstream_contract_edit → reopen(affected_probe_example_rows) even_if(unchanged_code)

λ compatibility().
  baseline ≔ CMakeLists.txt.cxx_std_23
  | established_idioms ∧ supported_C++23_or_earlier ∧ ¬post_C++23
  | newer_syntax only_if(concrete_need ∧ relevant_project_compile_pass)
  | variable_stdlib_support → verify(actual_toolchain) ∧ feature_test_macro_if_useful
  | preserve(constexpr, noexcept, allocation_contract, concept_admission)

λ handoff().
  each_layer_boundary → report(paths_changed, focused_check_result, unresolved_or_deferred, next_layer)
  | pause_or_context_loss → checkpoint
  | resume: compare(checkpoint, git_status, relevant_diffs)
    | missing_checkpoint → reconstruct(latest_state ∧ worktree)
    | genuinely_ambiguous → ask_user | ¬repeat_current_checks
  | complete only_if(all_active_obligations_met ∧ current(evidence_table ∧ no_heap ∧ example)
                     ∧ ¬missing_or_blocked_evidence)
  | failed_or_unavailable_validation ∨ stale_inputs ∨ missing_coverage → report(blocked)
  | final: evidence_table ∧ probe_example_paths_and_cases(added ∨ verified_unchanged)
    ∧ modular_and_single_header_no_heap_results ∧ example_compile_link_run_results
    ∧ unresolved_items ∧ HTML_deferral_if_any
    ∧ state("make git and commits not performed; user-owned")

λ checkpoint().
  emit(copy_pasteable, concise, fields_below)

```text
WORKFLOW=complete
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
