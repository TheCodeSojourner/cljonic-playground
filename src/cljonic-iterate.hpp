#pragma once

#include <concepts>
#include <cstddef>
#include <functional>
#include <type_traits>
#include <utility>

#include <cljonic-concepts.hpp>
#include <cljonic-core-collection-maximum-element-count.hpp>

namespace cljonic {

namespace concepts_detail {

/** Internal tag (REQ-DIAG-012): selects the `Iterate` diagnostic
 *  specialization during an out-of-domain class template argument deduction. */
struct IterateDomainViolation {};

/** The `Iterate` class-template gate (REQ-DIAG-012): a valid initial value and
 *  step, or the internal diagnostic tag used only to redirect a rejected
 *  deduction. */
template <typename T, typename Step>
concept IterateClassArgument = (concepts::NothrowCollectionElement<T> && concepts::IterateStep<T, Step>) ||
                               (std::same_as<std::remove_cvref_t<T>, IterateDomainViolation> &&
                                std::same_as<std::remove_cvref_t<Step>, IterateDomainViolation>);

} // namespace concepts_detail

/** \anchor Iterate
 * An Iterate producer is unbounded and created with `iterate(step, initial)`. It stores the initial value and a copy
 * of the step callable, emits the initial value first, and repeatedly applies the step to the previously emitted
 * value. For an initial value x and step function f, the observed sequence is `x, (f x), (f (f x)), ...`.
 *
 * The initial and produced values may themselves be cljonic collections or producers, including nested cljonic
 * composites. The step may be a lambda, a named function, or a function object.
 *
 * Observation is bounded by \b CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE and supports const C++ range traversal
 * in both constant evaluation and runtime code. Values can be passed directly to cljonic operations that accept a
 * source.
 *
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 constexpr auto add_two(int value) noexcept -> int { return value + 2; }

 int main() {
   // A named function can be used as the constexpr step callback.
   constexpr auto constexpr_sum = [] {
     constexpr auto values = iterate(add_two, 1);
     auto sum = 0;
     auto observed = 0;
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
   auto runtime_sum = 0;
   auto runtime_observed = 0;
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
template <typename T, typename Step>
    requires concepts_detail::IterateClassArgument<T, Step>
class Iterate {
  public:
    using value_type = T;

    class const_iterator {
      public:
        using value_type = T;
        using difference_type = std::ptrdiff_t;

        constexpr const_iterator() noexcept = default;

        constexpr const_iterator(const T& current, const Step* step, std::size_t remaining) noexcept
            : current_(current), step_(step), remaining_(remaining) {
        }

        [[nodiscard]] constexpr auto operator*() const noexcept -> const T& {
            return current_;
        }

        constexpr auto operator++() noexcept -> const_iterator& {
            if (remaining_ > 1U) {
                const T next = std::invoke(*step_, current_);
                current_ = next;
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

    // The default form requires a default-constructed step. Constraining the
    // constructor keeps a non-default-constructible Step from satisfying
    // NothrowCollectionElement in unevaluated contexts (REQ-VAL-017D).
    constexpr Iterate() noexcept
        requires std::default_initializable<Step>
        : initial_{}, step_{} {
    }

    constexpr Iterate(const T& initial, const Step& step) noexcept : initial_(initial), step_(step) {
    }

    template <typename Initial, typename OtherStep>
        requires(!concepts::SafeImplicitConversion<T, Initial> || !std::same_as<std::remove_cvref_t<OtherStep>, Step>)
    Iterate(Initial initial, OtherStep step) = delete;

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

/** Diagnostic specialization (REQ-DIAG-012): reached only through the
 *  redirecting deduction guide below. Constructing it is always a compile-time
 *  error whose single message names the class template, the rejected argument
 *  type, and the violated domain rule. */
template <>
class Iterate<concepts_detail::IterateDomainViolation, concepts_detail::IterateDomainViolation> {
  public:
    template <typename... Args>
    constexpr explicit Iterate([[maybe_unused]] Args&&... args) noexcept {
        static_assert(concepts_detail::dependent_false<Args...>,
                      "cljonic::Iterate: the initial value and step cannot be used to build an Iterate. The initial "
                      "value must be storable without throwing, and the step must be a pure, non-allocating, "
                      "non-throwing callable that accepts the initial value and returns the element type.");
    }
};

/** Redirecting deduction guide (REQ-DIAG-012): viable only for a rejected
 *  deduction, so an admitted deduction is never affected. */
template <typename T, typename Step>
    requires(!(concepts::NothrowCollectionElement<T> && concepts::IterateStep<T, Step>))
Iterate(const T&, const Step&)
    -> Iterate<concepts_detail::IterateDomainViolation, concepts_detail::IterateDomainViolation>;

template <typename Step, typename T>
    requires concepts::NothrowCollectionElement<std::remove_cvref_t<T>> &&
             concepts::IterateStep<std::remove_cvref_t<T>, std::decay_t<Step>>
[[nodiscard]] constexpr auto iterate(Step&& step, T&& initial) noexcept
    -> Iterate<std::remove_cvref_t<T>, std::decay_t<Step>> {
    return Iterate<std::remove_cvref_t<T>, std::decay_t<Step>>{std::forward<T>(initial), std::forward<Step>(step)};
}

// Diagnostic fallback (REQ-DIAG-009): the (step, initial) arguments are outside
// the closed producer domain. This overload explains the rejection; it never
// returns a value and is never a supported call target.
template <typename Step, typename T>
    requires(!(concepts::NothrowCollectionElement<std::remove_cvref_t<T>> &&
               concepts::IterateStep<std::remove_cvref_t<T>, std::decay_t<Step>>))
constexpr auto iterate([[maybe_unused]] Step&& step, [[maybe_unused]] T&& initial) noexcept
    -> concepts_detail::RejectedProducerFactory {
    static_assert(
        concepts_detail::dependent_false<Step, T>,
        "cljonic::iterate: the (step, initial) arguments cannot be used to build an Iterate. The initial value must be "
        "safe to store without throwing, and the step must be a callable that takes one value of that type and returns "
        "the same type without throwing. Note the argument order is iterate(step, initial).");
    return {};
}

} // namespace cljonic

namespace cljonic::concepts_detail {

template <concepts::NothrowCollectionElement T, typename Step>
    requires concepts::IterateStep<T, Step>
struct producer_traits<Iterate<T, Step>> {
    static constexpr bool is_cljonic_producer = true;
    static constexpr producer_kind kind = producer_kind::iterate;
};

// The stored step is a callable component, so Iterate never admits producer
// parameter equality and is never usable as a map key or set
// element (REQ-CAP-010, REQ-FN-014B).
template <concepts::NothrowCollectionElement T, typename Step>
    requires concepts::IterateStep<T, Step>
struct contains_callable<Iterate<T, Step>> : std::true_type {};

template <concepts::NothrowCollectionElement T, typename Step>
    requires concepts::IterateStep<T, Step>
struct contains_floating_point<Iterate<T, Step>> : std::bool_constant<contains_floating_point_v<T>> {};

template <concepts::NothrowCollectionElement T, typename Step>
    requires concepts::IterateStep<T, Step>
struct contains_standard_range<Iterate<T, Step>> : std::bool_constant<contains_standard_range_v<T>> {};

} // namespace cljonic::concepts_detail
