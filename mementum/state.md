## Session State

- last_session_id: 2026-08-07T00:00:00Z
- current_timestamp: 2026-08-07T00:00:00Z
- recover: Run `make upsert-gate` after each source/test upsert, then run `make upsert-gate-strict` as the session-end checkpoint.

## Task

- Removed `specs/architecture/` category (5 allium files) and the `CoordinationProtocol` traceability stub from `tests/vector_spec_tests.cpp`.
- Identified that `specs/architecture/` was a design antipattern: architecture constrains specs; expressing architecture as allium specs breaks the authority chain and produces zombie traceability stubs with no behavioral content.
- Architecture now lives authoritatively in `architecture.md` only.

## Questions

- Does having `specs/architecture/*.allium` as a tooling target buy anything that `architecture.md` alone wouldn't provide? No — no skill does meaningful spec-vs-code conformance checking against architecture entities, and the traceability chain has no production-code leg.

## Decisions

- Delete `specs/architecture/` permanently; do not recreate via `gybis-arch-propagate` or any other means.
- `gybis-arch-propagate` output target is `specs/collections/` (and future feature domains), not an architecture category.

## Next

- Continue standalone `get` semantics work in `specs/collections`, then add minimal `assoc` and `conj` slices.
- Run dependency-aware/full-set spec checks before broader code propagation.
- Keep default mandatory complexity scope at `src`; use file-scoped `COMPLEXITY_PATH` only for local investigation.

