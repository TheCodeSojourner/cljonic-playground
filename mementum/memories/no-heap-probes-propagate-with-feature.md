---
type: Pattern
symbol: 🔁
title: no-heap-probes-propagate-with-feature
related: [no-heap-symbol-scan-gate.md, behavior-change-propagation-chain.md]
---
Every public free-function addition or behavior change must update or add a dedicated `tests/no_heap/` probe alongside ordinary tests, regardless of whether the function operates on producers, collections, scalars, or another domain. The no-heap harness is a separate contract surface, not a build side effect: place entity behavior in its family probe or free-function behavior in a `cljonic-core-{function}-probes.cpp` probe, cover both modular and single-header builds, then run `make no-heap` or a strict gate.
