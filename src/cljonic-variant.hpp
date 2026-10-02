#pragma once

#include <cljonic-concepts.hpp>
#include <cstddef>
#include <type_traits>
#include <utility>
#include <variant>

namespace cljonic {

namespace variant_detail {

// The converting constructor selects exactly one alternative by decayed type;
// zero or multiple matches leave the constructor non-viable, so an ambiguous or
// foreign argument is rejected at compile time rather than converting.
template <typename T, typename... Alternatives>
inline constexpr std::size_t matching_alternative_count_v =
    (std::size_t{0} + ... + (std::same_as<T, Alternatives> ? std::size_t{1} : std::size_t{0}));

// Alternative-strict equality within a single active index. Recursion is by
// index so only same-alternative comparisons are instantiated: a two-variant
// std::visit would instantiate cross-alternative comparisons (which need not
// exist) and fail to compile.
template <std::size_t Index = 0, typename Variant>
[[nodiscard]] constexpr auto same_index_equal(const Variant& lhs, const Variant& rhs) noexcept -> bool {
    if constexpr (Index + 1 == std::variant_size_v<Variant>) {
        // Last alternative: the caller guarantees the active index matches.
        return std::get<Index>(lhs) == std::get<Index>(rhs);
    } else {
        if (lhs.index() == Index) {
            return std::get<Index>(lhs) == std::get<Index>(rhs);
        }
        return same_index_equal<Index + 1>(lhs, rhs);
    }
}

// Alternative-strict ordering within a single active index; mirrors
// same_index_equal so only same-alternative comparisons are instantiated.
template <std::size_t Index = 0, typename Variant>
[[nodiscard]] constexpr auto same_index_less(const Variant& lhs, const Variant& rhs) noexcept -> bool {
    if constexpr (Index + 1 == std::variant_size_v<Variant>) {
        // Last alternative: the caller guarantees the active index matches.
        return std::get<Index>(lhs) < std::get<Index>(rhs);
    } else {
        if (lhs.index() == Index) {
            return std::get<Index>(lhs) < std::get<Index>(rhs);
        }
        return same_index_less<Index + 1>(lhs, rhs);
    }
}

} // namespace variant_detail

/** \anchor Variant
 * \brief A composite value holding exactly one of several alternatives.
 *
 * \b Variant is the cljonic composite: it holds exactly one of its alternatives at a time and is the supported
 * composite in the cljonic value domain.
 *
 * - Every alternative must be default-constructible and copyable without
 *   throwing, so a `Variant` is a storable value: it can be a map value, a
 *   vector element, or a queue element.
 * - Two values compare equal only when they hold the same alternative and that
 *   alternative's values compare equal (alternative-strict equality). Values
 *   holding different alternatives always compare unequal.
 * - Equality is provided only when every alternative is comparable without
 *   throwing; otherwise `==` is not provided and the value cannot be used in an
 *   equality position. A `Variant` may therefore be storable without being
 *   comparable, exactly like a collection of floating-point numbers.
 * - A `Variant` always holds an active alternative: it has no empty or
 *   valueless state.
 *
 * \b Examples
 *
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 int main() {
   constexpr auto v = Variant<int, long>{1};
   static_assert(v.index() == 0);
   static_assert(v.holds<int>());
   static_assert(v == Variant<int, long>{1});
   static_assert(Variant<int, long>{1} != Variant<int, long>{1L});
   return 0;
 }
 ~~~~~
 */
template <typename... Alternatives>
    requires(sizeof...(Alternatives) >= 1) && (concepts::NothrowVariantAlternative<Alternatives> && ...)
class Variant {
  public:
    constexpr Variant() noexcept = default;

    template <typename T>
        requires(variant_detail::matching_alternative_count_v<std::remove_cvref_t<T>, Alternatives...> == 1)
    constexpr Variant(T&& value) noexcept : storage_{std::forward<T>(value)} {
    }

    /** Returns the zero-based index of the active alternative. */
    [[nodiscard]] constexpr auto index() const noexcept -> std::size_t {
        return storage_.index();
    }

    /** Reports whether the active alternative is \p T. */
    template <typename T>
    [[nodiscard]] constexpr auto holds() const noexcept -> bool {
        return std::holds_alternative<T>(storage_);
    }

    /** Alternative-strict equality; provided only when every alternative is
     *  comparable without throwing. */
    [[nodiscard]] friend constexpr auto operator==(const Variant& lhs, const Variant& rhs) noexcept -> bool
        requires(concepts::ComparableVariantAlternative<Alternatives> && ...)
    {
        return (lhs.storage_.index() == rhs.storage_.index()) &&
               variant_detail::same_index_equal(lhs.storage_, rhs.storage_);
    }

    /** Alternative-strict ordering; provided only when every alternative is
     *  totally ordered. Alternatives are ordered by index first, then by value. */
    [[nodiscard]] friend constexpr auto operator<(const Variant& lhs, const Variant& rhs) noexcept -> bool
        requires(concepts::TotallyOrdered<Alternatives> && ...)
    {
        return (lhs.storage_.index() != rhs.storage_.index())
                   ? (lhs.storage_.index() < rhs.storage_.index())
                   : variant_detail::same_index_less(lhs.storage_, rhs.storage_);
    }
    [[nodiscard]] friend constexpr auto operator>(const Variant& lhs, const Variant& rhs) noexcept -> bool
        requires(concepts::TotallyOrdered<Alternatives> && ...)
    {
        return rhs < lhs;
    }

    [[nodiscard]] friend constexpr auto operator<=(const Variant& lhs, const Variant& rhs) noexcept -> bool
        requires(concepts::TotallyOrdered<Alternatives> && ...)
    {
        return !(rhs < lhs);
    }

    [[nodiscard]] friend constexpr auto operator>=(const Variant& lhs, const Variant& rhs) noexcept -> bool
        requires(concepts::TotallyOrdered<Alternatives> && ...)
    {
        return !(lhs < rhs);
    }

  private:
    std::variant<Alternatives...> storage_{};
};

} // namespace cljonic

namespace cljonic::concepts_detail {

// Nominal admission for the cljonic composite value type (REQ-CAP-011).
template <typename... Alternatives>
struct cljonic_variant_traits<cljonic::Variant<Alternatives...>> {
    static constexpr bool is_cljonic_variant = true;
};

// Recursive component analysis: a cljonic::Variant is a composite, so each
// walker recurses over its alternatives, mirroring the collection and producer
// headers. std::variant is never admitted, so no standard-library specialization
// exists.
template <typename... Alternatives>
struct contains_floating_point<cljonic::Variant<Alternatives...>>
    : std::bool_constant<(contains_floating_point_v<Alternatives> || ...)> {};

template <typename... Alternatives>
struct contains_callable<cljonic::Variant<Alternatives...>>
    : std::bool_constant<(contains_callable_v<Alternatives> || ...)> {};

template <typename... Alternatives>
struct contains_standard_range<cljonic::Variant<Alternatives...>>
    : std::bool_constant<(contains_standard_range_v<Alternatives> || ...)> {};

} // namespace cljonic::concepts_detail
