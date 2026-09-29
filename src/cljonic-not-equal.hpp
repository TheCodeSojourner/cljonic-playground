#ifndef CLJONIC_NOT_EQUAL_HPP
#define CLJONIC_NOT_EQUAL_HPP

#include <cljonic-equal.hpp>

namespace cljonic {

/** \anchor NotEqual
 * \brief Compares one or more values for inequality.
 *
 * \b NotEqual is the negation of \ref Equal : it answers the same question and returns the opposite result, over the
 * same values and by the same rules.
 *
 * - Calling `not_equal` with one value always returns false, because a value is
 *   never unequal to itself.
 * - Two ordinary values are unequal when they are the same type and compare
 *   unequal with `==`.
 * - Vectors, Queues, Ranges, Repeats, Cycles, Iterates, and Repeatedlys are
 *   unequal when they do not contain or produce the same elements in the same
 *   order; producers are compared by what they produce, one element at a time,
 *   and the comparison always finishes.
 * - Maps are unequal when they do not hold the same entries, regardless of
 *   insertion order; Sets are unequal when they do not hold the same elements,
 *   regardless of insertion order; Strings are unequal when they do not hold
 *   the same characters.
 *
 * The supported values are exactly those of \ref Equal : mismatched collection element types, collections compared to
 * ordinary values, and values that cannot be compared stably (such as floating-point numbers) are rejected. With more
 * than two arguments, `not_equal` is true when at least one adjacent pair is unequal.
 *
 * \b Examples
 *
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 int main() {
   // Compile-time demonstration.
   constexpr auto v = Vector<int, 4>{1, 2, 3};
   constexpr auto r = Range<int>{1, 4, 1}; // produces 1, 2, 3
   constexpr auto q = Queue<int, 4>{1, 2, 3};
   constexpr auto m1 = Map<int, int, 4>{MapEntry<int, int>{1, 10}};
   constexpr auto m2 = Map<int, int, 4>{MapEntry<int, int>{2, 20}};
   constexpr auto s1 = Set<int, 4>{3, 1, 2};
   constexpr auto s2 = Set<int, 4>{1, 2, 3};
   static_assert(not_equal(1, 2));
   static_assert(!not_equal(1, 1));
   static_assert(!not_equal(1));                      // single value
   static_assert(not_equal(1, 2, 1));                 // one unequal pair
   static_assert(!not_equal(1, 1, 1));                // every pair equal
   static_assert(!not_equal(v, r));                   // same elements
   static_assert(not_equal(v, q) == not_equal(q, v)); // symmetric
   static_assert(not_equal(v, Vector<int, 4>{1, 2, 4}));
   static_assert(not_equal(m1, m2));  // different entries
   static_assert(!not_equal(s1, s2)); // same elements

   // Runtime demonstration.
   auto runtime_v = Vector<int, 4>{1, 2, 3};
   auto runtime_r = Range<int>{0, 3, 1}; // produces 0, 1, 2
   return not_equal(runtime_v, runtime_r) ? 0 : 1;
 }
 ~~~~~
 */
template <typename T>
    requires(concepts_detail::equal_pair_admissible_v<T, T>)
[[nodiscard]] constexpr auto not_equal(const T& value) noexcept -> bool {
    return !equal(value);
}

template <typename Lhs, typename Rhs>
    requires(concepts_detail::equal_pair_admissible_v<Lhs, Rhs>)
[[nodiscard]] constexpr auto not_equal(const Lhs& lhs, const Rhs& rhs) noexcept -> bool {
    return !equal(lhs, rhs);
}

template <typename Lhs, typename Rhs, typename... Rest>
    requires((sizeof...(Rest) >= 1) && concepts_detail::all_adjacent_pairs_admissible_v<Lhs, Rhs, Rest...>)
[[nodiscard]] constexpr auto not_equal(const Lhs& lhs, const Rhs& rhs, const Rest&... rest) noexcept -> bool {
    return !equal(lhs, rhs, rest...);
}

} // namespace cljonic

#endif // CLJONIC_NOT_EQUAL_HPP
