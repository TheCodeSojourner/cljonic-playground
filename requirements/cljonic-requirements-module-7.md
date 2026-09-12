# cljonic Requirements - Module 7: Specialized Value Domains & State

## Purpose and Scope

This module specifies specialized domain conveniences built on top of lower modules: set algebra, relational operations (`index`, `project`, `rename`, `join`), text/regex functions, runtime debug formatting (`fits_print`, `print_to`), keyword enum name mapping, state primitives (`Atom<T>`), struct/interop boundaries, and the mandatory test suite verification requirements (`REQ-TEST-001`–`005`).

## Set Algebra and Relational Operations

REQ-FN-023. The first-class bounded set algebra layer MUST include binary `union`, `intersection`, and `difference` operations over compatible `Set<T, N>` and `Set<T, M>` inputs, returning owning bounded `Set<T, max(N, M)>` values, plus `is_subset` and `is_superset` predicates returning `bool`. The derived result capacity MUST be a compile-time value and MUST remain within `CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT`; a statically invalid derived capacity MUST fail at compile time. Set algebra MUST require the stable equality capability applicable to set membership, MUST preserve the semantic unorderedness of set traversal, and MUST leave all input sets unchanged.

`fits_set_algebra(left, right)` MUST be the non-throwing, non-allocating complete-result preflight for the result-producing set algebra operations. `intersection` and `difference` MUST produce complete results within the derived capacity. When `union` has more distinct values than the derived capacity, `union` MUST return a deterministic bounded prefix containing the first `max(N, M)` distinct values encountered by its defined operand traversal, and `fits_set_algebra` MUST return `false`; the returned set remains semantically unordered and no completeness state may be stored in it. The defined operand traversal for this purpose MUST be left operand traversal followed by right operand traversal, with duplicate values skipped on first encounter; this deterministic prefix rule does not create a semantic ordering guarantee for sets.

REQ-FN-024. The relational map/set operations `index`, `project`, `rename`, and `join` MUST conform to the approved relation model in `REQ-FN-030`. Their correspondence to Clojure namespace symbols MUST NOT weaken the bounded, owning, no-allocation, static-capability, duplicate, traversal, capacity, preflight, or failure requirements of that model.

REQ-FN-028. The set namespace convenience family MUST include `select`, `map_invert`, and `rename_keys`. `select(predicate, set)` MUST return an owning `Set` with the same element type and capacity as its input, containing exactly the input values for which the pure, non-allocating predicate returns true. `map_invert(map)` MUST return an owning `Map<V, K, N>` for an input `Map<K, V, N>`, MUST require stable equality for `V`, and MUST associate each source value with its source key. `rename_keys(map, key_map)` MUST return an owning map with the source map's capacity, MUST replace each source key found in the bounded `key_map` with its mapped key, and MUST preserve source keys not found in `key_map` through the documented key conversion capability. All three operations MUST preserve their inputs and require no dynamic allocation or exceptions. `rename_keys` MUST remain distinct from the relational operation `rename`.

REQ-FN-030. The approved relational operations MUST use cljonic data structures directly. A relation MUST be represented as an owning bounded `Set<Row, N>` value, and a row MUST be represented as an owning bounded `Map<Key, Value, M>` value; a C++ struct row representation MUST NOT be required. `index`, `project`, `rename`, and `join` MUST be free functions. Their row, relation, map, set, sequence, value, selector, and combiner requirements MUST be expressed through named C++ concepts or equivalent compile-time constraints. `index` MUST use the source relation's bounded row capacity as the compile-time upper bound for its result map capacity; the runtime number of distinct key values MAY be smaller and MUST NOT determine the result type. Each grouped result set MUST use the source relation's row capacity. Any statically invalid derived capacity MUST fail at compile time. `join` MUST use the caller-supplied destination capacity for the output relation, and every intermediate bounded result MUST have a capacity derived from the corresponding source or destination bound; complete-result preflight MUST account for all such bounds.

- `project(relation, fields)` MUST return an owning bounded set of rows containing only the requested keys present in each input row.
- `rename(relation, field_map)` MUST return an owning bounded set of rows with keys replaced according to the bounded field map.
- `index(relation, key)` MUST return an owning bounded map from each present key value to an owning bounded set of matching source rows.
- `join(left, right, left_key, right_key, destination)` MUST compare values obtained from static key selectors and merge matching map rows into the caller-supplied owning bounded relation destination. `fits_join` MUST be the non-throwing, non-allocating preflight for complete join materialization.

## Text, Regex, and Parsing Domain

REQ-FN-015. Single-match regex functions corresponding to `re_find` and `re_matches` MUST return an owning `String` and MUST return the empty `String` when no match exists.

REQ-FN-016. Single-match regex functions MUST provide corresponding non-throwing, non-allocating preflight predicates, such as `has_re_find` and `has_re_matches`, that determine whether a match exists.

REQ-FN-017. Compile-time regex construction MUST use the root `cljonic::Regex<N>` type with braced literal syntax such as `cljonic::Regex{"^[A-Z]+$"}`. `N` MUST be the bounded pattern-storage capacity, the pattern MUST fit within `N` bytes excluding its terminator, and the pattern MUST be parsed and validated during constant evaluation. An omitted `N` MAY be inferred from the literal's content length.

REQ-FN-018. Runtime regex construction MUST be provided through `cljonic::core::re_pattern` using bounded cljonic string input and MUST return a documented tagged checked construction result with a success state containing a `Regex<N>` whose pattern capacity equals the input `String<N>` content capacity and a failure state for an invalid pattern. The result MUST use fixed bounded storage and require no dynamic allocation or exceptions. An invalid runtime pattern MUST produce the failure state rather than a `Regex<N>` claiming to contain a valid pattern.

REQ-FN-019. Clojure core regex functions MUST be provided in `cljonic::core` with hyphens translated to underscores, including `re_find`, `re_seq`, `re_matches`, `re_pattern`, `re_matcher`, and `re_groups`.

REQ-FN-020. Regex capture results MUST be owning bounded values. A multi-group match MUST expose its full match and captures through a bounded `Vector` of owning `String` values.

REQ-FN-022. The first-class bounded string convenience layer MUST include ASCII-preserving or ASCII-transforming operations corresponding to `is_blank`, `capitalize`, `ends_with`, `escape`, `includes`, `index_of`, `last_index_of`, `lower_case`, `re_quote_replacement`, `reverse`, `starts_with`, `trim`, `trim_newline`, `triml`, `trimr`, and `upper_case`. Operations corresponding to `join`, `replace`, `replace_first`, `split`, and `split_lines` MUST also be supported as bounded string convenience operations when their destination string or collection capacities are explicit and their complete-result preflight behavior is documented.

REQ-FN-031. The bounded text model MUST govern every supported or subsequently approved `clojure.string` operation. Text values MUST be owning `String<N>` values containing only bytes in the range `0x01` through `0x7F`. Text operations MUST be ASCII-deterministic, MUST NOT allocate or throw, and MUST NOT depend on locale, Unicode normalization, external string lifetime, hidden caches, or runtime reflection.

REQ-FN-002J. Runtime text parsing MUST provide `can_parse_int` and `parse_int` for the supported fixed-width integer target, `can_parse_float` and `parse_float` for the supported floating-point target, and `can_parse_bool` and `parse_bool` for documented boolean spellings.

## Bounded Debug Formatting & Keyword Enum Mapping

REQ-PLAT-033. cljonic MUST provide bounded human-readable debug formatting for supported built-in values through an explicit fixed-capacity destination without dynamic allocation, iostreams, exceptions, or RTTI.

REQ-PLAT-034. Formatting of supported built-in collections SHOULD use readable Clojure-like delimiters where practical, but it MUST be treated as debug output rather than stable serialization.

REQ-PLAT-035. Application-defined enum keys MUST be formatted using their underlying numeric values by default. An explicitly supplied `KeywordEnumNameMap` MAY provide bounded human-readable debug names for selected enum values.

REQ-PLAT-038. `KeywordEnumNameEntry` MUST represent one application-defined enum value and one bounded ASCII display name for debug formatting.

REQ-PLAT-039. `KeywordEnumNameMap<Enum, EntryCount, NameCapacity>` MUST associate one scoped application enum type with a bounded collection of `KeywordEnumNameEntry` values and one bounded ASCII display name for the enum type.

REQ-PLAT-040. `make_keyword_enum_name_map` MUST provide a convenience construction path for `KeywordEnumNameMap`.

REQ-PLAT-041. When a supplied `KeywordEnumNameMap` contains a display name for an enum value, debug formatting MUST emit that name with a leading `:`. When no entry exists, formatting MUST emit `EnumTypeName(value)` or the underlying numeric value.

REQ-PLAT-042. `KeywordEnumNameContext` MUST be capable of bundling zero or more `KeywordEnumNameMap` values without dynamic storage or type erasure.

REQ-FN-002K. `fits_print(destination, value, keyword_enum_name_maps...)` MUST report whether the complete debug representation fits, and `print_to(destination, value, keyword_enum_name_maps...)` MUST return the destination collection type directly.

REQ-FN-002O. `KeywordEnumNameContext` MAY bundle zero or more `KeywordEnumNameMap` values for reuse with `fits_print` and `print_to`.

## State Reference (`Atom<T>`) & Additional Core Conveniences

REQ-VAL-022. A bounded single-threaded `Atom<T>` MUST be supported as an explicit mutable reference to one cljonic value. The atom MUST use automatic or static storage, MUST not provide thread-safety or concurrent mutation, and MUST preserve the stored value's ordinary value semantics.

REQ-FN-002I. `deref` MUST read the current value of an `Atom<T>`, `reset` MUST replace it with a value assignable to `T`, and `swap` MUST synchronously invoke its update function with the current `T` value and replace the atom with a result assignable to `T`. `swap` MUST be evaluated exactly once per call without retry or compare-and-set semantics.

REQ-FN-021. Functions corresponding to Clojure nil-returning operations MUST use typed cljonic absence semantics: single element or value results return their documented default value; collection results return an empty bounded collection; boolean predicates return `false`; and single-match regex functions return an empty owning `String`. `some` MUST return the first truthy predicate result or typed default, and `has_some` MUST be the corresponding presence predicate.

REQ-FN-029. The additional core convenience family MUST include `get_in`, `assoc_in`, `update`, `update_in`, `merge`, `merge_with`, `select_keys`, `keys`, `vals`, `zipmap`, `update_keys`, `update_vals`, `vector`, `vec`, `hash_map`, `hash_set`, `swap_vals`, `reset_vals`, `str`, `pr_str`, `prn_str`, and `replicate`.

## User Struct & Scoped Keyword Enum Invariants

REQ-PLAT-024. The supported collection contract MUST support simple user-defined C++ structs with public data members, no user-defined methods, no private data, and non-allocating value operations.

REQ-PLAT-025. Destructuring of supported tuple-like values and simple aggregate structs MUST use ordinary C++ structured bindings or positional decomposition.

REQ-PLAT-026. Map access MUST use `get`, `contains`, and pre-access predicates rather than a special map-destructuring syntax.

REQ-PLAT-027. cljonic MUST NOT provide runtime `Symbol` values or symbol-based namespace resolution.

REQ-PLAT-028. Application-defined scoped enumeration types declared with `enum class` MUST be supported as cljonic map keys and set elements when their fixed underlying representation provides stable equality.

REQ-PLAT-029. Application guidance SHOULD recommend a globally scoped `enum class Keywords` when an application has one shared vocabulary of configuration or data keys.

REQ-PLAT-030. cljonic MUST NOT provide runtime or attached metadata for collection or scalar values.

REQ-PLAT-031. cljonic MUST NOT provide a Clojure reader, EDN parser, reader conditionals, macro system, syntax-quoting system, evaluator, or runtime namespace-resolution system.

REQ-PLAT-032. cljonic MUST NOT provide runtime type inspection, reflection, runtime class or hierarchy queries, or runtime dynamic dispatch.

## Verification Requirements

REQ-TEST-001. Every behavioral `REQ-*` clause MUST be mapped to at least one executable unit test or compile-only test, and the mapping MUST identify the requirement designator.

REQ-TEST-002. Tests MUST verify persistent value semantics by demonstrating that update operations return independently valid results and leave their input collections unchanged.

REQ-TEST-003. Tests MUST install allocation and deallocation traps sufficient to verify that no dynamic allocation or deallocation occurs in supported public paths, including failure paths.

REQ-TEST-004. Tests MUST verify totality and preflight agreement, including typed absence, checked conversions, capacity failures, and deterministic bounded-prefix results across supported collections and operations.

REQ-TEST-005. The test suite MUST compile and execute applicable coverage in configurations with exceptions and RTTI disabled, and MUST verify the no-exception and no-RTTI constraints.

## Traceability and Related Requirements

- **Downstream Artifact**: Relational algebra functions (`index`, `project`, `rename`, `join`), string/regex functions, `fits_print`/`print_to`, `Atom<T>`, and the test suite (`REQ-TEST-*`).
- **Governed REQs**: `REQ-FN-015`–`024`, `REQ-FN-028`–`031`, `REQ-PLAT-024`–`042`, `REQ-VAL-022`, `REQ-TEST-001`–`005`.
