#ifndef CLJONIC_POP_HPP
#define CLJONIC_POP_HPP

#include <cljonic-concepts.hpp>
#include <utility>

namespace cljonic {

/** \anchor Pop
 * \brief Yields an updated collection with the accessible element removed.
 *
 * \b Examples
 * ~~~~~{.cpp}
 * #include <cljonic.hpp>
 *
 * int main() {
 *   using namespace cljonic;
 *
 *   constexpr auto q0 = conj(conj(Queue<int, 4>{}, 1), 2);
 *   constexpr auto q1 = pop(q0);
 *   static_assert(peek(q1) == 2);
 *
 *   return 0;
 * }
 * ~~~~~
 */
template <typename C>
[[nodiscard]] constexpr auto pop(const C &collection) noexcept {
  return collection.pop();
}

} // namespace cljonic

#endif // CLJONIC_POP_HPP
