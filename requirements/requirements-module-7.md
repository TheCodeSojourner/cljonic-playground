# cljonic Requirements - Module 7: Specialized Value Domains & State

## Purpose and Scope

This module specifies specialized domain conveniences built on top of lower modules: set algebra, relational operations (`index`, `project`, `rename`, `join`), text/regex functions, runtime debug formatting (`fits_print`, `print_to`), keyword enum name mapping, state primitives (`Atom<T>`), struct/interop boundaries, and the mandatory test suite verification requirements (`REQ-TEST-001`–`005`).

## Set Algebra and Relational Operations

λ REQ-FN-023(x).
  ∀ context: the first-class bounded set algebra layer includes binary `union`, `intersection`, and `difference` operations over compatible `Set<T, N>` and `Set<T, M>` inputs, returning owning bounded `Set<T, max(N, M)>` values, plus `is_subset` and `is_superset` predicates returning `bool`
  ∧ ∀ derived result capacity: a compile-time value ∧ remains within `CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT` ∧ ∀ statically invalid derived capacity: fails at compile time
  ∧ ∀ set_algebra: requires the stable equality capability applicable to set membership ∧ preserves the semantic unorderedness of set traversal ∧ leaves all input sets unchanged
  ∧ ∀ context: `fits_set_algebra(left, right)` is the non-throwing, non-allocating complete-result preflight for the result-producing set algebra operations
  ∧ ∀ (`intersection` ∧ `difference`): produce complete results within the derived capacity
  ∧ ∀ `union` with more distinct values than the derived capacity: returns a deterministic bounded prefix containing the first `max(N, M)` distinct values encountered by its defined operand traversal ∧ `fits_set_algebra` returns `false` ∧ the returned set remains semantically unordered ∧ ¬∃ storage of completeness state in it
  ∧ ∀ defined operand traversal for this purpose: left operand traversal followed by right operand traversal, with duplicate values skipped on first encounter ∧ the deterministic prefix rule does not create a semantic ordering guarantee for sets
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-FN-024(x).
  ∀ relational map/set operation ∈ {`index`, `project`, `rename`, `join`}: conforms to the approved relation model in `REQ-FN-030`
  ∧ ∀ correspondence to Clojure namespace symbols: ¬∃ weakening of the bounded, owning, no-allocation, static-capability, duplicate, traversal, capacity, preflight, or failure requirements of that model
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-FN-028(x).
  ∀ context: the set namespace convenience family includes `select`, `map_invert`, and `rename_keys`
  ∧ ∀ `select(predicate, set)`: returns an owning `Set` with the same element type and capacity as its input, containing exactly the input values for which the pure, non-allocating predicate returns true
  ∧ ∀ `map_invert(map)` on an input `Map<K, V, N>`: returns an owning `Map<V, K, N>` ∧ requires stable equality for `V` ∧ associates each source value with its source key
  ∧ ∀ `rename_keys(map, key_map)`: returns an owning map with the source map's capacity ∧ replaces each source key found in the bounded `key_map` with its mapped key ∧ preserves source keys not found in `key_map` through the documented key conversion capability
  ∧ ∀ three_operations: preserve their inputs ∧ require no dynamic allocation or exceptions
  ∧ ∀ `rename_keys`: remains distinct from the relational operation `rename`
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-FN-030(x).
  ∀ approved relational operation: uses cljonic data structures directly — a relation is represented as an owning bounded `Set<Row, N>` value ∧ a row is represented as an owning bounded `Map<Key, Value, M>` value ∧ ¬∃ requirement of a C++ struct row representation
  ∧ ∀ (`index` ∧ `project` ∧ `rename` ∧ `join`): free functions ∧ their row, relation, map, set, sequence, value, selector, and combiner requirements expressed through named C++ concepts or equivalent compile-time constraints
  ∧ ∀ `index`: uses the source relation's bounded row capacity as the compile-time upper bound for its result map capacity ∧ the runtime number of distinct key values MAY be smaller ∧ ¬∃ determination of the result type by it ∧ each grouped result set uses the source relation's row capacity ∧ ∀ statically invalid derived capacity: fails at compile time
  ∧ ∀ `join`: uses the caller-supplied destination capacity for the output relation ∧ every intermediate bounded result has a capacity derived from the corresponding source or destination bound ∧ complete-result preflight accounts for all such bounds
  ∧ per-operation semantics:
    - ∀ `project(relation, fields)`: returns an owning bounded set of rows containing only the requested keys present in each input row
    - ∀ `rename(relation, field_map)`: returns an owning bounded set of rows with keys replaced according to the bounded field map
    - ∀ `index(relation, key)`: returns an owning bounded map from each present key value to an owning bounded set of matching source rows
    - ∀ `join(left, right, left_key, right_key, destination)`: compares values obtained from static key selectors ∧ merges matching map rows into the caller-supplied owning bounded relation destination ∧ `fits_join` is the non-throwing, non-allocating preflight for complete join materialization
  {source: stakeholder_decided, decided_by: original_spec_author}

## Text, Regex, and Parsing Domain

λ REQ-FN-015(x).
  ∀ single-match regex function corresponding to (`re_find` ∨ `re_matches`): returns an owning `String` ∧ returns the empty `String` when no match exists
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-FN-016(x).
  ∀ single-match regex function: provides corresponding non-throwing, non-allocating preflight predicates, such as `has_re_find` and `has_re_matches`, that determine whether a match exists
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-FN-017(x).
  ∀ compile_time regex construction: uses the root `cljonic::Regex<N>` type with braced literal syntax such as `cljonic::Regex{"^[A-Z]+$"}`
  ∧ ∀ `N`: the bounded pattern-storage capacity ∧ the pattern fits within `N` bytes excluding its terminator ∧ the pattern is parsed and validated during constant evaluation
  ∧ ∃ permitted_path: an omitted `N` inferred from the literal's content length
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-FN-018(x).
  ∀ runtime regex construction: provided through `cljonic::core::re_pattern` using bounded cljonic string input ∧ returns a documented tagged checked construction result with a success state containing a `Regex<N>` whose pattern capacity equals the input `String<N>` content capacity and a failure state for an invalid pattern
  ∧ ∀ result: uses fixed bounded storage ∧ requires no dynamic allocation or exceptions
  ∧ ∀ invalid runtime pattern: produces the failure state rather than a `Regex<N>` claiming to contain a valid pattern
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-FN-019(x).
  ∀ Clojure core regex function ∈ {`re_find`, `re_seq`, `re_matches`, `re_pattern`, `re_matcher`, `re_groups`}: provided in `cljonic::core` with hyphens translated to underscores
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-FN-020(x).
  ∀ regex capture result: owning bounded value ∧ ∀ multi-group match: exposes its full match and captures through a bounded `Vector` of owning `String` values
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-FN-022(x).
  ∀ context: the first-class bounded string convenience layer includes ASCII-preserving or ASCII-transforming operations corresponding to `is_blank`, `capitalize`, `ends_with`, `escape`, `includes`, `index_of`, `last_index_of`, `lower_case`, `re_quote_replacement`, `reverse`, `starts_with`, `trim`, `trim_newline`, `triml`, `trimr`, and `upper_case`
  ∧ ∀ operation corresponding to (`join` ∨ `replace` ∨ `replace_first` ∨ `split` ∨ `split_lines`): also supported as a bounded string convenience operation when its destination string or collection capacities are explicit and its complete-result preflight behavior is documented
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-FN-031(x).
  ∀ supported ∨ subsequently approved `clojure.string` operation: the bounded text model governs it
  ∧ ∀ text_value: owning `String<N>` containing only bytes in the range `0x01` through `0x7F`
  ∧ ∀ text_operation: ASCII-deterministic ∧ ¬∃ (allocation ∨ throw) ∧ ¬∃ dependence on (locale ∨ Unicode normalization ∨ external string lifetime ∨ hidden caches ∨ runtime reflection)
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-FN-002J(x).
  ∀ runtime text parsing: provides `can_parse_int` and `parse_int` for the supported fixed-width integer target, `can_parse_float` and `parse_float` for the supported floating-point target, and `can_parse_bool` and `parse_bool` for documented boolean spellings
  {source: stakeholder_decided, decided_by: original_spec_author}

## Bounded Debug Formatting & Keyword Enum Mapping

λ REQ-PLAT-033(x).
  ∀ context: cljonic provides bounded human-readable debug formatting for supported built-in values through an explicit fixed-capacity destination without (dynamic allocation ∨ iostreams ∨ exceptions ∨ RTTI)
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-PLAT-034(x).
  ∀ formatting of supported built-in collections: ∧ preferred — uses readable Clojure-like delimiters where practical ∧ treated as debug output rather than stable serialization
  rationale: debug formatting is not a stable serialization contract, so readability trumps exactness
  {source: stakeholder_decided, decided_by: original_spec_author, rationale_source: origin_artifact}

λ REQ-PLAT-035(x).
  ∀ application-defined enum key: formatted using its underlying numeric values by default
  ∧ ∃ permitted_path: an explicitly supplied `KeywordEnumNameMap` provides bounded human-readable debug names for selected enum values
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-PLAT-038(x).
  ∀ `KeywordEnumNameEntry`: represents one application-defined enum value and one bounded ASCII display name for debug formatting
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-PLAT-039(x).
  ∀ `KeywordEnumNameMap<Enum, EntryCount, NameCapacity>`: associates one scoped application enum type with a bounded collection of `KeywordEnumNameEntry` values and one bounded ASCII display name for the enum type
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-PLAT-040(x).
  ∀ context: `make_keyword_enum_name_map` provides a convenience construction path for `KeywordEnumNameMap`
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-PLAT-041(x).
  ∀ supplied `KeywordEnumNameMap` containing a display name for an enum value: debug formatting emits that name with a leading `:`
  ∧ ∀ case where no entry exists: formatting emits `EnumTypeName(value)` or the underlying numeric value
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-PLAT-042(x).
  ∀ `KeywordEnumNameContext`: capable of bundling zero or more `KeywordEnumNameMap` values without (dynamic storage ∨ type erasure)
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-FN-002K(x).
  ∀ `fits_print(destination, value, keyword_enum_name_maps...)`: reports whether the complete debug representation fits ∧ ∀ `print_to(destination, value, keyword_enum_name_maps...)`: returns the destination collection type directly
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-FN-002O(x).
  ∃ permitted_path: `KeywordEnumNameContext` bundles zero or more `KeywordEnumNameMap` values for reuse with `fits_print` and `print_to`
  {source: stakeholder_decided, decided_by: original_spec_author}

## State Reference (`Atom<T>`) & Additional Core Conveniences

λ REQ-VAL-022(x).
  ∀ bounded single-threaded `Atom<T>`: supported as an explicit mutable reference to one cljonic value
  ∧ ∀ atom: uses automatic or static storage ∧ ¬∃ (provision of thread-safety ∨ concurrent mutation) ∧ preserves the stored value's ordinary value semantics
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-FN-002I(x).
  ∀ `deref`: reads the current value of an `Atom<T>` ∧ ∀ `reset`: replaces it with a value assignable to `T` ∧ ∀ `swap`: synchronously invokes its update function with the current `T` value and replaces the atom with a result assignable to `T`
  ∧ ∀ `swap`: evaluated exactly once per call without (retry ∨ compare-and-set semantics)
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-FN-021(x).
  ∀ function corresponding to a Clojure nil-returning operation: uses typed cljonic absence semantics — single element or value results return their documented default value ∧ collection results return an empty bounded collection ∧ boolean predicates return `false` ∧ single-match regex functions return an empty owning `String`
  ∧ ∀ `some`: returns the first truthy predicate result or typed default ∧ ∀ `has_some`: the corresponding presence predicate
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-FN-029(x).
  ∀ context: the additional core convenience family includes `get_in`, `assoc_in`, `update`, `update_in`, `merge`, `merge_with`, `select_keys`, `keys`, `vals`, `zipmap`, `update_keys`, `update_vals`, `vector`, `vec`, `hash_map`, `hash_set`, `swap_vals`, `reset_vals`, `str`, `pr_str`, `prn_str`, and `replicate`
  {source: stakeholder_decided, decided_by: original_spec_author}

## User Struct & Scoped Keyword Enum Invariants

λ REQ-PLAT-024(x).
  ∀ supported collection contract: supports simple user-defined C++ structs with (public data members ∧ ¬∃ user-defined methods ∧ ¬∃ private data ∧ non-allocating value operations)
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-PLAT-025(x).
  ∀ destructuring of supported tuple-like values ∧ simple aggregate structs: uses ordinary C++ structured bindings or positional decomposition
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-PLAT-026(x).
  ∀ map access: uses `get`, `contains`, and pre-access predicates rather than a special map-destructuring syntax
  rationale: struct-style destructuring syntax has no cljonic analog that preserves bounds-checked, default-returning map access semantics
  {source: stakeholder_decided, decided_by: original_spec_author, rationale_source: origin_artifact}

λ REQ-PLAT-027(x).
  ∀ context: cljonic ¬∃ provision of runtime `Symbol` values or symbol-based namespace resolution
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-PLAT-028(x).
  ∀ application-defined scoped enumeration type declared with `enum class` whose fixed underlying representation provides stable equality: supported as a cljonic map key and set element
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-PLAT-029(x).
  ∧ preferred: application guidance recommends a globally scoped `enum class Keywords` when an application has one shared vocabulary of configuration or data keys
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-PLAT-030(x).
  ∀ context: cljonic ¬∃ provision of runtime or attached metadata for collection or scalar values
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-PLAT-031(x).
  ∀ context: cljonic ¬∃ provision of (a Clojure reader ∨ EDN parser ∨ reader conditionals ∨ macro system ∨ syntax-quoting system ∨ evaluator ∨ runtime namespace-resolution system)
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-PLAT-032(x).
  ∀ context: cljonic ¬∃ provision of (runtime type inspection ∨ reflection ∨ runtime class or hierarchy queries ∨ runtime dynamic dispatch)
  {source: stakeholder_decided, decided_by: original_spec_author}

## Verification Requirements

λ REQ-TEST-001(x).
  ∀ behavioral `REQ-*` clause: mapped to at least one executable unit test or compile-only test ∧ the mapping identifies the requirement designator
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-TEST-002(x).
  ∀ test: verifies persistent value semantics by demonstrating that update operations return independently valid results and leave their input collections unchanged
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-TEST-003(x).
  ∀ test suite: installs allocation and deallocation traps sufficient to verify that no dynamic allocation or deallocation occurs in supported public paths, including failure paths
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-TEST-004(x).
  ∀ test suite: verifies totality and preflight agreement, including typed absence, checked conversions, capacity failures, and deterministic bounded-prefix results across supported collections and operations
  {source: stakeholder_decided, decided_by: original_spec_author}

λ REQ-TEST-005(x).
  ∀ test suite: compiles and executes applicable coverage in configurations with exceptions and RTTI disabled ∧ verifies the no-exception and no-RTTI constraints
  {source: stakeholder_decided, decided_by: original_spec_author}

## Traceability and Related Requirements

- **Downstream Artifact**: Relational algebra functions (`index`, `project`, `rename`, `join`), string/regex functions, `fits_print`/`print_to`, `Atom<T>`, and the test suite (`REQ-TEST-*`).
- **Governed REQs**: `REQ-FN-015`–`024`, `REQ-FN-028`–`031` (incl. `REQ-FN-002I`, `REQ-FN-002J`, `REQ-FN-002K`, `REQ-FN-002O`), `REQ-PLAT-024`–`035`, `REQ-PLAT-038`–`042`, `REQ-VAL-022`, `REQ-TEST-001`–`005`.