---
type: Insight
symbol: 💡
title: doxygen-markdown-headings-do-not-autolink
related: [doxygen-html-cheatsheet-structure.md, doxygen-prose-formatting.md, /mementum/knowledge/doxygen-doc-tooling.md]
---
Doxygen does not auto-link words that appear in Markdown headings. A plain
`## Set Functions` renders as plain text even when a documented entity named
`Set` exists; only an explicit `\ref Set "Set"` creates a link. Inline prose is
the opposite: a bare `Set` auto-links, and the `%` escape did not reliably
suppress it in Doxygen 1.18.

So a heading is plain by default: add `\ref` only when the heading names the
exact entity you intend to link. In the `\mainpage` cheatsheet, the
`Set Functions` and `String Functions` headings must stay plain — they group
the planned `clojure.set` and `clojure.string` library functions, not the
`cljonic::Set` and `cljonic::String` collection types. `\ref Set` there
previously sent readers to the collection class instead.

Verify heading rendering by regenerating `docs/` and checking the generated
`<h2>`: Doxygen emits a link element only when one was requested. The same
treatment already applies in the aliases table, where the unresolved
`Namespace_Set` and `Namespace_String` references render unlinked.
