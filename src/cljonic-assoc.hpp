#ifndef CLJONIC_ASSOC_HPP
#define CLJONIC_ASSOC_HPP

#include <cljonic-concepts.hpp>
#include <utility>

namespace cljonic {

/** \anchor Assoc
 * \brief Associates a key and value in an associative collection or index in a
 * vector.
 *
 * \b Examples
 * ~~~~~{.cpp}
 * #include <cljonic.hpp>
 *
 * int main() {
 *   using namespace cljonic;
 *
 *   // Compile-time demonstration.
 *   constexpr auto m_const = assoc(Map<int, int, 4>{}, 1, 100);
 *   static_assert(m_const.contains(1));
 *   static_assert(m_const(1) == 100);
 *
 *   // Runtime demonstration.
 *   auto m_runtime = Map<int, int, 4>{};
 *   auto m1 = assoc(m_runtime, 2, 200);
 *
 *   return (m1.contains(2) && m1(2) == 200) ? 0 : 1;
 * }
 * ~~~~~
 */
template <typename C, typename K, typename V>
[[nodiscard]] constexpr auto assoc(const C& collection, const K& key, const V& value) noexcept {
    return collection.assoc(key, value);
}

} // namespace cljonic

#endif // CLJONIC_ASSOC_HPP
