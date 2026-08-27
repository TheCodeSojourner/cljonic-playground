#ifndef CLJONIC_PEEK_HPP
#define CLJONIC_PEEK_HPP

#include <cljonic-concepts.hpp>
#include <utility>

namespace cljonic {

/** \anchor Peek
 * \brief Observes the accessible element without removal.
 *
 * \b Examples
 * ~~~~~{.cpp}
 * #include <cljonic.hpp>
 *
 * int main() {
 *   using namespace cljonic;
 *
 *   constexpr auto q = conj(Queue<int, 4>{}, 99);
 *   static_assert(peek(q) == 99);
 *
 *   return 0;
 * }
 * ~~~~~
 */
template <typename C>
[[nodiscard]] constexpr auto peek(const C &collection) noexcept {
  return collection.peek();
}

} // namespace cljonic

#endif // CLJONIC_PEEK_HPP
