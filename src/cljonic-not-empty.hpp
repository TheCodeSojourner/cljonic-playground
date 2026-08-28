#ifndef CLJONIC_NOT_EMPTY_HPP
#define CLJONIC_NOT_EMPTY_HPP

#include <cljonic-concepts.hpp>

namespace cljonic {

/** \anchor NotEmpty
 * \brief Returns an owning copy of the input when nonempty, otherwise the
 * corresponding empty owning value.
 *
 * Per REQ-FN-026, `not_empty` preserves the input collection type and capacity
 * and returns an owning copy of the input when nonempty or the corresponding
 * empty value when empty.
 *
 * \b Examples
 * ~~~~~{.cpp}
 * #include <cljonic.hpp>
 *
 * int main() {
 *   using namespace cljonic;
 *
 *   // Compile-time demonstration.
 *   constexpr auto v_const = Vector<int, 4>{1};
 *   constexpr auto e_const = Vector<int, 4>{};
 *   static_assert(count(not_empty(v_const)) == 1U);
 *   static_assert(count(not_empty(e_const)) == 0U);
 *
 *   // Runtime demonstration.
 *   auto v_runtime = Vector<int, 4>{10};
 *   auto nv_runtime = not_empty(v_runtime);
 *
 *   return (count(nv_runtime) == 1U) ? 0 : 1;
 * }
 * ~~~~~
 */
template <typename C>
[[nodiscard]] constexpr auto not_empty(const C &collection) noexcept -> C {
  return collection.empty() ? C{} : C{collection};
}

} // namespace cljonic

#endif // CLJONIC_NOT_EMPTY_HPP