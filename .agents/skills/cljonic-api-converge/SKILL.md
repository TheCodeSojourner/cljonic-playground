---
name: cljonic-api-converge
description: "Use for /cljonic-api-converge <function>."
argument-hint: "<function> [canonical API name] [targeted|full]"
user-invocable: true
disable-model-invocation: true
---

# Converge a cljonic Free-Function API

Use this skill only when the user explicitly invokes `/cljonic-api-converge`. The required argument is the free-function identifier, such as `count`; the optional canonical API name may be supplied, such as `Count`. The optional mode is `targeted` or `full`; default to `targeted`. Derive omitted API names from repository artifacts, and ask only if the target is ambiguous.

This skill coordinates the user's final cross-layer review and automatically invokes the relevant Gybis skills through the skill tool. The user should not need to invoke those commands separately. The coordinator remains responsible for honoring any approval or decision gates raised by a Gybis skill.

## Hard Boundaries

- Never create a Git commit, stage changes, or discard user changes.
- Never run `make git`, directly or through another command. The user owns that final gate.
- Invoke each required Gybis skill with the skill tool; do not ask the user to invoke slash commands manually. Use only the matching check/weed skill and internal support skills it explicitly requires.
- This coordinator is read-only for product artifacts: inspect files and worktree state, maintain a validation ledger in the conversation, and dispatch Gybis skills. Do not directly edit requirements, vocabulary, architecture, specs, tests, implementation, docs, or generated artifacts. A Gybis skill may make its documented changes; record and validate them.
- Human decisions remain authoritative. If a Gybis skill requests a choice or approval, use the ask-user tool and wait for the answer before continuing. Never infer approval or resolve a substantive divergence autonomously.
- Preserve unrelated and pre-existing changes. Do not treat an unresolved, skipped, or investigated divergence as convergence.

## Startup and Ledger

1. Inspect `git status --short` and identify the API-related changed files. Do not attribute unrelated changes to this API.
2. Map changed artifacts to layers: requirements, vocabulary, architecture, specs, tests, implementation, docs, and generated outputs.
3. Maintain a compact ledger in the conversation with:
   - Each check/weed command and its reported status.
   - The files it read or validated when known.
   - Files changed after that result.
   - Which results are still current, invalidated, or unresolved.
   - A copy-pasteable resume checkpoint using the fixed format below.
4. Prefer Gybis skill output and concrete artifact evidence over memory or inferred success. If the skill result is missing or ambiguous, inspect its transcript/output if available; otherwise pause and ask for the concise status, findings, and modified-file summary.
5. After every Gybis invocation, inspect `git status --short` and relevant diffs, update the ledger, and invalidate only dependent checks.
6. When resuming after a Gybis invocation or context loss, reread `git status --short` and inspect relevant diffs against the checkpoint. Never assume a skill ran or a prior result is still current based only on conversation order.

Use this checkpoint format at every pause, with concise values:

```text
WORKFLOW=converge
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

## Targeted Mode

Use `targeted` mode to validate the API's changed surface and relevant cross-layer edges, not unrelated repository content. Invoke applicable Gybis skills automatically in this order, skipping only layers whose artifacts were not changed and whose current evidence remains valid:

1. `/gybis-req-check` when requirements changed or downstream readiness is not currently established. It probes vocabulary, architecture, and Allium readiness; avoid duplicating those checks unless detailed reports are needed or those artifacts change later.
2. `/gybis-spec-check` when an active spec changed or lacks a current check. It may repair specs. If it changes specs after requirements/downstream readiness checks, invalidate and rerun only affected checks before weeding.
3. Applicable weed passes for changed layer edges, in dependency order: `/gybis-req-weed`, `/gybis-vocab-weed`, `/gybis-arch-weed`, `/gybis-spec-weed`. Run only relevant passes. `req-weed` compares requirements with multiple downstream artifacts, not only vocabulary. `/gybis-spec-weed` includes strict obligation-to-test coverage and may run tests.
4. After every invocation, inspect `git status --short` and relevant diffs, record the reported result/files, and invalidate only checks whose inputs changed. Never mark unresolved, skipped, or investigated divergences as converged.
5. Continue automatically through non-interactive checks. If a Gybis skill requests a decision, ask the user one question using the ask-user tool, then resume automatically when answered. Do not ask the user to manually invoke a Gybis skill.

## Full Mode

Use `full` mode only when the user requests the complete final sweep. Automatically invoke this exact order, inspecting status/diffs and updating the ledger after each skill:

1. `/gybis-req-check`
2. `/gybis-vocab-check`
3. `/gybis-arch-check`
4. `/gybis-spec-check`
5. `/gybis-req-weed`
6. `/gybis-vocab-weed`
7. `/gybis-arch-weed`
8. `/gybis-spec-weed`

Before starting, explain that this is not eight read-only checks: `spec-check` can repair specs, and weed commands can modify overlapping layers. `/gybis-spec-weed` includes strict obligation-to-test coverage and runs the test suite. Stop for user decisions when requested, then resume automatically after receiving them. Do not run a duplicate full suite immediately afterward unless later edits invalidate that result.

Before starting step 5, compare the worktree with the state before step 1. If `spec-check` or another check repaired an artifact, mark every earlier result that depended on the changed file as invalid. Rerun only those checks before weeding; in particular, rerun `/gybis-req-check` if its downstream-readiness result predates spec repairs and that result is needed for coverage convergence. Do not begin a weed pass while a required owning check is failing or stale.

## Human Approval Gates

- Do not hand slash-command execution back to the user. Invoke Gybis skills automatically and proceed through non-interactive work.
- When a Gybis skill requires a substantive user choice or approval, ask through the ask-user tool and pause only for that decision. Include the compact resume checkpoint at such pauses so the workflow can resume without repeating valid checks.
- The user decides every weed resolution. Record `investigate` or `skip` as unresolved rather than calling the sweep converged.
- After each skill result, compare the current worktree with the ledger. A skill that changed files invalidates only checks or cross-layer comparisons whose inputs include those files.

## Invalidation and Stop Rules

- A changed requirements artifact invalidates its requirements check and comparisons from requirements to downstream layers.
- A changed vocabulary artifact invalidates its vocabulary check and comparisons from vocabulary downstream.
- A changed architecture artifact invalidates its architecture check and architecture-to-spec/code comparisons.
- Changed specs invalidate their spec check, Allium/traceability results, and comparisons involving specs.
- Changed tests or implementation invalidate test results and spec-to-code comparisons. Changes under `tests/no_heap/` invalidate `make no-heap`; changes to compile-fail test inputs invalidate their corresponding compile-fail gate. Changed `TRACE_ID`s or specs invalidate strict traceability against the obligation snapshot.
- Changed public source headers invalidate relevant modular and single-header tests, compile-fail checks, no-heap results, the amalgamated header, and `make docs-examples`. Changes to public API declarations/signatures or Doxygen comments invalidate generated docs. Generated outputs are current only when regenerated from the final source version.
- After all requested passes, request only the owning checks invalidated by final edits. Reuse a Gybis pass's built-in conditional validation when its report explicitly covers the final file versions.
- Stop when no new files have changed since the last relevant pass, all required current checks pass, and no unresolved divergence remains. Otherwise identify the specific invalidated edge and recommend only the next needed command.
- Finish by summarizing current passing evidence, unresolved items, and any user-owned next action. State that `make git` and any commit were not performed; do not run either.