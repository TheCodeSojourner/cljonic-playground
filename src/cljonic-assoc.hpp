#pragma once

#include <cljonic-concepts.hpp>

namespace cljonic {

/** \anchor Assoc
 * \brief Associates a value with a key in a Map, or with an index in an indexed
 *        collection (Vector, String).
 *
 * Supported for `Map` (key/value association), `Vector` (index association; appends at the logical count when capacity
 * remains), and `String` (index association, applying the invalid-character policy). `Set` and `Queue` provide no
 * associative capability and are rejected by the boundary constraint. The result is a distinct collection value; the
 * source is unchanged.
 *
 * \b Examples
 ~~~~~{.cpp}
 #include "cljonic.hpp"

 int main() {
   using namespace cljonic;

   // Compile-time demonstration.
   constexpr auto m_const = assoc(Map<int, int, 4>{}, 1, 100);
   static_assert(m_const(1) == 100);

   constexpr auto v_const = assoc(Vector<int, 4>{10, 20}, 1, 200);
   static_assert(v_const(1) == 200);

   // Runtime demonstration.
   auto m_runtime = Map<int, int, 4>{};
   const auto m1 = assoc(m_runtime, 2, 200);

   return (contains(m1, 2) && m1(2) == 200) ? 0 : 1;
 }
 ~~~~~
 */
template <typename C, typename K, typename V>
    requires concepts::AssociativeCollection<C> && requires(const C& collection, const K& key, const V& value) {
        { collection.assoc(key, value) } noexcept -> std::same_as<C>;
    }
[[nodiscard]] constexpr auto assoc(const C& collection, const K& key,
                                   const V& value) noexcept(noexcept(collection.assoc(key, value))) -> C {
    return collection.assoc(key, value);
}

} // namespace cljonic
