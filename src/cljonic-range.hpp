#pragma once

#include <concepts>
#include <cstddef>
#include <type_traits>
#include <utility>

#include <cljonic-concepts.hpp>
#include <cljonic-core-collection-maximum-element-count.hpp>

namespace cljonic {

namespace concepts_detail {

struct RangeNextAccess;

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
 * \p step, defaulting to start `0` and step `1`.
 *
 * Use \ref RangeFactory "range" to create a Range.
 *
 * A zero step repeats \p start indefinitely, taking precedence over otherwise-empty-range cases, including equal start
 * and end. A nonzero step that moves away from \p end produces an empty range.
 *
 * Observation is bounded by its finite span or by \b CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE for a zero-step
 * range, and it supports const C++ range traversal in both constant evaluation and runtime code. Values can be passed
 * directly to cljonic operations that accept a source.
 *
 * Explicitly typed parameters follow the \ref implicit-conversions "safe implicit conversion rules".
 *
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;
 int main() {
   // Default: start 0, end 0, step 1 (empty). CTAD cannot deduce from zero
   // arguments.
   [[maybe_unused]] constexpr auto empty = range<int>();

   // Single argument is the exclusive end; start defaults to 0, step to 1.
   [[maybe_unused]] constexpr auto to_five = range(5);

   // Explicit start and end; step still defaults to 1.
   [[maybe_unused]] constexpr auto two_to_five = range(2, 5);

   // Explicit start, end, and step.
   [[maybe_unused]] constexpr auto evens = range(0, 10, 2);

   // A zero step repeats start indefinitely rather than being empty.
   [[maybe_unused]] constexpr auto repeating = range(0, 5, 0);

   // A billion elements exceeds the system's allowed maximum, so only that
   // maximum number of elements is available.
   [[maybe_unused]] constexpr auto huge = range(1000000000);

   // -----------------------------------------------------------------------
   // C++ interoperability: a Range supports const traversal.
   // -----------------------------------------------------------------------
   constexpr auto odds = range(1, 12, 2);
   auto runtime_odds = range(-1, -11, -2);

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
    friend struct concepts_detail::RangeNextAccess;

  public:
    using value_type = T;

    /** A bounded, generating const iterator used for free-function observation;
     *  it does not expose start/end/step parameters. */
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

    /** Conservative observation count: the synthesis cap for a zero step, otherwise
     *  the exact finite cardinality saturated at the synthesis cap. */
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
     *  start/end/step parameters, never the produced sequence.
     *
     *  It is the only producer parameter-equality surface; the `equal` free function
     *  instead compares bounded logical observations of the produced sequences. Two
     *  Range values with different parameters compare unequal even when their
     *  produced or observed sequences coincide (for example two zero-step ranges
     *  with different endpoints both produce an infinite repetition of their
     *  respective start values). O(1), no traversal, no allocation. */
    [[nodiscard]] friend constexpr auto operator==(const Range& lhs, const Range& rhs) noexcept -> bool {
        return lhs.start_ == rhs.start_ && lhs.end_ == rhs.end_ && lhs.step_ == rhs.step_;
    }

  private:
    using ExtentType = std::make_unsigned_t<T>;

    /** Caps an internal extent at the public observation maximum. */
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

namespace concepts_detail {

struct DeducedRangeElementType {};

template <typename RequestedElementType, typename... Args>
struct range_factory_element_type {
    using type = RequestedElementType;
};

template <typename First, typename... Rest>
struct range_factory_element_type<DeducedRangeElementType, First, Rest...> {
    using type = std::remove_cvref_t<First>;
};

template <typename RequestedElementType, typename... Args>
using range_factory_element_t = range_factory_element_type<RequestedElementType, Args...>::type;

template <typename RequestedElementType, typename... Args>
concept RangeFactoryHasElementType =
    sizeof...(Args) > 0U || !std::same_as<RequestedElementType, DeducedRangeElementType>;

template <typename RequestedElementType, typename... Args>
concept RangeFactoryArguments =
    RangeFactoryHasElementType<RequestedElementType, Args...> &&
    concepts::RangeElement<range_factory_element_t<RequestedElementType, Args...>> && sizeof...(Args) <= 3U &&
    ((!std::same_as<RequestedElementType, DeducedRangeElementType> &&
      (concepts::SafeImplicitConversion<RequestedElementType, Args> && ...)) ||
     (std::same_as<RequestedElementType, DeducedRangeElementType> &&
      (std::same_as<range_factory_element_t<RequestedElementType, Args...>, std::remove_cvref_t<Args>> && ...)));

} // namespace concepts_detail

/** \anchor RangeFactory
 * The \b range free function API creates an owning \ref Range "Range" without materializing its results.
 *
 * Use `range(end)`, `range(start, end)`, or `range(start, end, step)` with arguments of the same signed integral type.
 * Use `range<T>(...)` to select a signed integral element type under the \ref implicit-conversions "safe implicit
 * conversion rules"; `range<T>()` creates the default empty range. An untyped empty call cannot deduce an element
 * type. All forms preserve the equivalent constructor's defaults, zero-step repetition, exclusive endpoint, and
 * observation bound, including \b CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE. Construction is non-throwing and
 * non-allocating.
 *
~~~~~{.cpp}
#include "cljonic.hpp"
using namespace cljonic;
int main() {
  constexpr auto empty = range<int>();
  constexpr auto ascending = range(5);
  constexpr auto descending = range(5, 0, -2);
  constexpr auto widened = range<long>(0, 5, 2);
  static_assert(is_empty(empty));
  static_assert(
      equal(into(vector<int, 5>(), ascending), vector(0, 1, 2, 3, 4)));
  static_assert(equal(into(vector<int, 3>(), descending), vector(5, 3, 1)));
  static_assert(equal(into(vector<long, 3>(), widened), vector(0L, 2L, 4L)));
  auto end = 5;
  const auto runtime_values = range(end);
  return count(runtime_values) == 5U ? 0 : 1;
}
~~~~~
 */
template <typename RequestedElementType = concepts_detail::DeducedRangeElementType, typename... Args>
    requires concepts_detail::RangeFactoryArguments<RequestedElementType, Args...>
[[nodiscard]] constexpr auto range(Args&&... args) noexcept
    -> Range<concepts_detail::range_factory_element_t<RequestedElementType, Args...>> {
    using ElementType = concepts_detail::range_factory_element_t<RequestedElementType, Args...>;
    return Range<ElementType>{std::forward<Args>(args)...};
}

template <typename RequestedElementType = concepts_detail::DeducedRangeElementType, typename... Args>
    requires(!concepts_detail::RangeFactoryArguments<RequestedElementType, Args...>)
constexpr auto range([[maybe_unused]] Args&&... args) noexcept -> void {
    static_assert(concepts_detail::dependent_false<RequestedElementType, Args...>,
                  "cljonic::range: use range(end), range(start, end), or range(start, end, step) with the same signed "
                  "integral type for every argument. An empty call requires an explicit element type: range<T>(). "
                  "With an explicit signed integral T, zero through three arguments must safely convert to T "
                  "without loss of range or precision.");
}

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
