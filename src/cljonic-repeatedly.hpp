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

/** Internal tag (REQ-DIAG-012): selects the `Repeatedly` diagnostic
 *  specialization. A `Repeatedly`'s element type is never deducible from its
 *  step argument, so any deduced `Repeatedly` is a rejected deduction. */
struct RepeatedlyDomainViolation {};

/** The `Repeatedly` class-template gate (REQ-DIAG-012): a valid element type and
 *  zero-argument step, or the internal diagnostic tag pair used only to redirect
 *  a rejected deduction. */
template <typename T, typename Step>
concept RepeatedlyClassArgument = (concepts::NothrowCollectionElement<T> && concepts::RepeatedlyStep<T, Step>) ||
                                  (std::same_as<std::remove_cvref_t<T>, RepeatedlyDomainViolation> &&
                                   std::same_as<std::remove_cvref_t<Step>, RepeatedlyDomainViolation>);

} // namespace concepts_detail

/** \anchor Repeatedly
 * A Repeatedly producer is created with `repeatedly(step)` or `repeatedly(count, step)`. It stores a copy of the
 * zero-argument step callable and produces a fresh element by invoking the step for every observed element. The step
 * may maintain state or produce side effects, but it must not throw or allocate when invoked. `repeatedly(step)` is
 * unbounded, while `repeatedly(count, step)` yields exactly \p count elements. The step may be a lambda, a named
 * function, or a function object. Observation is bounded by \b CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE, by the
 * requested count, or by an explicit destination capacity, and it supports const C++ range traversal in both constant
 * evaluation and runtime code. Values can be passed directly to cljonic operations that accept a source, or converted
 * into an owning destination with `into` when explicit materialization is required. Finite counts follow the \ref
 * implicit-conversions "safe implicit conversion rules": use unsigned integer types whose ranges fit \c std::size_t.
 * Callback types and produced element types remain exact.
 *
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 constexpr auto five() noexcept -> int { return 5; }

 int main() {
   // A named function can be used as the constexpr step callback.
   constexpr auto constexpr_sum = [] {
     constexpr auto values = repeatedly(3U, five);
     auto sum = 0;
     for (const auto value : values) {
       sum += value;
     }
     return sum;
   }();

   // Runtime construction also accepts an inline lambda step callback.
   auto runtime_values = repeatedly(2U, []() noexcept { return 7; });

   // An uncounted Repeatedly is an unbounded producer whose observation is
   // bounded by CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE or a destination
   // capacity.
   [[maybe_unused]] constexpr auto unbounded = repeatedly(five);

   // -----------------------------------------------------------------------
   // C++ interoperability: Repeatedly supports const traversal.
   // -----------------------------------------------------------------------
   auto runtime_sum = 0;
   for (const auto value : runtime_values) {
     runtime_sum += value;
   }

   return constexpr_sum == 15 && runtime_sum == 14 ? 0 : 1;
 }
 ~~~~~
 */
template <typename T, typename Step>
    requires concepts_detail::RepeatedlyClassArgument<T, Step>
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

    // The default form requires a default-constructed step. Constraining the
    // constructor keeps a non-default-constructible Step from satisfying
    // NothrowCollectionElement in unevaluated contexts (REQ-VAL-017D).
    constexpr Repeatedly() noexcept
        requires std::default_initializable<Step>
        : step_{}, count_{0U}, is_finite_{true} {
    }

    constexpr explicit Repeatedly(const Step& step) noexcept : step_(step), count_(0U), is_finite_(false) {
    }

    constexpr Repeatedly(const Step& step, std::size_t count) noexcept : step_(step), count_(count), is_finite_(true) {
    }

    template <typename OtherStep>
        requires(!std::same_as<std::remove_cvref_t<OtherStep>, Step>)
    explicit Repeatedly(OtherStep step) = delete;

    template <typename OtherStep, typename Count>
        requires(!std::same_as<std::remove_cvref_t<OtherStep>, Step> || !concepts_detail::ProducerCount<Count>)
    Repeatedly(OtherStep step, Count count) = delete;

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

/** Diagnostic specialization (REQ-DIAG-012): selected by the redirecting
 *  deduction guides below. A `Repeatedly`'s element type is produced by its
 *  step, so it is never deducible from a constructor argument; every deduced
 *  `Repeatedly` reaches this specialization instead of a failed-deduction
 *  candidate list. */
template <>
class Repeatedly<concepts_detail::RepeatedlyDomainViolation, concepts_detail::RepeatedlyDomainViolation> {
  public:
    template <typename... Args>
    constexpr explicit Repeatedly([[maybe_unused]] Args&&... args) noexcept {
        static_assert(concepts_detail::dependent_false<Args...>,
                      "cljonic::Repeatedly: the element type cannot be deduced from a step alone. Use the repeatedly() "
                      "free function, as in cljonic::repeatedly(step) or cljonic::repeatedly(count, step).");
    }
};

/** Redirecting deduction guides (REQ-DIAG-012). No admitted `Repeatedly`
 *  deduction exists, so these guides can never affect a supported construction. */
template <typename Step>
Repeatedly(const Step&)
    -> Repeatedly<concepts_detail::RepeatedlyDomainViolation, concepts_detail::RepeatedlyDomainViolation>;

template <typename Step, typename Count>
Repeatedly(const Step&, Count)
    -> Repeatedly<concepts_detail::RepeatedlyDomainViolation, concepts_detail::RepeatedlyDomainViolation>;

template <typename Step, typename T = std::invoke_result_t<const std::decay_t<Step>&>>
    requires concepts::RepeatedlyStep<T, std::decay_t<Step>>
[[nodiscard]] constexpr auto repeatedly(Step&& step) noexcept -> Repeatedly<T, std::decay_t<Step>> {
    return Repeatedly<T, std::decay_t<Step>>{std::forward<Step>(step)};
}

template <concepts_detail::ProducerCount Count, typename Step,
          typename T = std::invoke_result_t<const std::decay_t<Step>&>>
    requires concepts::RepeatedlyStep<T, std::decay_t<Step>>
[[nodiscard]] constexpr auto repeatedly(Count count, Step&& step) noexcept -> Repeatedly<T, std::decay_t<Step>> {
    return Repeatedly<T, std::decay_t<Step>>{std::forward<Step>(step), count};
}

// Diagnostic fallback (REQ-DIAG-009): the step argument is outside the closed
// producer domain. This overload explains the rejection; it never returns a
// value and is never a supported call target.
template <typename Step>
    requires(!concepts_detail::ValidRepeatedlyStep<Step>)
constexpr auto repeatedly([[maybe_unused]] Step&& step) noexcept -> concepts_detail::RejectedProducerFactory {
    static_assert(concepts_detail::dependent_false<Step>,
                  "cljonic::repeatedly: this step cannot be used to build a Repeatedly. The step must be a callable -- "
                  "a function, a lambda, or a function object -- that takes no arguments, can be copied, and returns a "
                  "value that can be stored without throwing. To repeat one value, use cljonic::repeat(value) or "
                  "cljonic::repeat(value, count).");
    return {};
}

template <typename Count, typename Step>
    requires(!concepts_detail::ValidRepeatedlyStep<Step> || !concepts_detail::ProducerCount<Count>)
constexpr auto repeatedly([[maybe_unused]] Count count, [[maybe_unused]] Step&& step) noexcept
    -> concepts_detail::RejectedProducerFactory {
    static_assert(concepts_detail::dependent_false<Step>,
                  "cljonic::repeatedly: this step cannot be used to build a Repeatedly. The step must be a callable -- "
                  "a function, a lambda, or a function object -- that takes no arguments, can be copied, and returns a "
                  "value that can be stored without throwing. To repeat one value, use cljonic::repeat(value) or "
                  "cljonic::repeat(value, count). The count must be a non-boolean, non-character unsigned integer "
                  "type whose range fits std::size_t.");
    return {};
}

} // namespace cljonic

namespace cljonic::concepts_detail {

template <concepts::NothrowCollectionElement T, typename Step>
    requires concepts::RepeatedlyStep<T, Step>
struct producer_traits<Repeatedly<T, Step>> {
    static constexpr bool is_cljonic_producer = true;
    static constexpr producer_kind kind = producer_kind::repeatedly;
};

// The stored step is a callable component, so Repeatedly never admits producer
// parameter equality and is never usable as a map key or set
// element (REQ-CAP-010, REQ-FN-014B).
template <concepts::NothrowCollectionElement T, typename Step>
    requires concepts::RepeatedlyStep<T, Step>
struct contains_callable<Repeatedly<T, Step>> : std::true_type {};

template <concepts::NothrowCollectionElement T, typename Step>
    requires concepts::RepeatedlyStep<T, Step>
struct contains_floating_point<Repeatedly<T, Step>> : std::bool_constant<contains_floating_point_v<T>> {};

template <concepts::NothrowCollectionElement T, typename Step>
    requires concepts::RepeatedlyStep<T, Step>
struct contains_standard_range<Repeatedly<T, Step>> : std::bool_constant<contains_standard_range_v<T>> {};

} // namespace cljonic::concepts_detail
