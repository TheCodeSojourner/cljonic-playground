---
type: Insight
symbol: 💡
title: generic-deleted-templates-over-type-whitelist
---
When guarding a templated constructor against implicit truncating conversions (e.g. `float`/`double` silently narrowing to an integral `value_type`), do not enumerate concrete non-matching type combinations (`Range(float, float) = delete`, `Range(int, int, float) = delete`, ...). That whitelist is always incomplete once the class is templated over an open set of types.

Instead use a single constrained deleted template per arity:
```cpp
template <typename T>
  requires (!std::same_as<T, value_type>)
constexpr Range(T) noexcept = delete;
```
This rejects every mismatched type for every instantiation, and is shorter than the whitelist it replaces.
