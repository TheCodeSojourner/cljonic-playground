#pragma once

#include <cljonic-concepts.hpp>

namespace cljonic {

/** \anchor Contains
 * \brief Tests key presence, element membership, or index validity in a Map, Set, Vector, or String.
 *
 * Map keys and Set elements must exactly match the declared lookup type. Vector and String accept integral indexes.
 *
 * `contains` returns `true` when the key or element is present, or when the index is valid; otherwise, it returns
 * `false`.
 *
 * \b Examples
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 int main() {
   // Compile-time demonstration.
   constexpr auto v_const = Vector<int, 4>{10, 20, 30};
   constexpr auto m_const = Map{MapEntry{1, 100}};
   constexpr auto s_const = Set{5};
   constexpr auto st_const = String<8>{"abc"};
   static_assert(contains(v_const, 0U));
   static_assert(!contains(v_const, 9U));
   static_assert(contains(m_const, 1));
   static_assert(!contains(m_const, 2));
   static_assert(contains(s_const, 5));
   static_assert(!contains(s_const, 8));
   static_assert(contains(st_const, 1U));

   // Runtime demonstration.
   auto v_runtime = Vector<int, 4>{10, 20};
   const auto in_range = contains(v_runtime, 0U);

   return (in_range && !contains(v_runtime, 9U)) ? 0 : 1;
 }
 ~~~~~
 */
namespace concepts_detail {

template <typename C>
concept ContainsLookupCollection =
    concepts::LookupCollection<C> && (concepts::CljonicMap<C> || concepts::CljonicSet<C>);

template <typename C>
concept ContainsIndexedCollection =
    concepts::IndexedCollection<C> && (concepts::CljonicVector<C> || concepts::CljonicString<C>);

template <typename C>
concept ContainsSupportedCollection = ContainsLookupCollection<C> || ContainsIndexedCollection<C>;

template <typename C, typename K>
concept ContainsAdmissible =
    (ContainsLookupCollection<C> && std::same_as<std::remove_cvref_t<K>, typename C::lookup_type>) ||
    (ContainsIndexedCollection<C> && std::integral<std::remove_cvref_t<K>>);

} // namespace concepts_detail

template <typename C, typename K>
    requires concepts_detail::ContainsAdmissible<C, K>
[[nodiscard]] constexpr auto contains(const C& collection, const K& key) noexcept(noexcept(collection.contains(key)))
    -> bool {
    return collection.contains(key);
}

template <typename C, typename K>
    requires(concepts::CljonicCollection<C> || concepts::CljonicProducer<C>) &&
            (!concepts_detail::ContainsSupportedCollection<C>)
[[nodiscard]] constexpr auto contains([[maybe_unused]] const C& collection, [[maybe_unused]] const K& key) noexcept
    -> bool {
    static_assert(concepts_detail::dependent_false<C>,
                  "cljonic::contains: the first argument must be a Map, Set, Vector, or String.");
    return false;
}

template <typename C, typename K>
    requires concepts_detail::ContainsSupportedCollection<C> && (!concepts_detail::ContainsAdmissible<C, K>)
[[nodiscard]] constexpr auto contains([[maybe_unused]] const C& collection, [[maybe_unused]] const K& key) noexcept
    -> bool {
    if constexpr (concepts_detail::ContainsLookupCollection<C>) {
        static_assert(concepts_detail::dependent_false<C, K>,
                      "cljonic::contains: Map and Set lookup arguments must exactly match the declared lookup type; "
                      "implicit conversions are not accepted.");
    } else {
        static_assert(concepts_detail::dependent_false<C, K>,
                      "cljonic::contains: Vector and String indexes must have an integral type.");
    }
    return false;
}

} // namespace cljonic
