# Restart prompt for next session

Use this prompt in a new chat session:

Continue the cljonic C++26 library design discussion using DESIGN-NOTES.md and NEXT-AGENDA.md in this repo as the source of truth.

Constraints already decided:
- Embedded target.
- No heap allocation.
- No exceptions.
- Data structures based on std::array.
- Immutability via value-style copy-on-modify semantics.
- No explicit error returns; invalid/missing access returns default sentinel `T{}`.
- Keywords are global-only (no namespaces).
- Keyword representation is enum catalog.
- Map keys may be non-keyword; if keyword keys are used they must be from the enum catalog.
- Small collection sizes; linear key/value search is acceptable.
- Canonical comparison API names are `equal`, `less`, `less_equal`, `greater`, `greater_equal`.
- Canonical comparisons are variadic with chained semantics.
- Zero-arity and one-arity comparison calls return `true`.
- `equal` is deep and supports nested collection value equality.
- Floating-point comparisons are forbidden in canonical APIs (compile-time error).
- Dynamic value type excludes floating-point entirely.
- Any float comparison override must be explicit per call via comparator-based APIs (for example `equal_with`); implicit override is disallowed.
- Vector, set, map, and string share the same constructor pattern: literal-deduced and explicit-capacity forms.
- Explicit-capacity empty construction is valid.
- Explicit-capacity construction must fail at compile time when initializer count exceeds capacity.
- Lazy sequence behavior is default where practical, especially for range, repeat, and cycle.
- Lazy views are value-owning.
- Materialization must remain explicit at sink operations only.
- Clojure parity is a priority for lazy sequence semantics where feasible.
- Canonical `interleave` is lazy and shortest-terminating.
- Range is lazy by default and uses explicit finite or infinite cardinality metadata.
- Infinite range does not use max size_t as canonical size.
- Range precomputes internal fields at compile time when inputs are compile-time constants, otherwise once at runtime.
- Step zero normalizes to an empty finite range.
- CTRE is the selected compile-time regex library for optional regex support.
- Runtime regex is an opt-in profile for Clojure-style `re-pattern` and `re-matcher`.
- Invalid runtime pattern compilation returns an invalid pattern sentinel.
- `pattern_valid(p)` is available to explicitly check pattern validity.
- High-fit namespace roadmap is locked with this MVP set: clojure.string, clojure.set, clojure.data, clojure.walk, clojure.zip.
- Optional namespace set is: clojure.edn, clojure.math, clojure.uuid, clojure.instant, clojure.xml, clojure.core.reducers, clojure.parallel.
- C++26 usage guidelines are locked: contracts at API/invariant boundaries with deterministic non-throwing violation handling, semantic concepts for compile-time API constraints, functional pipeline style with lazy transforms and explicit sinks, and ARM profile-specific optimization based on measured workloads.
- Packaging is locked as header-only with separate production and tests directories, one function per production implementation header file, and automatic amalgamation of production headers into one distributable single header.
- Quality toolchain is locked: Catch2 with generator-based test patterns, host-profile sanitizer usage, CI linting gates, Doxygen with sample code per public API, automatic testing of documented samples, automatic Doxygen HTML generation, MVP-core 100% line coverage policy with documented exclusions, and layered no-heap verification checks.
- Map/set MVP key model is locked: keyword enum, integral, enum, and fixed-capacity static string keys.
- User-defined key types are optional via explicit non-MVP concept gates.
- Sentinel policy is locked: missing or invalid access returns `T{}` and no explicit error/status channel is added.
- Probe-first guidance is locked for ambiguous domains: `contains` plus `get`, and `has_index` plus indexed `get`.
- Core collection API style is locked as free-function-first.
- Canonical MVP collection operations are locked: `get`, `assoc`, `dissoc`, `conj`, `contains`, `count`, `first`, `rest`.
- Full-capacity `assoc` and `conj` semantics are locked: deterministic unchanged return.
- Comparator override direction is locked as explicit per-call `_with` APIs (for example `equal_with`).

Important working style:
- Discussion only unless I explicitly ask for code or file changes.
- Do not scaffold files or run builds unless asked.

Please start by proposing the next top 3 design decisions to finalize after the newly locked API and policy decisions above, with tradeoffs for each.
