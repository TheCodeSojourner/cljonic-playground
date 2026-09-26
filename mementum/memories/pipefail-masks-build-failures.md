---
type: Mistake
symbol: ❌
title: pipefail-masks-build-failures
related: [ai-upsert-quality-loop.md]
---
Piping a validation command (build, test, gate) through `tail` (or any filter) without `set -o pipefail` masks failures: the pipeline exit status becomes the filter's, so a failed build looks green. Always `set -o pipefail` first, or capture output to a file instead of filtering a live pipeline.
