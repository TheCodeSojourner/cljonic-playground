---
type: Preference
symbol: 🎯
title: defer-doc-regeneration-until-final-git
---
During incremental development, do not regenerate repository documentation or run documentation-producing targets after every edit. Prefer focused validation that does not rewrite `docs/`. The developer will run the final `make git` when ready to push; that final gate may regenerate documentation and other derived artifacts.
