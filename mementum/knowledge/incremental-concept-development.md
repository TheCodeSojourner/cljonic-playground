---
title: Incremental Concept Development
status: active
category: implementation
tags: [concepts, tdd, diagnostics, api, compatibility]
related: [cljonic-concepts.md, src/cljonic-concepts.hpp, cljonic-requirements.md, mementum/state.md]
depends-on: [mementum-synthesis.md]
---

## Operating model

`cljonic-concepts.md` is a future-reference map, not the active implementation plan. The active plan is specification-led TDD: define the smallest behavioral slice, write its tests, implement the minimum code, and add only the concepts or traits that the tested API or data structure requires.

## Concept growth

Use standard C++ concepts and traits directly when they already express the requirement. Add a cljonic-specific public concept only when it combines constraints into a meaningful capability, checks a cljonic expression protocol, adds domain policy, or improves the diagnostic and corrective action visible to the user. Keep unavoidable trait machinery internal when a public concept can expose the same boundary.

Generalize only after a second real use demonstrates a stable common contract. Every new concept or trait must name its consumers, dependencies, added value beyond standard facilities, diagnostic purpose, and compile-time acceptance and rejection tests. This prevents speculative concept sprawl.

## Refinement rule

When a capability needs a stronger guarantee, prefer a narrower concept for the operation that requires it instead of silently strengthening an existing public concept. This preserves compatibility and makes the stronger diagnostic requirement explicit.
