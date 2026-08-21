---
type: Insight
symbol: 💡
title: constexpr-calls-defeat-gcov-coverage
---
GCC constant-folds `constexpr` function calls whenever all arguments are literals, even at `-O0` with `--coverage`. The folded branch never executes at runtime, so `gcov`/`lcov` report it as uncovered even though a `STATIC_REQUIRE`/`static_assert` exercised it at compile time.

Fix: pass arguments through `volatile` locals at the runtime call site to defeat constant-expression evaluation and force a genuine call. Declaring the object `constexpr` vs plain does not help; only `volatile`-derived arguments reliably work.
