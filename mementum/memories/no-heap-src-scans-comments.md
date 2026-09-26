---
type: Pattern
symbol: 🔁
title: no-heap-src-scans-comments
related: [no-heap-probes-propagate-with-feature.md, no-heap-symbol-scan-gate.md]
---
The `no-heap-src` source scanner is textual, not semantic: a comment inside `src/*.hpp` that mentions forbidden container or allocator names (e.g. "std::vector, std::span, std::string, std::map") trips the gate even though no code allocates. Confirmed 2026-09-26 when `src/cljonic-equal.hpp` documented the rejected interop types in prose. Write comments in src headers without spelling the forbidden names — describe the categories instead ("standard-library range and container types"). Verify with `make no-heap-src` before assuming a code problem.