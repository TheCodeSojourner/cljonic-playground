---
type: Reference
title: Artifact Boundary Discipline
status: active
description: Which durable claim belongs in which artifact — the witness test for spec entities, intentional aspirational scope in vocabulary/architecture, and the traceability gate as the active-surface boundary.
tags: [governance, specs, architecture, vocabulary, traceability, verification]
related:
  - /mementum/memories/spec-to-code-strict-fail-gate.md
  - /mementum/memories/vocab-arch-weed-skip-aspirational-scope.md
  - /mementum/memories/governance-invariants-do-not-belong-in-behavioral-specs.md
  - /mementum/knowledge/verification-signal-discipline.md
  - /mementum/knowledge/mementum-synthesis.md
depends-on: []
---

# Artifact Boundary Discipline

Synthesis of three memories (2026-09-26) that all answer one recurring question:
**which durable claim belongs in which artifact, and how do you tell?** Written
for future AI sessions — read this before adding a spec entity, scoping a weed
sweep, or reasoning about the traceability gate.

## The witness test for specifications

A spec entity or invariant earns its place only when some executable artifact
(concept, trait, function, storage behavior) can witness it. The strict
traceability gate (`make traceability-spec-to-code`) requires executable test
evidence for every active obligation, so a spec statement with no witness is not
merely noise — it is a gate failure waiting to happen.

Counter-example: `ApiLifecycleGate` (2026-09-26). Governance-process invariants
(CandidateStatus / DeferredStatus / ExcludedStatus / RequirementsBacked) were
encoded as an allium entity, generating seven plan obligations that no runtime
behavior could honestly satisfy, because the implemented surface legitimately
contains deferred-labeled functions (`seq`, `first`, `empty`, `not_empty`). The
entity was reverted. Governance vocabulary stays in architecture λ-rules and
`vocabulary.md`; its enforcement channel is the gate process itself, not a
runtime assertion.

## Aspirational scope is intentional in vocabulary and architecture

`vocabulary.md` and `architecture.md` deliberately describe scope beyond the
current implementation (Map/Set/Queue/String, Repeat/Cycle/Iterate/Repeatedly,
view/NonOwningView, Regex, ThreadingForm are documented before code exists). A
full "unused term" or "principle missing in code" weed sweep therefore surfaces
dozens of false-positive divergences. Default: scope `/gybis-vocab-weed` and
`/gybis-arch-weed` to already-implemented areas, or explicitly disposition the
aspirational groups as keep. Specifications are a different matter — they are
the active behavioral surface the gate enforces.

## The gate draws the active boundary

Strict traceability defines the active surface: set-scoped `allium check` +
`allium analyse`, a synchronized committed snapshot at
`spec-to-code-traceability/spec-to-code-obligation-ids.snapshot.txt`, and
TRACE_ID test coverage for every obligation in active, implementation-backed
specs. Specs marked `lifecycle: deferred` keep their behavioral contracts but sit
outside the strict active gate until implementation begins. Intentional
obligation changes require `make traceability-spec-to-code-update-snapshot`.

## One rule

Each artifact has exactly one enforcement channel: vocabulary/architecture →
review and weed; specs → traceability gate plus executable evidence;
memory/knowledge → recall. Place a claim where its channel can actually witness
it; otherwise the claim is drift in waiting.
