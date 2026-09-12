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

## The 5-Point Lifecycle Contract

Every header added to `src/` must complete all five stages before being considered integrated:

```
+-------------------+      +--------------------+      +--------------------+
| 1. Specification  | ---> | 2. Header & Docs   | ---> | 3. Tests           |
| (specs/*.allium)  |      | (src/*.hpp)        |      | (tests/*-tests.cpp)|
+-------------------+      +--------------------+      +--------------------+
                                                                  |
                                                                  v
+-------------------+      +--------------------+      +--------------------+
| 5. Quality Gates  | <--- | 5. No-Heap Probe   | <--- | 4. Full Coverage   |
| (upsert-gate-*)   |      | (tests/no_heap/*)  |      | (100% SF visibility|
+-------------------+      +--------------------+      +--------------------+
```

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
