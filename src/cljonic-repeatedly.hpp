#pragma once

#include <concepts>
#include <cstddef>
#include <functional>
#include <type_traits>
#include <utility>

#include <cljonic-concepts.hpp>
#include <cljonic-core-collection-maximum-element-count.hpp>

namespace cljonic {

/** \anchor Repeatedly
 * \b Repeatedly is a producer created with `repeatedly(step)` or `repeatedly(count, step)`. It stores a copy of the
 * zero-argument step callable and produces a fresh element by invoking the step for every observed element. The step
 * may maintain state or produce side effects, but it must not throw or allocate when invoked. `repeatedly(step)` is
 * unbounded, while `repeatedly(count, step)` yields exactly \p count elements. The step may be a lambda, a named
 * function, or a function object. Repeatedly observation is bounded by the configured collection maximum, by the
 * requested count, or by an explicit destination capacity, and it supports const C++ range traversal in both constant
 * evaluation and runtime code. Repeatedly values can be passed directly to cljonic operations that accept a source, or
 * converted into an owning destination with `into` when explicit materialization is required.
 *
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 constexpr auto five() noexcept -> int { return 5; }

 int main() {
   // A named function can be used as the constexpr step callback.
   constexpr auto constexpr_sum = [] {
     constexpr auto values = repeatedly(3, five);
     int sum = 0;
     for (const auto value : values) {
       sum += value;
     }
     return sum;
   }();

   // Runtime construction also accepts an inline lambda step callback.
   auto runtime_values = repeatedly(2, []() noexcept { return 7; });

   // An uncounted Repeatedly is an unbounded producer whose observation is
   // bounded by the configured collection maximum or a destination capacity.
   [[maybe_unused]] constexpr auto unbounded = repeatedly(five);

   // -----------------------------------------------------------------------
   // C++ interoperability: Repeatedly supports const traversal.
   // -----------------------------------------------------------------------
   int runtime_sum = 0;
   for (const auto value : runtime_values) {
     runtime_sum += value;
   }

   return constexpr_sum == 15 && runtime_sum == 14 ? 0 : 1;
 }
 ~~~~~
 */
template <concepts::NothrowCollectionElement T, typename Step>
    requires concepts::RepeatedlyStep<T, Step>
class Repeatedly {
  public:
    using value_type = T;

    class const_iterator {
      public:
        using value_type = T;
        using difference_type = std::ptrdiff_t;

        constexpr const_iterator() noexcept = default;

        constexpr const_iterator(const Step* step, std::size_t remaining) noexcept
            : step_(step), remaining_(remaining) {
            if (remaining_ > 0U) {
                current_ = std::invoke(*step_);
            }
        }

        [[nodiscard]] constexpr auto operator*() const noexcept -> const T& {
            return current_;
        }

        constexpr auto operator++() noexcept -> const_iterator& {
            if (remaining_ > 1U) {
                current_ = std::invoke(*step_);
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
        const Step* step_{nullptr};
        std::size_t remaining_{0U};
        T current_{};
    };

    constexpr explicit Repeatedly(Step step) noexcept : step_(std::move(step)), count_(0U), is_finite_(false) {
    }

    constexpr Repeatedly(Step step, std::size_t count) noexcept
        : step_(std::move(step)), count_(count), is_finite_(true) {
    }

    [[nodiscard]] constexpr auto count() const noexcept -> std::size_t {
        return is_finite_ ? count_ : CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE;
    }

    [[nodiscard]] constexpr auto is_finite() const noexcept -> bool {
        return is_finite_;
    }

    [[nodiscard]] constexpr auto begin() const noexcept -> const_iterator {
        return const_iterator{&step_, count()};
    }

    [[nodiscard]] constexpr auto end() const noexcept -> const_iterator {
        return const_iterator{&step_, 0U};
    }

  private:
    Step step_;
    std::size_t count_;
    bool is_finite_;
};

template <typename Step, typename T = std::invoke_result_t<const std::decay_t<Step>&>>
    requires concepts::RepeatedlyStep<T, std::decay_t<Step>>
[[nodiscard]] constexpr auto repeatedly(Step&& step) noexcept -> Repeatedly<T, std::decay_t<Step>> {
    return Repeatedly<T, std::decay_t<Step>>{std::forward<Step>(step)};
}

template <typename Step, typename T = std::invoke_result_t<const std::decay_t<Step>&>>
    requires concepts::RepeatedlyStep<T, std::decay_t<Step>>
[[nodiscard]] constexpr auto repeatedly(std::size_t count, Step&& step) noexcept -> Repeatedly<T, std::decay_t<Step>> {
    return Repeatedly<T, std::decay_t<Step>>{std::forward<Step>(step), count};
}

} // namespace cljonic

namespace cljonic::concepts_detail {

template <concepts::NothrowCollectionElement T, typename Step>
    requires concepts::RepeatedlyStep<T, Step>
struct producer_traits<Repeatedly<T, Step>> {
    static constexpr bool is_cljonic_producer = true;
    static constexpr producer_kind kind = producer_kind::repeatedly;
};

} // namespace cljonic::concepts_detail
