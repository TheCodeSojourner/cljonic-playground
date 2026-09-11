## Session State

- last_session_id: 2026-09-11-gybis-fini-recovery
- current_timestamp: 2026-09-11
- recover: 1
- session_complete: true

Task:
1. Implement const C++ interoperability for Vector, Map, Set, Queue, and String.
2. Preserve bounded, non-allocating, read-only semantics and collection-specific logical order.

Questions:
1. No user questions unresolved.

Decisions:
1. C++ interoperability is implemented as const range-compatible traversal plus collection-owned `.view()` members where contiguous views exist; `seq`, `first`, `next`, `rest`, `into`, and `fits_into` remain outside this increment.
2. Vector, Map, Set, and String expose contiguous read-only views; String excludes its null terminator.
3. Queue exposes logical FIFO traversal for every state and does not provide a `view`; this avoids a partial optional-span API and never copies or normalizes storage.
4. Queue traversal and all views are const, non-owning, non-allocating, non-throwing, and constexpr-capable where supported.
5. The interoperability Allium contract is recorded in `specs/collections/cpp-interoperability.allium` and its obligations are covered by modular and single-header tests.
6. Intentional C-array literal deduction, checked array indexing, project macros, test macro expansions, test complexity, test static initialization, and internal enum sizing are suppressed as non-actionable for this project.
7. Architecture now treats read-only interoperability and const traversal as active S3/S1 policy; Queue remains traversal-only without a contiguous view.
8. The architecture current-scope summary uses canonical vocabulary terms directly; detailed interoperability policy remains centralized in S3 and operational guidance in S1.
9. The Vector documentation example demonstrates both const range iteration and the non-owning `view(Vector)` span; documentation examples compile successfully.
10. C++ interoperability uses collection-owned `.view()` members where contiguous views exist; the redundant free-function `view(...)` forwarding layer was removed, while Queue remains `begin()`/`end()` only.
11. The String documentation example demonstrates content-only const range iteration and `String::view()` as a non-owning `std::string_view` excluding the null terminator.
12. The Set documentation example demonstrates const range iteration and `Set::view()` while explicitly preserving unordered traversal semantics.
13. The Queue documentation example demonstrates constexpr and runtime FIFO iteration through `begin()`/`end()` after circular storage wraps; it does not imply a contiguous view.
14. Queue's const iterator now supports logical `operator[]` indexing, making wrapped FIFO examples clearer while retaining forward-range semantics.
15. Collection Doxygen examples use starless fenced code blocks for copy/paste ergonomics; Queue was normalized to match Vector, String, and Set.
16. The Map documentation example demonstrates const `MapEntry` range traversal and `Map::view()` member interoperability.
17. Direct source construction remains a total bounded operation: compile-time-known static-span overflow is rejected, runtime-sized dynamic-span overflow materializes a bounded prefix.
18. Complete-fit source semantics belong to the preflight/materialization pair: `fits_into` reports whether the complete source fits, while `into` performs bounded-prefix materialization when it does not; no checked constructor is planned.
2. `WarningsAsErrors: '*'` is enabled after the curated clang-tidy set reached a clean baseline.
3. Low-risk lint fixes removed redundant `typename`, simplified preprocessor conditions, concatenated an empty nested namespace, and named previously unnamed parameters.
4. `ReflowComments` remains enabled in the repository policy, but the custom Doxygen pass owns prose wrapping so clang-format does not insert malformed blank lines or concatenate comment markers.
5. Doxygen prose is wrapped to the 120-character column limit; fenced C++ examples continue through clang-format with their comment prefixes preserved.
6. All collection traversal (`seq`, `first`, `next`, `rest`) and C++ interoperability/range access remain deferred to a subsequent phase.
7. `is_empty` is the current boolean predicate; `empty` is a deferred value-producing operation corresponding to Clojure `empty`; `not_empty` is a deferred value-producing operation corresponding to Clojure `not-empty`.
8. `String::append` is an unrequired implementation detail and was removed with its tests and documentation example; `conj` does not apply to String.
9. `empty` and `not_empty` were removed from the active umbrella, specs, tests, probes, and traceability snapshot while their future semantics remain in requirements and architecture.
10. Documentation generation remains a publishing-step concern; the final `make git` checkpoint has now completed successfully.

Validation:
1. Focused interoperability tests pass in modular and single-header suites.
2. `make validate` passes: format, lint, complexity, sanitizer, 100% coverage, strict traceability, and no-heap checks.
3. `make cljonic` regenerates the public single header successfully.
4. Existing String-focused behavior remains validated; the prior `cljonic.hpp` change was regenerated as part of this increment.
5. Architecture structure and interoperability coherence checks pass.
6. Canonical vocabulary references replace interoperability shorthand in the architecture summary.
7. `git diff --check` passes.

Current Increment:
1. The collection C++ interoperability increment is implemented, specified, tested, traceable, documented with copy-paste-friendly examples, and generated into the public header.
2. The lint-policy increment is implemented and validated.
3. The String increment is implemented, specified, tested, traceable, documented, and published.
4. Semantic traversal (`seq`, `first`, `next`, `rest`) remains deferred for all collections.
5. `empty` and `not_empty` remain deferred value-producing operations; `is_empty` remains the active boolean predicate.
6. The active vocabulary and API surface retain the requirements-first boundary: construction, observation, lookup, mutation-copy, capacity, and C++ interoperability are active; unresolved future operation families remain out of scope.

Next:
1. Generalize and approve the source construction/materialization contract across collections, including `std::string_view` and compatible span/range sources, with the Vector constructor policy as the baseline.
2. Keep the clang-tidy suppression list narrow; newly introduced unsuppressed diagnostics now fail `make lint`.
3. Design and approve the all-collection semantic traversal increment before implementing `seq`/`first`/`next`/`rest` behavior.
4. Design and approve the deferred `empty`/`not_empty` increment before restoring their specs, tests, and public umbrella exposure.
5. Re-run the relevant requirements, architecture, vocabulary, specification, test, traceability, no-heap, and publication gates after the next approved increment.
