#ifndef CLJONIC_DISJ_HPP
#define CLJONIC_DISJ_HPP

#include <cljonic-concepts.hpp>
#include <utility>

namespace cljonic {

/** \anchor Disj
 * \brief Disjoins an element from a set.
 *
 * \b Examples
 * ~~~~~{.cpp}
 * #include <cljonic.hpp>
 *
 * int main() {
 *   using namespace cljonic;
 *
 *   constexpr auto s0 = conj(Set<int, 4>{}, 42);
 *   constexpr auto s1 = disj(s0, 42);
 *   static_assert(!s1.contains(42));
 *
 *   return 0;
 * }
 * ~~~~~
 */
template <typename C, typename T>
[[nodiscard]] constexpr auto disj(const C &collection,
                                  const T &value) noexcept {
  return collection.disj(value);
}

} // namespace cljonic

#endif // CLJONIC_DISJ_HPP
