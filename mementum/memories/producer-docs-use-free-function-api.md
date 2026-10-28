---
type: Pattern
symbol: 🔁
title: producer-docs-use-free-function-api
---
Producer Doxygen examples, public prose, and behavioral tests should demonstrate the free-function workflow and observable behavior, not implementation-facing member details such as `count()` or `is_finite()`. Prefer free functions such as `count`, `is_empty`, `fits_into`, and `into` when asserting producer behavior. Keep direct member usage only where the public API specifically requires documenting or testing that member. This keeps producer documentation and examples aligned with cljonic's free-function-first public surface as new producer families are introduced.