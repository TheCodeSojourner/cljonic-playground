---
title: Mementum Synthesis
status: active
category: synthesis
tags: [synthesis, architecture, specs, verification, docs, workflow]
related: [mementum/state.md, architecture.md, Makefile, vocabulary.md]
depends-on: []
---

This synthesis distills the current durable operating model across mementum memories and knowledge.

1. Authority chain and artifact boundaries
- Architecture is authoritative and must remain in architecture.md.
- Architecture should not be mirrored as specs/architecture because that breaks the chain architecture > spec > tests > code.
- Instruction metadata and project architecture must stay separate.

2. Architecture curation loop
- Define referenced primitives in-file and invoke them explicitly.
- Replace placeholders and abstract gates with measurable thresholds and explicit failure behavior.
- Re-run checks until architecture findings are fully closed.

3. Verification and quality discipline
- Canonical inner loop is make upsert-gate; default full validation without docs is make upsert-gate-strict.
- Use make git only when docs generation is explicitly required in the final gate.
- Strict mode includes no-heap verification and spec-to-code traceability gates.
- Verification output should maximize durable signal and suppress low-value noise.

4. Traceability invariants
- Spec-to-code obligations must be synchronized with the committed snapshot.
- New requires clauses in Allium rules add new rule-failure obligations that require matching TRACE_ID test coverage.

5. Embedded runtime constraints
- No heap, no exceptions, fixed-capacity value semantics.
- No-heap enforcement is layered: source scan, probe build, symbol scan.

6. API and header organization
- Public API boundaries should be concept-first, with trait mechanics hidden in cljonic::concepts.
- Core operations belong in cljonic-core-function headers and are exposed via cljonic-core.hpp umbrella include.
- Header and test naming follows cljonic- prefixed dash-separated conventions.
- Constructor contracts should keep explicit compile-time diagnostics while remaining compliant with enforced complexity thresholds.

7. Documentation and examples
- Doxygen examples should be complete runnable programs with main().
- User-facing docs should be concise, contract-first, and avoid implementation-detail terms.
- Construction-focused examples should avoid surfacing capabilities reserved for standalone function docs.
- Use formatter-native wrapping unless formatting policy is intentionally changed.

8. Coverage and formatting workflow
- Coverage loop is line-based and should be scoped to the active production header.
- lcov extraction must be anchored to project src paths to avoid dependency contamination.
- Run make format before commit to reduce style-only diff noise.

9. Vocabulary governance
- vocabulary.md is for durable cross-layer language only.
- Canonical terms use PascalCase; incidental terms and implementation knobs stay out.

10. Memory routing policy
- Repo-specific memory and knowledge belong in mementum only.
- Copilot memory is reserved for cross-project personal preferences.

11. Requirements-to-bootstrap execution strategy
- `cljonic-requirements.md` currently describes a scope much larger than implemented vector/count bootstrap behavior.
- Safe execution favors incremental refinement of the existing nucleus before adding new capabilities.
- Vocabulary and architecture propagation should precede broader implementation expansion.

12. Documentation edit boundaries
- Legacy Doxygen mainpage and sample-program narrative blocks may be intentionally retained by maintainers.
- Do not rewrite those blocks during generic stale-comment cleanup unless explicitly requested.
