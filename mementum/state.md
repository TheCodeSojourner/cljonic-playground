## Session State

- last_session_id: 2026-09-07-map-mapentry-review-and-tooling-fix
- current_timestamp: 2026-09-07
- recover: 1
- session_complete: true

Task:
1. Complete the Map requirements-to-documentation increment using the Vector contract as the local template.
2. Add fold-over-assoc/conj pack-literal construction to Map, Set, and Queue (in that order), matching Vector's/String's existing CTAD-deducible pack-construction pattern.
3. Reduce `can_assoc` from a 3-argument free function (`can_assoc(collection, key, value)`) to a 2-argument form (`can_assoc(collection, key)`), removing the value parameter entirely since it never affects the result.
4. Constrain `MapEntry`'s template parameters to match `Map`'s own admission contract.
5. Fix `scripts/spec_weed_check.py`'s stale `expected_ops` table and the deeper bugs discovered alongside it.

Questions:
1. No user questions unresolved.

Decisions:
1. All user-defined stored collection types follow the `NothrowCollectionElement` contract: nothrow default construction, copy construction, copy assignment, and destruction.
2. `Map`'s `KeyType`/`ValueType` template parameters are constrained directly by `concepts::NothrowStableEqualityComparable`/`concepts::NothrowCollectionElement`; the now-redundant `NothrowCopyableElement` `static_assert`s were removed since the template constraints already enforce them.
3. `NothrowStableEqualityComparable` (`StableEqualityComparable<T> && NothrowCollectionElement<T>`) was added to `cljonic-concepts.hpp` as the shared map-key/set-element admission concept, mirroring how `TotallyOrdered` layers onto `StableEqualityComparable`.
4. Dead `VectorElement`/`NothrowVectorElement` backward-compatibility aliases were removed; no source template used them.
5. Vector's template parameters were renamed to PascalCase (`ElementType`, `CapacityValue`, `IndexType`) for consistency with Map/Set/Queue/String/MapEntry and Vector's own `collection_traits` specialization.
6. Capacity-overflow `static_assert` messages were standardized across Map/Set/Queue/String to Vector's `CLJONIC_STRINGIFY`-interpolated form.
7. Redundant constructor member-initializer lists (`: storage_{}, logical_size_{0}` etc.) were removed from Vector/Map/Set/Queue/String since every member already has a matching in-class default member initializer.
8. Map, Set, and Queue previously had no pack-literal/CTAD construction (only `<Type,N>{}` + builder chains); Vector and String already had it. This was a real spec gap, not an oversight — closed by adding `REQ-COLL-018/018A` (Map), `019/019A` (Set), `020/020A` (Queue) and matching `.allium` invariants (`SupportsLiteralDeducedConstruction`, `SupportsCapacityInferredLiteralEquivalentSemantics`, `OversizedInitializerIsCompileTimeFailure`, `PackConstructionFoldsOverAssoc`/`PackConstructionFoldsOverConj`).
9. Pack construction folds the collection's own approved primitive over the argument pack in order: `assoc` for Map (pack elements are `MapEntry<K,V>`; later duplicate key replaces, per `REQ-COLL-004A`), `conj` for Set (later duplicate value is a no-op, per `REQ-COLL-005A`) and Queue (FIFO order matches argument order, no dedup).
10. The oversized-pack compile-time check uses `sizeof...(Args) <= CapacityValue` (pack size, not post-fold deduplicated count) for all three, matching Vector's/String's existing "content length" wording in `REQ-VAL-011`; this is conservative but never allows a real overflow.
11. Map/Set/Queue's separate no-argument default constructor was removed in favor of a single variadic pack constructor accepting zero or more arguments, exactly matching Vector's design (no ambiguity, no separate ctor needed).
12. CTAD deduction guides added: `Map(MapEntry<K,V>, Rest...) -> Map<K,V,1+sizeof...(Rest)>`; `Set(First, Rest...) -> Set<First,...>`; `Queue(First, Rest...) -> Queue<First,...>` (Set/Queue mirror Vector's guide exactly).
13. `Map`'s single unified pack constructor was replaced with two constructors: `constexpr Map() noexcept = default;` for the empty case, and a separate `template <std::same_as<value_type>... Entries> requires(sizeof...(Entries) >= 1) constexpr Map(const Entries&... entries) noexcept`. Unlike Set/Queue/Vector (which legitimately accept any type convertible to their element type), Map's pack elements must be *exactly* `MapEntry<KeyType, ValueType>` (nominal `same_as`, not structural `convertible_to`) and pack-literal construction now requires at least one argument — the empty case is handled only by the separate default constructor, never by this one. Verified empirically that this closes even the hypothetical loophole of a user-defined implicit-conversion type satisfying the old `convertible_to`-based check, that it never competes with copy construction in overload resolution (a `Map` argument doesn't satisfy `same_as<Map<...>, MapEntry<...>>`), and that a bare-scalar pack (`Map<int,int,4>{1,2,3}`) is rejected at compile time with a clear diagnostic. Added `REQ-COLL-018` wording tightening and a new `PackConstructionRequiresAtLeastOneEntry` invariant in `map.allium`.
14. GCC does not reliably treat `requires{ expr; }` as SFINAE-friendly when `expr` involves overload resolution against a constrained template/constructor selected via brace-init; recorded in `mementum/memories/requires-expression-constrained-template-sfinae-limitation.md`. Negative compile-time-failure checks in this repo are documented via `TRACE_ID` only, never via an actual negative-compile assertion.
15. `can_assoc` was reduced from `can_assoc(collection, key, value)` (3 args) to `can_assoc(collection, key)` (2 args), and `Map`'s corresponding 2-arg member `can_assoc(key, value)` was deleted, keeping only the 1-arg `can_assoc(key)` member. Rationale: `assoc`'s success/failure never depends on the value being associated (only on whether the key already exists or capacity remains), so the value parameter was pure noise — confirmed by the fact the old 2-arg member already ignored it (`/*value*/`). This is a real, deliberate API-shape change (not a dead-code cleanup), requiring `REQ-FN-002M` wording revision, the architecture preflight-signature table, and the `CanAssoc` vocabulary example, propagated alongside code/tests.
16. `MapEntry<KeyType, ValueType>`'s template parameters were unconstrained (`template <typename KeyType, typename ValueType>`) despite `REQ-COLL-001A` already requiring "map-entry fields" to satisfy `NothrowCollectionElement`, and `map-entry.allium` already declaring `RequiresNothrowDefaultConstruction`/`CopyConstruction`/`CopyAssignment`/`Destruction` invariants — none of it was actually enforced by the code. Fixed by constraining `MapEntry` with the exact same concepts Map already requires of its own `KeyType`/`ValueType`: `concepts::NothrowStableEqualityComparable KeyType, concepts::NothrowCollectionElement ValueType`. Fully backward-compatible (every existing `Map<K,V,N>` already satisfies this on its own template parameters); additionally closes a real gap where a standalone `MapEntry<double, int>{...}` could previously be constructed with a floating-point key despite `REQ-NUM-002` prohibiting floating-point map keys. Added a clarifying sentence to `REQ-COLL-001A` and a new `RequiresNothrowCollectionElementKeyAndValueAdmission` invariant in `map-entry.allium`, mirroring Map's own invariant name.
17. `scripts/spec_weed_check.py` (the `/gybis-spec-weed` support tool) had four real bugs, not just a stale `expected_ops` table: (a) the invariant-extraction regex expected `invariant.Name {` but actual `.allium` syntax is `invariant Name {`, silently producing 0 invariants for every entity and making the spec→test coverage check permanently, vacuously "converged"; (b) `MapEntry` (a `struct` with no explicit `public:` label) was invisible to the method-detection logic, which required literally finding `public:` in the file; (c) class-name extraction grabbed the *first* `class`/`struct` keyword anywhere in the raw file, including inside Doxygen-comment example code (`struct Key`/`struct Pixel` in Map's/Vector's own doc examples), misidentifying the real class; (d) `"map-entry".capitalize()` produced `"Map-entry"` instead of `"MapEntry"`, never matching real trace IDs (a dormant `IndexError` crash, unreachable until bug (a) was fixed). All four fixed; script now reports accurate, clean, zero-divergence output across all 6 collection entities.

Validation:
1. `allium check specs` and `allium analyse specs`: zero diagnostics and findings across all specifications.
2. `make traceability-spec-to-code-update-snapshot` and `make traceability-spec-to-code`: passed.
3. `make no-heap`: passed, including source and symbol scans.
4. `make docs-examples`: passed for all updated runnable examples.
5. `make cljonic`: passed and regenerated the synchronized single header.
6. `make docs`: passed and regenerated the Doxygen site.
7. `make format`: passed.
8. `make all`: passed, 104/104 tests throughout. Map spec-test assertions: 49→58 (pack construction) → 57 (after removing the deleted 2-arg `can_assoc(key,value)` member test line).
9. `python3 scripts/spec_weed_check.py`: now reports 48/48, 21/21, 45/45, 46/46, 43/43, 43/43 invariants traced (Map, MapEntry, Queue, Set, String, Vector) and zero method-surface divergences, replacing a previously always-vacuously-passing check.

Current Increment:
1. None open; this session's tasks are complete.

Current Increment Validation:
1. See Validation above; all gates green after the full Map → Set → Queue propagation, the subsequent Map constructor tightening, the MapEntry admission fix, and the weed-check tooling repair.

Next:
1. `Set`'s element template parameter (`concepts::StableEqualityComparable T`) should be tightened to `concepts::NothrowStableEqualityComparable` for full consistency with Map's `KeyType`, when Set next gets a dedicated review pass (recorded previously in `mementum/memories/nothrow-stable-equality-comparable-concept.md`).
2. Consider whether Vector deserves its own dedicated `REQ-COLL-0xx` pack-construction requirement (it currently relies only on the generic `REQ-VAL-011/012/013`), for parity with String/Map/Set/Queue's dedicated requirements — noted but not actioned since it wasn't requested.
3. `MapEntry::contains(index)` is formally specified (`REQ-SEQ-013`, `SupportsIndexedLookup` invariant) but has zero callers anywhere in the codebase outside its own test — flagged as a candidate for a future spec-simplification decision, not resolved.
4. Continue the Map/Vector-style staged review for any remaining collection-family gaps (Set, Queue, String) as directed.

## Historical Session Records

Task:
1. Complete Phase B: add concept spec tests covering the 46 obligations in `specs/capabilities/concepts.allium` (`tests/cljonic-concepts-spec-tests.cpp`) plus no-heap probe (`tests/no_heap/cljonic-concepts-probes.cpp`), then reconcile the spec-to-code traceability snapshot.
2. Refactor the value capability concepts to prefer standard concepts (`std::equality_comparable`, `std::floating_point`, `std::totally_ordered`) over manual requires-clauses and trait predicates; drop now-unused `#include <utility>`.
3. Unify the membership predicate to Clojure `contains?` parity: rename `valid_index`/String `valid()` → `contains` across all collection kinds (map key presence, set element presence, vector/string index-in-range), add free `contains(c, x)` in `src/cljonic-contains.hpp`, add `specs/primitives/contains.allium`, and remove `ValidIndex`/`valid_index` from vocabulary.
4. Validate end-to-end coherence via gybis-vocab-check, gybis-arch-check, gybis-vocab-weed, gybis-spec-check, gybis-arch-weed, gybis-spec-weed.

Questions:
1. None unresolved.

Decisions:
1. Value capability concepts now use standard concepts: `StableEqualityComparable = std::equality_comparable<T> && !std::floating_point<std::remove_cvref_t<T>>`; `TotallyOrdered = StableEqualityComparable<T> && std::totally_ordered<T>`.
2. `contains` is the single Clojure `contains?`-parity membership predicate: map key presence, set element presence, vector/string index-in-range. `IndexedCollection` requires `c.contains(i)` (replacing `c.valid_index(i)`); `AssociativeCollection` keeps `c.contains(k)`.
3. Free `contains(c, x)` is a non-allocating, non-throwing forwarder to `collection.contains(x)`; lives in `src/cljonic-contains.hpp`, included by `cljonic-core.hpp`.
4. Added `specs/primitives/contains.allium` (10 obligations incl. `ContainsDoesNotPerformDefaultReturningAccess`); renamed `ValidIndexIsCanonicalIndexPredicate` → `ContainsIsCanonicalIndexPredicate` (vector/string) and `RequiresValidIndexPredicate`/`UsesValidIndexPredicate` → `RequiresContainsPredicate`/`UsesContainsPredicate` (concepts).
5. Removed the `ValidIndex` vocabulary term and all `valid_index`/`ValidIndex` references; kept `ValidPredicatePrefix` (distinct: general `valid_` prefix).
6. Full strict gate (`make upsert-gate-strict`) and 80/80 tests pass; traceability snapshot regenerated.

Next:
1. Phase C: containers conform to count/is_empty/operators (complete the container member surface so actual containers satisfy `SequenceableCollection`/`IndexedCollection`/`AssociativeCollection`).
2. Phase D: constrain primitive free functions with the concept layer.
3. Phase E: full-stack verification via `make git`.

## Historical Session Records

Task:
1. Reconfigure project formatting with a repo-root `.clang-format` file tuned for AI/parser token density and reformat the codebase.
2. Upgrade clang-format from 18 to 20 (Ubuntu 24.04 apt `clang-format-20`; symlink `/usr/bin/clang-format`).
3. Verify the reconfiguration end-to-end (`make format`, `make lint`, `make test`).

Questions:
1. None unresolved.

Decisions:
1. `.clang-format` keys: `Standard: c++20`, `BreakBeforeBraces: Attach`, `BinPackArguments/Parameters: true`, `SortIncludes: CaseSensitive`, `RequiresClausePosition: OwnLine`, `IndentRequiresClause: true`, `ColumnLimit: 120`, `IndentWidth: 4`, `PointerAlignment: Left`, `SpaceBeforeParens: ControlStatements`, `AllowShort*: false`, `AlwaysBreakTemplateDeclarations: true` (Allman dropped).
2. `make format` and `make format-doc-samples` both invoke `clang-format` with no `-style` flag, so raw C/C++ and Doxygen example blocks share the root `.clang-format`.
3. clang-format 20.1.2 confirmed active after upgrade; `make format:ok`, `make lint:ok`, `make test` 54/54 pass.
4. All source/test files reformatted to the new style (large working-tree diff).

Next:
1. Commit the reformatting session: `.clang-format` + reformatted `src/`/`tests/` + mementum state/memory.

## Historical Session Records

Task:
1. Align vocabulary.md and architecture.md with the C++ Core Guidelines concept-layer naming and rewrite architecture prose into lambda notation; all gybis-vocab-check, gypis-arch-check, and gypis-vocab-weed gates pass.
2. Establish 1:1 artifact granularity across specifications (`specs/primitives/*.allium`), headers (`src/cljonic-*.hpp` with dual compile-time/runtime Doxygen examples), tests (`tests/cljonic-*-spec-tests.cpp` with runtime volatile coverage), and dedicated probe files (`tests/no_heap/*.cpp`).
3. Ensure 100% line coverage visibility across all 22 source headers in `make coverage` and `coverage-src.info`.
4. Synthesize session learnings into `mementum/knowledge/header-addition-and-verification-lifecycle.md` and optimize `CMakeLists.txt` with `file(GLOB ... CONFIGURE_DEPENDS)`.

Questions:
1. None unresolved.

Decisions:
1. Group A and Group B primitive free functions are fully specified in Allium, implemented in dedicated headers, tested with Catch2, and verified across all strict quality gates.
2. Every header in `src/` provides runnable `main()` Doxygen examples with compile-time and runtime demonstrations across supported collection types.
3. Every header has a dedicated no-heap probe file under `tests/no_heap/` and appears in the coverage report at 100% line coverage.
4. `CMakeLists.txt` uses `file(GLOB ... CONFIGURE_DEPENDS)` for automatic test and probe source discovery.
5. All strict quality gates (`make upsert-gate-strict`, `make no-heap`, `make coverage-cli`) pass cleanly.

Next:
1. Proceed with the next increment of Module 3 free functions (`last`, `key`, `val`, `contains`, `nth`, comparisons `equal`/`not_equal`/etc.) or downstream Module 4 requirements as requested.

## Historical Session Records

Task:
1. Optimize `CMakeLists.txt` to replace manual source enumerations with `file(GLOB ... CONFIGURE_DEPENDS)` for both `CLJONIC_SPEC_TEST_SOURCES` and `CLJONIC_NO_HEAP_SOURCES`.
2. Validate that all quality gates (`make upsert-gate-strict`, `make no-heap`, `make coverage-cli`) pass cleanly with automatic source discovery.

Questions:
1. None unresolved.

Decisions:
1. Replaced static file lists in `CMakeLists.txt` with:
   - `file(GLOB CLJONIC_SPEC_TEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/tests/cljonic-*-spec-tests.cpp")`
   - `file(GLOB CLJONIC_NO_HEAP_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/tests/no_heap/*.cpp")`
2. `CONFIGURE_DEPENDS` guarantees CMake automatically detects added, deleted, or renamed test/probe files without requiring manual `CMakeLists.txt` editing.
3. Verified all strict quality gates pass with 100% coverage and zero heap allocation.

Next:
1. Ready for next increment.

## Historical Session Records

Task:
1. Verify and reconcile test/code implementations against specifications, architecture, and requirements for Modules 1-3 using gybis-spec-weed.
2. Resolve test coverage gap in MapEntry negative-index guard and eliminate false positive in no-heap scanner for String documentation comment.
3. Validate complete repository state across all quality gates (format, lint, complexity, sanitizers, coverage, traceability, no-heap, docs).

Task:
1. Update architecture.md with Module 3 core collection types, storage strategies, and primitive free functions.
2. Verify architecture.md internal integrity and consistency with vocabulary.md and Module 3 requirements using gybis-arch-check and gybis-vocab-weed.

Questions:
1. None unresolved.

Decisions:
1. Module 3 defines concrete collection types (Vector, Map, Set, Queue, String), contiguous array-backed storage, linear scan lookup, swap-and-remove policies, and primitive free functions (conj, assoc, dissoc, disj, peek, pop, seq, can_conj, can_assoc).
2. architecture.md successfully updated and verified through gybis-arch-check with 0 errors, 0 warnings, and 0 divergences.
3. Quality gates (make upsert-gate-strict) pass with 100% line coverage, no-heap verification, linting, complexity, sanitizers, and traceability.

Next:
1. Proceed with downstream Module 3 specifications (specs/collections/*.allium) or test/code implementation for Map, Set, Queue, String, and primitive free functions.

## Historical Session Records

Task:
1. Update vocabulary.md with Module 3 core collection types, storage strategies, and primitive free functions.
2. Deduplicate collection entries (Map, Set, Queue, String) in vocabulary.md.
3. Validate vocabulary.md using gybis-vocab-check to ensure syntax, completeness, and semantic integrity pass with 0 errors and 0 warnings.

Questions:
1. None unresolved.

Decisions:
1. Module 3 defines concrete collection types (Vector, Map, Set, Queue, String), contiguous array-backed storage, linear scan lookup, swap-and-remove policies, and primitive free functions (conj, assoc, dissoc, disj, peek, pop, seq, can_conj, can_assoc).
2. All collection types are unified into single canonical entries in vocabulary.md.
3. vocabulary.md validation passes cleanly with 0 errors and 0 warnings.

Next:
1. Proceed with downstream Module 3 artifacts (specs or architecture refinement) as requested.



