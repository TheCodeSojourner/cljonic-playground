#pragma once

#include <concepts>
#include <cstddef>
#include <functional>
#include <optional>
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
struct RepeatedlyNextAccess;

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
 * zero-argument step callable and produces a fresh element by invoking the step for every observed element.
 * `repeatedly(step)` is unbounded, while `repeatedly(count, step)` yields exactly \p count elements.
 *
 * The step may be a lambda, a named function, or a function object, including a callable with a non-const call
 * operator. It may maintain state or produce side effects, but it must not throw or allocate when invoked. Each
 * traversal invokes a copy of the stored step: mutable state held by value starts from the stored state each time,
 * while state reached through references or static storage remains shared across traversals. Callback types and
 * produced element types remain exact.
 *
 * Calling \ref Next "next" on a nonempty Repeatedly eagerly invokes a copy of the stored step once and discards that
 * produced value. The returned Repeatedly owns the advanced copy of the step, so value-held mutable state advances in
 * the tail without changing the source; effects through references or static storage are shared. A finite tail has one
 * fewer element, while an unbounded tail retains its observation cap. An empty Repeatedly does not invoke its step.
 *
 * Observation is bounded by \b CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE or by the requested count, and it
 * supports const C++ range traversal in both constant evaluation and runtime code. Values can be passed directly to
 * cljonic operations that accept a source.
 *
 * Finite counts follow the \ref implicit-conversions "safe implicit conversion rules". Counts accept signed or
 * unsigned integer types whose nonnegative ranges fit \c std::size_t, excluding Boolean and character types. Zero or
 * negative counts produce an empty finite Repeatedly without invoking the callback.
 *
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 constexpr auto five() noexcept -> int { return 5; }

 int main() {
   // A named function can be used as the constexpr step callback.
   constexpr auto constexpr_sum = [] {
     constexpr auto values = repeatedly(3, five);
     auto sum = 0;

     // -----------------------------------------------------------------------
     // C++ interoperability: Repeatedly supports const traversal.
     // -----------------------------------------------------------------------
     for (const auto value : values) {
       sum += value;
     }
     return sum;
   }();

   // Mutable value-captured state starts from the stored value on each
   // traversal.
   const auto runtime_values = repeatedly(2, [value = 0]() mutable noexcept {
     value += 3;
     return value;
   });

   auto first_runtime_sum = 0;
   for (const auto value : runtime_values) {
     first_runtime_sum += value;
   }
   auto second_runtime_sum = 0;
   for (const auto value : runtime_values) {
     second_runtime_sum += value;
   }

   // Static state is shared and continues across traversals.
   const auto shared_values = repeatedly(2, []() noexcept {
     static int shared_value = 0;
     return ++shared_value;
   });
   auto first_shared_sum = 0;
   for (const auto value : shared_values) {
     first_shared_sum += value;
   }
   auto second_shared_sum = 0;
   for (const auto value : shared_values) {
     second_shared_sum += value;
   }

   // An uncounted Repeatedly is an unbounded producer whose observation is
   // bounded by CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE or a destination
   // capacity.
   [[maybe_unused]] constexpr auto unbounded = repeatedly(five);

   return constexpr_sum == 15 && first_runtime_sum == 9 &&
                  second_runtime_sum == 9 && first_shared_sum == 3 &&
                  second_shared_sum == 7
              ? 0
              : 1;
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

        constexpr explicit const_iterator(std::size_t remaining) noexcept : remaining_(remaining) {
        }

        constexpr const_iterator(const Step& step, std::size_t remaining) noexcept
            : step_{step}, remaining_(remaining) {
            if (remaining_ > 0U) {
                current_ = std::invoke(*step_);
            }
        }

        constexpr const_iterator(const const_iterator&) noexcept = default;

        constexpr auto operator=(const const_iterator& other) noexcept -> const_iterator& {
            if (this != &other) {
                step_.reset();
                if (other.step_.has_value()) {
                    step_.emplace(*other.step_);
                }
                remaining_ = other.remaining_;
                current_ = other.current_;
            }
            return *this;
        }

        constexpr const_iterator(const_iterator&& other) noexcept : const_iterator(other) {
        }

        constexpr auto operator=(const_iterator&& other) noexcept -> const_iterator& {
            return *this = other;
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
        std::optional<Step> step_{};
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

    template <concepts_detail::ProducerCount Count>
    constexpr Repeatedly(const Step& step, Count count) noexcept
        : step_(step), count_(concepts_detail::normalize_producer_count(count)), is_finite_(true) {
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
        const auto traversal_count = count();
        return traversal_count == 0U ? const_iterator{0U} : const_iterator{step_, traversal_count};
    }

    [[nodiscard]] constexpr auto end() const noexcept -> const_iterator {
        return const_iterator{0U};
    }

  private:
    friend struct concepts_detail::RepeatedlyNextAccess;

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

template <typename Step, typename T = std::invoke_result_t<std::decay_t<Step>&>>
    requires concepts::RepeatedlyStep<T, std::decay_t<Step>>
[[nodiscard]] constexpr auto repeatedly(Step&& step) noexcept -> Repeatedly<T, std::decay_t<Step>> {
    return Repeatedly<T, std::decay_t<Step>>{std::forward<Step>(step)};
}

template <concepts_detail::ProducerCount Count, typename Step, typename T = std::invoke_result_t<std::decay_t<Step>&>>
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
                  "cljonic::repeat(count, value).");
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
                  "cljonic::repeat(count, value). The count must be a non-boolean, non-character integer "
                  "type whose nonnegative range fits std::size_t.");
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
