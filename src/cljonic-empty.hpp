#ifndef CLJONIC_EMPTY_HPP
#define CLJONIC_EMPTY_HPP

#include <cljonic-concepts.hpp>

namespace cljonic {

/** \anchor Empty
 * \brief Returns an empty owning value of the same collection type as the
 * input.
 *
 * Per REQ-FN-026, `empty` produces an empty owning value of the same supported
 * collection type as its input.
 *
 * \b Examples
 * ~~~~~{.cpp}
 * #include <cljonic.hpp>
 *
 * int main() {
 *   using namespace cljonic;
 *
 *   // Compile-time demonstration.
 *   constexpr auto v_const = Vector<int, 4>{1, 2, 3};
 *   [[maybe_unused]] constexpr auto e_const = empty(v_const);
 *   static_assert(is_empty(e_const));
 *
 *   // Runtime demonstration.
 *   auto v_runtime = Vector<int, 4>{10, 20};
 *   auto e_runtime = empty(v_runtime);
 *
 *   return (is_empty(e_runtime) && count(v_runtime) == 2U) ? 0 : 1;
 * }
 * ~~~~~
 */
template <concepts::SequenceableCollection C>
[[nodiscard]] constexpr auto empty(const C&) noexcept -> C {
    return C{};
}

} // namespace cljonic

#endif // CLJONIC_EMPTY_HPP