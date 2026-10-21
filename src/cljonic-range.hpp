#pragma once

#include <concepts>
#include <cstddef>
#include <type_traits>

#include <cljonic-concepts.hpp>
#include <cljonic-core-collection-maximum-element-count.hpp>

namespace cljonic {

namespace concepts_detail {

/** Internal tag (REQ-DIAG-012): selects the `Range` diagnostic specialization
 *  during an out-of-domain class template argument deduction. It is never a
 *  supported element type and is never produced by an admitted call. */
struct RangeElementDomainViolation {};

/** The `Range` class-template gate (REQ-DIAG-012): a valid `Range` element
 *  domain, or the internal diagnostic tag used only to redirect a rejected
 *  deduction. Keeping the valid domain first preserves soft, SFINAE-friendly
 *  type-level rejection for an out-of-domain element type. */
template <typename T>
concept RangeClassArgument =
    concepts::RangeElement<T> || std::same_as<std::remove_cvref_t<T>, RangeElementDomainViolation>;

} // namespace concepts_detail

/** \anchor Range
 * A Range is a producer describing an arithmetic sequence from an inclusive \p start to an exclusive \p end by a fixed
 * \p step, defaulting to start `0` and step `1`. A zero step repeats \p start indefinitely, taking precedence over
 * otherwise-empty-range cases, including equal start and end. A nonzero step that moves away from \p end produces an
 * empty range. Observation is bounded by its finite span, by \b CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE for a
 * zero-step range, or by an explicit destination capacity, and it supports const C++ range traversal in both constant
 * evaluation and runtime code. Values can be passed directly to cljonic operations that accept a source, or converted
 * into an owning destination with `into` when explicit materialization is required. Explicitly typed parameters follow
 * the \ref implicit-conversions "safe implicit conversion rules".
 *
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;
 int main() {
   // Default: start 0, end 0, step 1 (empty). CTAD cannot deduce from zero
   // arguments.
   [[maybe_unused]] constexpr auto empty = Range<int>{};

   // Single argument is the exclusive end; start defaults to 0, step to 1.
   [[maybe_unused]] constexpr auto to_five = Range{5};

   // Explicit start and end; step still defaults to 1.
   [[maybe_unused]] constexpr auto two_to_five = Range{2, 5};

   // Explicit start, end, and step.
   [[maybe_unused]] constexpr auto evens = Range{0, 10, 2};

   // A zero step repeats start indefinitely rather than being empty.
   [[maybe_unused]] constexpr auto repeating = Range{0, 5, 0};

   // A billion elements exceeds the system's allowed maximum, so only that
   // maximum number of elements is available.
   [[maybe_unused]] constexpr auto huge = Range{1000000000L};

   // -----------------------------------------------------------------------
   // C++ interoperability: a Range supports const traversal.
   // -----------------------------------------------------------------------
   constexpr auto odds = Range{1, 12, 2};
   auto runtime_odds = Range{-1, -11, -2};

   // Use C++ interoperability to sum the values in a constexpr range
   constexpr auto odds_sum = [odds] {
     auto sum = 0;
     for (const auto value : odds) {
       sum += value;
     }
     return sum;
   }();
   static_assert(odds_sum == 36);

   // Use C++ interoperability to sum the values in a runtime range
   auto runtime_odds_sum = 0;
   for (const auto value : runtime_odds) {
     runtime_odds_sum += value;
   }

   return runtime_odds_sum == -25 ? 0 : 1;
 }
 ~~~~~
 */
template <concepts_detail::RangeClassArgument T>
class Range {
  public:
    using value_type = T;

    /** A bounded, generating const iterator used by free-function observation
     *  (e.g. \ref Into "into"); it does not expose start/end/step parameters. */
    class const_iterator {
      public:
        using value_type = T;
        using difference_type = std::ptrdiff_t;

        constexpr const_iterator() noexcept = default;

        constexpr const_iterator(T current, std::size_t remaining, T step) noexcept
            : current_(current), remaining_(remaining), step_(step) {
        }

        [[nodiscard]] constexpr auto operator*() const noexcept -> T {
            return current_;
        }

        constexpr auto operator++() noexcept -> const_iterator& {
            if (remaining_ > 1U) {
                current_ = static_cast<T>(current_ + step_);
            }
            --remaining_;
            return *this;
        }

        constexpr auto operator++(int) noexcept -> const_iterator {
            auto previous = *this;
            ++(*this);
            return previous;
        }

        [[nodiscard]] friend constexpr auto operator==(const const_iterator& lhs, const const_iterator& rhs) noexcept
            -> bool {
            return lhs.remaining_ == rhs.remaining_;
        }

      private:
        T current_{};
        std::size_t remaining_{0U};
        T step_{};
    };

    constexpr Range() noexcept : start_(T{0}), end_(T{0}), step_(T{1}) {
    }

    constexpr explicit Range(T end) noexcept : start_(T{0}), end_(end), step_(T{1}) {
    }

    constexpr Range(T start, T end) noexcept : start_(start), end_(end), step_(T{1}) {
    }

    constexpr Range(T start, T end, T step) noexcept : start_(start), end_(end), step_(step) {
    }

    template <typename End>
        requires(!concepts::SafeImplicitConversion<T, End>)
    explicit Range(End end) = delete;

    template <typename Start, typename End>
        requires(!concepts::SafeImplicitConversion<T, Start> || !concepts::SafeImplicitConversion<T, End>)
    Range(Start start, End end) = delete;

    template <typename Start, typename End, typename Step>
        requires(!concepts::SafeImplicitConversion<T, Start> || !concepts::SafeImplicitConversion<T, End> ||
                 !concepts::SafeImplicitConversion<T, Step>)
    Range(Start start, End end, Step step) = delete;

    /** Conservative materialization size: the synthesis cap for a zero step, otherwise the
     *  exact finite cardinality saturated at the synthesis cap. */
    [[nodiscard]] constexpr auto count() const noexcept -> std::size_t {
        return step_ == T{0} ? CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE : saturate(nonzero_step_extent());
    }

    /** Reports whether this Range has a finite complete result. */
    [[nodiscard]] constexpr auto is_finite() const noexcept -> bool {
        return step_ != T{0};
    }

    /** Index-in-range predicate over the available bounded prefix; O(1), no traversal. */
    [[nodiscard]] constexpr auto contains(std::size_t index) const noexcept -> bool {
        return index < count();
    }

    template <typename Index>
        requires(!concepts::SafeImplicitConversion<std::size_t, Index>)
    auto contains(Index index) const -> bool = delete;

    /** Bounded generating iteration up to the effective size returned by count(). */
    [[nodiscard]] constexpr auto begin() const noexcept -> const_iterator {
        return const_iterator{start_, count(), step_};
    }

    [[nodiscard]] constexpr auto end() const noexcept -> const_iterator {
        return const_iterator{T{}, 0U, T{}};
    }

    /** Producer parameter equality (REQ-FN-014B): compares only the stored
     *  start/end/step parameters, never the produced sequence. It is the only
     *  producer parameter-equality surface; the `equal` free function instead
     *  compares bounded logical observations of the produced sequences. Two
     *  Range values with different parameters compare unequal even when their
     *  produced or observed sequences coincide (for example two zero-step
     *  ranges with different endpoints both produce an infinite repetition of
     *  their respective start values). O(1), no traversal, no allocation. */
    [[nodiscard]] friend constexpr auto operator==(const Range& lhs, const Range& rhs) noexcept -> bool {
        return lhs.start_ == rhs.start_ && lhs.end_ == rhs.end_ && lhs.step_ == rhs.step_;
    }

  private:
    using ExtentType = std::make_unsigned_t<T>;

    /** Caps an internal extent at the public materialization maximum. */
    [[nodiscard]] static constexpr auto saturate(ExtentType raw_count) noexcept -> std::size_t {
        return raw_count > CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE
                   ? CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE
                   : static_cast<std::size_t>(raw_count);
    }

    /** Converts a signed step to an unsigned magnitude without signed overflow. */
    [[nodiscard]] static constexpr auto magnitude(T value) noexcept -> ExtentType {
        const auto unsigned_value = static_cast<ExtentType>(value);
        return value < T{0} ? ExtentType{0} - unsigned_value : unsigned_value;
    }

    /** Computes how many stepped positions fit inside an unsigned span. */
    [[nodiscard]] constexpr auto extent_from_span(ExtentType span) const noexcept -> ExtentType {
        const auto step = magnitude(step_);
        return (span / step) + (span % step == ExtentType{0} ? ExtentType{0} : ExtentType{1});
    }

    /** Chooses the finite extent calculation after zero-step handling. */
    [[nodiscard]] constexpr auto nonzero_step_extent() const noexcept -> ExtentType {
        return step_ > T{0} ? ascending_extent() : descending_extent();
    }

    /** Computes the finite extent for a range moving upward. */
    [[nodiscard]] constexpr auto ascending_extent() const noexcept -> ExtentType {
        if (start_ >= end_) {
            return ExtentType{0};
        }
        return extent_from_span(static_cast<ExtentType>(end_) - static_cast<ExtentType>(start_));
    }

    /** Computes the finite extent for a range moving downward. */
    [[nodiscard]] constexpr auto descending_extent() const noexcept -> ExtentType {
        if (end_ >= start_) {
            return ExtentType{0};
        }
        return extent_from_span(static_cast<ExtentType>(start_) - static_cast<ExtentType>(end_));
    }

    T start_;
    T end_;
    T step_;
};

/** Diagnostic specialization (REQ-DIAG-012): reached only through the
 *  redirecting deduction guides below, which select it for exactly the
 *  out-of-domain class template argument deduction. Constructing it is always a
 *  compile-time error whose single message names the class template, the
 *  rejected argument type, and the violated domain rule. It is never a
 *  supported construction target and exposes no usable state. */
template <>
class Range<concepts_detail::RangeElementDomainViolation> {
  public:
    template <typename... Args>
    constexpr explicit Range([[maybe_unused]] Args&&... args) noexcept {
        static_assert(concepts_detail::dependent_false<Args...>,
                      "cljonic::Range: the element type must be a signed integral type. Every start, end, and step "
                      "argument must be a signed integral value, because Range describes a signed arithmetic "
                      "sequence whose span arithmetic requires a signed element type.");
    }
};

/** Redirecting deduction guides (REQ-DIAG-012), one per supported arity. Each is
 *  viable only for a rejected deduction, so an admitted deduction is never
 *  affected. */
template <typename T>
    requires(!concepts::RangeElement<std::remove_cvref_t<T>>)
Range(T) -> Range<concepts_detail::RangeElementDomainViolation>;

template <typename Start, typename End>
    requires(!(concepts::RangeElement<std::remove_cvref_t<Start>> && concepts::RangeElement<std::remove_cvref_t<End>>))
Range(Start, End) -> Range<concepts_detail::RangeElementDomainViolation>;

template <typename Start, typename End, typename Step>
    requires(!(concepts::RangeElement<std::remove_cvref_t<Start>> && concepts::RangeElement<std::remove_cvref_t<End>> &&
               concepts::RangeElement<std::remove_cvref_t<Step>>))
Range(Start, End, Step) -> Range<concepts_detail::RangeElementDomainViolation>;

} // namespace cljonic

namespace cljonic::concepts_detail {

template <typename T>
struct producer_traits<Range<T>> {
    static constexpr bool is_cljonic_producer = true;
    static constexpr producer_kind kind = producer_kind::range;
};

template <typename T>
struct contains_floating_point<Range<T>> : std::bool_constant<contains_floating_point_v<T>> {};

template <typename T>
struct contains_callable<Range<T>> : std::bool_constant<contains_callable_v<T>> {};

template <typename T>
struct contains_standard_range<Range<T>> : std::bool_constant<contains_standard_range_v<T>> {};

} // namespace cljonic::concepts_detail
