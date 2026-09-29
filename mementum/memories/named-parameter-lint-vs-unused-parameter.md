---
type: Mistake
symbol: ❌
title: named-parameter-lint-vs-unused-parameter
related: [doc-sample-extraction-requires-unindented-fence.md, use-upsert-gate-strict-for-full-checks-without-docs.md, gybis-fini-gates-before-commit.md]
---
An unnamed function parameter (`const T&`) compiles clean under `-Wall -Wextra -Werror`, but this repo's lint gate rejects it: clang-tidy's `readability-named-parameter` is in `WarningsAsErrors`, so `make lint` fails with "all parameters should be named in a function". Naming the parameter instead trips `-Werror=unused-parameter`. The two gates pull in opposite directions; neither the unnamed form nor the plain named form satisfies both.

Fix: name the parameter and mark it `[[maybe_unused]]` in the **prefix** position:

```cpp
[[nodiscard]] constexpr auto equal([[maybe_unused]] const T& value) noexcept -> bool;
```

The trailing form (`const T& [[maybe_unused]] value`) fails with `'maybe_unused' attribute cannot be applied to types`.

Found 2026-09-29 on the unary `equal` overload, after its doc sample was moved to the bare-fence style and compiled for the first time. `empty(const C&)` in `cljonic-empty.hpp` uses the unnamed form only because it is outside the lint TU — do not copy it. Always confirm with `make lint`, not just a compile.
