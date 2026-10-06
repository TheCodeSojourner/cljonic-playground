#pragma once

#include <cljonic-concepts.hpp>

namespace cljonic {

/** \anchor Seq
 * \brief Converts a collection into an owning, value-semantic sequence.
 *
 * For a Vector this returns the owning vector itself; for other sequenceable collections it produces an owning Vector
 * of their traversal elements.
 *
 * \note Not yet part of the public API; the sequence-shaping operations are deferred.
 *
 * \b Examples
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 int main() {
   // Compile-time demonstration.
   constexpr auto v_const = Vector<int, 4>{1, 2, 3};
   constexpr auto s_const = seq(v_const);
   static_assert(count(s_const) == 3U);

   // Runtime demonstration.
   auto v_runtime = Vector<int, 4>{10, 20};
   auto s_runtime = seq(v_runtime);

   return (count(s_runtime) == 2U) ? 0 : 1;
 }
 ~~~~~
 */
template <typename C>
[[nodiscard]] constexpr auto seq(const C& collection) noexcept -> C {
    return collection;
}

} // namespace cljonic
