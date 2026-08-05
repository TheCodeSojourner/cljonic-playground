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

1. Key/value type system for map and set
- Decide allowed key categories (keyword enum, integral, enum, static string type, user-defined key types).
- Define required concepts for keys and values.
- Confirm final scalar equality rules for non-floating types.

2. Sentinel default ambiguity policy
- Missing access returns `T{}` by design.
- Decide library guidance for domains where `T{}` is a valid value.
- Decide whether optional probe APIs (for example contains/has_index helpers only) are sufficient.

3. Collection API surface (Clojure-inspired)
- Finalize names and overload strategy for get, assoc, dissoc, conj, contains, count, first, rest.
- Choose member functions vs free functions vs both.
- Define behavior for full-capacity assoc/conj (currently return unchanged value).
- Finalize `equal_with` and optional ordering override API shape for explicit per-call comparator injection.

4. Map internal layout details
- Confirm linear search and insertion-order storage for small N.
- Decide duplicate-key update semantics (replace in place in returned copy).
- Decide removal compaction behavior and ordering guarantees.

5. Keyword enum catalog details
- Establish keyword naming conventions (global-only, no namespace separators).
- Decide compile-time literal mapping API and diagnostics style.
- Define runtime text-to-keyword fallback behavior, if any.

6. Static string design
- Define capacity semantics and null termination rules.
- Decide normalization and comparison behavior.
- Confirm how static string interacts with keyword and map keys.

7. Lazy sequence execution model
- Define exact semantics for lazy repeat, cycle, take, drop, map, filter, reduce, and interpose.
- Define explicit materialization sink APIs and guarantees.
- Define infinite-source safety rules and recommended bounded-consumer patterns.

8. Constexpr/consteval boundaries
- List which operations must be constexpr.
- List which validations must be consteval.
- Identify any operations intentionally runtime-only.

9. Regex profile details (CTRE)
- Define supported regex surface and any deliberate exclusions for embedded targets.
- Define build/profile toggles for enabling or disabling regex support.
- Define fallback behavior when regex profile is disabled.
- Define exact `re-pattern` and `re-matcher` semantics across compile-time and runtime regex profiles.
- Define invalid-pattern behavior and `pattern_valid(p)` contract details.

10. Testing strategy before implementation
- Define compile-time test cases (static_assert) for behavior contracts.
- Define runtime tests for edge cases and capacity boundaries.
- Define minimal acceptance criteria for first implementation milestone.

11. Namespace acceptance criteria execution
- Finalize pass/fail criteria for each MVP namespace.
- Define optional-namespace readiness gates by profile.
- Confirm deterministic behavior requirements for every namespace profile.

12. C++26 guideline execution
- Define contract portability wrapper behavior across compiler/toolchain profiles.
- Finalize concept taxonomy and naming for key API constraints.
- Define ARM profile benchmark set and optimization acceptance thresholds.

13. Packaging pipeline execution
- Define exact production folder layout for per-function headers.
- Define deterministic header ordering and include policy for amalgamation.
- Define CI checks ensuring amalgamated header regeneration stays in sync.

14. Quality toolchain execution
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
