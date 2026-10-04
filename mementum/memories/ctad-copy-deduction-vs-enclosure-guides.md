---
type: Insight
symbol: 💡
title: ctad-copy-deduction-vs-enclosure-guides
related: [/mementum/knowledge/collection-source-interoperability.md, /mementum/memories/same-type-constructor-pack-preference.md]
---
Clojure-style enclosure (`[v]` wraps `v` as one element) is achievable in
cljonic via a user-defined deduction guide, despite C++'s "copy over wrap"
default.

Standard rules ([over.match.class.deduct], [over.match.best], P0702R1, C++17+):
- CTAD always adds a **copy deduction candidate** `C(C)`.
- `std::vector v2{v1};` deduces `vector<int>` (copy), not `vector<vector<int>>`.
- BUT a user guide *as specialized as* the copy candidate, formed from a
  user-defined guide, is preferred over implicitly-generated candidates. The
  standard's own example `A(A<T>) -> A<A<T>>; A b2 = a;` wraps.

Verified on GCC 16.2 and clang 22.1 (both agree):
- `Vector{Vector<int,2>{...}}` (no guide) -> `Vector<int,2>` copy (count 2).
- With `template<class T, size_t N> Vector(Vector<T,N>) -> Vector<Vector<T,N>,1>`
  -> `Vector<Vector<int,2>,1>` nest (count 1).
- Cross-class (`Vector{Set<...>}`, `Set{Range{...}}`) already nests via the pack
  guide (`First, Rest...`).
- Scalars unaffected (`Vector{1,2,3}` -> `Vector<int,3>`).

Consequence: explicit-type construction `Vector<int,2>{v}` still uses the copy
constructor (an explicit template-argument list suppresses CTAD); a *mismatched*
explicit type (`Vector<int,4>{some_vector}`) is rejected by the narrowed source
constructor (REQ-FN-027A). Enclosure is the CTAD form; copying is the same-type
copy form.
