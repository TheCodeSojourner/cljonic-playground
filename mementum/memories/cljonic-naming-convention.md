---
type: Pattern
symbol: 🔁
title: cljonic-naming-convention
---
All src and test files use `cljonic-` prefix with dashes. Patterns:
- `src/cljonic-{entity}.hpp` — collection type (e.g., `cljonic-vector.hpp`)
- `src/cljonic-concepts.hpp` — shared concept constraints
- `src/cljonic-core-{function}.hpp` — one file per core function (e.g., `cljonic-core-count.hpp`)
- `src/cljonic-core.hpp` — umbrella include; holds `\mainpage` + includes all core function headers
- `tests/cljonic-{entity}-spec-tests.cpp` — spec tests per entity
- `tests/no_heap/cljonic-{entity}-probes.cpp` — no-heap probes per entity
- `tests/no_heap/cljonic-core-{function}-probes.cpp` — no-heap probes per core function

Infrastructure files keep no prefix: `harness_main.cpp`, `poison.hpp`, `probes.hpp`.

Core functions (count, conj, etc.) belong in `cljonic-core-{function}.hpp`, not in collection files.
Makefile `help` entries must be added in alphabetical order.

C++ identifier naming follows the C++ Core Guidelines:
- Types and concepts: PascalCase (concepts per `NL.17`)
- Namespaces: lowercase_with_underscores (Clojure-analog `cljonic::core`, `cljonic::set`, `cljonic::string`)
- Free and member functions: snake_case (Clojure parity: `count`, `is_empty`, `contains`)
- Nominal concepts keep `Cljonic` prefix; capability/value concepts do not require it
