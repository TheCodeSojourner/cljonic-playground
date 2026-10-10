#pragma once

#include <cljonic-concepts.hpp>

namespace cljonic {

namespace concepts_detail {

template <typename C, typename T>
concept ConjValueAdmissible = concepts::ConjableCollection<C> && requires(const C& collection, const T& value) {
    { collection.conj(value) } noexcept -> std::same_as<C>;
};

} // namespace concepts_detail

/** \anchor Conj
 * \brief Adds an element to a collection according to its type conventions.
 *
 * Supported for `Vector`, `Set`, `Map`, `Queue`, and `String`. `Vector` appends at the end. `Queue` enqueues at the
 * rear. `String` appends at its logical count. For `String`, invalid characters are rejected at compile time and
 * replaced with `.` at runtime; see String for character validation and normalization policy. `Set` adds absent
 * elements; duplicates are no-ops. `Map` associates a `MapEntry`; an existing key's value is replaced. At full
 * capacity, `Vector`, `Queue`, and `String` return unchanged copies. A full `Set` or `Map` also returns an unchanged
 * copy when the element or key is absent. Each operation returns a distinct collection value and preserves the source.
 *
 * Three arities are provided: `conj(collection)` returns an unchanged copy, `conj(collection, value)` adds one value,
 * and `conj(collection, value₁, value₂, …)` adds two or more values left to right, so `conj(c, x₁, x₂)`
 * equals `conj(conj(c, x₁), x₂)`. Each value must be one the collection can hold, as described above; a value that
 * does not fit because the collection is full is skipped, and the remaining values are still added.
 *
 * \b Examples
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 int main() {
   // Compile-time demonstration.
   constexpr auto q_const = conj(Queue<int, 4>{}, 10);
   constexpr auto s_const = conj(Set<int, 4>{}, 20);
   static_assert(peek(q_const) == 10);
   static_assert(contains(s_const, 20));

   constexpr auto v_const = conj(Vector<int, 4>{10, 20}, 30);
   static_assert(v_const(2) == 30);
   constexpr auto text_const = conj(String<4>{"Hi"}, '!');
   static_assert(text_const(2) == '!');
   constexpr auto m_const = conj(Map<int, int, 4>{}, MapEntry<int, int>{1, 100});
   static_assert(m_const(1) == 100);

   // Zero-value and variadic arities.
   constexpr auto v_same = conj(v_const);
   static_assert(v_same.count() == 3U);
   constexpr auto v_many = conj(Vector<int, 4>{10}, 20, 30);
   static_assert(v_many(2) == 30);
   constexpr auto text_many = conj(String<4>{"H"}, 'i', '!');
   static_assert(text_many(2) == '!');

   // Runtime demonstration.
   auto q_runtime = Queue<int, 4>{};
   const auto q1 = conj(q_runtime, 100);

   return (peek(q1) == 100) ? 0 : 1;
 }
 ~~~~~
 */
template <typename C, typename T>
    requires concepts_detail::ConjValueAdmissible<C, T>
[[nodiscard]] constexpr auto conj(const C& collection, const T& value) noexcept(noexcept(collection.conj(value))) -> C {
    return collection.conj(value);
}

/** Zero-value `conj`: returns an unchanged copy of the collection, matching Clojure's `(conj coll)`. */
template <typename C>
    requires concepts::ConjableCollection<C>
[[nodiscard]] constexpr auto conj(const C& collection) noexcept -> C {
    return collection;
}

namespace concepts_detail {

// Every value in the argument list is admissible for the collection. The
// recursion consumes one value per step; an empty tail is admissible.
template <typename C, typename... Values>
struct all_conj_values_admissible : std::true_type {};

template <typename C, typename T, typename... Rest>
struct all_conj_values_admissible<C, T, Rest...>
    : std::bool_constant<ConjValueAdmissible<C, T> && all_conj_values_admissible<C, Rest...>::value> {};

template <typename C, typename... Values>
inline constexpr bool all_conj_values_admissible_v = all_conj_values_admissible<C, Values...>::value;

// Left-to-right fold of the remaining values into an accumulator that already
// holds the first value(s); each step reuses the single-value overload's
// contract. The accumulator is threaded by reference (never passed by value) so
// the fold cannot be intercepted by a collection's source-construction
// constructor.
template <typename C>
constexpr void fold_conj_values_into([[maybe_unused]] C& result) noexcept {
}

template <typename C, typename T, typename... Rest>
constexpr void fold_conj_values_into(C& result, const T& value, const Rest&... rest) noexcept {
    result = result.conj(value);
    fold_conj_values_into(result, rest...);
}

} // namespace concepts_detail

/** Variadic `conj`: adds two or more values left to right, as if they were added one at a time. Each value must be one
 *  the collection can hold; a value that does not fit because the collection is full is skipped, and the remaining
 *  values are still added. */
template <typename C, typename T1, typename T2, typename... Rest>
    requires concepts::ConjableCollection<C> && concepts_detail::all_conj_values_admissible_v<C, T1, T2, Rest...>
[[nodiscard]] constexpr auto conj(const C& collection, const T1& value1, const T2& value2, const Rest&... rest) noexcept
    -> C {
    C result = conj(conj(collection, value1), value2);
    concepts_detail::fold_conj_values_into(result, rest...);
    return result;
}

// Diagnostic fallback (REQ-DIAG-009): explain collection and value-domain
// rejections without making either a supported call target.
template <typename C, typename T>
    requires concepts::CljonicCollection<C> && (!concepts::ConjableCollection<C>)
[[nodiscard]] constexpr auto conj([[maybe_unused]] const C& collection, [[maybe_unused]] const T& value) noexcept -> C {
    static_assert(concepts_detail::dependent_false<C>, "cljonic::conj: the first argument must be a Vector, a Set, a "
                                                       "Map, a Queue, or a String.");
    return collection;
}

template <typename C, typename T>
    requires concepts::ConjableCollection<C> && (!concepts_detail::ConjValueAdmissible<C, T>)
[[nodiscard]] constexpr auto conj([[maybe_unused]] const C& collection, [[maybe_unused]] const T& value) noexcept -> C {
    static_assert(concepts_detail::dependent_false<C, T>,
                  "cljonic::conj: this value cannot be added to the collection with conj. A Map takes a MapEntry; "
                  "other collections take a value of the type they store.");
    return collection;
}

template <typename C>
    requires concepts::CljonicCollection<C> && (!concepts::ConjableCollection<C>)
[[nodiscard]] constexpr auto conj([[maybe_unused]] const C& collection) noexcept -> C {
    static_assert(concepts_detail::dependent_false<C>, "cljonic::conj: the first argument must be a Vector, a Set, a "
                                                       "Map, a Queue, or a String.");
    return collection;
}

template <typename C, typename T1, typename T2, typename... Rest>
    requires concepts::CljonicCollection<C> && (!concepts::ConjableCollection<C>)
[[nodiscard]] constexpr auto conj([[maybe_unused]] const C& collection, [[maybe_unused]] const T1& value1,
                                  [[maybe_unused]] const T2& value2, [[maybe_unused]] const Rest&... rest) noexcept
    -> C {
    static_assert(concepts_detail::dependent_false<C>, "cljonic::conj: the first argument must be a Vector, a Set, a "
                                                       "Map, a Queue, or a String.");
    return collection;
}

template <typename C, typename T1, typename T2, typename... Rest>
    requires concepts::ConjableCollection<C> && (!concepts_detail::all_conj_values_admissible_v<C, T1, T2, Rest...>)
[[nodiscard]] constexpr auto conj([[maybe_unused]] const C& collection, [[maybe_unused]] const T1& value1,
                                  [[maybe_unused]] const T2& value2, [[maybe_unused]] const Rest&... rest) noexcept
    -> C {
    static_assert(concepts_detail::dependent_false<C>,
                  "cljonic::conj: every value after the collection must be one this collection can add. A Map takes "
                  "MapEntry values; other collections take a value of the type they store.");
    return collection;
}

} // namespace cljonic
