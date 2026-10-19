---
name: cljonic-full-converge-auto
description: "Use for /cljonic-full-converge-auto: run every gybis layer check, then adjacent-layer weed passes, and repair divergences without approval pauses."
user-invocable: true
disable-model-invocation: true
---

# Full Repository Convergence (Autonomous)

Use this skill only when the user explicitly invokes `/cljonic-full-converge-auto`. It takes no arguments and covers the whole repository. When per-divergence human decisions are wanted, use `/cljonic-api-converge` instead.

This skill takes its structure from `/cljonic-api-converge` and changes three of its rules:

- Scope is the entire repository, not one free-function API.
- Each weed pass compares **only the next layer** downstream, not every downstream artifact.
- All approval and decision gates are pre-cleared: apply the recommended repair as soon as it arises.

It still obeys the repository's safety boundaries: no commits, no `make git`, no destructive edits, and no fabricated behavioral contracts.

## Autonomy Contract

Invoking this skill is a standing human decision for the duration of the run:

1. Run every check and weed pass listed below.
2. For each finding, apply the owning skill's recommended repair immediately.
3. Never call the ask-user tool or otherwise pause for approval, including when an owning Gybis skill would normally ask.

The pre-approval covers only repairs inside the owning skill's documented repair scope. It does **not** authorize:

- Changing normative meaning (a requirement's admitted domain, observable result, failure policy, or constraint).
- Adding a new behavioral contract or public API.
- Deleting a durable artifact (`requirements/`, `vocabulary.md`, `architecture.md`, `specs/`).
- Resolving a genuine conflict between two stakeholder-decided requirements.

When a finding needs one of those, record it as `UNRESOLVED` in the ledger, apply every non-conflicting repair, and continue. Never invent semantics to make a check pass, and never mark an unresolved item as converged.

## Hard Boundaries

- Never commit, stage, revert, or discard changes.
- Never run `make git`, directly or through another command.
- Preserve unrelated and pre-existing worktree edits; never attribute them to this run.
- Do not overwrite a generated artifact that contains ambiguous pre-existing edits; record it and continue.
- Do not modify vendored `.agents/skills/internal/**` files.
- Report at the end that `make git` and any commit remain user-owned.

## Startup

1. Run `git status --short` and record pre-existing changes as the baseline.
2. Map artifacts to layers: requirements, vocabulary, architecture, specs, tests, implementation, docs, generated outputs.
3. Start the ledger below and update it after every skill invocation and every edit.

## Phase 1 — Layer Checks (one layer at a time)

Invoke each Gybis skill with the skill tool; never hand slash-command execution back to the user. Do not start the next layer until the current one has reported.

Use the documented Make targets and Allium spellings above rather than probing `--help` or guessing command names.

1. Requirements — `/gybis-req-check`
2. Vocabulary — `/gybis-vocab-check`
3. Architecture — `/gybis-arch-check`
4. Specifications — `/gybis-spec-check`

Apply every recommended repair immediately (see Decision Policy). After a repair, rerun only the checks whose inputs changed. `spec-check` may edit specs; if it does, invalidate any earlier result that depended on those files.

Repository-native companions, run only when the corresponding layer changed:

- `make requirements-structure`
- `make vocabulary-structure`
- `allium check specs` and `allium analyse specs` (both must report zero diagnostics and zero findings; use `analyse`, not `analyze`)
- `make audit-scripts` after changing a checker script

## Phase 2 — Adjacent-Layer Weeds

Run exactly these four passes, in this order. Each compares one edge only.

1. `/gybis-req-weed` — requirements → **vocabulary only**. Restrict the comparison to canonical term use and undefined-term findings (`REQ_undefined_term`, `REQ_non_canonical_term`). Do not open requirement→architecture, →spec, or →test coverage findings in this pass.
2. `/gybis-vocab-weed` — vocabulary → **architecture only**. Check non-canonical spellings, undefined domain terms, and canonical terms the architecture should use.
3. `/gybis-arch-weed` — architecture → **specifications only**. Check that every active spec entity is reflected or refined by an architecture rule, and that no spec contradicts an architecture principle.
4. `/gybis-spec-weed` — specifications → **implementation and tests only**. Check obligation-to-test coverage, targeted diagnostics, no-heap probes, and runtime behavior. Do not reopen architecture→code findings; those belong to the previous edge.

For each divergence, resolve with the Decision Policy and apply the correction. Re-run the owning check for any artifact the correction changed.

## Decision Policy (replaces the question gates)

Durable truth outranks replaceable detail:

```text
requirements > vocabulary > architecture > specifications > tests > implementation
```

Apply these rules in order:

1. **Upstream wins on behavior.** If a downstream artifact contradicts an upstream contract, correct the downstream artifact.
2. **Mechanical staleness is corrected upstream.** If an upstream artifact is stale only in spelling, identity, or inventory (renamed symbol, canonical term, footer range, snapshot, deferred marker contradicted by a now-active contract), update the upstream artifact.
3. **Canonical vocabulary wins.** Prefer the PascalCase term. Treat a snake_case free-function spelling as the case-folded variant of that same term, not a competing term.
4. **Minimal, identity-preserving edits.** Never renumber designators, never delete a durable artifact, and never split or merge clauses unless the owning skill's policy requires it.
5. **No semantic invention.** If a finding needs a new behavioral contract or a genuine design choice, record `UNRESOLVED` with the evidence and continue.
6. **Coverage gaps at a ready stage** are repaired by propagating the upstream contract into the downstream artifact only when the contract already exists; otherwise record `UNRESOLVED`.
7. **Deferred work stays deferred.** Do not promote a deferred requirement or API merely to clear a finding unless an upstream artifact already declares it active.

## Verification After Edits

Validate each changed artifact with the narrowest owning check, then widen only when an input changed:

- Requirements → `make requirements-structure` and rerun the requirements check.
- Vocabulary → `make vocabulary-structure` and rerun the vocabulary check.
- Architecture → rerun the architecture check; confirm the S5→S1 order and section integrity.
- Specs → `allium check` and `allium analyse` on the changed file, then the full set when a cross-spec concern exists. Refresh the obligation snapshot only after spec obligations and test `TRACE_ID`s are aligned, then run `make traceability-spec-to-code`.
- Tests → build and run the focused test target. New or changed trace IDs invalidate strict traceability.
- Public headers or Doxygen comments → `make lint`, `make complexity`, `make cljonic`, `make docs-examples`, and `make docs`.
- `tests/no_heap/` → `make no-heap`.
- Compile-fail inputs → the matching `make *-compile-fail` target.
- Checker scripts → `make diagnostic-coverage`, `make diagnostic-message-labels`, or the affected audit target.

After the final edit, run one integration gate sized to the change:

- Any code, header, test, or probe change → `make test` and `make upsert-gate-strict`.
- Documentation-, requirements-, vocabulary-, or architecture-only changes → `make upsert-gate-fast`.

Do not rerun a passing check while its inputs are unchanged, and do not run a duplicate full suite immediately afterward unless a later edit invalidated that result.

## Invalidation Rules

- Changed requirements invalidate the requirements check and every requirements→downstream comparison.
- Changed vocabulary invalidates the vocabulary check and every vocabulary→downstream comparison.
- Changed architecture invalidates the architecture check and the architecture→specification comparison.
- Changed specs invalidate the spec check, Allium results, the obligation snapshot, and spec→code comparisons.
- Changed tests or implementation invalidate test results and spec→code comparisons.
- Changed public headers invalidate modular and single-header tests, compile-fail gates, no-heap, `cljonic.hpp`, and `docs-examples`; changed declarations or Doxygen comments also invalidate `docs/`.
- Generated outputs are current only when regenerated from the final source version.

## Termination

Stop when all of the following hold:

- No file has changed since the last relevant pass.
- Every required current check passes.
- The ledger contains no unresolved divergence and no `UNRESOLVED` item left without evidence.

If a gate fails after all permitted repairs, record the failing artifact and the specific next owning command, then finish with status `BLOCKED` rather than looping.

Finish by reporting: checks run with status, divergences found and repaired, files changed, gates run, any `UNRESOLVED` items, and the user-owned `make git` step.

## Ledger

Update this ledger after every invocation and edit. Use concise values.

```text
WORKFLOW=full-converge-auto
PHASE=
LAST_COMMAND_STATUS=
COMPLETED=
MODIFIED_PATHS=
CURRENT_VALIDATIONS=
INVALIDATED=
UNRESOLVED=
NEXT_ACTION=
```

When resuming after context loss, reread `git status --short` and the relevant diffs, and treat no earlier result as current until the worktree matches its inputs.
