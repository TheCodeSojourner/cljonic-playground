# cljonic next discussion agenda

## Priority order

1. Key/value type system for map and set
- Decide allowed key categories (keyword enum, integral, enum, static string type, user-defined key types).
- Define required concepts for keys and values.
- Confirm equality semantics across scalar types.

2. Sentinel default ambiguity policy
- Missing access returns `T{}` by design.
- Decide library guidance for domains where `T{}` is a valid value.
- Decide whether optional probe APIs (for example contains/has_index helpers only) are sufficient.

3. Collection API surface (Clojure-inspired)
- Finalize names and overload strategy for get, assoc, dissoc, conj, contains, count, first, rest.
- Choose member functions vs free functions vs both.
- Define behavior for full-capacity assoc/conj (currently return unchanged value).

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

7. Constexpr/consteval boundaries
- List which operations must be constexpr.
- List which validations must be consteval.
- Identify any operations intentionally runtime-only.

8. Testing strategy before implementation
- Define compile-time test cases (static_assert) for behavior contracts.
- Define runtime tests for edge cases and capacity boundaries.
- Define minimal acceptance criteria for first implementation milestone.

## First milestone proposal
- Implement keyword enum catalog API.
- Implement array-backed map with linear search and sentinel-return get.
- Add compile-time and runtime tests for the agreed behavior contracts.
