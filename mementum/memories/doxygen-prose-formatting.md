---
type: Decision
symbol: 🎯
title: doxygen-prose-formatting
---
Doxygen prose in this repository follows these conventions:

- Free-function API summaries start with “The `<name>` free function API”; bold the function name with `\b` but do not link it.
- When documentation prose names a collection, producer, or other documented entity, use an explicit Doxygen reference so the generated HTML links to the entity's named anchor.
- Bold `CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE` whenever it appears in Doxygen prose.
- Disable typographic ligatures in inline-code spans through the Doxygen extra stylesheet so `==` remains visually distinct.
- Keep `not_equal` documentation concise: link to `equal` for supported values and comparison rules instead of repeating them.
- In equality documentation, distinguish collections that contain elements from producers that yield elements, and link named types.
