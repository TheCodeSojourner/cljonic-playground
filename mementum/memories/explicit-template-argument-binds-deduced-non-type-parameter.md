---
type: Decision
symbol: 🎯
title: explicit-template-argument-binds-deduced-non-type-parameter
related:
  - /mementum/memories/factory-consistency-for-user-createable-values.md
  - /mementum/memories/typed-empty-factory-form-follows-collection-principles.md
  - /mementum/memories/deferred-api-promotion-checklist.md
---
When a deduced factory form and an explicit-capacity form must coexist, do not
deduce the deduced form from a non-type parameter of the same kind the explicit
form takes.

Promoting `string` (2026-10-23) exposed this: with

    template <std::size_t N> string(const char (&)[N])

the call `string<2>("a")` bound the *deduced* overload (N = 2 matched the
two-byte array), returned `String<1>`, and silently ignored the explicit
capacity. Overload resolution never reached the capacity overload, so no
diagnostic fired. Tests only failed when an assertion checked
`capacity() == 2`.

Fix: deduce from a type parameter and compute the capacity from the type, e.g.

    template <typename Literal>
        requires string_factory_literal_v<Literal>
    string(Literal&&) -> String<std::extent_v<Literal> - 1U>

An explicit `<Capacity>` then cannot bind that overload (a value is not a type),
so the capacity overload always wins.

Guidance: when adding the deduced form of any factory whose explicit form takes
`<Capacity>`, write a test that asserts the result's exact type for
`factory<Capacity>(...)`, not just observable content — silent wrong-overload
selection is otherwise invisible.
