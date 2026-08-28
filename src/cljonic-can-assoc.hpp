#ifndef CLJONIC_CAN_ASSOC_HPP
#define CLJONIC_CAN_ASSOC_HPP

#include <cljonic-concepts.hpp>
#include <utility>

namespace cljonic {

/** \anchor CanAssoc
 * \brief Checks if assoc can succeed without capacity overflow.
 *
 * \b Examples
 * ~~~~~{.cpp}
 * #include <cljonic.hpp>
 *
 * int main() {
 *   using namespace cljonic;
 *
 *   // Compile-time demonstration.
 *   constexpr Map<int, int, 4> m_const{};
 *   static_assert(can_assoc(m_const, 1, 100));
 *
 *   // Runtime demonstration.
 *   auto m_runtime = Map<int, int, 4>{};
 *   const auto ok = can_assoc(m_runtime, 2, 200);
 *
 *   return ok ? 0 : 1;
 * }
 * ~~~~~
 */
template <typename C, typename K, typename V>
[[nodiscard]] constexpr auto can_assoc(const C &collection, const K &key,
                                       const V &value) noexcept -> bool {
  return collection.can_assoc(key, value);
}

} // namespace cljonic

#endif // CLJONIC_CAN_ASSOC_HPP
