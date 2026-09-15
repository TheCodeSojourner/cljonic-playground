---
type: Reference
title: Development order: requirements → vocabulary → architecture → specs → tests → implementation
status: active
---
This repository enforces a strict development order. The lifecycle is not optional and must be followed in sequence:

1. requirements
2. vocabulary
3. architecture
4. specs
5. tests
6. implementation

The architecture layer is not a license to edit source or tests directly. When architecture changes are the last approved step, the next action is to propagate them into the formal behavioral specs and traceability obligations before writing or modifying implementation or tests.

This rule exists to keep the project aligned with the GYBIS workflow, protect traceability, and prevent accidental source changes that bypass the documented approval chain.
