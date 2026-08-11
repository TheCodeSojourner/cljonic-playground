# cljonic Build Target Strategy

**Decision:** Minimize doc generation by using granular make targets for intermediate checks; only run `make git` for final comprehensive validation.

**Why:** `make git` invokes Doxygen (expensive operation) as its final step. Running it multiple times during a session unnecessarily regenerates HTML docs.

**Available Granular Targets (no doc generation):**
- `make test` — Build + test cycle
- `make lint` — Format + clang-tidy checks
- `make no-heap-src` — Source-level heap-allocation checks
- `make no-heap-symbols` — Binary-level allocator symbol scan
- `make complexity-cli` — Complexity checks (quiet mode)
- `make coverage-cli` — Coverage check with percentage output
- `make sanitizer-cli` — ASan/UBSan run (quiet mode)
- `make traceability-spec-to-code` — Spec-to-code obligation validation

**Comprehensive Gate (with docs):**
- `make git` — format, lint, complexity-cli, sanitizer-cli, coverage-cli, traceability-spec-to-code, no-heap, **docs**

**Workflow:**
1. During development/iteration → use focused targets above
2. After all changes committed → run `make git` once for final validation
3. To explicitly regenerate docs → `make docs`

**Session lesson (2026-08-11):** Ran `make git` twice unnecessarily during BuildProfile/vocabulary cleanup. Should have used intermediate targets and final `make git` only.
