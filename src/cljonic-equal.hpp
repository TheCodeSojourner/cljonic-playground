#ifndef CLJONIC_EQUAL_HPP
#define CLJONIC_EQUAL_HPP

#include <cljonic-concepts.hpp>
#include <ranges>
#include <type_traits>
#include <variant>

namespace cljonic::concepts_detail {

enum class equality_family { none, sequential, map, set, string };

// A std::variant specialization is part of the closed composite value domain
// (REQ-CAP-010) even though it is neither a scalar nor an aggregate.
template <typename T>
struct is_std_variant : std::false_type {};

template <typename... Alternatives>
struct is_std_variant<std::variant<Alternatives...>> : std::true_type {};

template <typename T>
inline constexpr bool is_std_variant_v = is_std_variant<std::remove_cvref_t<T>>::value;

// The non-cljonic fallthrough domain of `equal` (REQ-FN-002G) is the closed
// value domain: scalars and scoped enums, aggregate-like structs, and std::
// variant composites. Standard-library range and container types belong to the
// C++ interoperability surface, not the equality domain, and are rejected.
template <typename T>
inline constexpr bool in_non_cljonic_fallthrough_domain_v =
    !is_cljonic_collection_v<T> && !is_cljonic_producer_v<T> && !std::ranges::range<T> &&
    (std::is_scalar_v<T> || std::is_aggregate_v<T> || is_std_variant_v<T>);

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

// Compile-time admissibility of one operand pair (REQ-FN-002G): the
// disjunction of the binary equal overload gates, kept in lockstep with the
// five binary overloads so every arity rejects the same pairs at compile
// time. Both members of a family pair always expose the corresponding member
// types, so the void fallback only guards the non-family substitution path.
template <typename Lhs, typename Rhs>
inline constexpr bool equal_pair_admissible_v =
    (equal_family_of_v<Lhs> == equality_family::none && equal_family_of_v<Rhs> == equality_family::none &&
     in_non_cljonic_fallthrough_domain_v<Lhs> && in_non_cljonic_fallthrough_domain_v<Rhs> && std::same_as<Lhs, Rhs> &&
     concepts::StableEqualityComparable<Lhs>) ||
    (equal_family_of_v<Lhs> == equality_family::sequential && equal_family_of_v<Rhs> == equality_family::sequential &&
     std::same_as<equal_value_type_of_t<Lhs>, equal_value_type_of_t<Rhs>> &&
     concepts::StableEqualityComparable<equal_value_type_of_t<Lhs>>) ||
    (equal_family_of_v<Lhs> == equality_family::string && equal_family_of_v<Rhs> == equality_family::string) ||
    (equal_family_of_v<Lhs> == equality_family::map && equal_family_of_v<Rhs> == equality_family::map &&
     std::same_as<equal_key_type_of_t<Lhs>, equal_key_type_of_t<Rhs>> &&
     std::same_as<equal_association_value_type_of_t<Lhs>, equal_association_value_type_of_t<Rhs>> &&
     concepts::StableEqualityComparable<equal_key_type_of_t<Lhs>> &&
     concepts::StableEqualityComparable<equal_association_value_type_of_t<Lhs>>) ||
    (equal_family_of_v<Lhs> == equality_family::set && equal_family_of_v<Rhs> == equality_family::set &&
     std::same_as<equal_value_type_of_t<Lhs>, equal_value_type_of_t<Rhs>> &&
     concepts::StableEqualityComparable<equal_value_type_of_t<Lhs>>);

// Every adjacent pair of a variadic equal argument list is admissible
// (REQ-FN-002G); one or zero trailing operands satisfy the rule vacuously.
template <typename... Ts>
struct all_adjacent_pairs_admissible : std::true_type {};

template <typename First, typename Second, typename... Rest>
struct all_adjacent_pairs_admissible<First, Second, Rest...>
    : std::bool_constant<equal_pair_admissible_v<First, Second> &&
                         all_adjacent_pairs_admissible<Second, Rest...>::value> {};

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
 * - Calling `equal` with one value always returns true. - Two ordinary values compare equal when they are the same
 * type and
 *   compare equal with `==` (e.g., `int`, a scoped enum, or a simple struct
 *   with an explicit or defaulted `operator==`).
 * - Vectors, Queues, Ranges, Repeats, Cycles, Iterates, and Repeatedlys
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
 * floating-point numbers, are not supported. Nested collections and composite values compare by the same rules.
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

   // Runtime demonstration.
   auto runtime_v = Vector<int, 4>{1, 2, 3};
   auto runtime_r = Range<int>{1, 4, 1};
   return equal(runtime_v, runtime_r) ? 0 : 1;
 }
 ~~~~~
 */
template <typename Lhs, typename Rhs>
    requires((concepts_detail::equal_family_of_v<Lhs> == concepts_detail::equality_family::none) &&
             (concepts_detail::equal_family_of_v<Rhs> == concepts_detail::equality_family::none) &&
             concepts_detail::in_non_cljonic_fallthrough_domain_v<Lhs> &&
             concepts_detail::in_non_cljonic_fallthrough_domain_v<Rhs> && std::same_as<Lhs, Rhs> &&
             concepts::StableEqualityComparable<Lhs>)
[[nodiscard]] constexpr auto equal(const Lhs& lhs, const Rhs& rhs) noexcept -> bool {
    return lhs == rhs;
}

template <typename Lhs, typename Rhs>
    requires((concepts_detail::equal_family_of_v<Lhs> == concepts_detail::equality_family::sequential) &&
             (concepts_detail::equal_family_of_v<Rhs> == concepts_detail::equality_family::sequential) &&
             std::same_as<typename Lhs::value_type, typename Rhs::value_type> &&
             concepts::StableEqualityComparable<typename Lhs::value_type>)
[[nodiscard]] constexpr auto equal(const Lhs& lhs, const Rhs& rhs) noexcept -> bool {
    return concepts_detail::equal_prefix_walk(lhs, rhs);
}

template <typename Lhs, typename Rhs>
    requires((concepts_detail::equal_family_of_v<Lhs> == concepts_detail::equality_family::string) &&
             (concepts_detail::equal_family_of_v<Rhs> == concepts_detail::equality_family::string))
[[nodiscard]] constexpr auto equal(const Lhs& lhs, const Rhs& rhs) noexcept -> bool {
    return concepts_detail::equal_prefix_walk(lhs, rhs);
}

template <typename Lhs, typename Rhs>
    requires((concepts_detail::equal_family_of_v<Lhs> == concepts_detail::equality_family::map) &&
             (concepts_detail::equal_family_of_v<Rhs> == concepts_detail::equality_family::map) &&
             std::same_as<typename Lhs::key_type, typename Rhs::key_type> &&
             std::same_as<typename Lhs::association_value_type, typename Rhs::association_value_type> &&
             concepts::StableEqualityComparable<typename Lhs::key_type> &&
             concepts::StableEqualityComparable<typename Lhs::association_value_type>)
[[nodiscard]] constexpr auto equal(const Lhs& lhs, const Rhs& rhs) noexcept -> bool {
    if (lhs.count() != rhs.count()) {
        return false;
    }
    return equal_match_detail::map_entries_all_match(lhs, rhs);
}

template <typename Lhs, typename Rhs>
    requires((concepts_detail::equal_family_of_v<Lhs> == concepts_detail::equality_family::set) &&
             (concepts_detail::equal_family_of_v<Rhs> == concepts_detail::equality_family::set) &&
             std::same_as<typename Lhs::value_type, typename Rhs::value_type> &&
             concepts::StableEqualityComparable<typename Lhs::value_type>)
[[nodiscard]] constexpr auto equal(const Lhs& lhs, const Rhs& rhs) noexcept -> bool {
    if (lhs.count() != rhs.count()) {
        return false;
    }
    return equal_match_detail::set_elements_all_contained(lhs, rhs);
}

template <typename T>
    requires(concepts_detail::equal_pair_admissible_v<T, T>)
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

} // namespace cljonic

#endif // CLJONIC_EQUAL_HPP
