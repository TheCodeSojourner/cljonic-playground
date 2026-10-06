#pragma once

#include <cljonic-concepts.hpp>

namespace cljonic {

/** \anchor CanAssoc
 * \brief Checks if assoc can succeed without capacity overflow.
 *
 * \b Examples
 ~~~~~{.cpp}
 #include "cljonic.hpp"

 int main() {
   using namespace cljonic;

   // Compile-time demonstration.
   constexpr Map<int, int, 4> m_const{};
   static_assert(can_assoc(m_const, 1));

   // Runtime demonstration.
   auto m_runtime = Map<int, int, 4>{};
   const auto ok = can_assoc(m_runtime, 2);

   return ok ? 0 : 1;
 }
 ~~~~~
 */
template <typename C, typename K>
    requires concepts::AssociativeCollection<C> && requires(const C& collection, const K& key) {
        { collection.can_assoc(key) } noexcept -> std::same_as<bool>;
    }
[[nodiscard]] constexpr auto can_assoc(const C& collection, const K& key) noexcept(noexcept(collection.can_assoc(key)))
    -> bool {
    return collection.can_assoc(key);
}

} // namespace cljonic
