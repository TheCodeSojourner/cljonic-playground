---
type: Pattern
symbol: 🔁
title: no-heap-probes-propagate-with-feature
related: [no-heap-symbol-scan-gate.md, behavior-change-propagation-chain.md]
---
Every public free-function addition or behavior change must update or add a dedicated `tests/no_heap/` probe alongside ordinary tests, regardless of whether the function operates on producers, collections, scalars, or another domain. The no-heap harness is a separate contract surface, not a build side effect: place entity behavior in its family probe or free-function behavior in a `cljonic-core-{function}-probes.cpp` probe, cover both modular and single-header builds, then run `make no-heap` or a strict gate.

Behavior changes include constructor overload-set changes, not just free functions (confirmed 2026-09-26: the SameTypeArgumentIsOneElement slice initially shipped without a probe; the human flagged the gap). `no-heap-src`/`no-heap-symbols` only scan the implementation for heap calls — they cannot detect that new behavior is exercised. A behavior slice is not done until a probe covering it passes in both builds.
