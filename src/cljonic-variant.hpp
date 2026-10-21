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

namespace concepts_detail {

/** Internal tag (REQ-DIAG-012): selects the `Variant` diagnostic
 *  specialization. A `Variant`'s alternatives are never deducible from its
 *  constructor arguments, so any deduced `Variant` is a rejected deduction. */
struct VariantDomainViolation {};

/** The `Variant` class-template gate (REQ-DIAG-012): at least one storable
 *  alternative, or the internal diagnostic tag used only to redirect a rejected
 *  deduction. */
template <typename... Alternatives>
concept VariantClassArgument =
    (sizeof...(Alternatives) >= 1 && (concepts::NothrowVariantAlternative<Alternatives> && ...)) ||
    (sizeof...(Alternatives) == 1 && (std::same_as<std::remove_cvref_t<Alternatives>, VariantDomainViolation> && ...));

} // namespace concepts_detail

/** \anchor Variant
 * A composite value holding exactly one of several alternatives.
 *
 * A composite value holding exactly one of several alternatives. It is the supported composite in the cljonic value
 * domain.
 *
 * - Every alternative must have a default value and be copyable without
 *   throwing, so a Variant is a storable value: it can be a map value, a
 *   vector element, or a queue element.
 * - Two values compare equal only when they hold the same alternative and that
 *   alternative's values compare equal (alternative-strict equality). Values
 *   holding different alternatives always compare unequal.
 * - Equality is provided only when every alternative is comparable without
 *   throwing; otherwise `==` is not provided and the value cannot be used in an
 *   equality position. A Variant may therefore be storable without being
 *   comparable, exactly like a collection of floating-point numbers. When it is
 *   comparable, a Variant takes part in cljonic equality: as an `equal` or
 *   `not_equal` operand, as a map key or set element, and as a stored member of a
 *   collection or producer whose own equality is used; when it is not, all of
 *   those equality positions reject it.
 * - A Variant always holds an active alternative: it has no empty or
 *   valueless state.
 *
 * \b Examples
 *
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 int main() {
   // Default construction holds the first alternative at its default value; the
   // first alternative here is int, whose default value is 0.
   [[maybe_unused]] constexpr auto defaulted = Variant<int, long>{};

   // A converting construction selects the alternative matching the argument's
   // decayed type; an argument matching no alternative is rejected at compile
   // time.
   [[maybe_unused]] constexpr auto as_int = Variant<int, long>{1};
   [[maybe_unused]] constexpr auto as_long = Variant<int, long>{1L};

   // Copy construction preserves the active alternative and its value.
   [[maybe_unused]] constexpr auto copied = as_int;

   // A Variant nests inside another Variant and is storable as a collection
   // element or a map key.
   [[maybe_unused]] constexpr auto nested =
       Variant<Variant<int, long>, char>{as_int};
   [[maybe_unused]] constexpr auto as_vector_element =
       Vector{Variant<int, long>{1}, Variant<int, long>{2L}};
   [[maybe_unused]] constexpr auto as_set_element = Set{Variant<int, long>{1}};
   [[maybe_unused]] constexpr auto as_map_key = MapEntry{as_int, 1};

   return 0;
 }
 ~~~~~
 */
template <typename... Alternatives>
    requires concepts_detail::VariantClassArgument<Alternatives...>
class Variant {
  public:
    constexpr Variant() noexcept = default;

    template <typename T>
        requires(variant_detail::matching_alternative_count_v<std::remove_cvref_t<T>, Alternatives...> == 1)
    explicit constexpr Variant(const T& value) noexcept : storage_{value} {
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

/** Diagnostic specialization (REQ-DIAG-012): selected by the redirecting
 *  deduction guide below. A `Variant`'s alternatives are never deducible from
 *  its constructor arguments, so every deduced `Variant` reaches this
 *  specialization, which turns the failed-deduction candidate list into one
 *  targeted message. */
template <>
class Variant<concepts_detail::VariantDomainViolation> {
  public:
    template <typename... Args>
    constexpr explicit Variant([[maybe_unused]] Args&&... args) noexcept {
        static_assert(concepts_detail::dependent_false<Args...>,
                      "cljonic::Variant: the alternative types cannot be deduced from this argument. Name the "
                      "alternatives explicitly, as in cljonic::Variant<int, long>{value}.");
    }
};

/** Redirecting deduction guide (REQ-DIAG-012). No admitted `Variant` deduction
 *  exists, so this guide can never affect a supported construction. */
template <typename T>
Variant(const T&) -> Variant<concepts_detail::VariantDomainViolation>;

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
