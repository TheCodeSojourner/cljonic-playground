---
type: Pattern
symbol: 🔁
title: no-heap-probe-covers-deduction-guides
related: [no-heap-probes-propagate-with-feature.md, class-template-ctad-diagnostic-redirect.md]
---
Adding a deduction guide is a constructor overload-set change, so it owes a
`tests/no_heap/` upsert under `no-heap-probes-propagate-with-feature`.

A green `make no-heap` is not evidence of coverage. `no-heap-src` and
`no-heap-symbols` only scan the implementation, and a probe may not use CTAD at
all — when REQ-DIAG-012 landed, the `range`, `iterate`, `repeatedly`, and
`variant` probes never used a deduced construction, so their new guides were
unexercised. Confirm by grepping the probes for a deduced construction of the
changed type, not by trusting the gate.

A compile-fail harness is not a substitute. `scripts/check-ctad-diagnostics.py`
exercises every guide in both builds, but the no-heap harness is a separate
contract surface: a compile-fail run never exercises the admitted deduction at
runtime.

Asymmetry worth stating: where no admitted deduction exists (`Repeatedly`,
`Variant` — every deduced form redirects to the diagnostic), no runtime probe
can compile. Say so in the probe instead of omitting it silently. `Repeat`
already covered both of its arities.

2026-10-16: human confirmed the reading; the `range`, `cycle`, and `iterate`
probes gained deduced constructions.
