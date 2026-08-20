---
type: Insight
symbol: 💡
title: arch-quality-gates-need-thresholds
---
A mandatory quality gate is ambiguous until it states measurable criteria and fail behavior. Encode concrete thresholds directly in architecture lambdas (for example core MVP line coverage = 100 in host profiles and ASan+UBSan required) and add explicit enforcement (`any_quality_gate_fails(x) → reject_change(x)`). Do not require TSan unless the repo actually introduces concurrent behavior worth validating.