---
type: Insight
symbol: 💡
title: doxygen-html-cheatsheet-structure
---
`src/cljonic-core.hpp` is Doxygen Markdown source that renders to HTML. Its cheatsheet formatting is intentionally multiline: Markdown tables require the header, separator, and every row on their own source lines, while Markdown lists require every bullet on its own source line. Do not collapse these rows or items into one line; the current style is necessary for correct HTML rendering. `doc/mainpage-example.hpp` is the source-format fixture for only this `Core_Cheatsheet` block. `scripts/check-core-cheatsheet-format.py`, run by `make core-cheatsheet-format`, rejects collapsed Markdown in that block and self-tests a collapsed heading. Keep Doxygen `\ref` commands inside table cells or list items, regenerate `docs/` from `doc/Doxyfile`, and verify the generated HTML structure.
