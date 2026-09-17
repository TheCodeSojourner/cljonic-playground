---
type: Mistake
symbol: ❌
title: doc-sample-extraction-requires-unindented-fence
---
`scripts/compile-doc-samples.py` only recognizes a code fence line matching `^[ \t]*~~~~~\{\.cpp\}[ \t]*$` — a bare `~~~~~{.cpp}` with only leading whitespace, no `*` prefix. Doxygen doc comments written in the asterisk-continuation style (`* ~~~~~{.cpp}`, one line per `*`) do NOT match this regex and are silently skipped: `docs-examples` reports success without ever compiling them. Only `Vector`/`Map`/`Set`/`Queue`/`String` use the recognized bare-fence style; most free-function headers (assoc, contains, count, conj, etc.) use the asterisk-prefixed style and their examples are never actually verified to compile, despite `make docs-examples` passing. When adding or editing a `\b Examples` block, use the bare (non-asterisk-prefixed) fence style so it is actually compiled — check `docs-examples:compiled=N` count before/after to confirm the block was picked up, don't just trust `docs-examples:ok`. This gap predates this session and affects most existing headers; it was not fixed project-wide, only avoided for newly-touched files (Range, Into, FitsInto).
