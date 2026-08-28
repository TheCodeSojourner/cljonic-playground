#ifndef CLJONIC_COUNT_HPP
#define CLJONIC_COUNT_HPP

#include <cljonic-concepts.hpp>
#include <cstddef>

namespace cljonic {

/** \anchor Count
 * \brief Returns the logical size (number of active elements) of a collection.
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
 *   constexpr auto m_const = assoc(Map<int, int, 4>{}, 1, 100);
 *   constexpr auto q_const = conj(Queue<int, 4>{}, 9);
 *   static_assert(count(v_const) == 3U);
 *   static_assert(count(m_const) == 1U);
 *   static_assert(count(q_const) == 1U);
 *
 *   // Runtime demonstration.
 *   auto v_runtime = Vector<int, 4>{10, 20};
 *   const auto sz = count(v_runtime);
 *
 *   return (sz == 2U) ? 0 : 1;
 * }
 * ~~~~~
 */
template <typename C>
[[nodiscard]] constexpr auto count(const C& collection) noexcept -> std::size_t {
    return collection.size();
}

} // namespace cljonic

#endif // CLJONIC_COUNT_HPP