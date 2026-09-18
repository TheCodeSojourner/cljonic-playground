## Session State

- last_session_id: 1bf753d4-171d-42bc-a0da-4ba1c47e3ec7
- current_timestamp: 2026-09-18
- recover: 1
- session_complete: true

Task:
1. Guard the Doxygen Markdown formatting of `src/cljonic-core.hpp`'s `Core_Cheatsheet` so the shared formatter cannot collapse its headings, table rows, or list items.

Questions:
1. No blocking questions remain.

Decisions:
1. Scope enforcement only to `src/cljonic-core.hpp` and its `Core_Cheatsheet` block; do not impose a project-wide Doxygen Markdown rule.
2. `doc/mainpage-example.hpp` is a developer-only source-format fixture, excluded from Doxygen input; it is a formatting model rather than a content-equality oracle.
3. `scripts/format-doc-samples.pl` preserves structural Markdown only after `\anchor Core_Cheatsheet` in that header, while the new `core-cheatsheet-format` Make target validates the canonical source and a deliberately collapsed-heading negative case.
4. `make format-doc-samples` invokes the focused checker after it formats documentation comments, so the normal `make format`, `make lint`, and `make git` paths all enforce the invariant.

Validation:
1. `make core-cheatsheet-format`: PASS, including the embedded negative regression check.
2. `make format-doc-samples`: PASS, with the automatic focused guard passing.
3. `make docs`: PASS; generated `docs/index.html` contains a Markdown aliases table and distinct collection-list entries.
4. `make git`: PASS, including format, lint, complexity, compile-fail, sanitizers, 100% coverage, traceability, no-heap, docs, documentation examples, and tests.

Next:
1. Review and commit the non-`mementum/` implementation and documentation changes when ready; this finish step commits only `mementum/` state.
2. If continuing Module 4, implement remaining producer families (`Repeat`, `Cycle`, `Iterate`, `Repeatedly`) one at a time through requirements → vocabulary → architecture → specs → traceability → tests → source.
3. Extend `into`/`fits_into` destination support beyond `Vector` only after each destination's append/materialization semantics are explicitly specified.
4. Defer `nth` and any Range positional value retrieval until Module 5 is formally elicited and approved.
