# cljonic Architecture - Module 4: Sequence Producers & Materialization Pipeline

## Purpose and Scope

This document specifies the implementation architecture for Module 4 of `cljonic`. It translates sequence producer types, materialization logic (`into`, `fits_into`), C++ standard view imports (`std::span`, `std::string_view`), and collection-owned non-owning observation views from `cljonic-requirements-module-4.md` into C++ template structures.

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

Direct construction from an explicitly supported bounded C++ source is a separate bounded-construction path. It copies into owned destination storage and does not retain the source lifetime. A compile-time-known source extent greater than destination capacity is rejected at compile time; a runtime-sized source that exceeds capacity produces the deterministic bounded prefix. Direct construction does not expose a checked-constructor result channel. Complete source materialization uses `fits_into` before `into` when completeness matters.

## Standard View Interoperability Architecture (`view`)

Collection-owned `view()` member accessors return non-owning, read-only standard views:
- `Vector<T, N>::view()` $\rightarrow$ `std::span<const T>`
- `Set<T, N>::view()` $\rightarrow$ `std::span<const T>`
- `Queue<T, N>` $\rightarrow$ const logical traversal through `begin()`/`end()`; no contiguous `view` is required
- `Map<K, V, N>::view()` $\rightarrow$ `std::span<const MapEntry<K, V>>`
- `String<N>::view()` $\rightarrow$ `std::string_view`

Views do NOT extend source lifetime and do NOT admit external types into the nominal collection domain.

## Traceability

- Governed Requirements: `cljonic-requirements-module-4.md` (`REQ-VAL-014`–`017`, `REQ-SEQ-015`–`021`, `REQ-FN-009`–`014C`, `REQ-FN-027`–`027A`, `REQ-PLAT-017`–`023`).
- Downstream Modules: Module 5 (Higher-Order Algorithms), Module 7 (Specialized Domains).
