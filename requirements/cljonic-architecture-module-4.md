# cljonic Architecture - Module 4: Sequence Producers & Materialization Pipeline

## Purpose and Scope

This document specifies the implementation architecture for Module 4 of `cljonic`. It translates sequence producer types, materialization logic (`into`, `fits_into`), C++ standard view imports (`std::span`, `std::string_view`), and collection-owned non-owning observation views from `cljonic-requirements-module-4.md` into C++ template structures.

## `cljonic_source` Design Rule & Producer Architecture

The architecture distinguishes stored collections from explicit sequence producers:
- `cljonic_collection<T>`: Closed set of owning bounded collection types (`Vector`, `Map`, `Set`, `Queue`, `String`).
- `cljonic_producer<T>`: Standalone generator types (`Range`, `Repeat`, `Cycle`, `Iterate`, `Repeatedly`).
- `const_input_range<T>`: A structural const-source traversal capability equivalent to `std::ranges::input_range<const T>`.
- `nothrow_const_input_range<T>`: A structural const-source traversal capability equivalent to `const_input_range<T>` whose begin, end, dereference, increment, and iterator/sentinel comparison operations are non-throwing.
- `cljonic_source<T>`: `(cljonic_collection<T> || cljonic_producer<T>) && nothrow_const_input_range<T>`.
- `iterate_step<T, Step>`: A callable capability requiring a `NothrowCollectionElement` `T` and a copy-constructible stored `Step` whose const invocation with one `T` argument is non-throwing and returns exactly `T`. The public factory uses the decayed callable type, so named functions are stored as copyable function pointers while lambdas and function objects retain their decayed value types.

Producers store parameters by value without allocating result buffers or retaining references. A producer satisfying `cljonic_source` is a direct input to source-taking free functions; `into` is the explicit conversion path into an owning destination.

```cpp
namespace cljonic {

template<class T>
class Range {
    T m_start;
    T m_end;
    T m_step;
    // Stateless iteration generation; no internal storage buffer.
};

template<class T>
class Repeat {
    T m_value;
    std::size_t m_count;
    bool m_is_finite;
    // No result buffer; materialization copies m_value into a destination.
};

template<cljonic_source Source>
class Cycle {
    Source m_source;
    // The only public form is cycle(source); traversal is unbounded and capped
    // only when observed or materialized into an explicit destination. Finite
    // sources restart after exhaustion; unbounded sources preserve their
    // observable traversal without requiring a complete source result.
};

template<class T, iterate_step<T> Step>
class Iterate {
    T m_initial;
    Step m_step;
    // Unbounded state transition; no result buffer and no callback evaluation
    // during construction. Traversal emits m_initial first, then applies m_step
    // to the previously emitted value until the observable cap or destination
    // capacity is reached. For initial value x and step f, the sequence is
    // x, (f x), (f (f x)), ... . The type and traversal operations are constexpr-capable
    // and remain callable at runtime when their arguments are runtime values.
};

} // namespace cljonic
```

`repeat(value)` constructs `Repeat<T>` with `m_is_finite == false`; `repeat(value, count)` constructs it with `m_is_finite == true` and records `count`. `Repeat<T>` owns `m_value`, exposes `count()`, `is_finite()`, and const bounded `begin()`/`end()` traversal, has no producer indexed access, and does not retain a destination or source reference. An unbounded Repeat reports the configured observable traversal cap and `false` from `is_finite()`; a finite Repeat reports its stored runtime count and `true`. The materialization adapter emits a copy of `m_value` for each finite count, or until the explicit destination becomes full for an unbounded repeat.

`cycle(source)` constructs a source-parameterized `Cycle<Source>` that stores an independent owned copy of the source value. For a Producer source, the stored copy consists of the producer's parameters and state, not a materialized result sequence. Cycle is always unbounded, exposes the configured observable traversal cap and `false` from `is_finite()`, and repeats finite source sequences in logical traversal order. When the source is unbounded, Cycle preserves the source's bounded observable prefix without requiring the source to produce a complete result. An empty finite source produces empty observation. The `Cycle` source constraint is inherited from `cljonic_source<Source>` and therefore includes `nothrow_const_input_range<Source>`.

`iterate(step, initial)` constructs an unbounded `Iterate<T, std::decay_t<Step>>` only when `T` satisfies `NothrowCollectionElement` and the const step callback is a copy-constructible, non-throwing exact `T -> T` transition. The factory decays `Step` before storing it: a named function becomes a copyable function pointer, while a lambda or function object is stored as its decayed value type. `Step` may be moved from an rvalue during construction when supported, but the resulting producer remains copyable. `Iterate` owns copies of `initial` and the decayed `step`, emits `initial` first, and applies `step` to the previously emitted value for each subsequent element. For an initial value x and step f, its sequence is `x, (f x), (f (f x)), ...`. It exposes the configured observable traversal cap and `false` from `is_finite()`, satisfies `cljonic_source`, may be consumed directly by source-taking free functions, and converts into an owning destination through `into`; `fits_into` returns `false`. `Iterate` is not indexed or invocable and exposes no `contains`, positional retrieval, key-based lookup, or `get`. Its construction and traversal operations are constexpr-capable when their arguments are, and remain usable at runtime. The callback is not evaluated during construction.

## Unbounded Traversal & Deep Equality Restriction Architecture

- Open-ended producers (`range` without end, `repeat` without count, `cycle`, `iterate`) do NOT satisfy `stable_equality_comparable`.
- Direct `==` comparison on an unbounded producer fails at compile time.
- Nested collections containing an unbounded producer lose default deep-equality capabilities.

## Materialization Pipeline (`into` & `fits_into`)

Complete producer materialization requires an explicit bounded destination; producers may also be consumed directly by source-taking free functions:

```cpp
namespace cljonic {

template<cljonic_collection Dest, cljonic_source Source>
constexpr bool fits_into(const Dest& dest, const Source& source) {
    if constexpr (cljonic_producer<Source>) {
        if (!source.is_finite()) return false;
    }
    return source.count() <= Dest::capacity() - dest.count();
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

Direct construction from an explicitly supported bounded C++ source is a separate bounded-construction path. It copies into owned destination storage and does not retain the source lifetime. A compile-time-known source extent greater than destination capacity is rejected at compile time; a runtime-sized source that exceeds capacity produces the deterministic bounded prefix. Direct construction does not expose a checked-constructor result channel. Complete source materialization uses `fits_into` before `into` when completeness matters. `into` appends source values after the existing logical destination contents; `fits_into` measures whether that complete append fits the remaining capacity and returns false for an unbounded producer regardless of its capped traversal count.

`String<N>` additionally accepts `std::span<const char, Extent>` as a direct source. Fixed extents use the Vector-style compile-time capacity assertion; dynamic extents use runtime bounded-prefix copying. Span imports normalize invalid String bytes in the same way as external runtime text imports.

## Standard View Interoperability Architecture (`view`)

Collection-owned `view()` member accessors return non-owning, read-only standard views:
- `Vector<T, N>::view()` $\rightarrow$ `std::span<const T>`
- `Set<T, N>::view()` $\rightarrow$ `std::span<const T>`
- `Queue<T, N>` $\rightarrow$ const logical traversal through `begin()`/`end()`; no contiguous `view` is required
- `Map<K, V, N>::view()` $\rightarrow$ `std::span<const MapEntry<K, V>>`
- `String<N>::view()` $\rightarrow$ `std::string_view`

Views do NOT extend source lifetime and do NOT admit external types into the nominal collection domain.

## Traceability

- Governed Requirements: `cljonic-requirements-module-4.md` (`REQ-VAL-014`–`017`, `REQ-SEQ-015`–`021`, `REQ-FN-009`–`014C`, including `REQ-FN-013B`, `REQ-FN-027`–`027A`, `REQ-PLAT-017`–`023`).
- Downstream Modules: Module 5 (Higher-Order Algorithms), Module 7 (Specialized Domains).
