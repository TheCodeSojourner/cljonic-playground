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
 *   // Compile-time demonstration.
 *   constexpr auto q0_const = conj(conj(Queue<int, 4>{}, 1), 2);
 *   constexpr auto q1_const = pop(q0_const);
 *   static_assert(peek(q1_const) == 2);
 *
 *   // Runtime demonstration.
 *   auto q0_runtime = conj(conj(Queue<int, 4>{}, 10), 20);
 *   auto q1_runtime = pop(q0_runtime);
 *
 *   return (peek(q1_runtime) == 20) ? 0 : 1;
 * }
 * ~~~~~
 */
template <typename C>
[[nodiscard]] constexpr auto pop(const C& collection) noexcept {
    return collection.pop();
}

} // namespace cljonic

#endif // CLJONIC_POP_HPP
