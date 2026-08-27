#ifndef CLJONIC_CONJ_HPP
#define CLJONIC_CONJ_HPP

#include <cljonic-concepts.hpp>
#include <utility>

namespace cljonic {

/** \anchor Conj
 * \brief Adds an element to a collection according to its type conventions.
 *
 * \b Examples
 * ~~~~~{.cpp}
 * #include <cljonic.hpp>
 *
 * int main() {
 *   using namespace cljonic;
 *
 *   constexpr auto q = conj(Queue<int, 4>{}, 10);
 *   constexpr auto s = conj(Set<int, 4>{}, 20);
 *
 *   static_assert(peek(q) == 10);
 *   static_assert(s.contains(20));
 *
 *   return 0;
 * }
 * ~~~~~
 */
template <typename C, typename T>
[[nodiscard]] constexpr auto conj(const C &collection,
                                  const T &value) noexcept {
  return collection.conj(value);
}

} // namespace cljonic

#endif // CLJONIC_CONJ_HPP
