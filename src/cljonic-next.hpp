#ifndef CLJONIC_NEXT_HPP
#define CLJONIC_NEXT_HPP

#include <cljonic-concepts.hpp>

namespace cljonic {

/** \anchor Next
 * \brief Returns a collection without its first element.
 *
 * For a Queue, `next` dequeues the front element (the accessible element) and
 * returns the remaining collection.
 *
 * \b Examples
 * ~~~~~{.cpp}
 * #include <cljonic.hpp>
 *
 * int main() {
 *   using namespace cljonic;
 *
 *   // Compile-time demonstration.
 *   constexpr auto q0_const = conj(conj(conj(Queue<int, 4>{}, 1), 2), 3);
 *   constexpr auto q1_const = next(q0_const);
 *   static_assert(peek(q1_const) == 2);
 *
 *   // Runtime demonstration.
 *   auto q0_runtime = conj(conj(Queue<int, 4>{}, 10), 20);
 *   auto q1_runtime = next(q0_runtime);
 *
 *   return (peek(q1_runtime) == 20) ? 0 : 1;
 * }
 * ~~~~~
 */
template <typename C>
[[nodiscard]] constexpr auto next(const C& collection) noexcept -> decltype(collection.pop()) {
    return collection.pop();
}

} // namespace cljonic

#endif // CLJONIC_NEXT_HPP