---
type: Insight
symbol: 💡
title: capability-concept-member-types-belong-in-body
related: [use-concepts-and-hide-traits-in-cljonic-concepts.md, requires-expression-constrained-template-sfinae-limitation.md, two-level-concept-model.md, rejection-diagnostic-fallback.md]
---
Naming a member type in a requires-expression **parameter list** makes the
concept eager and hard-erroring, not SFINAE-friendly:

    concept LookupCollection = CljonicCollection<C> &&
      requires(const C& c, const C::lookup_type& key) { ... };   // HARD ERROR

For a type lacking `lookup_type` (e.g. `Queue`), substitution of the parameter
list fails with `error: no type named 'lookup_type'` — a compiler error, not
`false`. A free function constrained on such a concept (e.g. `get`, which uses
`LookupCollection<C> || IndexedCollection<C>`) therefore breaks SFINAE and emits
a cascade instead of a clean rejection.

Fix: put the member type in the requires-expression **body**, referenced through
`std::declval`:

    concept LookupCollection = CljonicCollection<C> &&
      requires(const C& c) {
        typename C::lookup_type;
        { c(std::declval<const typename C::lookup_type&>()) };
        { c.contains(std::declval<const typename C::lookup_type&>()) } -> std::same_as<bool>;
      };

Same fix applied to `AssociativeCollection` (which also used `value_type` where
the implementation needs `association_value_type` — for `Map`,
`value_type = MapEntry<K,V>` but `assoc` takes the mapped `V`).

Verified GCC 16.2 + clang 22.1, 2026-10-06. A `std::void_t` sentinel variant also
works but yields a vaguer message; the body form reports the invalid required
type by name. Detection must use the named concept directly (bare `false`), not
a nested `requires { call(...) }`.
