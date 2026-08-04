# Restart prompt for next session

Use this prompt in a new chat session:

Continue the cljonic C++26 library design discussion using DESIGN-NOTES.md and NEXT-AGENDA.md in this repo as the source of truth.

Constraints already decided:
- Embedded target.
- No heap allocation.
- No exceptions.
- Data structures based on std::array.
- Immutability via value-style copy-on-modify semantics.
- No explicit error returns; invalid/missing access returns default sentinel `T{}`.
- Keywords are global-only (no namespaces).
- Keyword representation is enum catalog.
- Map keys may be non-keyword; if keyword keys are used they must be from the enum catalog.
- Small collection sizes; linear key/value search is acceptable.

Important working style:
- Discussion only unless I explicitly ask for code or file changes.
- Do not scaffold files or run builds unless asked.

Please start by proposing the top 3 design decisions to finalize next, with tradeoffs for each.
