🔄 gybis-fini: Quality gates must precede git commit

**Issue:** In session 2026-08-11-mixed-collection-return-types-clarification, quality gates were run AFTER the git commit. This is backwards.

**Correct sequence:**
1. Make changes (modifications to files)
2. **Run quality gates** (`make upsert-gate-strict`) — VALIDATION BARRIER
3. **Only if gates pass** → `git add && git commit`

**Why this matters:**
- Quality gates are a PRE-commit verification, not post-commit validation
- Broken code should never reach git history
- In team contexts, broken commits can be pulled before gates catch them
- Strict projects like cljonic (lint, complexity, sanitizer, coverage, traceability, no-heap all mandatory) require this discipline

**gybis-fini Phase 4 correction:**
The protocol states `git_preserves_all→git_add()→git_commit()`, but this should be guarded by successful gate completion. Gates must run before the commit, not after.

**Pattern for future sessions:**
```
Phase 1: Read state and identify work
Phase 2: Make changes
Phase 3: Update mementum/state.md
Phase 3b: RUN QUALITY GATES ← Insert here
Phase 4: git add && git commit (only if gates pass)
```

**Blocker handling:** If gates fail, treat as `strong_blocker` and report to user before committing.
