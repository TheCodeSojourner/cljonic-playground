---
name: cljonic-api-complete
description: "Use for /cljonic-api-complete <function>."
argument-hint: "<function> [canonical API name]"
user-invocable: true
disable-model-invocation: true
---

# Complete a cljonic Free-Function API

Use this skill only when the user explicitly invokes `/cljonic-api-complete`. The required argument is the free-function identifier, such as `count`; the optional second argument is its canonical API name, such as `Count`. If omitted, derive the canonical name from nearby requirements, vocabulary, specs, and code. Ask only if the target remains ambiguous.

## Hard Boundaries

- Never create a Git commit.
- Never run `make git`, directly or through another command. The user owns that final gate.
- Do not stage, revert, or discard user changes. Inspect the worktree first and preserve unrelated or pre-existing edits.
- Do not edit vendored Gybis skills or invoke Gybis commands on your own initiative.
- Do not claim an API is complete when active requirements or obligations remain unmet. Keep explicitly deferred work identified as deferred.

## Startup

1. Read applicable repository instructions and inspect `git status` before editing.
2. Locate the target free function, its owning implementation, related requirements, vocabulary, architecture, spec, tests, documentation, and resource probes. Start from the closest concrete anchor; do not map unrelated areas.
3. Identify one falsifiable local hypothesis about the API gap and one cheap check that could disconfirm it. If evidence shows the implementation already behaves correctly, avoid unnecessary implementation changes.
4. Record the intended API domain and semantics from repository evidence. Do not copy another API's semantics by analogy.

## Layered Workflow

Treat one slash invocation as a multi-turn workflow. Complete only one layer per checkpoint. After its focused check passes, summarize the edits and result, then pause for human review. Continue to the next layer only after the user explicitly says to proceed. If the check fails, repair the same slice and rerun that check before widening scope. If the workflow resumes after context loss or without a reliable current layer, reconstruct it from the latest checkpoint and worktree; ask rather than infer when ambiguous.

1. **Requirements:** Upsert the existing requirement where possible. Define admitted inputs, observable results, errors or diagnostics, and relevant resource constraints. Preserve identifiers, provenance, and lifecycle decisions. Run the narrowest existing structural audit for the changed requirement or module.
2. **Vocabulary:** Add or refine the canonical term and its relationships. Check uniqueness, required fields, Related links, and frontmatter with the narrowest available audit.
3. **Architecture:** Add only the missing architectural rule at the owning abstraction. Check local consistency and the changed diff; do not repeat unrelated requirement or vocabulary audits unless their inputs changed.
4. **Specification:** Add explicit fields and invariants for the active contract. Do not mark work deferred to make validation pass. Run `allium check` on the changed spec; run broader analysis only when a cross-spec concern warrants it.
5. **Tests:** Test direct free-function calls for meaningful admitted domains and behaviors. Add rejected-domain and targeted-diagnostic coverage when required by repository policy, and add `TRACE_ID`s for new obligations. Build the relevant test target and run its focused test filter.
6. **Implementation, docs, and resource probes:** Fix the controlling implementation only when behavior is missing or wrong. Update the public Doxygen contract and runnable sample when needed. Extend suitable existing no-heap or other resource probes when the contract requires it. Validate changed surfaces with focused tests, `make docs-examples` for changed samples, or `make no-heap` for changed no-heap probes/contracts, as applicable.
7. **Generated artifacts and integration checks:** Before running any generator, inspect the status and diff of every output path. `make cljonic` overwrites `cljonic.hpp`; `make docs` removes `docs/` before regenerating it. If an output contains pre-existing or ambiguous user edits, stop and ask for approval before running the generator; never clean, restore, or overwrite user changes without approval. After source edits, regenerate `cljonic.hpp` with `make cljonic`; regenerate Doxygen output with `make docs` when a public API declaration/signature or Doxygen comment changed. Refresh the traceability snapshot only after spec obligations and test trace IDs are aligned, then run `make traceability-spec-to-code`. Run only the additional focused gates relevant to changed behavior. Do not run the final repository gate; report that `make git` remains for the user.

## Validation Policy

- For each edit, run the cheapest check that can falsify that edit, before further reading or editing.
- Do not rerun a passing check while its inputs are unchanged. A later edit invalidates only checks that depend on the changed files or generated outputs.
- Prefer the narrowest relevant checker, test target/filter, compile, or resource gate. Do not run the full test suite or broad audits at every layer merely for reassurance.
- Reserve cross-layer checks such as strict traceability for after the related spec, test IDs, and snapshot are synchronized.
- Run no final catch-all gate. In particular, never run `make git`; list it as a user-owned next step.

## C++23 Compatibility

- Treat the repository's `cxx_std_23` target in `CMakeLists.txt` as the language baseline. Use only C++23-or-earlier facilities supported by the configured toolchain; do not introduce post-C++23 features.
- Prefer established project idioms. Use a C++23 feature only when it solves a concrete API need, and validate it with the relevant project compile target.
- For standard-library facilities whose implementation support may vary, verify availability in the configured standard library; use a feature-test macro when useful, and compile against the actual project target.
- Preserve the API's documented constraints such as `constexpr`, `noexcept`, allocation behavior, and concept admission; do not trade them away for newer syntax.

## Handoff

At each layer boundary, report the files changed, the focused check and result, any unresolved or explicitly deferred obligation, and the next layer awaiting approval. Include this copy-pasteable resume checkpoint, filling each field with concise values:

```text
WORKFLOW=complete
API=
MODE_OR_LAYER=
LAST_COMMAND_STATUS=
COMPLETED=
MODIFIED_PATHS=
CURRENT_VALIDATIONS=
INVALIDATED=
OPEN_DECISIONS=
NEXT_ACTION=
```

On continuation, compare the checkpoint with `git status` and relevant diffs; do not repeat checks unless their inputs changed. If the checkpoint is missing or ambiguous, ask before resuming. At final handoff, summarize focused validations that passed and clearly state that `make git` and any commit were not performed and remain the user's responsibility.