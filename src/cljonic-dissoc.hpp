#pragma once

#include <cljonic-concepts.hpp>
#include <type_traits>

namespace cljonic {

namespace concepts_detail {

template <typename C, typename K>
concept DissocKeyAdmissible = concepts::CljonicMap<C> && requires(const C& collection, const K& key) {
    { collection.dissoc(key) } noexcept -> std::same_as<C>;
};

template <typename C, typename... Keys>
struct all_dissoc_keys_admissible : std::false_type {};

template <typename C>
struct all_dissoc_keys_admissible<C> : std::bool_constant<concepts::CljonicMap<C>> {};

template <typename C, typename K, typename... Rest>
struct all_dissoc_keys_admissible<C, K, Rest...>
    : std::bool_constant<DissocKeyAdmissible<C, K> && all_dissoc_keys_admissible<C, Rest...>::value> {};

template <typename C, typename... Keys>
inline constexpr bool all_dissoc_keys_admissible_v = all_dissoc_keys_admissible<C, Keys...>::value;

template <typename C>
constexpr void fold_dissoc_keys_into([[maybe_unused]] C& result) noexcept {
}

template <typename C, typename K, typename... Rest>
constexpr void fold_dissoc_keys_into(C& result, const K& key, const Rest&... rest) noexcept {
    result = result.dissoc(key);
    fold_dissoc_keys_into(result, rest...);
}

} // namespace concepts_detail

/** \anchor Dissoc
 * \brief Removes zero or more keys from a Map, returning the resulting map value.
 *
 * `dissoc` is supported only for `Map`. With no keys it returns an unchanged copy; otherwise it removes keys
 * left-to-right. Keys may be supplied using any type convertible to the map's key type. Present keys are removed via
 * swap-and-remove; absent or repeated keys are no-ops. The source map is never modified.
 *
 * \b Examples
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 int main() {
   // Compile-time demonstration.
   constexpr auto m0_const =
       Map{MapEntry{1, 100}, MapEntry{2, 200}, MapEntry{3, 300}};
   constexpr auto unchanged_const = dissoc(m0_const);
   static_assert(contains(unchanged_const, 1));
   static_assert(contains(unchanged_const, 2));
   static_assert(contains(unchanged_const, 3));
   constexpr auto m1_const = dissoc(m0_const, 1, 3, 1);
   static_assert(!contains(m1_const, 1));
   static_assert(contains(m1_const, 2));
   static_assert(!contains(m1_const, 3));
   static_assert(contains(m0_const, 1));

   // Runtime demonstration.
   auto m0_runtime = Map{MapEntry{2, 200}};
   auto m1_runtime = dissoc(m0_runtime, 2);

   return (!contains(m1_runtime, 2) && contains(m0_runtime, 2) &&
           is_empty(m1_runtime))
              ? 0
              : 1;
 }
 ~~~~~
 */
template <typename C>
    requires concepts::CljonicMap<C>
[[nodiscard]] constexpr auto dissoc(const C& collection) noexcept -> C {
    return collection;
}

template <typename C, typename K>
    requires concepts_detail::DissocKeyAdmissible<C, K>
[[nodiscard]] constexpr auto dissoc(const C& collection, const K& key) noexcept -> C {
    return collection.dissoc(key);
}

template <typename C, typename K1, typename K2, typename... Rest>
    requires concepts::CljonicMap<C> && concepts_detail::all_dissoc_keys_admissible_v<C, K1, K2, Rest...>
[[nodiscard]] constexpr auto dissoc(const C& collection, const K1& key1, const K2& key2, const Rest&... rest) noexcept
    -> C {
    C result = collection.dissoc(key1);
    result = result.dissoc(key2);
    concepts_detail::fold_dissoc_keys_into(result, rest...);
    return result;
}

// Diagnostic fallbacks (REQ-DIAG-009): explain unsupported collections and
// inadmissible keys without making either call a supported API operation.
template <typename C>
    requires(!concepts::CljonicMap<C>)
[[nodiscard]] constexpr auto dissoc([[maybe_unused]] const C& collection) noexcept -> C {
    static_assert(concepts_detail::dependent_false<C>,
                  "cljonic::dissoc: the collection must be a Map; only Maps support dissoc.");
    return C{};
}

template <typename C, typename K>
    requires(!concepts::CljonicMap<C>)
[[nodiscard]] constexpr auto dissoc([[maybe_unused]] const C& collection, [[maybe_unused]] const K& key) noexcept -> C {
    static_assert(concepts_detail::dependent_false<C, K>,
                  "cljonic::dissoc: the first argument must be a Map; only Maps support dissoc.");
    return C{};
}

template <typename C, typename K>
    requires concepts::CljonicMap<C> && (!concepts_detail::DissocKeyAdmissible<C, K>)
[[nodiscard]] constexpr auto dissoc([[maybe_unused]] const C& collection, [[maybe_unused]] const K& key) noexcept -> C {
    static_assert(concepts_detail::dependent_false<C, K>,
                  "cljonic::dissoc: each key must be convertible to the Map's key type.");
    return C{};
}

template <typename C, typename K1, typename K2, typename... Rest>
    requires(!concepts::CljonicMap<C>)
[[nodiscard]] constexpr auto dissoc([[maybe_unused]] const C& collection, [[maybe_unused]] const K1& key1,
                                    [[maybe_unused]] const K2& key2, [[maybe_unused]] const Rest&... rest) noexcept
    -> C {
    static_assert(concepts_detail::dependent_false<C, K1, K2, Rest...>,
                  "cljonic::dissoc: the first argument must be a Map; only Maps support dissoc.");
    return C{};
}

template <typename C, typename K1, typename K2, typename... Rest>
    requires concepts::CljonicMap<C> && (!concepts_detail::all_dissoc_keys_admissible_v<C, K1, K2, Rest...>)
[[nodiscard]] constexpr auto dissoc([[maybe_unused]] const C& collection, [[maybe_unused]] const K1& key1,
                                    [[maybe_unused]] const K2& key2, [[maybe_unused]] const Rest&... rest) noexcept
    -> C {
    static_assert(concepts_detail::dependent_false<C, K1, K2, Rest...>,
                  "cljonic::dissoc: each key must be convertible to the Map's key type.");
    return C{};
}

} // namespace cljonic
