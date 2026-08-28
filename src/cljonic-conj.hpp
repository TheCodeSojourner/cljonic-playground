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
 *   // Compile-time demonstration.
 *   constexpr auto q_const = conj(Queue<int, 4>{}, 10);
 *   constexpr auto s_const = conj(Set<int, 4>{}, 20);
 *   static_assert(peek(q_const) == 10);
 *   static_assert(s_const.contains(20));
 *
 *   // Runtime demonstration.
 *   auto q_runtime = Queue<int, 4>{};
 *   auto q1 = conj(q_runtime, 100);
 *   auto s_runtime = Set<int, 4>{};
 *   auto s1 = conj(s_runtime, 200);
 *
 *   return (peek(q1) == 100 && s1.contains(200)) ? 0 : 1;
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
