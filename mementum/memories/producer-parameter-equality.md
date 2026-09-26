---
type: Decision
symbol: 🎯
title: producer-parameter-equality
related: [cljonic-design-notes.md, nothrow-stable-equality-comparable-concept.md]
---
Producer equality is producer parameter equality (REQ-FN-014B): `Range<T>` `operator==` compares `start_/end_/step_`; `Repeat<T>` compares `value_/count_/is_finite_`; `Cycle<Source>` compares the owned `source_` (recursively) plus `count_/is_finite_`. All three also expose a named `parameters_equal` free function with identical semantics, O(1), constexpr, noexcept, never traversing the produced sequence.

Consequences:
- Range/Repeat/Cycle satisfy `StableEqualityComparable` and `NothrowStableEqualityComparable` over stable components → usable as map keys, set elements, and `std::variant` composite components (REQ-CAP-010).
- Distinct parameters compare unequal even when produced sequences coincide: `Range{0,5,0} != Range{0,7,0}` and `Repeat{7} != Repeat{7,0U}`. This is the documented non-sequence semantics.
- `Iterate`/`Repeatedly` never get equality: their stored `step_` is a callable, so the `contains_callable` walker (per-header specialization) reports true and the type is excluded at the type boundary.
- REQ-SEQ-016 now withholds only SEQUENCE equality from statically-unbounded producers (compile-time failure); producer parameter equality is explicitly permitted.
- REQ-FN-014B is normative: any future producer with all-stable stored parameters MUST provide both `==` and `parameters_equal`.
- The walkers (`contains_floating_point`/`contains_callable`) now recurse into producer stored parameters; every producer header carries its own specializations.