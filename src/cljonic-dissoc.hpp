#ifndef CLJONIC_DISSOC_HPP
#define CLJONIC_DISSOC_HPP

#include <cljonic-concepts.hpp>
#include <utility>

namespace cljonic {

/** \anchor Dissoc
 * \brief Disassociates a key from a map.
 *
 * \b Examples
 * ~~~~~{.cpp}
 * #include <cljonic.hpp>
 *
 * int main() {
 *   using namespace cljonic;
 *
 *   // Compile-time demonstration.
 *   constexpr auto m0_const = assoc(Map<int, int, 4>{}, 1, 100);
 *   constexpr auto m1_const = dissoc(m0_const, 1);
 *   static_assert(!m1_const.contains(1));
 *
 *   // Runtime demonstration.
 *   auto m0_runtime = assoc(Map<int, int, 4>{}, 2, 200);
 *   auto m1_runtime = dissoc(m0_runtime, 2);
 *
 *   return (!m1_runtime.contains(2) && m1_runtime.empty()) ? 0 : 1;
 * }
 * ~~~~~
 */
template <typename C, typename K>
[[nodiscard]] constexpr auto dissoc(const C& collection, const K& key) noexcept {
    return collection.dissoc(key);
}

} // namespace cljonic

#endif // CLJONIC_DISSOC_HPP
