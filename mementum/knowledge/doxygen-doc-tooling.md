---
type: Reference
title: Doxygen Doc Tooling
status: active
description: The toolchain that formats, checks, compiles, and generates cljonic's Doxygen docs — script responsibilities, comment-structure gates, example rules, and regeneration discipline.
tags: [documentation, doxygen, tooling, formatting, examples, verification]
related:
  - /mementum/memories/doxygen-reflow-must-respect-list-markers.md
  - /mementum/memories/doc-sample-extraction-requires-unindented-fence.md
  - /mementum/memories/doxygen-legacy-blocks-require-explicit-change.md
  - /mementum/memories/docs-examples-must-be-runnable-programs.md
  - /mementum/memories/defer-doc-regeneration-until-final-git.md
  - /mementum/memories/doxygen-html-cheatsheet-structure.md
  - /mementum/memories/doxygen-examples-use-free-functions.md
  - /mementum/memories/doxygen-prose-must-be-user-facing.md
  - /mementum/memories/docs-user-facing-example-style.md
  - /mementum/memories/cljonic-doxygen-layout.md
  - /mementum/memories/user-facing-apis-require-mainpage-index-entry.md
  - /mementum/knowledge/artifact-boundary-discipline.md
depends-on: []
---

# Doxygen Doc Tooling

Operating model for the scripts and gates that guard cljonic's public docs.

## Source vs. generated

- `doc/` is committed source: `Doxyfile`, `doc-logo.png`, `mainpage-example.hpp`.
- `docs/` is committed generated HTML served from GitHub Pages; `docs/index.html`
  is the entry point.
- `make docs` runs `rm -rf docs/ && cd doc && doxygen Doxyfile` — the clear-first
  step prevents stale pages.
- The Doxygen target is standalone and deliberately **not** in
  `upsert-gate-strict` (too slow for the tight loop). `make git` is where docs
  generation belongs.

## Formatting: two responsibilities, two scripts

- `scripts/format-doc-samples.pl` is the **only** tool that reflows Doxygen
  prose — `clang-format` treats `/** */` text as opaque and never rewraps it.
  Comment layout correctness therefore lives here, not in `make format`.
  - A list marker line (`^[ \t]*\*\s?-\s`) must be a paragraph boundary: flush
    the current paragraph and start a new one. Without this, consecutive
    one-line bullets fuse into a single item and Doxygen renders the merged text
    (invisible in C++, visible only in HTML).
  - After editing any `/** */` block, run `make format` **twice** and diff:
    reflow defects are only visible once the formatter has run, and idempotency
    is the check.
- `scripts/check-core-cheatsheet-format.py` (`make core-cheatsheet-format`)
  guards the `Core_Cheatsheet` block in `src/cljonic-core.hpp`. Its Markdown is
  intentionally multiline — tables need header/separator/every row on their own
  lines and lists need every bullet on its own line — because collapsing them
  breaks HTML rendering. `doc/mainpage-example.hpp` is the source-format
  fixture for that block only.

## Example compilation

- `scripts/compile-doc-samples.py` (`make docs-examples`) compiles every doc
  C++ sample against the generated `cljonic.hpp`, not per-header.
- Extraction matches a **bare** open fence `^[ \t]*~~~~~\{\.cpp\}[ \t]*$`; an
  asterisk-prefixed fence (`* ~~~~~{.cpp}`) is silently skipped, and the
  extractor does not strip `* ` prefixes, so a block must be fully un-prefixed to
  compile. The `Equal`/`Vector` style is the reference.
- `docs-examples:ok` alone is not proof: confirm the `docs-examples:compiled=N`
  count when adding/editing a block.
- Headers documenting free functions that are deliberately not yet in the public
  API (`empty`, `first`, `next`, `not-empty`, `rest`, `seq`) are listed in
  `DEFERRED_NON_PUBLIC_HEADERS` and reported as
  `docs-examples:deferred-skipped=N`, with a `\note Not yet part of the public
  API` in each header. **Never re-introduce a silent skip** — the skip must stay
  visible.

## Example content rules

- Every example is a complete runnable program with a `main()`.
- `\b Examples` blocks interact with cljonic values only through value-level
  syntax and the free-function API, never `<instance>.<method>` calls
  (`x.count()`, `x.contains(k)`, `v.index()`). Member `.method` access exists to
  implement the free functions. Allowed: construction, the instance-call
  `operator()` where the type provides it (`values(0)` — required, not a member
  call), documented interop accessors (`view()`, `begin()`), and free functions
  in a free-function header.
- A **type's own** example (`Vector`, `Range`, `Variant`, `MapEntry`) shows only
  construction plus its own callable/interop surface and calls no free functions;
  each **function's own** header demonstrates that function with a few
  representative use cases. Split: types show how to construct, functions show
  how to use. Examples are illustrative, not exhaustive — a handful of cases that
  convey the basics suffice; do not enumerate every type, overload, or arity. A
  type whose free-function API is deferred (e.g. `cljonic::Variant`, Slice B)
  stays construction-only until that API exists; a particularly unfamiliar type
  (`Variant`) is still worth one explicit operand in a relevant function's
  example.
- Use `cljonic.hpp` in every user-facing example. Prefer named
  `const auto name = Type{...};`; mark intentionally unused variables
  `[[maybe_unused]]`, not `(void)var;`.
- Prose before `\b Examples` must be user-facing: what the type is, its
  defaults, one or two behavioral rules, and which free functions to call. Save
  capability-model/ownership reasoning (`Indexed`, `IFn`, `CljonicProducer`) for
  architecture.md / vocabulary.md. Terminology: describe the value a default
  construction yields as the **default value** (the cljonic `DefaultElement`
  term) — not "value-initialized", "default-constructed", or
  "default-initializable".

## Regeneration & preservation discipline

- Do **not** regenerate docs or run documentation-producing targets after every
  incremental edit; prefer focused validation that leaves `docs/` untouched. The
  final `make git` regenerates derived artifacts when the change is ready.
- Treat legacy user-valued narrative blocks (`\mainpage`, sample-program
  narratives) as protected: do not rewrite or prune them without an explicit
  instruction. Prefer nearby implementation-accuracy edits over broad churn.
- Adding a user-facing API requires its own source documentation **and** an
  explicit reference in the curated `\mainpage` cheatsheet; keep the existing mix
  of implemented and future/unresolved references; regenerate and verify the
  entry.
