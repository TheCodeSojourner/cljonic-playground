# cljonic Architecture - Module 4: Sequence Producers & Materialization Pipeline

## Purpose and Scope

This document specifies the implementation architecture for Module 4 of `cljonic`. It translates sequence producer types, materialization logic (`into`, `fits_into`), C++ standard view imports (`std::span`, `std::string_view`), and non-owning observation views (`view(collection)`) from `cljonic-requirements-module-4.md` into C++ template structures.

## `cljonic_source` Design Rule & Producer Architecture

The architecture distinguishes stored collections from explicit sequence producers:
- `cljonic_collection<T>`: Closed set of owning bounded collection types (`Vector`, `Map`, `Set`, `Queue`, `String`).
- `cljonic_producer<T>`: Standalone generator types (`Range`, `Repeat`, `Cycle`, `Iterate`, `Repeatedly`).
- `cljonic_source<T>`: `cljonic_collection<T> || cljonic_producer<T>`.

Producers store parameters by value without allocating result buffers or retaining references.

```cpp
namespace cljonic {

template<class T>
class Range {
    T m_start;
    T m_end;
    T m_step;
    // Stateless iteration generation; no internal storage buffer.
};

} // namespace cljonic
```

## Unbounded Traversal & Deep Equality Restriction Architecture

- Open-ended producers (`range` without end, `repeat` without count, `cycle`, `iterate`) do NOT satisfy `stable_equality_comparable`.
- Direct `==` comparison on an unbounded producer fails at compile time.
- Nested collections containing an unbounded producer lose default deep-equality capabilities.

## Materialization Pipeline (`into` & `fits_into`)

Materialization requires an explicit bounded destination:

```cpp
namespace cljonic {

template<cljonic_collection Dest, cljonic_source Source>
constexpr bool fits_into(const Dest& dest, const Source& source) {
    return (dest.count() + source.count()) <= Dest::capacity();
}

template<cljonic_collection Dest, cljonic_source Source>
constexpr Dest into(const Dest& dest, const Source& source) {
    Dest result = dest;
    for (auto&& item : source) {
        if (result.full()) break; // Bounded-prefix behavior on overflow
        result = conj(result, item);
    }
    return result;
}

} // namespace cljonic
```

## Standard View Interoperability Architecture (`view`)

The `view(collection)` free function returns non-owning, read-only standard views:
- `view(Vector<T, N>)` $\rightarrow$ `std::span<const T>`
- `view(Set<T, N>)` $\rightarrow$ `std::span<const T>`
- `view(Queue<T, N>)` $\rightarrow$ `std::span<const T>`
- `view(Map<K, V, N>)` $\rightarrow$ `std::span<const MapEntry<K, V>>`
- `view(String<N>)` $\rightarrow$ `std::string_view`

Views do NOT extend source lifetime and do NOT admit external types into the nominal collection domain.

## Traceability

- Governed Requirements: `cljonic-requirements-module-4.md` (`REQ-VAL-014`–`017`, `REQ-SEQ-015`–`021`, `REQ-FN-009`–`014C`, `REQ-FN-027`, `REQ-PLAT-017`–`023`).
- Downstream Modules: Module 5 (Higher-Order Algorithms), Module 7 (Specialized Domains).
