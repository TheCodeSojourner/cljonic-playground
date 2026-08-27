# cljonic Architecture - Module 6: Numeric & Callable Convenience

## Purpose and Scope

This document specifies the implementation architecture for Module 6 of `cljonic`. It translates checked fixed-width arithmetic, bitwise math, numeric conversion/parsing constraints, floating-point restriction gates, and callable convenience adapters from `cljonic-requirements-module-6.md` into C++ function templates.

## Fixed-Width Checked Arithmetic Architecture

Default arithmetic functions (`add`, `sub`, `mult`) perform checked fixed-width math with matching preflight predicates:

```cpp
namespace cljonic {

template<class T>
constexpr bool can_add(T a, T b) {
    if constexpr (std::is_signed_v<T>) {
        T result;
        return !__builtin_add_overflow(a, b, &result);
    } else {
        return (std::numeric_limits<T>::max() - a) >= b;
    }
}

template<class T>
constexpr T add(T a, T b) {
    if (!can_add(a, b)) return T{}; // Bounded checked default
    return a + b;
}

} // namespace cljonic
```

Wrapping (`add_wrap`) and saturating (`add_sat`) math are provided as distinct, explicitly named functions.

## Floating-Point Restriction Architecture

1. **Map Keys & Set Elements**: `float` and `double` are rejected at compile time via `stable_equality_comparable<T>` concepts.
2. **Sorting & Range**: Raw floats fail `totally_ordered<T>` and `discrete_numeric_type<T>` concept checks.
3. **No Implicit Epsilon**: Epsilon comparisons and automatic NaN normalizations are NOT provided.

## Stored Closure & Callable Adapter Architecture

- Closures meeting non-allocation and bounded storage requirements (`cljonic_callable<F>`) can be stored in `Vector` or `Map` as values.
- Stored closures do NOT acquire equality or ordering capabilities.
- Callable convenience builders (`partial`, `fnil`, `juxt`, `every_pred`, `some_fn`, `apply`) return non-allocating, compile-time checked functor wrappers.

```cpp
namespace cljonic {

template<class F, class... Args>
class PartialFn {
    F m_fn;
    std::tuple<Args...> m_captured;
public:
    // Non-allocating invocation operator
};

} // namespace cljonic
```

## Traceability

- Governed Requirements: `cljonic-requirements-module-6.md` (`REQ-NUM-*`, `REQ-FN-002N`, `REQ-VAL-021`, `REQ-FN-025`).
- Downstream Modules: Module 7 (Specialized Domains & State).
