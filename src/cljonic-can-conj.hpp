#pragma once

#include <cljonic-concepts.hpp>

namespace cljonic {

/** \anchor CanConj
 * \brief Checks whether conj can succeed without capacity overflow.
 *
 * For a `Set` element or `Map` key already present, returns true because insertion is a no-op or a value replacement
 * that needs no capacity; for `Vector` and `Queue` the result depends only on remaining capacity.
 *
 * \b Examples
 ~~~~~{.cpp}
 #include "cljonic.hpp"

 int main() {
   using namespace cljonic;

   // Compile-time demonstration.
   constexpr Queue<int, 4> q_const{};
   static_assert(can_conj(q_const, 1));
   constexpr Set<int, 4> s_const{};
   static_assert(can_conj(s_const, 1));
   constexpr auto m_const = assoc(Map<int, int, 4>{}, 1, 100);
   static_assert(can_conj(m_const, MapEntry<int, int>{1, 999}));

   // Runtime demonstration.
   auto q_runtime = Queue<int, 4>{};
   const auto q_ok = can_conj(q_runtime, 1);

   return q_ok ? 0 : 1;
 }
 ~~~~~
 */
template <typename C, typename V>
    requires concepts::ConjableCollection<C> && requires(const C& collection, const V& value) {
        { collection.can_conj(value) } noexcept -> std::same_as<bool>;
    }
[[nodiscard]] constexpr auto can_conj(const C& collection,
                                      const V& value) noexcept(noexcept(collection.can_conj(value))) -> bool {
    return collection.can_conj(value);
}

} // namespace cljonic
