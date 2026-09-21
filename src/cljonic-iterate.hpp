#pragma once

#include <concepts>
#include <cstddef>
#include <functional>
#include <type_traits>
#include <utility>

#include <cljonic-concepts.hpp>
#include <cljonic-core-collection-maximum-element-count.hpp>

namespace cljonic {

/** \anchor Iterate
 * \b Iterate is an unbounded producer created with `iterate(step, initial)`. It stores the initial value and a copy of
 * the step callable, emits the initial value first, and repeatedly applies the step to the previously emitted value.
 * For an initial value x and step function f, the observed sequence is `x, (f x), (f (f x)), ...`. The step may be a
 * lambda, a named function, or a function object. Iterate observation is bounded by the configured collection maximum
 * or by an explicit destination capacity, and it supports const C++ range traversal in both constant evaluation and
 * runtime code. Iterate values can be passed directly to cljonic operations that accept a source, or converted into an
 * owning destination with `into` when explicit materialization is required.
 *
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 constexpr auto add_two(int value) noexcept -> int { return value + 2; }

 int main() {
   // A named function can be used as the constexpr step callback.
   constexpr auto constexpr_sum = [] {
     constexpr auto values = iterate(add_two, 1);
     int sum = 0;
     int observed = 0;
     for (const auto value : values) {
       sum += value;
       if (++observed == 4) {
         break;
       }
     }
     return sum;
   }();

   // Runtime construction also accepts an inline lambda step callback.
   auto runtime_values =
       iterate([](int value) noexcept { return value * 2; }, 2);

   // -----------------------------------------------------------------------
   // C++ interoperability: Iterate supports const traversal.
   // -----------------------------------------------------------------------
   int runtime_sum = 0;
   int runtime_observed = 0;
   for (const auto value : runtime_values) {
     // A bounded observation makes the unbounded producer safe to inspect.
     runtime_sum += value;
     if (++runtime_observed == 4) {
       break;
     }
   }

   return constexpr_sum == 16 && runtime_sum == 30 ? 0 : 1;
 }
 ~~~~~
 */
template <concepts::NothrowCollectionElement T, typename Step>
    requires concepts::IterateStep<T, Step>
class Iterate {
  public:
    using value_type = T;

    class const_iterator {
      public:
        using value_type = T;
        using difference_type = std::ptrdiff_t;

        constexpr const_iterator() noexcept = default;

        constexpr const_iterator(T current, const Step* step, std::size_t remaining) noexcept
            : current_(std::move(current)), step_(step), remaining_(remaining) {
        }

        [[nodiscard]] constexpr auto operator*() const noexcept -> const T& {
            return current_;
        }

        constexpr auto operator++() noexcept -> const_iterator& {
            if (remaining_ > 1U) {
                current_ = std::invoke(*step_, current_);
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
        const Step* step_{nullptr};
        std::size_t remaining_{0U};
    };

    constexpr Iterate(T initial, Step step) noexcept : initial_(std::move(initial)), step_(std::move(step)) {
    }

    [[nodiscard]] constexpr auto count() const noexcept -> std::size_t {
        return CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE;
    }

    [[nodiscard]] constexpr auto is_finite() const noexcept -> bool {
        return false;
    }

    [[nodiscard]] constexpr auto begin() const noexcept -> const_iterator {
        return const_iterator{initial_, &step_, count()};
    }

    [[nodiscard]] constexpr auto end() const noexcept -> const_iterator {
        return const_iterator{};
    }

  private:
    T initial_;
    Step step_;
};

template <typename Step, typename T>
    requires concepts::NothrowCollectionElement<std::remove_cvref_t<T>> &&
             concepts::IterateStep<std::remove_cvref_t<T>, std::decay_t<Step>>
[[nodiscard]] constexpr auto iterate(Step&& step, T&& initial) noexcept
    -> Iterate<std::remove_cvref_t<T>, std::decay_t<Step>> {
    return Iterate<std::remove_cvref_t<T>, std::decay_t<Step>>{std::forward<T>(initial), std::forward<Step>(step)};
}

} // namespace cljonic

namespace cljonic::concepts_detail {

template <concepts::NothrowCollectionElement T, typename Step>
    requires concepts::IterateStep<T, Step>
struct producer_traits<Iterate<T, Step>> {
    static constexpr bool is_cljonic_producer = true;
    static constexpr producer_kind kind = producer_kind::iterate;
};

} // namespace cljonic::concepts_detail
