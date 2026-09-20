---
type: Pattern
symbol: 🔁
title: no-heap-probes-propagate-with-feature
related: [no-heap-symbol-scan-gate.md, behavior-change-propagation-chain.md]
---
Feature-family changes must update the dedicated `tests/no_heap/` probe alongside ordinary tests. The no-heap harness is a separate contract surface, not a build side effect: add new source/producer behavior to its family probe, then run `make no-heap` or a strict gate. For Cycle, producer-source behavior belongs in `cljonic-cycle-probes.cpp` for both modular and single-header probe builds.
