# Documentation Notes — AI-Native Single-Header Design

**Status:** Design agreed in conversation, not yet implemented.
**Date:** 2026-10-03
**Purpose:** Hand this file to a future AI session to restart and complete the work.

---

## How to Restart This Conversation

Paste this to a fresh AI session:

> Read `DOCUMENTATION_NOTES.md` in the repo root. It captures a design we agreed on for
> making the generated single header (`cljonic.hpp`) into a machine-only artifact that is
> optimized for two consumers: a C++ compiler and an AI. The plan is to (1) inject a
> high-density AI usage manual written in nucleus lambda as comments at the top of the
> header, and (2) strip the human-oriented C++ doc comments via the amalgamation script.
> Then implement it. Start by reading `scripts/amalgamate.py`, `scripts/compile-doc-samples.py`,
> and the `Makefile`.

---

## 1. The Core Decision

The generated single header `cljonic.hpp` is **not for human consumption**. It is intended to be:

- Readable by a **C++ compiler**
- Readable by an **AI** trying to use the library
- **Not** optimized for human readability

Humans read the modular headers in `src/`, the Doxygen docs, `README.md`, `vocabulary.md`, and
`architecture.md`. The single header is the machine-native distribution artifact.

This resolves the earlier objection: stripping comments is no longer destructive *if* the
information is transformed into the AI manual rather than deleted.

---

## 2. The Goal

Whenever somebody (an AI) is handed only `cljonic.hpp`, they should know **exactly what to do
with it** — without any external documentation. The header becomes self-contained and
self-teaching for a machine reader.

---

## 3. Verified Facts (Confirmed During Conversation)

These were checked against the actual repo and must not be re-litigated:

1. **`scripts/amalgamate.py` is a trivial include-expander.** It copies source lines verbatim
   and wraps them in `// Begin <file>` / `// End <file>` markers. It strips nothing today. It is
   a natural place to add comment transformation.

2. **`scripts/compile-doc-samples.py` reads `~~~~~{.cpp}` blocks from `src/*.hpp`** (via
   `source_dir.glob("*.hpp")`), NOT from the generated header. It rewrites the include to point
   at `cljonic.hpp` and compiles the sample *against* the generated header.
   - **Consequence:** stripping comments from `cljonic.hpp` does NOT break the `docs-examples`
     gate. The coupling is loose.

3. **Deferred headers** skipped by docs-examples (6): `cljonic-empty.hpp`, `cljonic-first.hpp`,
   `cljonic-next.hpp`, `cljonic-not-empty.hpp`, `cljonic-rest.hpp`, `cljonic-seq.hpp`.

4. **The gate `make git` currently passes fully** (validated 2026-10-03): format, lint,
   complexity, compile-fail checks, header-guards, sanitizer, 100% line coverage, traceability,
   no-heap, docs, docs-examples, single-header test.

5. **Verified drift bug:** `first`, `next`, `rest`, `seq`, `empty`, `not_empty` are *defined* in
   `src/*.hpp` but are **absent from the generated `cljonic.hpp`** (grep count 0 in generated, 1
   in src). Each carries a "Not yet part of the public API" note. This means "it compiles"
   depends on whether you consume modular headers or the single header — a real inconsistency.

6. **Verified aspirational-API bug:** `src/cljonic-core.hpp` lists ~80 "Core Functions" in its
   Doxygen cheatsheet (`Filter`, `Reduce`, `Take`, `Sort`, `Partition`, `Drop`, etc.), but only a
   subset is active. An AI reading the header would call non-existent functions.

---

## 4. Why This Design Is Right (Rationale)

- The repo already separates **canonical dense form** (requirements/architecture in nucleus
  lambda) from **human-rendered prose** (describe/explain skills). Making the distribution header
  machine-facing is the same philosophy extended to the artifact itself.
- It **structurally retires the two drift bugs** (items 5 and 6 above). If the manual's active
  surface manifest is *generated from the actual include graph*, the aspirational-cheatsheet and
  src-vs-header gap become impossible rather than merely unlikely.
- Self-contained: an AI handed only `cljonic.hpp` gets rules, active surface, and forbidden
  idioms with no external doc.
- Token economics: the manual is additive; it removes bulky Doxygen blocks and replaces them
  with denser records, so token cost goes *down* while useful signal goes *up*.

---

## 5. Three Non-Negotiable Conditions

### Condition 1 — Transform, do not delete
Strip a comment only **after** its content has been reduced into a manual contract record.
Load-bearing information must survive:
- `\note Not yet part of the public API` markers
- REQ ID references (e.g. `Per REQ-FN-026`)
- rationale that is not derivable from the code

Lossless-by-construction is the entire justification for stripping. Negotiable only if the
manual provably carries everything the comment carried.

### Condition 2 — Use a real C++ tokenizer, not a line matcher
The stripper must correctly handle:
- `//` and `/* */` comments, including backslash-newline continuation gotcha
  (a `//` comment swallows the next physical line)
- comment-like text inside string and char literals
- raw strings `R"delim(...)delim"`
- preprocessor lines

A naive regex will silently mangle a literal and ship a header that miscompiles in exactly one
corner case. Use a proper tokenizer or drive an existing tool. This is the real implementation
cost.

### Condition 3 — Gate drift like the traceability snapshot
Reuse the existing drift-gate pattern (`make traceability-spec-to-code`). Do not invent a new
mechanism. Required gates:
- **Freshness:** regenerate and diff against committed; mismatch fails the build.
- **Presence:** the manual block must exist (fenced).
- **Truth:** the manual's active-surface list must match the include graph.

---

## 6. Recommended Artifact Layout

- **Manual block location:** top of the generated header, **before includes**, so a prefix-read
  yields the rules. Fence it so tooling can extract just the manual:
  ```
  // cljonic-ai-manual:begin
  ...manual...
  // cljonic-ai-manual:end
  ```

- **Update the generator:** extend `scripts/amalgamate.py` with a style switch:
  - `human` → byte-for-byte today's output (keep for review/debug)
  - `ai` → transform doc comments into manual contract records (lossless), drop the originals,
    inject the manual block at the top

- **Emit two artifacts** (recommended): `cljonic.hpp` (ai style) and a human variant for review,
  OR a single `ai` header plus a standalone manual file.

- **Add `#line` directives** in the amalgamated code mapping back to `src/` files. Because the
  file is machine-only and comments are stripped, `#line` restores precise diagnostics and lets a
  dev map a compile error back to source without re-introducing prose. Recommended even though
  the file is machine-facing.

- **Keep navigation markers:** retain `// Begin <file>` / `// End <file>` or convert them to
  `#line` directives; they are load-bearing for navigation.

---

## 7. Manual Content Model (Priority Order)

The manual must carry what an AI cannot derive from signatures. Ranked by value:

1. **Active-surface manifest** (highest value) — generated from the include graph. Lists active
   vs deferred functions. This alone prevents the `filter`/`reduce` trap.
2. **Negative knowledge / forbidden idioms** — where an LLM loses to its std priors. Suppress
   priors with explicit prohibitions: `optional` on lookup, `throw`, `std::vector`,
   `std::variant`, assuming negative index is an error.
3. **Deltas** from both std C++ and Clojure. Canonical example: `Vector{span}` *copies* elements
   vs `Vector{Vector{...}}` *encloses* the inner vector as one element (SameTypeArgumentIsOneElement).
   Same syntax, opposite meaning.
4. **Contract records** — sentinel-vs-preflight semantics, cost model (copy-on-modify is
   O(capacity), not O(delta)), bounds/failure behavior.
5. **Canonical idioms** — 8–10 compiler-approved snippet closings for common tasks.
6. **Antipatterns** — observed AI mistakes with corrections, indexed by observed mistake.

The manual must be **generated from specs and the include graph, never hand-maintained**,
otherwise it becomes a third drifting source of truth.

### Manual Skeleton (nucleus lambda)
```text
λ cljonic_ai_manual(x). sections: S0_active | S1_invariants | S2_deltas | S3_contracts | S4_idioms | S5_antipatterns
  | S0_active: generated(set(public_api)) | ¬aspirational | deferred → listed_separately
  | S1_invariants: ∀ op(x) → enforce(no_heap ∧ no_exception ∧ no_rtti ∧ bounded ∧ copy_on_modify)
  | S1_forbidden: ❌optional(¬contract) ❌throw ❌std::vector ❌std::variant ❌neg_index_is_error
  | S2_deltas: differs_from(std ∧ clojure) → explicit | e.g. first(v) ≡ v(0) XOR MapEntry_key ∧ ¬seq
  | S3_contracts: fn → {sig, semantics, sentinel(¬exception), preflight, cost, source_construction}
  | S4_idioms: task → canonical_expr | e.g. materialize(producer,dest) ≡ into(dest,src) guard can_assoc
  | S5_antipatterns: erf(❌) → fix(✅) | indexed_by(observed_ai_mistake)
  | discipline: S0 ∧ S1 generated ∧ gated(make) | drift ≡ build_failure | ¬hand_maintained
```

### Sample Contract Record (target density reference)
```text
λ fn_assoc(x). assoc(coll,key,val) → coll' | pure | copy_on_modify | ¬mutate(coll)
  | preflight: can_assoc(coll,key) | sentinel: ¬applicable | cost: O(capacity)
  | parity: clojure_assoc | delta: vector_assoc ≡ index_overwrite ¬grow
```

Density reference: match the density of `REQ-PLAT-001` in `requirements/requirements-module-1.md`,
not denser.

---

## 8. Manual Sizing Estimate

- Invariant/predicate core (all platform/identity/forbidden rules): a few hundred tokens.
- Per-function contract records at ~25–40 tokens each × ~150 functions ≈ 6–10k tokens.
- Total manual fits comfortably in context alongside the code.

Token note: bundling the manual in the header buys **discovery**, not raw token savings (an AI
reads a range, not the whole file, and the full header is ~30–40k tokens). If raw token cost
matters, also emit a standalone small manual. Best answer: both, from one generated source.

---

## 9. Cautions

**Density has an optimum below maximum.** Nucleus lambda is out-of-distribution for an LLM past a
point; over-compression *increases* hallucination rather than reducing it. Keep records at the
`REQ-PLAT-001` level of density, not denser.

**Notation-specific manual is a per-model tax.** A manual tuned to nucleus lambda helps a capable
model a lot and may help a smaller model less. Accept the audience as "capable agents with the
specs loaded."

**Don't duplicate the header.** If a record's signature can be grepped from `src/`, the manual
should carry only the semantic deltas for that function.

**README claim must be updated.** `README.md` currently says the header is "intentionally
readable." That promise is being deliberately changed for the release artifact. Update the
wording to "machine-native / AI-facing."

---

## 10. Open Questions (Not Yet Decided)

1. **Manual source:** hand-written nucleus file, or fully generated from requirements/architecture
   + include graph? (Recommendation: generated, gated.)
2. **Stripper implementation:** custom tokenizer, or drive an existing tool (e.g. clang)?
   Decide before writing code.
3. **Artifact naming:** one `cljonic.hpp` in ai style only, or also a human variant somewhere?
   (Recommendation: ai release header + optional human variant for review.)
4. **`#line` directives:** required in first cut, or deferred?
5. **Freshness gate hook:** new `make` target, or folded into `make cljonic` / `make git`?
6. **Deferred functions:** should `first`/`next`/`rest`/`seq`/`empty`/`not_empty` remain src-only,
   or be removed from `src/` until promoted? (Eliminates the modular-vs-single-header discrepancy
   entirely.)

---

## 11. Files To Read First When Implementing

- `scripts/amalgamate.py` — generator to extend with style/transform.
- `scripts/compile-doc-samples.py` — confirms docs gate reads `src/`, not the header.
- `Makefile` — targets `cljonic`, `cljonic-test`, `docs-examples`, `no-heap`, gates.
- `README.md` — single-header release policy; "intentionally readable" claim to update.
- `src/cljonic-core.hpp` — the mainpage cheatsheet (aspirational list to replace).
- `src/cljonic-*.hpp` — doc comments to transform; `~~~~~{.cpp}` blocks to preserve as compiled
  samples (note: the generator harvests these from `src/`, so keep them there).
- `requirements/requirements-module-1.md` — density reference (`REQ-PLAT-001`).
- `requirements/requirements-index.md` — REQ ID conventions.
- `architecture.md` — S5/S3 invariants that become S1 manual content.
- `vocabulary.md` — canonical terms; the manual's vocabulary must match.
- `mementum/` — repo-specific memory; consult `mementum/state.md` for current status.

---

## 12. Definition of Done

1. `make cljonic` produces an AI-native header: fenced manual block at top, transformed
   (lossless) comments, valid C++.
2. A real tokenizer performs the strip; literals/raw strings/comments all handled.
3. Freshness, presence, and active-surface-truth gates pass and fail correctly when drifted.
4. `make cljonic-test`, `make no-heap`, `make docs-examples`, `make traceability-spec-to-code`,
   and full `make git` all pass against the new artifact.
5. `README.md` updated to state the release header is machine-native.
6. The active-surface manifest matches the include graph exactly; the aspirational cheatsheet and
   the src-vs-header deferred-function drift can no longer occur.
7. Both documented drift bugs (§3 items 5 and 6) are verified fixed.

---

## 13. Definition of Done — Verification Commands

```bash
# Confirm deferred functions are now handled consistently
grep -c "auto first(" cljonic.hpp   # expect: consistent with src policy

# Confirm manual block present
grep -n "cljonic-ai-manual:begin\|cljonic-ai-manual:end" cljonic.hpp

# Confirm no human doc comments remain in ai artifact
grep -c "anchor\|\\\\brief\|~~~~~" cljonic.hpp   # expect 0 (except manual)

# Full gate
make git
```
