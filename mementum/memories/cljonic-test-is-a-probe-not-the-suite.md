---
type: Mistake
symbol: ❌
title: cljonic-test-is-a-probe-not-the-suite
related: [use-upsert-gate-strict-for-full-checks-without-docs.md, coverage-measures-during-ai-upsert-loop.md, pipefail-masks-build-failures.md]
---
`make cljonic-test` is NOT the test suite. Its Makefile recipe compiles and
runs only `tests/cljonic-single-header-probe.cpp`:

    cljonic-test: cljonic tests/cljonic-single-header-probe.cpp
        $(CXX) ... tests/cljonic-single-header-probe.cpp -o ...

That probe does not include the Catch2 spec tests
(`tests/cljonic-*-spec-tests.cpp`). It can pass while those files fail to
compile — so `cljonic-test:ok` is NOT evidence the tests build.

Observed 2026-10-06: after changing `Queue::can_conj()` to
`can_conj(const T&)`, two stale nullary calls
(`.can_conj()` at `tests/cljonic-queue-spec-tests.cpp` lines 139/158)
went unnoticed through several `cljonic-test:ok` runs; the full suite
finally failed during `make git`'s `sanitizer-cli` step.

The full suite (190 tests) is built only by the CMake targets:
- `make sanitizer` (ASan+UBSan, `build-sanitizers`)
- `make coverage` (lcov, `build-coverage`)
- `make upsert-gate-strict` / `make git` (which invoke them)

Rule: after editing ANY `tests/*.cpp` or a collection member signature,
validate with `make sanitizer` or `make coverage` (or `upsert-gate-strict`),
never with `cljonic-test` alone. Grep for the old callsite spelling too.
