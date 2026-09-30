# cljonic Requirements Index

This document outlines the modular implementation roadmap for `cljonic`. To ensure clean architecture and testable boundaries, the requirements are organized into 7 dependency-ordered module files that together form the current requirements set, transcribed into nucleus lambda notation (`λ REQ-<DOMAIN>-NNN(x).`).

## Domain Prefixes (Closed Set)

`REQ-PLAT` | `REQ-VAL` | `REQ-CAP` | `REQ-BOUNDS` | `REQ-ERR` | `REQ-DIAG` | `REQ-CONST` | `REQ-VOCAB` | `REQ-COLL` | `REQ-SEQ` | `REQ-FN` | `REQ-NUM` | `REQ-TEST`

Extend only with new domains via `/gybis-req-tend` (human approval required).

## Normative Mapping

MUST ≡ ∀/¬ (required by constraint) | SHOULD ≡ ∧ preferred | MAY ≡ ∃ permitted path

## Conventions

- **granularity**: `library_contract` — cljonic-grade clause density permitted; compound operator contracts MAY be marked `compound_by_design: true` in clause footers.
- **deferred_marker**: section headings containing `(Deferred` or a blockquote opener asserting non-binding status (used by Module 3 `REQ-SEQ-001`–`014` and Module 4 `REQ-PLAT-017`–`023`).

## Module Implementation Order & Summary

1. **[requirements-module-1.md](requirements-module-1.md)** — *Foundation & Nominal Type System*
   - **Purpose**: Establishes core memory allocation invariants and nominal type recognition without depending on any collection logic.
   - **Scope**: Zero heap allocation, header-only C++23 distribution, single-threaded execution, value immutability, persistent semantics via deep copying, and closed collection domain rules (`REQ-PLAT-001`–`013`, `REQ-VAL-001`–`020`).

2. **[requirements-module-2.md](requirements-module-2.md)** — *Capability Concepts & Preflight Infrastructure*
   - **Purpose**: Defines the C++20 concept capability framework and non-throwing preflight result model.
   - **Scope**: Outcome status classification (complete, bounded-prefix, default-returning, checked-failure, producer-only), preflight predicates (`fits_into`, `contains`, `is_empty`), compile-time diagnostic rules, `constexpr` constraints, and vocabulary conventions (`REQ-CAP-*`, `REQ-BOUNDS-*`, `REQ-ERR-*`, `REQ-DIAG-*`, `REQ-CONST-*`, `REQ-VOCAB-*`).

3. **[requirements-module-3.md](requirements-module-3.md)** — *Core Collection Types & Primitive Free Functions*
   - **Purpose**: Implements concrete, array-backed, bounded collection types and their primitive member and free-function operations.
   - **Scope**: `Vector`, `MapEntry`, `Map`, `Set`, `Queue`, `String`, bounded array-backed storage, swap-and-remove mechanics, callable collection syntax (`v(idx)`, `m(key)`, `s(val)`), and current primitive free functions (`count`, `get`, `conj`, `assoc`, `dissoc`, `disj`, `peek`, `pop`). Sequence primitives (`first`, `next`, `rest`, `seq`) remain deferred future work (`REQ-COLL-*`, deferred `REQ-SEQ-001`–`014` incl. `002A`–`002B`, `REQ-FN-001`–`008A`, `REQ-FN-026`).

4. **[requirements-module-4.md](requirements-module-4.md)** — *Sequence Producers & Materialization Pipeline*
   - **Purpose**: Adds explicit materialization, non-collection generator values, and standard view interop.
   - **Scope**: Active generator producers (`Range`, `Repeat`, `Cycle`, `Iterate`, `Repeatedly`); direct producer consumption by source-taking free functions; materialization free functions (`into`, `fits_into`); deferred const logical-range traversal; and deferred conditional C++ interoperability accessors (`std::span<const T>`-like and `std::string_view`-like representations) (`REQ-VAL-014`–`017D`, `REQ-SEQ-015`–`021`, `REQ-FN-009`–`014C` incl. `013A`–`013D`, `REQ-FN-027`–`027A`, deferred `REQ-PLAT-017`–`023` incl. `017A`, `022A`).

5. **[requirements-module-5.md](requirements-module-5.md)** — *Higher-Order Algorithms & Traversal*
   - **Purpose**: Implements generic sequence transformation algorithms over any `cljonic_source`.
   - **Scope**: Multi-source mapping and right-to-left composition (`map`, `comp`), and the bounded collection-shaping/traversal family with individually named downstream behavioral specifications (`REQ-SEQ-022`, `REQ-FN-002G`, `REQ-FN-002H` as governed by this module).

6. **[requirements-module-6.md](requirements-module-6.md)** — *Numeric & Callable Convenience*
   - **Purpose**: Adds scalar arithmetic, bitwise math, conversion/parsing controls, and functional closure builders.
   - **Scope**: Checked arithmetic with preflights (`can_add`, `can_subtract`, `can_multiply`), bitwise operations (`bit_and`, `bit_or`, `bit_shift_left`), raw floating-point stability restrictions, stored closure capabilities, and callable adapters (`partial`, `juxt`, `fnil`, `complement`, `apply`) (`REQ-NUM-000`–`017`, `REQ-FN-002N`, `REQ-VAL-021`, `REQ-FN-025`).

7. **[requirements-module-7.md](requirements-module-7.md)** — *Specialized Value Domains & State*
   - **Purpose**: Implements higher-level domain conveniences and state built on top of lower modules.
   - **Scope**: Set algebra (`union`, `intersection`, `difference`), relational operations (`index`, `project`, `rename`, `join`), bounded text/regex operations (`String`, `Regex`, `re_find`, `re_seq`, `split`, `join`), debug formatting (`fits_print`, `print_to`), keyword enum name mapping (`KeywordEnumNameMap`), `Atom<T>` state reference, and the master unit test suite verification requirements (`REQ-FN-015`–`024`, `REQ-FN-028`–`031`, `REQ-FN-002I`–`002K`, `REQ-FN-002J`, `REQ-FN-002O`, `REQ-PLAT-024`–`042` excl. `REQ-PLAT-017`–`023`, `REQ-VAL-021`–`022`, `REQ-TEST-001`–`005`).