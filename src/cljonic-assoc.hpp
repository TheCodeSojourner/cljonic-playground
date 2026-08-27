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
 *   constexpr auto m = assoc(Map<int, int, 4>{}, 1, 100);
 *   static_assert(m.contains(1));
 *   static_assert(m(1) == 100);
 *
 *   return 0;
 * }
 * ~~~~~
 */
template <typename C, typename K, typename V>
[[nodiscard]] constexpr auto assoc(const C &collection, const K &key,
                                   const V &value) noexcept {
  return collection.assoc(key, value);
}

} // namespace cljonic

#endif // CLJONIC_ASSOC_HPP
