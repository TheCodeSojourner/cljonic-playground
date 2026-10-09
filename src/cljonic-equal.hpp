#pragma once

#include <cljonic-concepts.hpp>
#include <ranges>
#include <type_traits>

namespace cljonic::concepts_detail {

enum class equality_family { none, sequential, map, set, string };

// The non-cljonic fallthrough domain of `equal` (REQ-FN-002G) is the closed
// value domain: arithmetic scalars and scoped enums, aggregate-like structs,
// and cljonic::Variant composites. Standard-library range and container types
// belong to the C++ interoperability surface, not the equality domain, and are
// rejected, as is the standard-library variant (it is not a cljonic value
// type). Unscoped enums and pointers (including member pointers) are also
// outside the domain: `std::is_scalar_v` would otherwise admit them.
template <typename T>
inline constexpr bool in_non_cljonic_fallthrough_domain_v =
    !is_cljonic_collection_v<T> && !is_cljonic_producer_v<T> && !std::ranges::range<T> &&
    ((std::is_arithmetic_v<T> || std::is_scoped_enum_v<T>) || std::is_aggregate_v<T> || is_cljonic_variant_v<T>);

// Equality-family classification for the general-equality free function `equal`
// (REQ-FN-002G): Vector and Queue join all five producers in the sequential
// family; Map, Set, and String each form their own family; every other type is
// outside the family classification.
template <typename T>
inline constexpr equality_family equal_family_of_v = []() constexpr -> equality_family {
    if constexpr (is_cljonic_collection_v<T>) {
        switch (collection_kind_of_v<T>) {
        case collection_kind::vector:
        case collection_kind::queue:
            return equality_family::sequential;
        case collection_kind::map:
            return equality_family::map;
        case collection_kind::set:
            return equality_family::set;
        case collection_kind::string:
            return equality_family::string;
        default:
            return equality_family::none;
        }
    } else if constexpr (is_cljonic_producer_v<T>) {
        return equality_family::sequential;
    } else {
        return equality_family::none;
    }
}();

// SFINAE-safe member-type detection: the admissibility predicates below must
// stay substitution-safe over the full closed value domain, where most types
// carry no collection member types at all; the void fallback keeps the
// corresponding disjunct false instead of ill-formed.
template <typename T, typename = void>
struct equal_value_type_of {
    using type = void;
};

template <typename T>
struct equal_value_type_of<T, std::void_t<typename T::value_type>> {
    using type = T::value_type;
};

template <typename T, typename = void>
struct equal_key_type_of {
    using type = void;
};

template <typename T>
struct equal_key_type_of<T, std::void_t<typename T::key_type>> {
    using type = T::key_type;
};

template <typename T, typename = void>
struct equal_association_value_type_of {
    using type = void;
};

template <typename T>
struct equal_association_value_type_of<T, std::void_t<typename T::association_value_type>> {
    using type = T::association_value_type;
};

template <typename T>
using equal_value_type_of_t = equal_value_type_of<T>::type;

template <typename T>
using equal_key_type_of_t = equal_key_type_of<T>::type;

template <typename T>
using equal_association_value_type_of_t = equal_association_value_type_of<T>::type;

// The value domain admitted by equal's family gates: stable equality whose
// comparison cannot throw (so the noexcept guarantee cannot be violated) and
// free of standard-library range components at any depth (equal is not part of
// the C++ interoperability surface).
template <typename T>
concept EqualDomainValue = concepts::NothrowEqualityComparable<T> && !contains_standard_range_v<T>;

// Equality-family gates shared by equal's overloads and not_equal's delegated
// gate (REQ-FN-002G, REQ-FN-002H); these are not ordering capabilities.
template <typename Lhs, typename Rhs>
concept EqualScalarPairAdmissible =
    equal_family_of_v<Lhs> == equality_family::none && equal_family_of_v<Rhs> == equality_family::none &&
    in_non_cljonic_fallthrough_domain_v<Lhs> && in_non_cljonic_fallthrough_domain_v<Rhs> && std::same_as<Lhs, Rhs> &&
    EqualDomainValue<Lhs>;

template <typename Lhs, typename Rhs>
concept EqualSequentialPairAdmissible =
    equal_family_of_v<Lhs> == equality_family::sequential && equal_family_of_v<Rhs> == equality_family::sequential &&
    std::same_as<equal_value_type_of_t<Lhs>, equal_value_type_of_t<Rhs>> &&
    EqualDomainValue<equal_value_type_of_t<Lhs>>;

template <typename Lhs, typename Rhs>
concept EqualStringPairAdmissible =
    equal_family_of_v<Lhs> == equality_family::string && equal_family_of_v<Rhs> == equality_family::string;

template <typename Lhs, typename Rhs>
concept EqualMapPairAdmissible =
    equal_family_of_v<Lhs> == equality_family::map && equal_family_of_v<Rhs> == equality_family::map &&
    std::same_as<equal_key_type_of_t<Lhs>, equal_key_type_of_t<Rhs>> &&
    std::same_as<equal_association_value_type_of_t<Lhs>, equal_association_value_type_of_t<Rhs>> &&
    EqualDomainValue<equal_key_type_of_t<Lhs>> && EqualDomainValue<equal_association_value_type_of_t<Lhs>>;

template <typename Lhs, typename Rhs>
concept EqualSetPairAdmissible =
    equal_family_of_v<Lhs> == equality_family::set && equal_family_of_v<Rhs> == equality_family::set &&
    std::same_as<equal_value_type_of_t<Lhs>, equal_value_type_of_t<Rhs>> &&
    EqualDomainValue<equal_value_type_of_t<Lhs>>;

// Keep every arity on the same family gates so unsupported pairs fail at
// compile time regardless of their position in the argument list.
template <typename Lhs, typename Rhs>
concept EqualPairAdmissible =
    EqualScalarPairAdmissible<Lhs, Rhs> || EqualSequentialPairAdmissible<Lhs, Rhs> ||
    EqualStringPairAdmissible<Lhs, Rhs> || EqualMapPairAdmissible<Lhs, Rhs> || EqualSetPairAdmissible<Lhs, Rhs>;

// Every adjacent pair of a variadic equal argument list is admissible
// (REQ-FN-002G); one or zero trailing operands satisfy the rule vacuously.
template <typename... Ts>
struct all_adjacent_pairs_admissible : std::true_type {};

template <typename First, typename Second, typename... Rest>
struct all_adjacent_pairs_admissible<First, Second, Rest...>
    : std::bool_constant<EqualPairAdmissible<First, Second> && all_adjacent_pairs_admissible<Second, Rest...>::value> {
};

template <typename... Ts>
inline constexpr bool all_adjacent_pairs_admissible_v = all_adjacent_pairs_admissible<Ts...>::value;

// Lazy element-wise bounded-prefix walk: terminates at the first differing
// element pair and requires both operands to be exhausted simultaneously for
// equality. All producers expose cap-bounded begin()/end() traversal, so the
// walk always terminates, including for two unbounded producers.
namespace equal_walk_detail {

// Reports whether both iterators have reached their respective ends after the
// walk, which requires the two sequences to have exhausted simultaneously.
template <typename LhsIterator, typename RhsIterator, typename LhsEnd, typename RhsEnd>
[[nodiscard]] constexpr auto both_exhausted(const LhsIterator& lhs_iterator, const LhsEnd& lhs_end,
                                            const RhsIterator& rhs_iterator, const RhsEnd& rhs_end) noexcept -> bool {
    return (lhs_iterator == lhs_end) && (rhs_iterator == rhs_end);
}

} // namespace equal_walk_detail

template <typename Lhs, typename Rhs>
[[nodiscard]] constexpr auto equal_prefix_walk(const Lhs& lhs, const Rhs& rhs) noexcept -> bool {
    using equal_walk_detail::both_exhausted;
    auto lhs_iterator = lhs.begin();
    const auto lhs_end = lhs.end();
    auto rhs_iterator = rhs.begin();
    const auto rhs_end = rhs.end();
    while ((lhs_iterator != lhs_end) && (rhs_iterator != rhs_end)) {
        if (!(*lhs_iterator == *rhs_iterator)) {
            return false;
        }
        ++lhs_iterator;
        ++rhs_iterator;
    }
    return both_exhausted(lhs_iterator, lhs_end, rhs_iterator, rhs_end);
}

} // namespace cljonic::concepts_detail

namespace cljonic::equal_match_detail {

// Order-insensitive Map matching: every logical entry of the left operand must
// have a matching key in the right operand whose associated value compares
// equal (REQ-COLL-021 Map equality semantics).
template <typename Lhs, typename Rhs>
[[nodiscard]] constexpr auto map_entries_all_match(const Lhs& lhs, const Rhs& rhs) noexcept -> bool {
    for (const auto& entry : lhs) {
        if (!rhs.contains(entry.key) || !(rhs(entry.key) == entry.value)) {
            return false;
        }
    }
    return true;
}

// Order-insensitive Set matching: every logical element of the left operand
// must be present in the right operand (REQ-COLL-021 Set equality semantics).
template <typename Lhs, typename Rhs>
[[nodiscard]] constexpr auto set_elements_all_contained(const Lhs& lhs, const Rhs& rhs) noexcept -> bool {
    for (const auto& element : lhs) {
        if (!rhs.contains(element)) {
            return false;
        }
    }
    return true;
}

} // namespace cljonic::equal_match_detail

namespace cljonic {

/** \anchor Equal
 * \brief Compares one or more values for equality.
 *
 * \b Equal compares values by their contents:
 *
 * - Calling `equal` with one value always returns true.
 * - Two ordinary values compare equal when they are the same type and compare
 *   equal with `==` (e.g., `int`, a scoped enum, or a simple struct with an
 *   explicit or defaulted `operator==`). The comparison must not throw, so a
 *   type whose `==` can throw is not supported; pointers and unscoped enums
 *   are not supported either.
 * - Vectors, Queues, Ranges, Repeats, Cycles, and the Iterate and Repeatedly
 *   producers
 *   compare equal when they contain or produce the same elements in the same
 *   order. Producers are compared by what they produce, one element at a
 *   time, and comparison always finishes: no producer yields more than
 *   `CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT` elements, so two unbounded
 *   producers are compared over that configured maximum.
 * - Maps compare equal when they hold the same entries, regardless of
 *   insertion order.
 * - Sets compare equal when they hold the same elements, regardless of
 *   insertion order.
 * - Strings compare equal when they hold the same characters.
 *
 * Compared collections and sets must have matching element types, and compared maps must have matching key and value
 * types. Collections are never compared to ordinary values. Values that cannot be compared stably, such as
 * floating-point numbers, are not supported. Standard-library range and container types are not supported at any
 * depth. Nested collections and composite values compare by the same rules; a nested producer is compared by its
 * stored parameters.
 *
 * With more than two arguments, every adjacent pair is compared, from left to right, stopping at the first unequal
 * pair.
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
   constexpr auto rep = Repeat<int>{1, 3U};
   constexpr auto m1 =
       Map<int, int, 4>{MapEntry<int, int>{1, 10}, MapEntry<int, int>{2, 20}};
   constexpr auto m2 =
       Map<int, int, 4>{MapEntry<int, int>{2, 20}, MapEntry<int, int>{1, 10}};
   constexpr auto s1 = Set<int, 4>{3, 1, 2};
   constexpr auto s2 = Set<int, 4>{1, 2, 3};
   constexpr auto str1 = String<8>{"abc"};
   constexpr auto str2 = String<16>{"abc"};
   static_assert(equal(1, 1));
   static_assert(!equal(1, 2));
   static_assert(equal(1));        // single value
   static_assert(equal(1, 1, 1));  // multiple values, all equal
   static_assert(!equal(1, 2, 1)); // stops at the first unequal pair
   static_assert(equal(v, r, q));  // Vector, Range, and Queue with the
                                   // same elements compare equal
   static_assert(equal(v, r));     // Vector vs Range
   static_assert(equal(v, q));     // Vector vs Queue
   static_assert(equal(Vector<int, 4>{1, 1, 1}, rep));
   static_assert(equal(m1, m2));     // Map equality ignores entry order
   static_assert(equal(s1, s2));     // Set equality ignores element order
   static_assert(equal(str1, str2)); // String equality compares content
   static_assert(equal(Repeat<int>{}, Repeat<int>{})); // bounded prefix

   // Variant equality is alternative-strict
   constexpr auto var_int = Variant<int, long>{1};
   static_assert(
       equal(var_int, Variant<int, long>{1})); // same alternative and value
   static_assert(
       !equal(var_int, Variant<int, long>{1L})); // different alternative

   // Runtime demonstration.
   auto runtime_v = Vector<int, 4>{1, 2, 3};
   auto runtime_r = Range<int>{1, 4, 1};
   return equal(runtime_v, runtime_r) ? 0 : 1;
 }
 ~~~~~
 */
template <typename Lhs, typename Rhs>
    requires concepts_detail::EqualScalarPairAdmissible<Lhs, Rhs>
[[nodiscard]] constexpr auto equal(const Lhs& lhs, const Rhs& rhs) noexcept -> bool {
    return lhs == rhs;
}

template <typename Lhs, typename Rhs>
    requires concepts_detail::EqualSequentialPairAdmissible<Lhs, Rhs>
[[nodiscard]] constexpr auto equal(const Lhs& lhs, const Rhs& rhs) noexcept -> bool {
    return concepts_detail::equal_prefix_walk(lhs, rhs);
}

template <typename Lhs, typename Rhs>
    requires concepts_detail::EqualStringPairAdmissible<Lhs, Rhs>
[[nodiscard]] constexpr auto equal(const Lhs& lhs, const Rhs& rhs) noexcept -> bool {
    return concepts_detail::equal_prefix_walk(lhs, rhs);
}

template <typename Lhs, typename Rhs>
    requires concepts_detail::EqualMapPairAdmissible<Lhs, Rhs>
[[nodiscard]] constexpr auto equal(const Lhs& lhs, const Rhs& rhs) noexcept -> bool {
    if (lhs.count() != rhs.count()) {
        return false;
    }
    return equal_match_detail::map_entries_all_match(lhs, rhs);
}

template <typename Lhs, typename Rhs>
    requires concepts_detail::EqualSetPairAdmissible<Lhs, Rhs>
[[nodiscard]] constexpr auto equal(const Lhs& lhs, const Rhs& rhs) noexcept -> bool {
    if (lhs.count() != rhs.count()) {
        return false;
    }
    return equal_match_detail::set_elements_all_contained(lhs, rhs);
}

template <typename T>
    requires concepts_detail::EqualPairAdmissible<T, T>
[[nodiscard]] constexpr auto equal([[maybe_unused]] const T& value) noexcept -> bool {
    return true;
}

template <typename Lhs, typename Rhs, typename... Rest>
    requires((sizeof...(Rest) >= 1) && concepts_detail::all_adjacent_pairs_admissible_v<Lhs, Rhs, Rest...>)
[[nodiscard]] constexpr auto equal(const Lhs& lhs, const Rhs& rhs, const Rest&... rest) noexcept -> bool {
    if (!equal(lhs, rhs)) {
        return false;
    }
    return equal(rhs, rest...);
}

// Diagnostic fallbacks (REQ-DIAG-009): a single targeted message for an operand
// outside the supported equality domain, in place of a list of rejected concept
// candidates. They explain rejection and are never a supported call target;
// domain support is detected through the admission concepts, not callability.
template <typename T>
    requires(!concepts_detail::EqualPairAdmissible<T, T>)
[[nodiscard]] constexpr auto equal([[maybe_unused]] const T& value) -> bool {
    static_assert(concepts_detail::dependent_false<T>,
                  "cljonic::equal: this value cannot be compared for equality. Floating-point values, callables, "
                  "pointers, unscoped enums, standard-library range and container types, the standard-library variant, "
                  "and values whose equality can throw are not supported.");
    return false;
}

template <typename Lhs, typename Rhs>
    requires(!concepts_detail::EqualPairAdmissible<Lhs, Rhs>)
[[nodiscard]] constexpr auto equal([[maybe_unused]] const Lhs& lhs, [[maybe_unused]] const Rhs& rhs) -> bool {
    static_assert(concepts_detail::dependent_false<Lhs, Rhs>,
                  "cljonic::equal: these two values cannot be compared for equality. They must both be ordinary values "
                  "of the same type, or two values from the same cljonic family -- Vector, Queue, and all producers "
                  "(the sequential family), a Map, a Set, or a String. Floating-point values, callables, pointers, "
                  "unscoped enums, standard-library range and container types, the standard-library variant, values "
                  "whose equality can throw, and pairs that mix cljonic and non-cljonic values are not supported.");
    return false;
}

template <typename Lhs, typename Rhs, typename... Rest>
    requires((sizeof...(Rest) >= 1) && (!concepts_detail::all_adjacent_pairs_admissible_v<Lhs, Rhs, Rest...>))
[[nodiscard]] constexpr auto equal([[maybe_unused]] const Lhs& lhs, [[maybe_unused]] const Rhs& rhs,
                                   [[maybe_unused]] const Rest&... rest) -> bool {
    static_assert(
        concepts_detail::dependent_false<Lhs, Rhs>,
        "cljonic::equal: at least one adjacent pair of values cannot be compared for equality. With three or more "
        "operands, every adjacent pair must follow the same rule as the two-operand form -- both ordinary values of "
        "the same type, or two values from the same cljonic family -- Vector, Queue, and all producers (the sequential "
        "family), a Map, a Set, or a String. Floating-point values, callables, pointers, unscoped enums, "
        "standard-library range and container types, the standard-library variant, values whose equality can throw, "
        "and pairs that mix cljonic and non-cljonic values are not supported.");
    return false;
}

} // namespace cljonic
