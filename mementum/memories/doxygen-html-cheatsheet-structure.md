---
type: Insight
symbol: 💡
title: doxygen-html-cheatsheet-structure
---
`src/cljonic-core.hpp` is Doxygen Markdown source that renders to HTML. Its cheatsheet formatting is intentionally multiline: Markdown tables require the header, separator, and every row on their own source lines, while Markdown lists require every bullet on its own source line. Do not collapse these rows or items into one line; the current style is necessary for correct HTML rendering. Keep Doxygen `\ref` commands inside table cells or list items, regenerate `docs/` from `doc/Doxyfile`, and verify the generated HTML structure.
