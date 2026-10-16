---
type: Insight
symbol: 💡
title: class-template-ctad-diagnostic-redirect
related: [/mementum/knowledge/rejection-diagnostics.md]
---
You cannot give a `requires`-constrained class template a targeted diagnostic
without a trick, because a rejected deduction is reported as a candidate list.
The obvious fix — replacing the constraint with a class-body `static_assert` —
is wrong: it makes the rejected specialization nameable, so
`CljonicRange<Range<double>>` and `NothrowCollectionElement<Range<double>>`
become `true` and soft rejection becomes a hard error. Measured, not assumed.

The workable mechanism keeps the domain constraint and adds three things: an
internal tag type; a gate concept `domain<T> || same_as<T, Tag>`; and deduction
guides constrained on the negation of the domain rule, one per supported arity,
returning the tag specialization. That specialization's constructor carries
`static_assert(dependent_false<Args...>)`, so it fires only on construction — a
non-dependent assert would fire at definition.

`decltype(Type{args})` inspects the redirect without triggering it, which is how
the traceability test asserts it. A guide must mirror the constructor's
deduction: `Repeat(const T&)` rather than by value, or an array argument deduces
`const char*` (which passes `NothrowCollectionElement`) and the redirect
silently never fires.
