#pragma once

#include <concepts>
#include <cstddef>
#include <type_traits>

#include <cljonic-concepts.hpp>
#include <cljonic-core-collection-maximum-element-count.hpp>

namespace cljonic {

/** \anchor Range
 * \b Range is a bounded producer describing an arithmetic sequence from an inclusive \p start to an exclusive \p end
 * by a fixed \p step, defaulting to start `0` and step `1`. A zero step repeats \p start indefinitely, taking
 * precedence over otherwise-empty-range cases, including equal start and end. A nonzero step that moves away from \p
 * end produces an empty range.
 *
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 int main() {
   // Default: start 0, end 0, step 1 (empty). CTAD cannot deduce from zero
   // arguments.
   [[maybe_unused]] constexpr Range<int> empty{};

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
 }
 ~~~~~
 */
template <std::signed_integral T>
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

    /** Conservative materialization size: the synthesis cap for a zero step, otherwise the
     *  exact finite cardinality saturated at the synthesis cap. */
    [[nodiscard]] constexpr auto count() const noexcept -> std::size_t {
        return step_ == T{0} ? CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE : saturate(nonzero_step_extent());
    }

    /** Index-in-range predicate over the available bounded prefix; O(1), no traversal. */
    [[nodiscard]] constexpr auto contains(std::size_t index) const noexcept -> bool {
        return index < count();
    }

    /** Bounded generating iteration up to the effective size returned by count(). */
    [[nodiscard]] constexpr auto begin() const noexcept -> const_iterator {
        return const_iterator{start_, count(), step_};
    }

    [[nodiscard]] constexpr auto end() const noexcept -> const_iterator {
        return const_iterator{T{}, 0U, T{}};
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

} // namespace cljonic

namespace cljonic::concepts_detail {

template <typename T>
struct producer_traits<Range<T>> {
    static constexpr bool is_cljonic_producer = true;
    static constexpr producer_kind kind = producer_kind::range;
};

} // namespace cljonic::concepts_detail
