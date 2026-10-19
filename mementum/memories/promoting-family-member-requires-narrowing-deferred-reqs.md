---
type: Pattern
symbol: 🔁
title: promoting-family-member-requires-narrowing-deferred-reqs
related:
  - /mementum/memories/deferred-api-promotion-checklist.md
  - /mementum/knowledge/header-addition-and-verification-lifecycle.md
---
Promoting one operation out of a deferred family leaves family requirements
still naming it. Promoting `first` on 2026-10-19 exposed this in REQ-SEQ-002,
003, 004, 005, 006, 007, and 009. The deferred section's blockquote was not
enough; `/cljonic-api-converge` flagged the clauses as
`deferred_REQ_has_downstream_content`. Human decision: narrow the deferred
clauses to the remaining operations (`seq`, `next`, `rest`) and keep the
promoted operation under its own binding requirement. Run the converge skill
after every promotion from a deferred family.
