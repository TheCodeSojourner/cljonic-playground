#pragma once

#include <cljonic-equal.hpp>

namespace cljonic {

/** \anchor NotEqual
 * The \b not_equal free function API compares one or more values for inequality.
 *
 * \b NotEqual is the negation of \ref Equal "equal" and uses the same supported values and comparison rules. Calling
 * `not_equal` with one value returns false. With multiple values, it returns true when at least one adjacent pair is
 * unequal.
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

   // Variant inequality follows alternative-strict equality
   constexpr auto var_int = Variant<int, long>{1};
   static_assert(
       not_equal(var_int, Variant<int, long>{1L})); // different alternative
   static_assert(
       !not_equal(var_int, Variant<int, long>{1})); // same alternative and value

   // Runtime demonstration.
   auto runtime_v = Vector<int, 4>{1, 2, 3};
   auto runtime_r = Range<int>{0, 3, 1}; // produces 0, 1, 2
   return not_equal(runtime_v, runtime_r) ? 0 : 1;
 }
 ~~~~~
 */
template <typename T>
    requires concepts_detail::EqualPairAdmissible<T, T>
[[nodiscard]] constexpr auto not_equal(const T& value) noexcept -> bool {
    return !equal(value);
}

template <typename Lhs, typename Rhs>
    requires concepts_detail::EqualPairAdmissible<Lhs, Rhs>
[[nodiscard]] constexpr auto not_equal(const Lhs& lhs, const Rhs& rhs) noexcept -> bool {
    return !equal(lhs, rhs);
}

template <typename Lhs, typename Rhs, typename... Rest>
    requires((sizeof...(Rest) >= 1) && concepts_detail::all_adjacent_pairs_admissible_v<Lhs, Rhs, Rest...>)
[[nodiscard]] constexpr auto not_equal(const Lhs& lhs, const Rhs& rhs, const Rest&... rest) noexcept -> bool {
    return !equal(lhs, rhs, rest...);
}

// Diagnostic fallbacks (REQ-DIAG-009): self-contained targeted messages over the
// domain `not_equal` shares with `equal`; never a supported call target.
template <typename T>
    requires(!concepts_detail::EqualPairAdmissible<T, T>)
[[nodiscard]] constexpr auto not_equal([[maybe_unused]] const T& value) -> bool {
    static_assert(
        concepts_detail::dependent_false<T>,
        "cljonic::not_equal: this value cannot be compared for inequality. not_equal supports the same values as "
        "equal. Floating-point values, callables, pointers, unscoped enums, standard-library range and container "
        "types, the standard-library variant, and values whose equality can throw are not supported.");
    return false;
}

template <typename Lhs, typename Rhs>
    requires(!concepts_detail::EqualPairAdmissible<Lhs, Rhs>)
[[nodiscard]] constexpr auto not_equal([[maybe_unused]] const Lhs& lhs, [[maybe_unused]] const Rhs& rhs) -> bool {
    static_assert(concepts_detail::dependent_false<Lhs, Rhs>,
                  "cljonic::not_equal: these two values cannot be compared for inequality. not_equal supports the same "
                  "values as equal -- both must be ordinary values of the same type, or two values from the same "
                  "cljonic family: Vector, Queue, and all producers (the sequential family), a Map, a Set, or a "
                  "String. Floating-point values, callables, pointers, unscoped enums, standard-library range and "
                  "container types, the standard-library variant, values whose equality can throw, and pairs that mix "
                  "cljonic and non-cljonic values are not supported.");
    return false;
}

template <typename Lhs, typename Rhs, typename... Rest>
    requires((sizeof...(Rest) >= 1) && (!concepts_detail::all_adjacent_pairs_admissible_v<Lhs, Rhs, Rest...>))
[[nodiscard]] constexpr auto not_equal([[maybe_unused]] const Lhs& lhs, [[maybe_unused]] const Rhs& rhs,
                                       [[maybe_unused]] const Rest&... rest) -> bool {
    static_assert(concepts_detail::dependent_false<Lhs, Rhs>,
                  "cljonic::not_equal: at least one adjacent pair of values cannot be compared for inequality. "
                  "not_equal supports the same values as equal. With three or more operands, every adjacent pair must "
                  "follow the same rule as the two-operand form -- both ordinary values of the same type, or two "
                  "values from the same cljonic family -- Vector, Queue, and all producers (the sequential family), a "
                  "Map, a Set, or a String. Floating-point values, callables, pointers, unscoped enums, "
                  "standard-library range and container types, the standard-library variant, values whose equality can "
                  "throw, and pairs that mix cljonic and non-cljonic values are not supported.");
    return false;
}

} // namespace cljonic
