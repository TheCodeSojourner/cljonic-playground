---
type: Decision
symbol: 🎯
title: producer-parameter-equality-single-surface
related: [/mementum/knowledge/value-equality-domain.md]
---
`parameters_equal` was removed as a public producer-comparison free function
(2026-10-16). Producer parameter equality now has exactly one surface: the
native `operator==`. `REQ-FN-014B` requires a producer to provide *only* `==`,
and no separately named comparison free function.

The alias delegated straight to `==` for `Range`, `Repeat`, and `Cycle`, and
had no production callers — only tests, docs, and governance. The durable
reason to keep it was the naming distinction: an explicit name separates
stored-parameter equality from the `equal` free function, which compares
bounded logical observations of produced sequences. That distinction now
lives on each producer's `operator==` documentation instead.

Removal must cross every layer in durability order. The easily missed tail is
governance: the traceability snapshot regenerates; obligations orphaned by
deleting a dedicated spec test must be re-homed into surviving spec-test
blocks; and both the diagnostic compile-check pass-case list and the no-heap
harness registration name the function. A deleted API's no-heap probe is
deleted, not rewritten.
