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
 * \brief Compares one or more values for general value equality, modeled on
 *        Clojure's `=` in its three arities.
 *
 * \b Equal implements general value equality (REQ-FN-002G) over the supported stable-equality domain in the three
 * arities of Clojure's `=`:
 *
 * - `equal(x)` returns true for a single operand admitted by the same
 *   compile-time domain gating as the binary form.
 * - `equal(a, b)` compares the two operands by the family rules below. - `equal(a, b, more...)` holds exactly when
 * every adjacent operand pair
 *   compares equal by those family rules, evaluated left to right and
 *   short-circuited at the first unequal pair; every adjacent pair is
 *   individually compile-time gated, so an unsupported or mixed pair fails
 *   compilation regardless of its position in the argument list.
 *
 * - Non-collection, non-producer values compare with `==` when both operands
 *   have the same type and satisfy \ref StableEqualityComparable
 *   "StableEqualityComparable"; floating-point values are rejected at compile
 *   time (REQ-NUM-006).
 * - Cljonic collections and producers are classified into equality families:
 *   Vector, Queue, Cycle, Iterate, Range, Repeat, and Repeatedly form the
 *   sequential family and are mutually comparable by their produced sequence;
 *   Map is comparable only to Map; Set is comparable only to Set; String is
 *   comparable only to String. Cross-family pairs, mixed cljonic-to-non-cljonic
 *   pairs, and operands outside the supported domain fail at compile time.
 * - Sequential comparison is order-sensitive, compares elements lazily without
 *   eagerly materializing any producer, terminates at the first differing
 *   element pair, and compares each operand's configured observable traversal
 *   cap when both operands are unbounded, so every call terminates.
 * - Element types must be identical for sequential comparison, and Map key and
 *   value types must be identical on both sides.
 * - Equality is recursive over nested values: nested collections and composite
 *   values compare by REQ-COLL-021, and nested producers compare by producer
 *   parameter equality (REQ-FN-014B).
 *
 * \b Examples
 * ~~~~~{.cpp}
 * #include "cljonic.hpp"
 *
 * int main() {
 *   using namespace cljonic;
 *
 *   // Compile-time demonstration.
 *   constexpr auto v = Vector<int, 4>{1, 2, 3};
 *   constexpr auto r = Range<int>{1, 4, 1}; // produces 1, 2, 3
 *   constexpr auto q = Queue<int, 4>{1, 2, 3};
 *   constexpr auto rep = Repeat<int>{1, 3U};
 *   constexpr auto m1 = Map<int, int, 4>{MapEntry<int, int>{1, 10}, MapEntry<int, int>{2, 20}};
 *   constexpr auto m2 = Map<int, int, 4>{MapEntry<int, int>{2, 20}, MapEntry<int, int>{1, 10}};
 *   constexpr auto s1 = Set<int, 4>{3, 1, 2};
 *   constexpr auto s2 = Set<int, 4>{1, 2, 3};
 *   constexpr auto str1 = String<8>{"abc"};
 *   constexpr auto str2 = String<16>{"abc"};
 *   static_assert(equal(1, 1));
 *   static_assert(!equal(1, 2));
 *   static_assert(equal(1));         // unary arity
 *   static_assert(equal(1, 1, 1));   // variadic arity
 *   static_assert(!equal(1, 2, 1));  // second adjacent pair is unequal
 *   static_assert(equal(v, r, q));   // variadic sequential family comparison
 *   static_assert(equal(v, r));      // sequential family: Vector vs Range
 *   static_assert(equal(v, q));      // sequential family: Vector vs Queue
 *   static_assert(equal(Vector<int, 4>{1, 1, 1}, rep));
 *   static_assert(equal(m1, m2));    // Map equality ignores entry order
 *   static_assert(equal(s1, s2));    // Set equality ignores element order
 *   static_assert(equal(str1, str2)); // String equality compares content
 *   static_assert(equal(Repeat<int>{}, Repeat<int>{})); // bounded prefix
 *
 *   // Runtime demonstration.
 *   auto runtime_v = Vector<int, 4>{1, 2, 3};
 *   auto runtime_r = Range<int>{1, 4, 1};
 *   return equal(runtime_v, runtime_r) ? 0 : 1;
 * }
 * ~~~~~
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
[[nodiscard]] constexpr auto equal(const T& value) noexcept -> bool {
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
