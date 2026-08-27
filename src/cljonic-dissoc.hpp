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
 *   constexpr auto m0 = assoc(Map<int, int, 4>{}, 1, 100);
 *   constexpr auto m1 = dissoc(m0, 1);
 *   static_assert(!m1.contains(1));
 *
 *   return 0;
 * }
 * ~~~~~
 */
template <typename C, typename K>
[[nodiscard]] constexpr auto dissoc(const C &collection,
                                    const K &key) noexcept {
  return collection.dissoc(key);
}

} // namespace cljonic

#endif // CLJONIC_DISSOC_HPP
