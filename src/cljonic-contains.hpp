#ifndef CLJONIC_CONTAINS_HPP
#define CLJONIC_CONTAINS_HPP

#include <cljonic-concepts.hpp>

namespace cljonic {

/** \anchor Contains
 * \brief Tests whether its argument belongs to a collection's lookup domain,
 * mirroring Clojure's `contains?` predicate.
 *
 * The meaning of the argument follows the collection kind:
 * - Map: tests key presence (`contains(m, key)` is true when key is present).
 * - Set: tests element presence (`contains(s, value)` is true when value is a
 *   member).
 * - Vector / String: tests whether a numeric index is in range
 *   (`contains(xs, index)` is true when index is valid, like Clojure's
 *   `contains?` over vector/string indices).
 *
 * `contains` never performs a default-returning access; it only answers the
 * membership question for the collection's lookup domain.
 *
 * \b Examples
 * ~~~~~{.cpp}
 * #include <cljonic.hpp>
 *
 * int main() {
 *   using namespace cljonic;
 *
 *   // Compile-time demonstration.
 *   constexpr auto v_const = Vector<int, 4>{10, 20, 30};
 *   constexpr auto m_const = assoc(Map<int, int, 4>{}, 1, 100);
 *   constexpr auto s_const = conj(Set<int, 4>{}, 5);
 *   constexpr auto st_const = String<8>{"abc"};
 *   static_assert(contains(v_const, 0U));
 *   static_assert(!contains(v_const, 9U));
 *   static_assert(contains(m_const, 1));
 *   static_assert(!contains(m_const, 2));
 *   static_assert(contains(s_const, 5));
 *   static_assert(!contains(s_const, 8));
 *   static_assert(contains(st_const, 1U));
 *
 *   // Runtime demonstration.
 *   auto v_runtime = Vector<int, 4>{10, 20};
 *   const auto in_range = contains(v_runtime, 0U);
 *
 *   return (in_range && !contains(v_runtime, 9U)) ? 0 : 1;
 * }
 * ~~~~~
 */
template <typename C, typename K>
[[nodiscard]] constexpr auto contains(const C& collection, const K& key) noexcept -> bool {
    return collection.contains(key);
}

} // namespace cljonic

#endif // CLJONIC_CONTAINS_HPP