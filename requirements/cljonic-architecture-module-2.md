# cljonic Architecture - Module 2: Capability Concepts & Preflight Infrastructure

## Purpose and Scope

This document specifies the implementation architecture for Module 2 of `cljonic`. It translates the capability concepts, outcome status model, preflight predicate contracts, diagnostic rules, and vocabulary conventions (`cljonic-requirements-module-2.md`) into C++20 concepts and preflight result definitions.

## Two-Level Type Model: Level 2 (Semantic Capability Gates)

Once a type passes Level 1 (`cljonic_collection`), Level 2 evaluates structural and value capabilities:

```cpp
namespace cljonic {

// Value capabilities
template<class T>
concept stable_equality_comparable = requires(const T& a, const T& b) {
    { a == b } -> std::same_as<bool>;
} && !std::is_floating_point_v<std::remove_cvref_t<T>>;

template<class T>
concept totally_ordered = stable_equality_comparable<T> && requires(const T& a, const T& b) {
    { a < b } -> std::same_as<bool>;
};

// Structural capabilities (gated by nominal collection identity)
template<class C>
concept sequenceable_cljonic_collection = cljonic_collection<C> && requires(const C& c) {
    { c.is_empty() } -> std::same_as<bool>;
    { c.count() } -> std::integral;
};

template<class C>
concept indexed_cljonic_collection = sequenceable_cljonic_collection<C> && requires(const C& c, std::size_t i) {
    { c.get(i) };
    { c.contains(i) } -> std::same_as<bool>;
};

template<class C>
concept associative_cljonic_collection = sequenceable_cljonic_collection<C> && requires(const C& c) {
    typename C::key_type;
    typename C::value_type;
};

} // namespace cljonic
```

## Outcome Status Classification Architecture

Public operations return outcomes classified into 5 canonical categories:
1. **Complete Result**: Owning bounded value containing full output.
2. **Bounded-Prefix Result**: Owning bounded value truncated to fit capacity.
3. **Default-Returning Result**: Documented default value (`T{}` or fallback) on missing key/index.
4. **Checked-Failure Result**: Checked result structure communicating operational failure non-throwingly.
5. **Producer-Only Result**: Standalone sequence producer value (unmaterialized).

## Preflight Predicate Architecture

For every operation whose complete result can fail or truncate, a non-throwing preflight predicate MUST be provided:

| Operation              | Canonical Preflight Predicate | Preflight Signature                 |
| :--------------------- | :---------------------------- | :---------------------------------- |
| `into(dest, producer)` | `fits_into`                   | `fits_into(dest, producer) -> bool` |
| `get(vector, index)`   | `contains`                    | `contains(vector, index) -> bool`   |
| `get(map, key)`        | `contains`                    | `contains(map, key) -> bool`        |
| `conj(set, val)`       | `can_conj`                    | `can_conj(set, val) -> bool`        |
| `assoc(map, k, v)`     | `can_assoc`                   | `can_assoc(map, k, v) -> bool`      |
| `add(a, b)`            | `can_add`                     | `can_add(a, b) -> bool`             |

## Diagnostic Strategy

- Diagnostic messages use precise, capability-oriented concept names (e.g., `stable_equality_comparable`, `sequenceable_cljonic_collection`).
- Context-dependent constraints trigger clear `static_assert` messages detailing the violated bound or requirement.

## Traceability

- Governed Requirements: `cljonic-requirements-module-2.md` (`REQ-BOUNDS-*`, `REQ-ERR-*`, `REQ-DIAG-*`, `REQ-CONST-*`, `REQ-VOCAB-*`).
- Downstream Modules: Module 3 (Core Collections), Module 4 (Producers), Module 5 (Algorithms).
