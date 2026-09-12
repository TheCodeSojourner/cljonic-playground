# cljonic Architecture - Module 5: Higher-Order Algorithms & Traversal

## Purpose and Scope

This document specifies the implementation architecture for Module 5 of `cljonic`. It translates higher-order sequence transformations, variadic function composition (`comp`), multi-source mapping, and collection shaping/traversal operations (`cljonic-requirements-module-5.md`) into generic template algorithms.

## Generic Core Algorithm Ownership

The generic core layer owns shared sequence algorithms across any `cljonic_source`:
- Shared functions: `map`, `filter`, `reduce`, `take`, `drop`, `partition`, `group_by`, `distinct`, `dedupe`, `sort`, `sort_by`.
- Free functions consume sequence traversal primitives (`first`, `next`, `rest`, `is_empty`) without depending on private storage layout.

## Multi-Source `map` Architecture

```cpp
namespace cljonic {

template<class F, cljonic_source Source, cljonic_source... Sources>
constexpr auto map(F&& f, const Source& source, const Sources&... sources) {
    // Computes smallest input capacity/count
    // Applies f(elem1, elem2, ...) element-by-element
    // Returns owning Vector<ResultT, N> or explicit producer
}

} // namespace cljonic
```

## Function Composition (`comp`) Architecture

`comp(f, g, h)` constructs a right-to-left composed functor:

```cpp
namespace cljonic {

template<class... Fs>
class ComposedFn;

template<class F, class G>
class ComposedFn<F, G> {
    F m_f;
    G m_g;
public:
    template<class Arg>
    constexpr auto operator()(Arg&& arg) const {
        return m_f(m_g(std::forward<Arg>(arg)));
    }
};

template<class... Fs>
constexpr auto comp(Fs&&... fs) {
    // Variadic composition; static_assert validates adjacent signature compatibility
}

} // namespace cljonic
```

## Algorithm-Sharing & Result Policy Rules

1. **Vector / Queue Source**: Preserves ordering.
2. **Map Source**: Sequence elements are `MapEntry<K, V>` values.
3. **Set Source**: Preserves unordered set semantics.
4. **Capacity Policy**: Operations producing sequence results return owning `Vector<ResultT, DerivedN>` when a finite capacity can be calculated, or explicit producers when unbounded.

## Traceability

- Governed Requirements: `cljonic-requirements-module-5.md` (`REQ-SEQ-022`, `REQ-FN-002G`, `REQ-FN-002H`).
- Downstream Modules: Module 6 (Numeric & Callable Convenience), Module 7 (Specialized Domains).
