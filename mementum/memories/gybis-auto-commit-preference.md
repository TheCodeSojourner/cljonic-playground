---
type: Insight
symbol: 💡
title: gybis-auto-commit-preference
---
In gybis workflows that explicitly include a commit step (especially gybis-fini), default behavior is to run `git add` + `git commit` automatically.

Only skip auto-commit when a strong blocker exists:
- explicit no-commit instruction in the current turn
- unresolved conflict
- git failure
- policy/safety conflict

If blocked, do not silently skip; ask the user whether to retry now, skip this session, or commit manually.