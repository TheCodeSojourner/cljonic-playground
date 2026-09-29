---
type: Insight
symbol: 💡
title: gybis-auto-commit-preference
---
In gybis workflows that explicitly include a commit step (especially gybis-fini), default behavior is to run `git add` + `git commit` automatically.

Default changed 2026-09-29: during free-function/API implementation work, do NOT auto-commit. Work the gybis layer sequence (requirements → vocabulary → architecture → specification → tests → code) one layer at a time, pausing after each layer for human verification before proceeding to the next. Do not run `git add`/`git commit` during a slice; leave changes in the working tree for review and commit only when the human explicitly asks. `/gybis-fini` retains its default commit step for session state unless the human says otherwise.

Only skip auto-commit when a strong blocker exists:
- explicit no-commit instruction in the current turn
- unresolved conflict
- git failure
- policy/safety conflict

If blocked, do not silently skip; ask the user whether to retry now, skip this session, or commit manually.