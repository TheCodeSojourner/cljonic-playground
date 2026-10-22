#pragma once

#include <cljonic-concepts.hpp>

namespace cljonic {

namespace concepts_detail {

template <typename C, typename V>
concept CanConjValueAdmissible = concepts::ConjableCollection<C> && requires(const C& collection, const V& value) {
    { collection.can_conj(value) } noexcept -> std::same_as<bool>;
};

template <typename C, typename... Values>
concept CanConjValuesAdmissible = (CanConjValueAdmissible<C, Values> && ...);

template <typename C>
constexpr auto can_conj_values_in_order([[maybe_unused]] C& collection) noexcept -> bool {
    return true;
}

template <typename C, typename V, typename... Rest>
constexpr auto can_conj_values_in_order(C& collection, const V& value, const Rest&... rest) noexcept -> bool {
    if (!collection.can_conj(value)) {
        return false;
    }
    collection = collection.conj(value);
    return can_conj_values_in_order(collection, rest...);
}

} // namespace concepts_detail

/** \anchor CanConj
 * The \b can_conj free function checks whether \ref Conj "conj" can add one or more values to a \ref
 * concepts::CljonicCollection "collection".
 *
 * `can_conj(collection)` returns true. The one-value form checks that value against the collection. The variadic form
 * checks each value left to right against the state produced by the preceding values, and returns true only if every
 * preflight succeeds. All forms preserve the source.
 *
 * Supported for \ref Vector "Vector", \ref Set "Set", \ref Map "Map", \ref Queue "Queue", and \ref String "String".
 * For \ref Vector "Vector" and \ref Queue "Queue", the result depends only on remaining capacity; the value does not
 * affect the preflight.
 *
 * For \ref String "String", both remaining capacity and a valid character are required; see \ref String "String" for
 * character validation policy. An invalid character returns false.
 *
 * For a \ref Set "Set" element or \ref Map "Map" key already present, returns true because insertion is a no-op or a
 * value replacement that needs no capacity. An absent element or key returns true only when capacity remains; when
 * full, it returns false.
 *
 * Values use the same \ref implicit-conversions "safe implicit conversion rules" as \ref Conj "conj".
 *
 * \b Examples
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 int main() {
   // Compile-time demonstration.
   constexpr auto q_const = Queue<int, 4>{};
   static_assert(can_conj(q_const));
   static_assert(can_conj(q_const, 1));
   static_assert(can_conj(Queue<int, 2>{}, 1, 2));
   constexpr auto q_full = Queue{1};
   static_assert(!can_conj(q_full, 2));
   constexpr auto s_const = Set<int, 4>{};
   static_assert(can_conj(s_const, 1));
   constexpr auto text_const = String<4>{"Hi"};
   static_assert(can_conj(text_const, '!'));
   constexpr auto m_const = Map{MapEntry{1, 100}};
   static_assert(can_conj(m_const, MapEntry{1, 999}));

   // Runtime demonstration.
   auto q_runtime = Queue<int, 4>{};
   const auto q_ok = can_conj(q_runtime, 1);

   return q_ok ? 0 : 1;
 }
 ~~~~~
 */
template <typename C>
    requires concepts::ConjableCollection<C>
[[nodiscard]] constexpr auto can_conj([[maybe_unused]] const C& collection) noexcept -> bool {
    return true;
}

template <typename C, typename V>
    requires concepts_detail::CanConjValueAdmissible<C, V>
[[nodiscard]] constexpr auto can_conj(const C& collection,
                                      const V& value) noexcept(noexcept(collection.can_conj(value))) -> bool {
    return collection.can_conj(value);
}

template <typename C, typename V1, typename V2, typename... Rest>
    requires concepts::ConjableCollection<C> && concepts_detail::CanConjValuesAdmissible<C, V1, V2, Rest...>
[[nodiscard]] constexpr auto can_conj(const C& collection, const V1& value1, const V2& value2,
                                      const Rest&... rest) noexcept -> bool {
    C accumulator = collection;
    return concepts_detail::can_conj_values_in_order(accumulator, value1, value2, rest...);
}

// Diagnostic fallback (REQ-DIAG-009): a cljonic collection or producer that
// does not support conj is rejected at the public boundary.
template <typename C>
    requires(concepts::CljonicCollection<C> || concepts::CljonicProducer<C>) && (!concepts::ConjableCollection<C>)
[[nodiscard]] constexpr auto can_conj([[maybe_unused]] const C& collection) noexcept -> bool {
    static_assert(concepts_detail::dependent_false<C>,
                  "cljonic::can_conj: the first argument must be a Vector, a Set, a Map, a Queue, or a String.");
    return false;
}

template <typename C, typename V>
    requires(concepts::CljonicCollection<C> || concepts::CljonicProducer<C>) && (!concepts::ConjableCollection<C>)
[[nodiscard]] constexpr auto can_conj([[maybe_unused]] const C& collection, [[maybe_unused]] const V& value) noexcept
    -> bool {
    static_assert(concepts_detail::dependent_false<C>,
                  "cljonic::can_conj: the first argument must be a Vector, a Set, a Map, a Queue, or a String.");
    return false;
}

template <typename C, typename V>
    requires concepts::ConjableCollection<C> && (!concepts_detail::CanConjValueAdmissible<C, V>)
[[nodiscard]] constexpr auto can_conj([[maybe_unused]] const C& collection, [[maybe_unused]] const V& value) noexcept
    -> bool {
    static_assert(concepts_detail::dependent_false<C, V>,
                  "cljonic::can_conj: this value cannot be added to the collection with conj. A Map takes a MapEntry; "
                  "other collections take a value of the type they store.");
    return false;
}

template <typename C, typename V1, typename V2, typename... Rest>
    requires(concepts::CljonicCollection<C> || concepts::CljonicProducer<C>) && (!concepts::ConjableCollection<C>)
[[nodiscard]] constexpr auto can_conj([[maybe_unused]] const C& collection, [[maybe_unused]] const V1& value1,
                                      [[maybe_unused]] const V2& value2, [[maybe_unused]] const Rest&... rest) noexcept
    -> bool {
    static_assert(concepts_detail::dependent_false<C>,
                  "cljonic::can_conj: the first argument must be a Vector, a Set, a Map, a Queue, or a String.");
    return false;
}

template <typename C, typename V1, typename V2, typename... Rest>
    requires concepts::ConjableCollection<C> && (!concepts_detail::CanConjValuesAdmissible<C, V1, V2, Rest...>)
[[nodiscard]] constexpr auto can_conj([[maybe_unused]] const C& collection, [[maybe_unused]] const V1& value1,
                                      [[maybe_unused]] const V2& value2, [[maybe_unused]] const Rest&... rest) noexcept
    -> bool {
    static_assert(concepts_detail::dependent_false<C, V1, V2, Rest...>,
                  "cljonic::can_conj: every value after the collection must be one this collection can add. A Map "
                  "takes MapEntry values; other collections take a value of the type they store.");
    return false;
}

} // namespace cljonic
