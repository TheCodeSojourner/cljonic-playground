---
type: Reference
title: header-addition-and-verification-lifecycle
status: active
tags: [lifecycle, quality-gates, coverage, doxygen, no-heap, allium]
related:
  - /mementum/knowledge/ai-upsert-quality-loop.md
  - /mementum/knowledge/verification-signal-discipline.md
---

# Header Addition and Verification Lifecycle

This document synthesizes the mandatory multi-layer discipline for introducing any new C++ header (`src/cljonic-*.hpp`) into the `cljonic` codebase.

## Required Order and Integration Checklist

The required development order is:

```
requirements -> vocabulary -> architecture -> specs -> tests -> implementation -> documentation
```

Every header added to `src/` must satisfy all five checkpoints below; their
numbering is an integration checklist, not a different dependency order.

### 1. Specification (`specs/` or `specs/primitives/`)
- Each free function or collection API must have a dedicated Allium specification file (`{name}.allium`).
- Invariants must declare all zero-heap, no-rtti, no-exceptions, and functional immutability contracts.
- Specifications must validate with 0 errors and 0 warnings via `allium check`.
- Traceability snapshots must be synchronized via `make traceability-spec-to-code-update-snapshot`.

### 2. Header Implementation & Doxygen Documentation (`src/`)
- Each API must live in its own dedicated header (`src/cljonic-{name}.hpp`).
- Included in the public header aggregator `src/cljonic-core.hpp`.
- Single-header artifact `cljonic.hpp` must be regenerated via `make cljonic`.
- Every header must feature complete Doxygen documentation with a runnable `main()` example program containing both:
  - **Compile-time demonstration**: `constexpr` values verified via `static_assert(...)`.
  - **Runtime demonstration**: Non-constexpr instances, operations, and a return expression validating runtime results.
- For polymorphic free functions supporting multiple collection types (e.g., `Queue`, `Set`), the example must demonstrate usage for every supported collection.

### 3. Specification Tests (`tests/`)
- Dedicated test translation unit (`tests/cljonic-{name}-spec-tests.cpp`) registered in `CMakeLists.txt`.
- Every specification obligation must map to an explicit `TRACE_ID("...")` macro call.
- Must include both compile-time checks (`STATIC_REQUIRE`) and runtime assertions (`REQUIRE`/`CHECK`).
- **Volatile Input Discipline**: To defeat GCC compile-time constant folding and force genuine `gcov` runtime execution, inputs must originate from `volatile` locals copied into plain locals (`volatile int v_raw = 42; int v = v_raw;`) to satisfy `const T&` qualifiers while guaranteeing line instrumentation.

### 4. Coverage Report Verification (`build-coverage/`)
- Every header in `src/` must appear as a distinct source file (`SF:`) in `coverage-src.info` and the `make coverage` HTML report.
- `lcov --summary` 100% reports must be corroborated against the total count of source headers to avoid empty-source masking.

### 5. Dedicated No-Heap Probes (`tests/no_heap/`)
- Every header must have a dedicated probe file (`tests/no_heap/cljonic-{name}-probes.cpp`).
- Probes must be declared in `tests/no_heap/probes.hpp` and invoked in `tests/no_heap/harness_main.cpp`.
- Must pass `make no-heap` with zero forbidden allocator symbols in both modular and single-header probe binaries.
- Both probe binaries must actually execute successfully. A symbol scan alone
  cannot establish probe behavior. The target now executes both binaries after
  scanning; enabling execution exposed a stale unequal-sequence expectation
  that a build-only check had never exercised.

## Promoting a Deferred Header to Public API

A header that already exists under `DEFERRED_NON_PUBLIC_HEADERS` promotes through
the same layers, but two registries fail only at the gates: `make lint` rejects an
unnamed parameter (`readability-named-parameter`), and `make docs-examples` keeps
skipping the example until the name leaves `DEFERRED_NON_PUBLIC_HEADERS`. Beyond
that, promotion requires adding the umbrella include in `src/cljonic-core.hpp`
(the amalgamation follows that include graph), adding the `dependent_false`
rejection overload, reclassifying `check-free-function-diagnostics.py` from
`excluded` to `fallback`, adding primitive compile-fail cases, a no-heap probe, an
active spec plus traced tests, and moving the operation out of the deferred set in
requirements, architecture, and the vocabulary term definition. Refresh the
obligation snapshot and regenerate `cljonic.hpp` and `docs/` last. Verified by
promoting `empty` on 2026-10-17.
