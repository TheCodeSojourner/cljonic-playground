---
type: Insight
symbol: 💡
title: if-consteval-trap-for-compile-time-only-rejection
---
To make a condition a hard compile error only when it occurs inside a required constant expression (e.g. `constexpr` object init), while leaving ordinary runtime behavior (e.g. silent clamping) untouched:

```cpp
static void reject_x_in_constant_expression() noexcept;  // declared, never defined

if (bad_condition) {
  if consteval {
    reject_x_in_constant_expression();
  }
  return fallback_value;
}
```

Calling an undefined function during constant evaluation makes that evaluation ill-formed only where a constant expression is actually required; ordinary runtime calls never reach the `if consteval` branch, so the function is never linked. Verified empirically with GCC: `constexpr` misuse produces a clear "call to non-constexpr function" diagnostic naming the trap function; plain runtime misuse compiles and runs normally.
