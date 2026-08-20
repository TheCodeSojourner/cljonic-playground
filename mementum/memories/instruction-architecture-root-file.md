---
type: Insight
symbol: 💡
title: instruction-architecture-root-file
---
Instruction architecture should keep stable instruction metadata separate from project-owned architecture.

- Keep architecture in root-level `ARCHITECTURE.md`.
- Keep stable/replaceable instruction preamble in dedicated instruction files.
- Avoid embedding architectural content inside loader metadata files.

This preserves discoverability, reduces duplication risk, and keeps regeneration boundaries clear between tool-managed instruction files and project-owned system design files.