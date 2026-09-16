#ifndef CLJONIC_GET_HPP
#define CLJONIC_GET_HPP

#include <cljonic-concepts.hpp>
#include <utility>

namespace cljonic {

/** \anchor Get
 * \brief Looks up an element by a key or index, returning the default or
 *        supplied fallback when absent.
 *
 * Dispatches to the collection's callable lookup operation, consistent with the `operator()` behavior of Map, Set,
 * Vector, and String. The fallback defaults to the collection's default lookup result when omitted.
 *
 * \b Examples
 * ~~~~~{.cpp}
 * #include "cljonic.hpp"
 *
 * int main() {
 *   using namespace cljonic;
 *
 *   // Compile-time demonstration.
 *   constexpr auto v_const = Vector<int, 4>{10, 20};
 *   constexpr auto m_const = assoc(Map<int, int, 4>{}, 1, 100);
 *   constexpr auto s_const = conj(Set<int, 4>{}, 5);
 *   static_assert(get(v_const, 0U) == 10);
 *   static_assert(get(v_const, 9U, -1) == -1);
 *   static_assert(get(m_const, 1) == 100);
 *   static_assert(get(m_const, 2, -1) == -1);
 *   static_assert(get(s_const, 5) == 5);
 *   static_assert(get(s_const, 8, -1) == -1);
 *
 *   // Runtime demonstration.
 *   auto v_runtime = Vector<int, 4>{10, 20};
 *   const auto val = get(v_runtime, 0U);
 *
 *   return (val == 10) ? 0 : 1;
 * }
 * ~~~~~
 */
/** Returns the stored value when present, otherwise the supplied fallback or
 * the collection's default lookup result when no fallback is supplied. */
template <typename C, typename K,
          typename V = std::remove_cvref_t<decltype(std::declval<const C&>()(std::declval<const K&>()))>>
    requires(concepts::LookupCollection<C> || concepts::IndexedCollection<C>) &&
            requires(const C& collection, const K& key, const V& fallback) { collection(key, fallback); }
[[nodiscard]] constexpr auto get(const C& collection, const K& key, const V& fallback = V{}) noexcept
    -> decltype(collection(key, fallback)) {
    return collection(key, fallback);
}

} // namespace cljonic

#endif // CLJONIC_GET_HPP