---
type: Mistake
symbol: ❌
title: doc-sample-extraction-requires-unindented-fence
related: [doxygen-prose-must-be-user-facing.md, vocab-arch-weed-skip-aspirational-scope.md, artifact-boundary-discipline.md]
---
`scripts/compile-doc-samples.py` matches only a bare open fence `^[ \t]*~~~~~\{\.cpp\}[ \t]*$`; an asterisk-prefixed fence (`* ~~~~~{.cpp}`) is silently skipped, so `docs-examples:ok` can be a false green. The extractor also does NOT strip `* ` prefixes, so a block must be fully un-prefixed to compile (the `Equal`/`Vector` style).

Fixed 2026-09-29: all 32 headers converted to the bare-fence style; `docs-examples:compiled` went 14→27. The conversion immediately exposed real drift — 6 headers (`empty`, `first`, `next`, `not-empty`, `rest`, `seq`) document free functions deliberately removed from the public API (commit cc376ce) and absent from the `cljonic-core.hpp` umbrella (`specs/sequences/collection-shaping.allium` is `lifecycle: deferred`), so those examples cannot compile against `cljonic.hpp`.

Resolution (human decision): an explicit, reported exclusion — `DEFERRED_NON_PUBLIC_HEADERS` in the script, printed as `docs-examples:deferred-skipped=6`, plus a `\note Not yet part of the public API` in each header. Never re-introduce a silent skip: the skip must stay visible. When adding/editing an `Examples` block, use the bare fence and confirm the compiled count changes; don't trust `docs-examples:ok` alone.
