# cljonic next discussion agenda

## Locked decisions

- Canonical deep comparison API names are `equal`, `less`, `less_equal`, `greater`, `greater_equal`.
- Comparison APIs are variadic with chained semantics.
- Zero-arity and one-arity comparison calls return `true` (Clojure-like behavior).
- `equal` performs deep nested value comparison.
- Floating-point comparisons are forbidden by default; canonical comparison APIs reject them at compile time.
- Dynamic value type excludes floating-point entirely.
- Any float comparison override must be explicit per call through comparator-based APIs; implicit override is disallowed.
- The same constructor pattern applies to vector, set, map, and string: literal-deduced and explicit-capacity forms are both supported.
- Explicit-capacity empty construction is valid.
- Explicit-capacity construction is a compile-time error when initializer count exceeds capacity.
- Lazy sequence behavior is the default where practical, including range, repeat, and cycle.
- Lazy views are value-owning, not borrowing-only.
- Canonical `interleave` is lazy and shortest-terminating for Clojure parity.
- Range is lazy by default, with explicit finite or infinite cardinality metadata.
- Infinite range does not use max size_t as canonical size.
- Range precomputes required internal fields at compile time when inputs are compile-time constants, otherwise once at runtime.
- Range with step zero normalizes to an empty finite range.
- CTRE is the selected compile-time regex library (optional profile).
- Runtime regex profile is also supported as opt-in for `re-pattern` and `re-matcher`.
- Invalid runtime pattern compilation returns an invalid pattern sentinel under no-error policy.
- `pattern_valid(p)` is available as an explicit probe.
- High-fit namespace roadmap is locked with explicit MVP and optional cut lines.
- C++26 usage guidelines are locked for contracts, concepts, functional style, and ARM optimization profiles.
- Packaging model is locked as header-only with one-function-per-file production headers and automated single-header amalgamation.
- Quality and verification toolchain is locked: Catch2, generators, sanitizer profiles, linting gates, Doxygen with tested samples, automatic HTML generation, MVP 100% line coverage policy, and layered no-heap verification.
- Key and value type model for map and set is locked for MVP: keyword enum, integral, enum, and fixed-capacity static string keys; user-defined key types are optional via explicit non-MVP concept gates.
- Sentinel ambiguity policy is locked: `get`-style access remains sentinel-only (`T{}`), and probe-first usage (`contains`/`has_index`) is required guidance for ambiguous domains.
- Core collection API style is locked as free-function-first.
- Full-capacity `assoc` and `conj` behavior is locked as deterministic unchanged return.
- Comparator override direction is locked as explicit per-call `_with` APIs (for example `equal_with`).
- Composition mechanism is locked: `thread_first` and `thread_last` are adopted for Clojure-style readable composition.
- `thread_first` threads prior result into first argument position; `thread_last` threads prior result into last argument position.
- `as_thread` is locked as a Clojure-style rebinding form where each step consumes the current value directly.
- `cond_thread_first` and `cond_thread_last` are locked as conditional threading forms.
- `some`-style threading variants are deferred pending explicit validity contracts that avoid sentinel-ambiguity pitfalls.
- Threaded sequence transforms remain lazy by default and materialization remains explicit at sink operations.

### Canonical API matrix source

- The single canonical MVP API matrix source of truth is in DESIGN-NOTES.md under "Locked MVP API matrix (summary)".
- NEXT-AGENDA.md tracks lock status and remaining decisions, and intentionally avoids duplicating matrix-level semantics.

## Locked namespace scope

### MVP

- clojure.string
- clojure.set
- clojure.data
- clojure.walk
- clojure.zip

### Optional

- clojure.edn
- clojure.math
- clojure.uuid
- clojure.instant
- clojure.xml
- clojure.core.reducers
- clojure.parallel

## Priority order

1. Map internal layout details
- Confirm linear search and insertion-order storage for small N.
- Decide duplicate-key update semantics (replace in place in returned copy).
- Decide removal compaction behavior and ordering guarantees.

2. Keyword enum catalog details
- Establish keyword naming conventions (global-only, no namespace separators).
- Decide compile-time literal mapping API and diagnostics style.
- Define runtime text-to-keyword fallback behavior, if any.

3. Static string design
- Define capacity semantics and null termination rules.
- Decide normalization and comparison behavior.
- Confirm how static string interacts with keyword and map keys.

4. Lazy sequence execution model
- Define exact semantics for lazy repeat, cycle, take, drop, map, filter, reduce, and interpose.
- Define explicit materialization sink APIs and guarantees.
- Define infinite-source safety rules and recommended bounded-consumer patterns.

5. Threading API execution details
- Finalize step descriptor grammar for `thread_first`, `thread_last`, and `as_thread` (for example direct callables vs explicit call wrappers).
- Finalize conditional-step descriptor grammar for `cond_thread_first` and `cond_thread_last`.
- Define compile-time diagnostics style for invalid step arity or concept mismatch.
- Define interactions between threading and comparator override APIs (`equal_with` family) in mixed pipelines.

6. Validity contract for deferred some-thread variants
- Define explicit validity concept or probe contract required for `some_thread_first` and `some_thread_last` short-circuit semantics.
- Define compile-time gating behavior when no validity contract is available for a threaded value type.

7. Constexpr/consteval boundaries
- List which operations must be constexpr.
- List which validations must be consteval.
- Identify any operations intentionally runtime-only.

8. Regex profile details (CTRE)
- Define supported regex surface and any deliberate exclusions for embedded targets.
- Define build/profile toggles for enabling or disabling regex support.
- Define fallback behavior when regex profile is disabled.
- Define exact `re-pattern` and `re-matcher` semantics across compile-time and runtime regex profiles.
- Define invalid-pattern behavior and `pattern_valid(p)` contract details.

9. Testing strategy before implementation
- Define compile-time test cases (static_assert) for behavior contracts.
- Define runtime tests for edge cases and capacity boundaries.
- Define minimal acceptance criteria for first implementation milestone.

10. Namespace acceptance criteria execution
- Finalize pass/fail criteria for each MVP namespace.
- Define optional-namespace readiness gates by profile.
- Confirm deterministic behavior requirements for every namespace profile.

11. C++26 guideline execution
- Define contract portability wrapper behavior across compiler/toolchain profiles.
- Finalize concept taxonomy and naming for key API constraints.
- Define ARM profile benchmark set and optimization acceptance thresholds.

12. Packaging pipeline execution
- Define exact production folder layout for per-function headers.
- Define deterministic header ordering and include policy for amalgamation.
- Define CI checks ensuring amalgamated header regeneration stays in sync.

13. Quality toolchain execution
- Define Catch2 test suite structure and generator coverage requirements.
- Define sanitizer matrix by host profile and target exceptions.
- Define linting gate thresholds and failure policy.
- Define docs-sample test harness and Doxygen HTML publication flow.
- Define coverage reporting and approved exclusion workflow.
- Define no-heap gate implementation details across compile or link-time, runtime, and binary symbol checks.

## Comparison test focus to include

- Static-fail tests for floating-point comparisons through canonical APIs.
- Positive deep-equality tests for nested vector/map/set/string combinations.
- Arity tests proving zero-arity and one-arity comparison calls return `true`.
- Lazy evaluation tests proving no hidden materialization and correct infinite-source behavior under bounded consumption.

## First milestone proposal
- Implement keyword enum catalog API.
- Implement array-backed map with linear search and sentinel-return get.
- Add compile-time and runtime tests for the agreed behavior contracts.
