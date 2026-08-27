#ifndef CLJONIC_CAN_CONJ_HPP
#define CLJONIC_CAN_CONJ_HPP

#include <cljonic-concepts.hpp>
#include <utility>

namespace cljonic {

/** \anchor CanConj
 * \brief Checks if conj can succeed without capacity overflow.
 *
 * \b Examples
 * ~~~~~{.cpp}
 * #include <cljonic.hpp>
 *
 * int main() {
 *   using namespace cljonic;
 *
 *   constexpr Queue<int, 4> q{};
 *   static_assert(can_conj(q));
 *
 *   constexpr Set<int, 4> s{};
 *   static_assert(can_conj(s, 1));
 *
 *   return 0;
 * }
 * ~~~~~
 */
template <typename C, typename... Args>
[[nodiscard]] constexpr auto can_conj(const C &collection,
                                      Args &&...args) noexcept -> bool {
  return collection.can_conj(std::forward<Args>(args)...);
}

} // namespace cljonic

#endif // CLJONIC_CAN_CONJ_HPP
