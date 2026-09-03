# cljonic Requirements Modules

This document outlines the modular implementation roadmap for `cljonic`. To ensure clean architecture and testable boundaries, the requirements are organized into 7 dependency-ordered module files that together form the current requirements set.

## Module Implementation Order & Summary

1. **[cljonic-requirements-module-1.md](cljonic-requirements-module-1.md)** / **[cljonic-architecture-module-1.md](cljonic-architecture-module-1.md)** — *Foundation & Nominal Type System*
   - **Goal**: Establishes core memory allocation invariants and nominal type recognition without depending on any collection logic.
   - **Scope**: Zero heap allocation, header-only C++23 distribution, single-threaded execution, value immutability, persistent semantics via deep copying, and closed collection domain rules (`REQ-PLAT-001`–`013`, `REQ-VAL-001`–`020`).

2. **[cljonic-requirements-module-2.md](cljonic-requirements-module-2.md)** / **[cljonic-architecture-module-2.md](cljonic-architecture-module-2.md)** — *Capability Concepts & Preflight Infrastructure*
   - **Goal**: Defines the C++20 concept capability framework and non-throwing preflight result model.
   - **Scope**: Outcome status classification (complete, bounded-prefix, default-returning, checked-failure, producer-only), preflight predicates (`fits_into`, `contains`, `is_empty`), compile-time diagnostic rules, `constexpr` constraints, and vocabulary conventions (`REQ-BOUNDS-*`, `REQ-ERR-*`, `REQ-DIAG-*`, `REQ-VOCAB-*`).

3. **[cljonic-requirements-module-3.md](cljonic-requirements-module-3.md)** / **[cljonic-architecture-module-3.md](cljonic-architecture-module-3.md)** — *Core Collection Types & Primitive Free Functions*
   - **Goal**: Implements concrete, array-backed, bounded collection types and their primitive member and free-function operations.
   - **Scope**: `Vector`, `MapEntry`, `Map`, `Set`, `Queue`, `String`, bounded array-backed storage, swap-and-remove mechanics, callable collection syntax (`v(idx)`, `m(key)`, `s(val)`), and core primitive free functions (`count`, `get`, `conj`, `assoc`, `dissoc`, `disj`, `peek`, `pop`, `first`, `next`, `rest`, `seq`) (`REQ-COLL-*`, `REQ-SEQ-001`–`014`, `REQ-SEQ-002A`–`002B`, `REQ-FN-001`–`008A`).

4. **[cljonic-requirements-module-4.md](cljonic-requirements-module-4.md)** / **[cljonic-architecture-module-4.md](cljonic-architecture-module-4.md)** — *Sequence Producers & Materialization Pipeline*
   - **Goal**: Adds explicit materialization, non-collection generator values, and standard view interop.
   - **Scope**: Generator producers (`Range`, `Repeat`, `Cycle`, `Iterate`, `Repeatedly`), materialization free functions (`into`, `fits_into`), const logical-range traversal, and conditional C++ interoperability accessors (`std::span<const T>`-like and `std::string_view`-like representations) (`REQ-VAL-014`–`017`, `REQ-SEQ-015`–`021`, `REQ-FN-009`–`014C`, `REQ-FN-027`, `REQ-PLAT-017`–`023`).

5. **[cljonic-requirements-module-5.md](cljonic-requirements-module-5.md)** / **[cljonic-architecture-module-5.md](cljonic-architecture-module-5.md)** — *Higher-Order Algorithms & Traversal*
   - **Goal**: Implements generic sequence transformation algorithms over any `cljonic_source`.
   - **Scope**: Core mapping/filtering (`map`, `filter`, `reduce`, `reduce_kv`, `keep`, `remove`), right-to-left function composition (`comp`), multi-source mapping, shaping and slicing (`take`, `drop`, `partition`, `interleave`, `interpose`), and ordering/grouping (`distinct`, `dedupe`, `flatten`, `tree_seq`, `group_by`, `sort`, `sort_by`) (`REQ-SEQ-022`, `REQ-FN-002G`, `REQ-FN-002H`).

6. **[cljonic-requirements-module-6.md](cljonic-requirements-module-6.md)** / **[cljonic-architecture-module-6.md](cljonic-architecture-module-6.md)** — *Numeric & Callable Convenience*
   - **Goal**: Adds scalar arithmetic, bitwise math, conversion/parsing controls, and functional closure builders.
   - **Scope**: Checked arithmetic with preflights (`can_add`, `can_subtract`, `can_multiply`), bitwise operations (`bit_and`, `bit_or`, `bit_shift_left`), raw floating-point stability restrictions, stored closure capabilities (`REQ-VAL-021`), and callable adapters (`partial`, `juxt`, `fnil`, `complement`, `apply`) (`REQ-NUM-*`, `REQ-FN-002N`, `REQ-FN-025`).

7. **[cljonic-requirements-module-7.md](cljonic-requirements-module-7.md)** / **[cljonic-architecture-module-7.md](cljonic-architecture-module-7.md)** — *Specialized Value Domains & State*
   - **Goal**: Implements higher-level domain conveniences and states built on top of lower modules.
   - **Scope**: Set algebra (`union`, `intersection`, `difference`), relational operations (`index`, `project`, `rename`, `join`), bounded text/regex operations (`String`, `Regex`, `re_find`, `re_seq`, `split`, `join`), debug formatting (`fits_print`, `print_to`), keyword enum name mapping (`KeywordEnumNameMap`), `Atom<T>` state reference, and the master unit test suite verification requirements (`REQ-TEST-001`–`005`) (`REQ-FN-015`–`024`, `REQ-FN-028`–`031`, `REQ-PLAT-024`–`042`, `REQ-TEST-*`).
