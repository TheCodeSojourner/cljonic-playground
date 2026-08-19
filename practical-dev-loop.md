# Practical Development Loop

This project develops incrementally through a specification-led version of test-driven development. The working sequence is:

**Vocabulary -> Architecture -> Specifications -> Tests -> Code**

Each layer constrains the layer below it. Code is the replaceable implementation detail; vocabulary, architecture, and behavioral specifications preserve the durable intent.

## 1. Observe the Need

Start with one concrete capability, defect, or unanswered design question. Keep the first slice small enough to describe with a few observable behaviors.

Before exploring broadly, recall the existing vocabulary, architecture, specifications, tests, and mementum records that may already govern the area.

## 2. Curate Vocabulary

Check whether the capability needs a durable cross-layer term in [vocabulary.md](vocabulary.md).

Add a term only when it is normative, reused across project layers, or ambiguous enough that future artifacts need one canonical name. Prefer canonical PascalCase terms so they can be reused consistently.

Do not add incidental implementation details, tool names, one-off examples, or configuration knobs to the vocabulary.

## 3. Refine Architecture

Confirm that the capability fits the existing architectural boundaries and authority order. Architecture should clarify:

- Which durable layer or capability is changing.
- What is in scope for this increment.
- Which existing contracts must remain stable.
- Which lifecycle, ownership, capacity, result, or API constraints apply.

Change architecture only when the capability changes a durable system boundary. Do not recreate architecture as behavioral specifications.

## 4. Write the Smallest Specification

Describe the behavior before implementing it. Define the smallest meaningful slice, including:

- Valid inputs and states.
- Observable results.
- Invariants that must always hold.
- Rejection behavior where applicable.
- Relationships to the canonical vocabulary.

The specification is the behavioral truth for the slice. It should preserve existing behavior unless a change is explicitly intended.

## 5. Add Specification Tests

Turn each specified behavior into focused executable evidence. Use both kinds of tests when appropriate:

- Compile-time tests for concepts, accepted and rejected types, and structural contracts.
- Runtime tests for observable values, state transitions, boundaries, and rejection behavior.

Cover the smallest useful cases first, then add empty, populated, boundary, and invalid cases required by the specification. Keep traceability identifiers aligned with specification obligations. A new specification requirement normally requires corresponding test evidence.

## 6. Implement the Minimum Code

Write only enough implementation to satisfy the current tests and specification. Do not add speculative APIs, abstractions, or generalized concepts.

Use standard C++ concepts and traits when they already express the requirement. Add a cljonic-specific concept only when it expresses a meaningful domain capability, checks a cljonic expression protocol, adds policy, or materially improves diagnostics.

Generalize after a second real use demonstrates a stable common contract. If a stronger guarantee is needed for one operation, prefer a narrower concept rather than silently tightening an existing public contract.

## 7. Run the Tight Feedback Loop

Within the current slice, use the familiar TDD cycle:

1. Write the smallest failing test for specified behavior.
2. Implement only enough code to satisfy it.
3. Run the narrow test.
4. Correct the local defect and rerun the same check.
5. Add the next behavior only after the current behavior is stable.

After the focused check passes, run the relevant broader quality gates for the repository, such as formatting, compilation, sanitizers, coverage, traceability, no-heap checks, and documentation checks.

## 8. Record Durable Knowledge

When the increment reveals a decision, mistake, reusable pattern, or unresolved question, record it in the repository's `mementum/` system. Update working state after significant changes so the next session can resume without reconstructing the reasoning.

A useful increment should leave the project more understandable, not merely more capable.

## Compact Checklist

- [ ] One concrete, narrow capability selected.
- [ ] Existing mementum and nearby artifacts recalled.
- [ ] Required vocabulary curated.
- [ ] Architectural fit and scope confirmed.
- [ ] Smallest behavioral specification written.
- [ ] Compile-time and/or runtime tests added.
- [ ] Traceability obligations covered.
- [ ] Minimum implementation added.
- [ ] Focused checks pass.
- [ ] Relevant quality gates pass.
- [ ] Decisions and lessons recorded in `mementum/`.
