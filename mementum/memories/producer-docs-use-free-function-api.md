---
type: Pattern
symbol: 🔁
title: producer-docs-use-free-function-api
---
Producer Doxygen examples and public prose should demonstrate the free-function workflow and observable behavior, not implementation-facing member details such as `count()` or `is_finite()`. Keep direct member usage only where the public API specifically requires documenting it. This keeps producer documentation aligned with cljonic's free-function-first public surface as new producer families are introduced.