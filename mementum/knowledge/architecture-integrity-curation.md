---
type: Reference
title: Architecture Integrity Curation
status: active
category: architecture
tags: [architecture, vsm, validation, coherence]
related: [architecture.md, vocabulary.md]
depends-on: []
---

When evolving `architecture.md`, treat integrity checks as architecture-shaping feedback, not formatting noise.

Working loop:
1. Ensure each referenced primitive is defined in-file and invoked explicitly.
2. Replace abstract guardrails with measurable thresholds and deterministic enforcement outcomes.
3. Remove S1 placeholders and choose explicit operational defaults when decisions are open.
4. Re-run architecture validation and close only when findings are fully cleared.

This loop preserves VSM intent while improving standalone interpretability for future sessions and downstream specification work.