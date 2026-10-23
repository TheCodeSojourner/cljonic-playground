#pragma once

#include <cljonic-concepts.hpp>
#include <type_traits>
#include <utility>

namespace cljonic {

namespace concepts_detail {

template <typename C, typename K>
concept GetKeyAdmissible =
    (concepts::LookupCollection<C> && concepts::SafeImplicitConversion<typename C::lookup_type, K>) ||
    (concepts::IndexedCollection<C> && IntegerIndexType<K>);

template <typename C, typename K>
using get_result_t = decltype(std::declval<const C&>()(std::declval<const K&>()));

template <typename C, typename K, typename R>
concept GetFallbackAdmissible = GetKeyAdmissible<C, K> && concepts::SafeImplicitConversion<get_result_t<C, K>, R>;

template <typename C, typename K>
constexpr auto diagnose_get_key() noexcept -> void {
    if constexpr (!(concepts::LookupCollection<C> || concepts::IndexedCollection<C>)) {
        static_assert(dependent_false<C>, "cljonic::get: the first argument must be a Map, Set, Vector, or String.");
    } else if constexpr (concepts::IndexedCollection<C>) {
        static_assert(dependent_false<C, K>,
                      "cljonic::get: a Vector or String index must be a non-boolean, non-character integer type.");
    } else {
        static_assert(dependent_false<C, K>,
                      "cljonic::get: the key must safely convert to the Map's key type or the Set's element type.");
    }
}

template <typename C, typename K, typename R>
constexpr auto diagnose_get_fallback() noexcept -> void {
    if constexpr (!GetKeyAdmissible<C, K>) {
        diagnose_get_key<C, K>();
    } else {
        static_assert(dependent_false<C, K, R>,
                      "cljonic::get: the fallback must safely convert to the collection's lookup result type.");
    }
}

} // namespace concepts_detail

/** \anchor Get
 * The \b get free function looks up a value in a \ref Map "Map", \ref Set "Set", \ref Vector "Vector", or \ref String
 * "String", returning the collection's default lookup result or a supplied fallback when no value is present.
 *
 * \ref Map "Map" keys and \ref Set "Set" values must safely convert to their declared lookup type. \ref Vector
 * "Vector" and \ref String "String" use integer indexes. The result has the same type as the collection's callable
 * lookup result; a supplied fallback must safely convert to that result type.
 *
 * \ref Queue "Queue" and producers do not provide lookup and are not accepted. Unsupported collections, keys, indexes,
 * or fallback types produce a targeted compile-time diagnostic. Use \ref Contains "contains" when presence or index
 * validity must be distinguished from a default-valued result.
 *
 * \b Examples
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 int main() {
   // Compile-time demonstration.
   constexpr auto v_const = vector(10, 20);
   constexpr auto m_const = hash_map(1, 100);
   constexpr auto s_const = Set{5};
   constexpr auto text_const = String{"ab"};
   static_assert(get(v_const, 0) == 10);
   static_assert(get(v_const, 9, -1) == -1);
   static_assert(get(m_const, 1) == 100);
   static_assert(get(m_const, 2, -1) == -1);
   static_assert(get(s_const, 5) == 5);
   static_assert(get(s_const, 8, -1) == -1);
   static_assert(get(text_const, 0) == 'a');
   static_assert(get(text_const, 9, 'x') == 'x');

   // Runtime demonstration.
   auto v_runtime = vector(10, 20);
   const auto val = get(v_runtime, 0);

   return (val == 10) ? 0 : 1;
 }
 ~~~~~
 */
template <typename C, typename K, typename V = std::remove_cvref_t<concepts_detail::get_result_t<C, K>>>
    requires concepts_detail::GetKeyAdmissible<C, K> && concepts_detail::GetFallbackAdmissible<C, K, V>
[[nodiscard]] constexpr auto get(const C& collection, const K& key,
                                 const V& fallback = V{}) noexcept(noexcept(collection(key, fallback)))
    -> decltype(collection(key, fallback)) {
    return collection(key, fallback);
}

template <typename C, typename K>
    requires(!concepts_detail::GetKeyAdmissible<C, K>)
constexpr auto get([[maybe_unused]] const C& collection, [[maybe_unused]] const K& key) noexcept -> void {
    concepts_detail::diagnose_get_key<C, K>();
}

template <typename C, typename K, typename R>
    requires(!concepts_detail::GetFallbackAdmissible<C, K, R>)
constexpr auto get([[maybe_unused]] const C& collection, [[maybe_unused]] const K& key,
                   [[maybe_unused]] const R& fallback) noexcept -> void {
    concepts_detail::diagnose_get_fallback<C, K, R>();
}

} // namespace cljonic
