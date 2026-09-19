#pragma once

#include <cstddef>
#include <span>
#include <utility>

#include <cljonic-concepts.hpp>
#include <cljonic-vector.hpp>

namespace cljonic {

/** \anchor Cycle
 * \b Cycle is a producer that repeats a bounded source sequence until it is exhausted, then wraps back to the start.
 * The approved public form is `cycle(source)` only. The producer is unbounded and terminates traversal only by the
 * observable cap used for bounded materialization.
 */
template <concepts::NothrowCollectionElement T, std::size_t CapacityValue>
class Cycle {
  public:
    using value_type = T;

    class const_iterator {
      public:
        using value_type = T;
        using difference_type = std::ptrdiff_t;

        struct position {
            std::size_t remaining;
            std::size_t index;
        };

        constexpr const_iterator() noexcept = default;

        constexpr const_iterator(std::span<const T> values, position cursor) noexcept
            : values_(values), remaining_(cursor.remaining), index_(cursor.index) {
        }

        [[nodiscard]] constexpr auto operator*() const noexcept -> const T& {
            return values_[index_ % values_.size()];
        }

        constexpr auto operator++() noexcept -> const_iterator& {
            if (values_.empty() || remaining_ == 0U) {
                remaining_ = 0U;
                return *this;
            }
            index_ = (index_ + 1U) % values_.size();
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
        std::span<const T> values_{};
        std::size_t remaining_{0U};
        std::size_t index_{0U};
    };

    constexpr explicit Cycle(Vector<T, CapacityValue> values) noexcept : values_(std::move(values)) {
    }

    [[nodiscard]] constexpr auto count() const noexcept -> std::size_t {
        return count_;
    }

    [[nodiscard]] constexpr auto is_finite() const noexcept -> bool {
        return is_finite_;
    }

    [[nodiscard]] constexpr auto begin() const noexcept -> const_iterator {
        if (values_.count() == 0U) {
            return const_iterator{std::span<const T>{}, typename const_iterator::position{0U, 0U}};
        }
        return const_iterator{std::span<const T>{values_.begin(), values_.count()},
                              typename const_iterator::position{count(), 0U}};
    }

    [[nodiscard]] constexpr auto end() const noexcept -> const_iterator {
        return const_iterator{std::span<const T>{}, typename const_iterator::position{0U, 0U}};
    }

  private:
    Vector<T, CapacityValue> values_{};
    std::size_t count_{CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE};
    bool is_finite_{false};
};

template <typename T, std::size_t CapacityValue>
[[nodiscard]] constexpr auto cycle(Vector<T, CapacityValue> values) noexcept -> Cycle<T, CapacityValue> {
    return Cycle<T, CapacityValue>{std::move(values)};
}

} // namespace cljonic

namespace cljonic::concepts_detail {

template <typename T, std::size_t CapacityValue>
struct producer_traits<Cycle<T, CapacityValue>> {
    static constexpr bool is_cljonic_producer = true;
    static constexpr producer_kind kind = producer_kind::cycle;
};

} // namespace cljonic::concepts_detail
