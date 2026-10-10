#pragma once

#include <cljonic-concepts.hpp>

namespace cljonic {

/** \anchor CanAssoc
 * \brief Checks whether one or more assoc pairs can be applied without capacity overflow.
 *
 * Supported for `Map` (an existing key can be replaced, or an absent key inserted when capacity remains), `Vector` (an
 * existing index can be replaced, or an index equal to the current count can append when capacity remains), and
 * `String` (the same index and append rule as Vector, with an invalid character returning false). `Set` and `Queue`
 * provide no associative capability and are rejected. The variadic form checks pairs left to right against the state
 * produced by earlier pairs, returning true only when every pair preflight succeeds. All forms leave the source
 * unchanged.
 *
 * \b Examples
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 int main() {
   // Compile-time demonstration.
   constexpr Map<int, int, 4> m_const{};
   static_assert(can_assoc(m_const, 1, 10));
   static_assert(can_assoc(m_const, 1, 10, 2, 20));

   // Runtime demonstration.
   auto m_runtime = Map<int, int, 4>{};
   const auto ok = can_assoc(m_runtime, 2, 20);

   return ok ? 0 : 1;
 }
 ~~~~~
 */
template <typename C, typename K, typename V>
    requires concepts::AssociativeCollection<C> && requires(const C& collection, const K& key, const V& value) {
        { collection.can_assoc(key, value) } noexcept -> std::same_as<bool>;
    }
[[nodiscard]] constexpr auto can_assoc(const C& collection, const K& key,
                                       const V& value) noexcept(noexcept(collection.can_assoc(key, value))) -> bool {
    return collection.can_assoc(key, value);
}

namespace concepts_detail {

// Test key and value admission separately so each rejected operand gets a
// focused diagnostic.
template <typename C, typename K>
concept CanAssocKeyAdmissible = concepts::AssociativeCollection<C> &&
                                requires(const C& collection, const K& key, const C::association_value_type& value) {
                                    { collection.can_assoc(key, value) } noexcept -> std::same_as<bool>;
                                };

template <typename C, typename V>
concept CanAssocValueAdmissible =
    concepts::AssociativeCollection<C> && requires(const C& collection, const C::key_type& key, const V& value) {
        { collection.can_assoc(key, value) } noexcept -> std::same_as<bool>;
    };

template <typename C, typename... Arguments>
struct all_assoc_arguments_admissible : std::false_type {};

template <typename C>
struct all_assoc_arguments_admissible<C> : std::true_type {};

template <typename C, typename K, typename V, typename... Rest>
struct all_assoc_arguments_admissible<C, K, V, Rest...>
    : std::bool_constant<CanAssocKeyAdmissible<C, K> && CanAssocValueAdmissible<C, V> &&
                         all_assoc_arguments_admissible<C, Rest...>::value> {};

template <typename C, typename... Arguments>
inline constexpr bool all_assoc_arguments_admissible_v = all_assoc_arguments_admissible<C, Arguments...>::value;

template <typename C>
constexpr auto can_assoc_pairs_in_order([[maybe_unused]] C& collection) noexcept -> bool {
    return true;
}

template <typename C, typename K, typename V, typename... Rest>
constexpr auto can_assoc_pairs_in_order(C& collection, const K& key, const V& value, const Rest&... rest) noexcept
    -> bool {
    if (!collection.can_assoc(key, value)) {
        return false;
    }
    collection = collection.assoc(key, value);
    return can_assoc_pairs_in_order(collection, rest...);
}

} // namespace concepts_detail

template <typename C, typename K1, typename V1, typename K2, typename V2, typename... Rest>
    requires concepts::AssociativeCollection<C> && (sizeof...(Rest) % 2 == 0) &&
             concepts_detail::all_assoc_arguments_admissible_v<C, K1, V1, K2, V2, Rest...>
[[nodiscard]] constexpr auto can_assoc(const C& collection, const K1& key1, const V1& value1, const K2& key2,
                                       const V2& value2, const Rest&... rest) noexcept -> bool {
    C accumulator = collection;
    return concepts_detail::can_assoc_pairs_in_order(accumulator, key1, value1, key2, value2, rest...);
}

// Diagnostic fallbacks (REQ-DIAG-009): one targeted message per rejection mode;
// never a supported call target.
template <typename C, typename K, typename V>
    requires concepts::CljonicCollection<C> && (!concepts::AssociativeCollection<C>)
[[nodiscard]] constexpr auto can_assoc([[maybe_unused]] const C& collection, [[maybe_unused]] const K& key,
                                       [[maybe_unused]] const V& value) noexcept -> bool {
    static_assert(concepts_detail::dependent_false<C>,
                  "cljonic::can_assoc: the first argument must be a Map, a Vector, or a String. A Set or Queue is "
                  "not supported.");
    return false;
}

template <typename C, typename K, typename V>
    requires concepts::AssociativeCollection<C> && (!concepts_detail::CanAssocKeyAdmissible<C, K>)
[[nodiscard]] constexpr auto can_assoc([[maybe_unused]] const C& collection, [[maybe_unused]] const K& key,
                                       [[maybe_unused]] const V& value) noexcept -> bool {
    static_assert(concepts_detail::dependent_false<C>,
                  "cljonic::can_assoc: this key has a type the collection cannot associate. A Map takes a key; a "
                  "Vector or a String takes an integer index.");
    return false;
}

template <typename C, typename K, typename V>
    requires concepts::AssociativeCollection<C> && concepts_detail::CanAssocKeyAdmissible<C, K> &&
             (!concepts_detail::CanAssocValueAdmissible<C, V>)
[[nodiscard]] constexpr auto can_assoc([[maybe_unused]] const C& collection, [[maybe_unused]] const K& key,
                                       [[maybe_unused]] const V& value) noexcept -> bool {
    static_assert(concepts_detail::dependent_false<C>,
                  "cljonic::can_assoc: this value has a type the collection cannot associate. The value must match "
                  "the type stored by the collection.");
    return false;
}

template <typename C, typename K1, typename V1, typename K2, typename V2, typename... Rest>
    requires concepts::CljonicCollection<C> && (!concepts::AssociativeCollection<C>) && (sizeof...(Rest) % 2 == 0)
[[nodiscard]] constexpr auto can_assoc([[maybe_unused]] const C& collection, [[maybe_unused]] const K1& key1,
                                       [[maybe_unused]] const V1& value1, [[maybe_unused]] const K2& key2,
                                       [[maybe_unused]] const V2& value2, [[maybe_unused]] const Rest&... rest) noexcept
    -> bool {
    static_assert(concepts_detail::dependent_false<C>,
                  "cljonic::can_assoc: the first argument must be a Map, a Vector, or a String. A Set or Queue is "
                  "not supported.");
    return false;
}

template <typename C, typename K1, typename V1, typename K2, typename V2, typename... Rest>
    requires concepts::AssociativeCollection<C> && (sizeof...(Rest) % 2 == 0) &&
             (!concepts_detail::all_assoc_arguments_admissible_v<C, K1, V1, K2, V2, Rest...>)
[[nodiscard]] constexpr auto can_assoc([[maybe_unused]] const C& collection, [[maybe_unused]] const K1& key1,
                                       [[maybe_unused]] const V1& value1, [[maybe_unused]] const K2& key2,
                                       [[maybe_unused]] const V2& value2, [[maybe_unused]] const Rest&... rest) noexcept
    -> bool {
    static_assert(concepts_detail::dependent_false<C, K1, V1, K2, V2, Rest...>,
                  "cljonic::can_assoc: every key/value pair must use types this collection can associate. A Map "
                  "takes its declared key and value types; a Vector or String takes an integer index and its stored "
                  "value type.");
    return false;
}

template <typename C, typename... Arguments>
    requires concepts::CljonicCollection<C> && concepts::AssociativeCollection<C> && (sizeof...(Arguments) >= 3) &&
             (sizeof...(Arguments) % 2 == 1)
[[nodiscard]] constexpr auto can_assoc([[maybe_unused]] const C& collection,
                                       [[maybe_unused]] const Arguments&... arguments) noexcept -> bool {
    static_assert(concepts_detail::dependent_false<C, Arguments...>,
                  "cljonic::can_assoc: supply complete key/value pairs; the variadic form cannot end with a key "
                  "without a value.");
    return false;
}

} // namespace cljonic
