#pragma once

#include <cljonic-concepts.hpp>
#include <type_traits>

namespace cljonic {

/** \anchor Assoc
 * \brief Associates a value with a key in a Map, or with an index in an indexed
 *        collection (Vector, String).
 *
 * Supported for `Map` (key/value association, replacing the value of an existing key), `Vector` (index association; an
 * index equal to the current count appends when capacity remains), and `String` (the same index and append rule,
 * applying the invalid-character policy: a non-ASCII or NUL character is rejected at compile time and replaced with
 * `.` at runtime). `Set` and `Queue` provide no associative capability and are rejected by the boundary constraint.
 * The result is a distinct collection value; the source is unchanged.
 *
 * The variadic form `assoc(collection, key₁, value₁, key₂, value₂, …)` applies two or more key-value pairs
 * left to right into the result, as if each were a separate `assoc`. A pair that cannot be applied (an invalid key, or
 * a full-capacity map or append index) is skipped and the remaining pairs still apply.
 *
 * \b Examples
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 int main() {
   // Compile-time demonstration.
   constexpr auto m_const = assoc(Map<int, int, 4>{}, 1, 100);
   static_assert(m_const(1) == 100);

   constexpr auto v_const = assoc(Vector<int, 4>{10, 20}, 1, 200);
   static_assert(v_const(1) == 200);

   constexpr auto m_multi = assoc(Map<int, int, 4>{}, 1, 100, 2, 200);
   static_assert(m_multi(2) == 200);

   constexpr auto v_multi = assoc(Vector<int, 4>{10, 20}, 1, 200, 2, 300);
   static_assert(v_multi(2) == 300);

   // Runtime demonstration.
   auto m_runtime = Map<int, int, 4>{};
   const auto m1 = assoc(m_runtime, 2, 200);

   return (contains(m1, 2) && m1(2) == 200) ? 0 : 1;
 }
 ~~~~~
 */
template <typename C, typename K, typename V>
    requires concepts::AssociativeCollection<C> && requires(const C& collection, const K& key, const V& value) {
        { collection.assoc(key, value) } noexcept -> std::same_as<C>;
    }
[[nodiscard]] constexpr auto assoc(const C& collection, const K& key,
                                   const V& value) noexcept(noexcept(collection.assoc(key, value))) -> C {
    return collection.assoc(key, value);
}

namespace concepts_detail {

// A single (key, value) pair is admissible for the variadic form when the
// collection admits it under its own key/value domain (identical to the
// single-pair overload's gate).
template <typename C, typename K, typename V>
concept AssocPairAdmissible =
    concepts::AssociativeCollection<C> && requires(const C& collection, const K& key, const V& value) {
        { collection.assoc(key, value) } noexcept -> std::same_as<C>;
    };

// Every (key, value) pair in the argument list is admissible. The recursion
// consumes two arguments per step; an empty tail is admissible and a single
// trailing argument (a key without its value) is not, so an odd argument count
// can never satisfy the constraint.
template <typename C, typename... Pairs>
struct all_assoc_pairs_admissible : std::false_type {};

template <typename C>
struct all_assoc_pairs_admissible<C> : std::true_type {};

template <typename C, typename K, typename V, typename... Rest>
struct all_assoc_pairs_admissible<C, K, V, Rest...>
    : std::bool_constant<AssocPairAdmissible<C, K, V> && all_assoc_pairs_admissible<C, Rest...>::value> {};

template <typename C, typename... Pairs>
inline constexpr bool all_assoc_pairs_admissible_v = all_assoc_pairs_admissible<C, Pairs...>::value;

// Left-to-right fold of the remaining pairs into an accumulator that already
// holds the first pair(s); each step reuses the single-pair overload's contract.
// The accumulator is threaded by reference (never passed by value) so the fold
// cannot be intercepted by a collection's source-construction constructor.
template <typename C>
constexpr void fold_assoc_pairs_into([[maybe_unused]] C& result) noexcept {
}

template <typename C, typename K, typename V, typename... Rest>
constexpr void fold_assoc_pairs_into(C& result, const K& key, const V& value, const Rest&... rest) noexcept {
    result = result.assoc(key, value);
    fold_assoc_pairs_into(result, rest...);
}

} // namespace concepts_detail

/** Variadic `assoc`: applies two or more key-value pairs left to right. Each pair is admitted (or rejected at compile
 *  time) under the same collection-specific key/value domain as the single-pair form; a pair that cannot be applied
 *  at full capacity is a no-op and the remaining pairs still apply. */
template <typename C, typename K1, typename V1, typename K2, typename V2, typename... Rest>
    requires concepts::AssociativeCollection<C> &&
             concepts_detail::all_assoc_pairs_admissible_v<C, K1, V1, K2, V2, Rest...>
[[nodiscard]] constexpr auto assoc(const C& collection, const K1& key1, const V1& value1, const K2& key2,
                                   const V2& value2, const Rest&... rest) noexcept -> C {
    C result = assoc(assoc(collection, key1, value1), key2, value2);
    concepts_detail::fold_assoc_pairs_into(result, rest...);
    return result;
}

// Diagnostic fallbacks (REQ-DIAG-009): one targeted message per rejection mode;
// never a supported call target. Each is viable only for a call the admitted
// overloads reject.
template <typename C, typename K, typename V>
    requires concepts::CljonicCollection<C> && (!concepts::AssociativeCollection<C>)
[[nodiscard]] constexpr auto assoc([[maybe_unused]] const C& collection, [[maybe_unused]] const K& key,
                                   [[maybe_unused]] const V& value) noexcept -> C {
    static_assert(concepts_detail::dependent_false<C>,
                  "cljonic::assoc: the first argument must be an associative collection -- Map, Vector, or String. "
                  "Set and Queue are not associative; use conj to add an element to a Set or Queue.");
    return C{};
}

template <typename C, typename K, typename V>
    requires concepts::AssociativeCollection<C> && (!concepts_detail::AssocPairAdmissible<C, K, V>)
[[nodiscard]] constexpr auto assoc([[maybe_unused]] const C& collection, [[maybe_unused]] const K& key,
                                   [[maybe_unused]] const V& value) noexcept -> C {
    static_assert(concepts_detail::dependent_false<C>,
                  "cljonic::assoc: the (key, value) pair is outside this collection's association domain. Map "
                  "associates a key with a value of its element type; Vector and String associate an integer index "
                  "with a value.");
    return C{};
}

template <typename C, typename K1, typename V1, typename K2>
    requires concepts::CljonicCollection<C>
[[nodiscard]] constexpr auto assoc([[maybe_unused]] const C& collection, [[maybe_unused]] const K1& key1,
                                   [[maybe_unused]] const V1& value1, [[maybe_unused]] const K2& key2) noexcept -> C {
    static_assert(concepts_detail::dependent_false<C>,
                  "cljonic::assoc: the argument after a key must be its value -- a key with no following value is not "
                  "a pair. Pass one pair (collection, key, value), or two or more pairs (collection, k1, v1, k2, v2, "
                  "...).");
    return C{};
}

template <typename C, typename K1, typename V1, typename K2, typename V2, typename... Rest>
    requires concepts::CljonicCollection<C> &&
             (!concepts_detail::all_assoc_pairs_admissible_v<C, K1, V1, K2, V2, Rest...>)
[[nodiscard]] constexpr auto assoc([[maybe_unused]] const C& collection, [[maybe_unused]] const K1& key1,
                                   [[maybe_unused]] const V1& value1, [[maybe_unused]] const K2& key2,
                                   [[maybe_unused]] const V2& value2, [[maybe_unused]] const Rest&... rest) noexcept
    -> C {
    static_assert(concepts_detail::dependent_false<C>,
                  "cljonic::assoc: the trailing arguments must be complete key/value pairs, each admissible for this "
                  "collection. A key is missing its value, or a (key, value) pair is outside the collection's "
                  "association domain: Map associates a key and value; Vector and String associate an integer index "
                  "and a value.");
    return C{};
}

} // namespace cljonic
