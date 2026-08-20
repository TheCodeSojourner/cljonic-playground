---
type: Insight
symbol: 💡
title: arch-replace-operational-placeholders
---
Temporary operational placeholders in S1 (for example `unknown_*` or `reason(...)`) should be replaced with explicit decisions before closing architecture checks. Even if the decision is conservative (`optional_dev_dependency_manager ≡ none`, `remote_ci_pipeline ≡ not_required_for_current_scope`), explicitness improves constraint integrity and future handoff clarity.