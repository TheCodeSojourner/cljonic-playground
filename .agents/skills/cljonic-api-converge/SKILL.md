---
name: cljonic-api-converge
description: "Use for /cljonic-api-converge <function>."
argument-hint: "<function> [canonical API name] [targeted|full]"
user-invocable: true
disable-model-invocation: true
---

# Converge a cljonic Free-Function API

Use this skill only when the user explicitly invokes `/cljonic-api-converge`. The required argument is the free-function identifier, such as `count`; the optional canonical API name may be supplied, such as `Count`. The optional mode is `targeted` or `full`; default to `targeted`. Derive omitted API names from repository artifacts, and ask only if the target is ambiguous.

This skill coordinates the user's final cross-layer review. It does not perform the work owned by Gybis commands.

## Hard Boundaries

- Never create a Git commit, stage changes, or discard user changes.
- Never run `make git`, directly or through another command. The user owns that final gate.
- Never invoke `/gybis-*` commands or simulate their results. Present one exact command at a time and wait for the user to invoke it and provide its result.
- This coordinator is read-only: inspect files and worktree state, maintain a validation ledger in the conversation, and recommend next steps. Do not edit requirements, vocabulary, architecture, specs, tests, implementation, or generated artifacts.
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
4. Prefer the command output and concrete artifact evidence over memory or inferred success. If a Gybis result is missing or ambiguous, ask for its concise status, findings, and modified-file summary.
5. When resuming after a Gybis command or context loss, reread `git status --short` and inspect relevant diffs against the checkpoint. Never assume a command ran or a prior result is still current based only on conversation order.

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

Use `targeted` mode to validate the API's changed surface and relevant cross-layer edges, not unrelated repository content.

1. Recommend the owning check for each changed requirements, vocabulary, architecture, or spec artifact when no current passing result exists. `/gybis-req-check` itself probes downstream readiness, including vocabulary and architecture checks and the Allium gate; avoid duplicating those checks unless the user wants their detailed standalone reports or those artifacts changed afterward.
2. Treat `/gybis-spec-check` as potentially mutating: it may repair specs. Run it before relying on cross-layer convergence results, and record any changed spec files. If it changes specs after `/gybis-req-check` or another downstream-readiness check, invalidate that readiness result and rerun only the affected checks before weeding.
3. Recommend only weed passes whose source/target relationship is in scope for this API. For a change spanning all layers, use the full dependency order: requirements, vocabulary, architecture, then specs/code/tests. Explain that `req-weed` compares requirements with multiple downstream artifacts; it is not limited to requirements-to-vocabulary.
4. After each command, inspect or ask for the changed-file set. Do not repeat a passing check merely because a later command ran.

## Full Mode

Use `full` mode only when the user requests the complete final sweep. Present the user's full order one command at a time:

1. `/gybis-req-check`
2. `/gybis-vocab-check`
3. `/gybis-arch-check`
4. `/gybis-spec-check`
5. `/gybis-req-weed`
6. `/gybis-vocab-weed`
7. `/gybis-arch-weed`
8. `/gybis-spec-weed`

Explain before starting that this is not eight read-only checks: `spec-check` can repair specs, and the weed commands can modify overlapping layers. `/gybis-spec-weed` includes strict obligation-to-test coverage and runs the test suite. Do not run a duplicate full suite immediately afterward unless later edits invalidate that result.

Before starting step 5, compare the worktree with the state before step 1. If `spec-check` or another check repaired an artifact, mark every earlier result that depended on the changed file as invalid. Rerun only those checks before weeding; in particular, rerun `/gybis-req-check` if its downstream-readiness result predates spec repairs and that result is needed for coverage convergence. Do not begin a weed pass while a required owning check is failing or stale.

## Human-Controlled Handoff

- Give the user the next exact slash command and its reason, then pause. Do not continue as though it ran until the user invokes it and returns its result.
- At each pause, include the compact resume checkpoint defined in Startup and Ledger so the workflow can resume after another skill is invoked or context is compacted.
- Gybis weed commands may request human resolution choices. The user, not this coordinator, decides the direction. Record `investigate` or `skip` as unresolved rather than calling the sweep converged.
- After each result, compare the current worktree with the ledger. A command that changed files invalidates only checks or cross-layer comparisons whose inputs include those files.

## Invalidation and Stop Rules

- A changed requirements artifact invalidates its requirements check and comparisons from requirements to downstream layers.
- A changed vocabulary artifact invalidates its vocabulary check and comparisons from vocabulary downstream.
- A changed architecture artifact invalidates its architecture check and architecture-to-spec/code comparisons.
- Changed specs invalidate their spec check, Allium/traceability results, and comparisons involving specs.
- Changed tests or implementation invalidate test results and spec-to-code comparisons. Changes under `tests/no_heap/` invalidate `make no-heap`; changes to compile-fail test inputs invalidate their corresponding compile-fail gate. Changed `TRACE_ID`s or specs invalidate strict traceability against the obligation snapshot.
- Changed public source headers invalidate relevant modular and single-header tests, compile-fail checks, no-heap results, the amalgamated header, and `make docs-examples`. Changes to public API declarations/signatures or Doxygen comments invalidate generated docs. Generated outputs are current only when regenerated from the final source version.
- After all requested passes, request only the owning checks invalidated by final edits. Reuse a Gybis pass's built-in conditional validation when its report explicitly covers the final file versions.
- Stop when no new files have changed since the last relevant pass, all required current checks pass, and no unresolved divergence remains. Otherwise identify the specific invalidated edge and recommend only the next needed command.
- Finish by summarizing current passing evidence, unresolved items, and the exact user-owned next action. State that `make git` and any commit were not performed; do not run either.