---
type: Insight
symbol: 💡
title: vocab-use-pascalcase-canonically
---
Canonical vocabulary terms should use PascalCase so they can be reused verbatim across gybis layers. Preserve spaced or hyphenated legacy phrasing only as deprecated synonyms in `vocabulary.md`.

Because cljonic users are developers, C++ concept identifiers (e.g. `CljonicCollection`, `SequenceableCollection`, `StableEqualityComparable`) are themselves first-class vocabulary terms — not hidden under Examples. Free-function names (snake_case, Clojure parity) are the only case-folded spelling variant of a PascalCase vocabulary term.