# cljonic Architecture - Module 7: Specialized Value Domains & State

## Purpose and Scope

This document specifies the implementation architecture for Module 7 of `cljonic`. It translates set algebra, relational operations (`index`, `project`, `rename`, `join`), text/regex semantics, debug formatting (`fits_print`, `print_to`), keyword enum name mapping (`KeywordEnumNameMap`), and `Atom<T>` state reference from `cljonic-requirements-module-7.md` into concrete C++ templates.

## Relational Model Architecture

A relation is represented directly using existing `cljonic` collection types:
- **Row**: `Map<Key, Value, M>` (owning bounded map).
- **Relation**: `Set<Row, N>` (owning bounded set of rows).

No C++ struct dynamic reflection or dynamic field lookup is used. Key selection is static or based on bounded key maps.

```cpp
namespace cljonic {

template<class RowSet, class FieldSet>
constexpr auto project(const RowSet& relation, const FieldSet& fields) {
    // Returns Set<ProjectedRow, N>
}

template<class LeftRel, class RightRel, class LeftKeySelector, class RightKeySelector, class DestRel>
constexpr DestRel join(const DestRel& dest, const LeftRel& left, const RightRel& right,
                       LeftKeySelector lkey, RightKeySelector rkey) {
    // Merges matching rows into dest using static key comparison
}

} // namespace cljonic
```

## Set Algebra Architecture

Binary set algebra (`union`, `intersection`, `difference`) operates on `Set<T, N>` and `Set<T, M>` inputs:
- Output type: `Set<T, std::max(N, M)>`.
- Preflight: `fits_set_algebra(left, right) -> bool`.
- Overflow policy for `union`: Returns deterministic bounded prefix when distinct count exceeds capacity.

## Text & Regex Architecture

1. **`String<N>` Invariants**: ASCII bytes (`0x01`–`0x7F`) with trailing null terminator. `std::string_view` runtime imports convert invalid bytes to `0x2E` (`.`).
2. **Regex Domain**: Root types `Regex`, `RegexMatcher`, `RegexMatch`, `RegexGroup` are self-contained bounded values. Compile-time patterns use CTRE integration; runtime patterns use bounded `re_pattern`.

## Debug Formatting & Keyword Enum Mapping Architecture

- `fits_print` / `print_to` format collections into fixed string destinations using Clojure-like delimiters (`[...]`, `{...}`).
- Scoped `enum class` keys format as numeric values by default, or as `:name` / `EnumName(value)` when an optional `KeywordEnumNameMap` or `KeywordEnumNameContext` is supplied.

## State Reference Architecture (`Atom<T>`)

```cpp
namespace cljonic {

template<class T>
class Atom {
    T m_value;
public:
    explicit constexpr Atom(T value) : m_value(std::move(value)) {}
    constexpr T deref() const { return m_value; }
    constexpr void reset(T new_value) { m_value = std::move(new_value); }
    
    template<class F, class... Args>
    constexpr T swap(F&& f, Args&&... args) {
        m_value = f(m_value, std::forward<Args>(args)...); // Exactly once synchronous update
        return m_value;
    }
};

} // namespace cljonic
```

## Traceability

- Governed Requirements: `cljonic-requirements-module-7.md` (`REQ-FN-015`–`024`, `REQ-FN-028`–`031`, `REQ-PLAT-011`, `REQ-PLAT-024`–`042`, `REQ-VAL-022`, `REQ-TEST-001`–`089`).
- Completes the entire 7-module implementation architecture roadmap for `cljonic`.
