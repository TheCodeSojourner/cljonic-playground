---
type: Mistake
symbol: ❌
title: requires-expression-constrained-template-sfinae-limitation
---
GCC (observed with GCC 14, `-std=c++23`) does not always treat a `requires { expr; }` simple-requirement as SFINAE-friendly when `expr` involves overload resolution against a *constrained template* (a class template with its own `requires`-clause, or a constrained constructor template selected via brace-init-list). Instead of the requires-expression silently evaluating to `false`, GCC can emit a hard compile error at the point of use.

Hit twice this session:
1. `STATIC_REQUIRE_FALSE(requires { typename cljonic::Map<ThrowingDefault, int, 4>; });` — testing a constrained class template inside a nested `requires{ typename ...; }`.
2. `static_assert(!requires { M<int,int,4>{1, 2, 3}; });` — testing constructor-overload rejection via brace-init against a `requires`-constrained constructor template.

Workaround: don't test the constrained template/constructor indirectly through a `requires{...}` wrapper. Instead assert the underlying trait/concept directly (e.g. `STATIC_REQUIRE_FALSE(cljonic::concepts::NothrowCollectionElement<ThrowingDefault>)`), or simply don't write a negative compile-test at all — this codebase already accepts that compile-time-failure invariants (`OversizedInitializerIsCompileTimeFailure`, etc.) are documented via `TRACE_ID` only, with no negative-compile-test harness, since a real one doesn't exist in this repo.
