---
type: Insight
symbol: 💡
title: arch-define-implicit-primitives-in-file
---
When architecture checks flag referenced-but-undefined primitives, define those primitives in `architecture.md` itself (for example `profile_gates(x)`, `stable_handle_model(x)`, `quality_gates(x)`) and then reference them explicitly (`...profile_gates(x)`). This keeps the architecture self-contained and improves coherence validation without changing system intent.