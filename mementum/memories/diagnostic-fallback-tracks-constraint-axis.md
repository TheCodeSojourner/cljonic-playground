---
type: Insight
symbol: 💡
title: diagnostic-fallback-tracks-constraint-axis
related: [rejection-diagnostic-fallback.md, source-construction-diagnostic-fallback.md, requires-expression-constrained-template-sfinae-limitation.md]
---
Diagnostic-fallback necessity tracks **which axis** a closed domain is gated
on — not whether the type has a class-template constraint. Most cljonic types
(`Vector`/`Set`/`Queue`/`Map`/`String`, `MapEntry`) *are* class-template-gated,
yet `Vector`/`Set` still need, and carry, a REQ-DIAG-010 fallback, so the
earlier phrasing "gated by a class-template constraint ⇒ no fallback" was wrong.

Axis A — class-instantiation (type-argument) constraint (`Range`'s
`signed_integral`, `MapEntry`'s `NothrowStableEqualityComparable`,
`NothrowCollectionElement`): rejection happens at instantiation, *before any
constructor is considered*, and C++ already emits one named constraint
diagnostic. A constructor fallback is unreachable and impossible.

Axis B — constructor-call (call-argument) constraint: rejection falls through
overload resolution to a *sibling* constructor (the pack ctor), whose generic
element-conversion `static_assert` misleads. A fallback is needed and possible
(REQ-DIAG-010). `Vector`/`Set` sit on both axes.

`Variant`: Axis B but no sibling ctor → accurate "no matching constructor", no
fallback. Aggregate (`MapEntry`): any ctor destroys aggregate status; Axis A
anyway. `Iterate`/`Repeatedly` default-ctor `requires default_initializable` is
an availability guard, not a rejection gate.

Confirmed by the 2026-10-06 constructor sweep: nothing to add.
