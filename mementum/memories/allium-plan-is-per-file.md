---
type: Insight
symbol: 💡
title: allium-plan-is-per-file
related: [/mementum/memories/spec-to-code-strict-fail-gate.md]
---
`allium plan` (v3.5.3) requires a single `.allium` FILE, not a directory:
`allium plan specs/` exits 1 with stderr `specs/: Is a directory (os error 21)`.

To enumerate obligations: loop per file (sorted) and parse the concatenated
JSON envelopes — one object per command, key `obligations`, each with `id`,
`category`, `source_construct`, `source_span`. The Makefile target
`_traceability-obligation-ids-current` does exactly this loop and skips files
whose text matches `^-- lifecycle: deferred$`.

Contrast: `allium check <dir>` and `allium analyse <dir>` DO accept a directory
recursively; only `plan` is per-file. Current counts: 33 active specs → 895
obligations (= committed snapshot); 2 deferred specs (variant-api,
collection-shaping) excluded.
