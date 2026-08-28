---
type: Insight
symbol: 💡
title: clang-format-config-for-ai-token-density
---
Reconfigured project formatting in a repo-root `.clang-format` file and upgraded clang-format 18 → 20 (Ubuntu 24.04 apt: `clang-format-20`, symlink `/usr/bin/clang-format`).

Active `.clang-format` keys chosen to favor AI/parser token density over human readability:
- `Standard: c++20` (was `Cpp11`) — accurate grammar for concept/requires parsing.
- `BreakBeforeBraces: Attach` (was `Allman`) — drops vertical waste, ~15–25% fewer lines.
- `BinPackArguments/Parameters: true` (was false) — packs within 120 cols.
- `SortIncludes: CaseSensitive` (was false) — deterministic ordering, less drift/merge noise.
- `RequiresClausePosition: OwnLine` + `IndentRequiresClause: true` — predictable concept formatting.
- Kept: `ColumnLimit: 120`, `IndentWidth: 4`, `PointerAlignment: Left`, `SpaceBeforeParens: ControlStatements`, `AllowShort*: false`, `AlwaysBreakTemplateDeclarations: true`.

Both `make format` (raw C/C++) and `make format-doc-samples` (Doxygen example blocks via `scripts/format-doc-samples.pl`) invoke `clang-format` with no `-style` flag, so both auto-detect and share the root `.clang-format`. Verified: clang-format 20.1.2, `make format:ok`, `make lint:ok`, `make test` passes.