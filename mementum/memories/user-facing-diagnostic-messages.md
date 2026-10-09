---
type: Decision
symbol: 🎯
title: user-facing-diagnostic-messages
related: [/mementum/memories/rejection-diagnostic-message-anchor.md, /mementum/memories/doxygen-prose-must-be-user-facing.md]
---
Diagnostic-fallback messages (the `static_assert` rejection text in `src/`) use
plain user-facing language, not capability-concept or implementation jargon.
Approved by human 2026-10-09.

Avoid: "...domain" (equality/producer/association/conj domain), "admissible",
capability names (Conjable, Associative), concept names
(NothrowCollectionElement, CljonicSource), "element type", "value_type",
"lookup type", "construction source", "nothrow-storable".

Prefer: name the operation (`cljonic::fn:`), say plainly what is rejected
("this value cannot be added with conj", "cannot be compared for equality"),
name concrete collection kinds, and state the correct usage (`use into`,
`use conj`, `write X{...}`).

Codified at REQ-DIAG-009 and architecture `S3_rejection_diagnostic`
(`use(user_facing_language(x)) ∧ ¬use(capability_concept_jargon(x))`).
REQ-DIAG-010 already required "user-facing terms" for constructors.

Applied repo-wide to every user-facing fallback message (19 headers); harness
constants updated in lockstep because they pin message content. Anchors that
changed: "outside the supported equality domain" → "cannot be compared for
equality"/"inequality"; "outside the supported producer domain" → per-function
phrases; "is not a construction source" → "you cannot construct a". `docs/` HTML
regenerates at `make git`, not per edit.
