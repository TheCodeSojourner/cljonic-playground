---
type: Decision
symbol: 🎯
title: doxygen-prose-must-be-user-facing
---
Doxygen type/function descriptions (the prose before `\b Examples`) must describe what a user needs to know to *use* the type, not internal design rationale. Avoid: ownership-model wording ("owns its parameters", "does not own materialized result storage"), capability-concept jargon (`Indexed`, `IFn`, `CljonicProducer`), and negatively-phrased implementation notes ("exposes no canonical accessors"). Prefer plain, positive statements of behavior and how to use the free-function API, matching `Vector`'s doc style: what it is, its defaults, one or two key behavioral rules, and which free functions to call. Save the deeper capability-model reasoning (Indexed vs IFn vs Counted, ownership semantics) for architecture.md/vocabulary.md, not the public Doxygen comment.
