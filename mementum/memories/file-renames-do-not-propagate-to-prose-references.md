---
type: Mistake
symbol: ❌
title: file-renames-do-not-propagate-to-prose-references
related: [instruction-architecture-root-file.md, convergence-sweep-corrections.md]
---
Renaming files (e.g. `requirements/cljonic-requirements-module-N.md` →
`requirements/requirements-module-N.md`) does **not** update references
to those paths in prose files (`architecture.md`,
`mementum/knowledge/*.md`). No gate parses prose for path validity, so
the stale references survive silently until a coherence check catches
them.

Observed 2026-10-06: the requirements module files were renamed at
commit `2519ee5`, but `architecture.md` kept 10 references (2 in S2
rules, 7 in `traceability_authorities`, 1 more) and
`value-equality-domain.md` kept 1 (`depends-on:` frontmatter) — all
pointing at non-existent paths. Caught only by `/gybis-arch-check`'s
`referenced_components_defined_within_architecture` check.

Rule: after any file/directory rename, grep the whole repo (including
prose and YAML frontmatter) for the old path and update every
reference. Treat prose path references as unverified until grepped.
