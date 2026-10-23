#pragma once

#include <cljonic-concepts.hpp>
#include <cljonic-map.hpp>

namespace cljonic {

/** \anchor First
 * The \b first free function returns the first logical element of a `Vector`, `Map`, `Set`, `Queue`, or `String`
 * value, or the key of a \ref MapEntryFactory "map_entry" value.
 *
 * `Vector` and `String` yield the element at index zero, `Queue` yields its front element, `Map` yields one
 * value-semantic \ref MapEntryFactory "map_entry" value, and `Set` yields one stored element. `Map` and `Set`
 * traversal order is not semantically significant. An empty collection yields its \ref DefaultElement "default
 * element".
 *
 * \b Examples
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 int main() {
   // Compile-time demonstration.
   constexpr auto v_const = vector(10, 20, 30);
   constexpr auto m_const = hash_map(1, 100);
   constexpr auto s_const = hash_set(7);
   constexpr auto q_const = queue(5, 6);
   constexpr auto text_const = String{"AB"};
   constexpr auto e_const = map_entry(1, 100);
   static_assert(first(v_const) == 10);
   static_assert(first(m_const) == map_entry(1, 100));
   static_assert(first(s_const) == 7);
   static_assert(first(q_const) == 5);
   static_assert(first(text_const) == 'A');
   static_assert(first(e_const) == 1);

   // Runtime demonstration.
   auto v_runtime = vector(100, 200);
   auto m_runtime = hash_map(2, 200);
   auto s_runtime = hash_set(8);
   auto q_runtime = queue(9, 10);
   auto text_runtime = String{"CD"};
   auto e_runtime = map_entry(3, 300);

   return (first(v_runtime) == 100 && first(m_runtime) == map_entry(2, 200) &&
           first(s_runtime) == 8 && first(q_runtime) == 9 &&
           first(text_runtime) == 'C' && first(e_runtime) == 3)
              ? 0
              : 1;
 }
 ~~~~~
 */
template <typename KeyType, typename ValueType>
[[nodiscard]] constexpr auto first(const MapEntry<KeyType, ValueType>& entry) noexcept -> KeyType {
    return entry.key;
}

/** Returns the first logical element of a Vector, Map, Set, Queue, or String value. */
template <concepts::SequenceableCollection C>
[[nodiscard]] constexpr auto first(const C& collection) noexcept -> C::value_type {
    if (collection.count() == 0U) {
        return typename C::value_type{};
    }
    return *collection.begin();
}

template <typename C>
    requires(!concepts::SequenceableCollection<C> && !concepts_detail::is_cljonic_map_entry_v<C>)
constexpr auto first([[maybe_unused]] const C& value) noexcept -> void {
    static_assert(concepts_detail::dependent_false<C>,
                  "cljonic::first: expected a Vector, Map, Set, Queue, String, or MapEntry value.");
}

} // namespace cljonic
