---
type: Reference
title: cljonic Next Discussion Agenda
status: open
description: Current approval-gated agenda for propagating repaired requirements into specifications, architecture, tests, and implementation.
tags: [roadmap, requirements, specifications, architecture, collections, verification]
related: [collection-api-surface-discipline.md, architecture-integrity-curation.md]
depends-on: []
---

## Current baseline (2026-09-03)

The requirements and Module 7 architecture governed-requirements reference were repaired and validated. The requirements-only inventory contains 233 defined designators with no duplicate or undefined exact references. The worktree currently contains downstream implementation and test changes; treat those as pending work until the repaired requirements and architecture are approved.

## Approval gate

1. Validate and approve the repaired requirements and architecture reference.
2. Keep implementation changes subordinate to the approved requirements and architecture; do not use existing code or tests to silently redefine behavioral truth.

## Next work after approval

1. Reconcile the operation-level behavioral specifications required by `REQ-SEQ-022`.
2. Start with the collection-shaping and traversal inventory, naming each operation contract individually before implementation readiness is claimed.
3. Propagate approved contracts into architecture, focused tests, traceability, and implementation in that order.
4. Re-run the relevant vocabulary, architecture, specification, traceability, build, no-heap, and coverage gates after each coherent increment.

## Deferred until their contracts exist

Do not promote unresolved operation families, collection return/capacity policies, key/value constraints, or runtime-only behavior into implementation-ready status. Record each decision in its owning behavioral specification first, then propagate it downstream.

## Retired agenda material

The former keyword-catalog, initial map milestone, and broad collection design-question list described an earlier planning stage. They are retained only in git history, not as current work items.
