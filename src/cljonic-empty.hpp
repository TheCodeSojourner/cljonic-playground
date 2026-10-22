#pragma once

#include <cljonic-concepts.hpp>

namespace cljonic {

/** \anchor Empty
 * The \b empty free function returns a new, empty collection of the same type as its argument. It works with `Vector`,
 * `Map`, `Set`, `Queue`, and `String`; the argument is left unchanged.
 *
 * \b Examples
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 int main() {
   // Compile-time demonstration.
   constexpr auto v_const = vector(1, 2, 3);
   [[maybe_unused]] constexpr auto e_const = empty(v_const);
   static_assert(is_empty(e_const));

   // Runtime demonstration.
   auto v_runtime = vector(10, 20);
   auto e_runtime = empty(v_runtime);
   auto m_runtime = Map{MapEntry{1, 100}};
   auto em_runtime = empty(m_runtime);

   return (is_empty(e_runtime) && count(v_runtime) == 2 &&
           is_empty(em_runtime) && count(m_runtime) == 1)
              ? 0
              : 1;
 }
 ~~~~~
 */
template <concepts::SequenceableCollection C>
[[nodiscard]] constexpr auto empty([[maybe_unused]] const C& collection) noexcept -> C {
    return C{};
}

template <typename C>
    requires(!concepts::SequenceableCollection<C>)
constexpr auto empty([[maybe_unused]] const C& collection) noexcept -> void {
    static_assert(concepts_detail::dependent_false<C>,
                  "cljonic::empty: expected a Vector, Map, Set, Queue, or String value.");
}

} // namespace cljonic
