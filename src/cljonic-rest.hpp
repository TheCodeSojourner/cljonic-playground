#pragma once

#include <cljonic-concepts.hpp>

namespace cljonic {

/** \anchor Rest
 * \brief Returns a collection without its first/accessible element.
 *
 * For a Queue, this removes the front element. Unlike `next`, `rest` returns an empty collection rather than signaling
 * absence on an empty input.
 *
 * \note Not yet part of the public API; the sequence-shaping operations are deferred.
 *
 * \b Examples
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 int main() {
   // Compile-time demonstration.
   constexpr auto q0_const = conj(conj(Queue<int, 4>{}, 1), 2);
   constexpr auto q1_const = rest(q0_const);
   static_assert(peek(q1_const) == 2);

   // Runtime demonstration.
   auto q0_runtime = conj(conj(Queue<int, 4>{}, 100), 200);
   auto q1_runtime = rest(q0_runtime);

   return (peek(q1_runtime) == 200) ? 0 : 1;
 }
 ~~~~~
 */
template <typename C>
[[nodiscard]] constexpr auto rest(const C& collection) noexcept -> decltype(collection.pop()) {
    return collection.pop();
}

} // namespace cljonic
