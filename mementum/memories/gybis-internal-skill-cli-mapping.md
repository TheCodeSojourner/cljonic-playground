---
type: Insight
symbol: 💡
title: gybis-internal-skill-cli-mapping
---
Internal gybis skill names like `allium-check`, `allium-analyse`, and `allium-gate` are skill aliases, not shell executables.
Correct shell commands are `allium check`, `allium analyse`, etc. (space-separated subcommands).
Never run `command -v allium-check`; probe availability with `command -v allium` only.
`gybis-spec-check` prohibits `allium gate` and `allium rerun`; use `allium check` + `allium analyse` directly.
