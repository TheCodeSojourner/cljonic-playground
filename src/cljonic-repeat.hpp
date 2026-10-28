#pragma once

#include <cstddef>
#include <type_traits>
#include <utility>

#include <cljonic-concepts.hpp>
#include <cljonic-core-collection-maximum-element-count.hpp>

namespace cljonic {

namespace concepts_detail {

struct RepeatNextAccess;

/** Internal tag (REQ-DIAG-012): selects the `Repeat` diagnostic specialization
 *  during an out-of-domain class template argument deduction. */
struct RepeatElementDomainViolation {};

/** The `Repeat` class-template gate (REQ-DIAG-012): a storable element, or the
 *  internal diagnostic tag used only to redirect a rejected deduction. */
template <typename T>
concept RepeatClassArgument =
    concepts::NothrowCollectionElement<T> || std::same_as<std::remove_cvref_t<T>, RepeatElementDomainViolation>;

} // namespace concepts_detail

/** \anchor Repeat
 * A Repeat is a producer that owns one value and yields copies of it. The value may itself be a cljonic collection or
 * producer, including nested cljonic composites. `repeat(value)` is unbounded, while `repeat(count, value)` yields
 * exactly \p count values.
 *
 * Observation is bounded by \b CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE or by the requested count, and it
 * supports const C++ range traversal in both constant evaluation and runtime code. Values can be passed directly to
 * cljonic operations that accept a source.
 *
 * Values and finite counts follow the \ref implicit-conversions "safe implicit conversion rules". Counts accept signed
 * or unsigned integer types whose nonnegative ranges fit \c std::size_t, excluding Boolean and character types. Zero
 * or negative counts produce an empty finite Repeat.
 *
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 struct Pixel {
   int x{};
   int y{};

   friend constexpr auto operator==(const Pixel &, const Pixel &) noexcept
       -> bool = default;
 };

 int main() {
   // A finite Repeat yields the value exactly the requested number of times.
   constexpr auto finite = Repeat{3, 7};
   constexpr auto pixels = Repeat{2, Pixel{4, 9}};

   // An uncounted Repeat is an unbounded producer.
   [[maybe_unused]] constexpr auto unbounded = Repeat{7};

   // -----------------------------------------------------------------------
   // C++ interoperability: Repeat supports const traversal.
   // -----------------------------------------------------------------------
   auto runtime_finite = Repeat{2, -1};

   constexpr auto finite_sum = [finite] {
     auto sum = 0;
     for (const auto value : finite) {
       sum += value;
     }
     return sum;
   }();
   static_assert(finite_sum == 21);

   constexpr auto pixel_sum = [pixels] {
     auto sum = 0;
     for (const auto pixel : pixels) {
       sum += pixel.x + pixel.y;
     }
     return sum;
   }();
   static_assert(pixel_sum == 26);

   auto runtime_sum = 0;
   for (const auto value : runtime_finite) {
     runtime_sum += value;
   }

   return runtime_sum == -2 ? 0 : 1;
 }
 ~~~~~
 */
template <concepts_detail::RepeatClassArgument T>
class Repeat {
    friend struct concepts_detail::RepeatNextAccess;

  public:
    using value_type = T;

    class const_iterator {
      public:
        using value_type = T;
        using difference_type = std::ptrdiff_t;

        constexpr const_iterator() noexcept = default;

        constexpr const_iterator(const T* value, std::size_t remaining) noexcept
            : value_(value), remaining_(remaining) {
        }

        [[nodiscard]] constexpr auto operator*() const noexcept -> const T& {
            return *value_;
        }

        constexpr auto operator++() noexcept -> const_iterator& {
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
        const T* value_{nullptr};
        std::size_t remaining_{0U};
    };

    constexpr Repeat() noexcept : value_{}, count_{0U}, is_finite_{true} {
    }

    constexpr explicit Repeat(const T& value) noexcept : value_(value), count_(0U), is_finite_(false) {
    }

    template <concepts_detail::ProducerCount Count>
    constexpr Repeat(Count count, const T& value) noexcept
        : value_(value), count_(concepts_detail::normalize_producer_count(count)), is_finite_(true) {
    }

    template <typename Value>
        requires(!concepts::SafeImplicitConversion<T, Value>)
    explicit Repeat(Value value) = delete;

    template <typename Value, typename Count>
        requires(!concepts::SafeImplicitConversion<T, Value> || !concepts_detail::ProducerCount<Count>)
    Repeat(Count count, Value value) = delete;

    [[nodiscard]] constexpr auto count() const noexcept -> std::size_t {
        return is_finite_ ? count_ : CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE;
    }

    [[nodiscard]] constexpr auto is_finite() const noexcept -> bool {
        return is_finite_;
    }

    [[nodiscard]] constexpr auto begin() const noexcept -> const_iterator {
        return const_iterator{&value_, count()};
    }

    [[nodiscard]] constexpr auto end() const noexcept -> const_iterator {
        return const_iterator{&value_, 0U};
    }

    /** Producer parameter equality (REQ-FN-014B): compares only the stored
     *  value, count, and finite-form parameters, never the produced sequence.
     *
     *  It is the only producer parameter-equality surface; the `equal` free
     *  function instead compares bounded logical observations of the produced
     *  sequences. The unbounded `repeat(value)` form is distinct from the
     *  finite `repeat(count, value)` forms even when their produced prefixes
     *  coincide, and a counted form with count zero is distinct from the
     *  unbounded form. O(1), no traversal, no allocation. */
    [[nodiscard]] friend constexpr auto operator==(const Repeat& lhs, const Repeat& rhs) noexcept -> bool
        requires concepts::StableEqualityComparable<T>
    {
        return lhs.value_ == rhs.value_ && lhs.count_ == rhs.count_ && lhs.is_finite_ == rhs.is_finite_;
    }

  private:
    T value_;
    std::size_t count_;
    bool is_finite_;
};

/** Diagnostic specialization (REQ-DIAG-012): reached only through the
 *  redirecting deduction guides below. Constructing it is always a compile-time
 *  error whose single message names the class template, the rejected argument
 *  type, and the violated domain rule. */
template <>
class Repeat<concepts_detail::RepeatElementDomainViolation> {
  public:
    template <typename... Args>
    constexpr explicit Repeat([[maybe_unused]] Args&&... args) noexcept {
        static_assert(concepts_detail::dependent_false<Args...>,
                      "cljonic::Repeat: the repeated value must be storable without throwing. The value must be "
                      "copyable and safe to create, store, and destroy without throwing, and a finite count must be "
                      "a non-boolean, non-character integer type whose nonnegative range fits std::size_t. "
                      "Use Repeat{value} or Repeat{count, value}.");
    }
};

/** Redirecting deduction guides (REQ-DIAG-012), one per supported arity. Each is
 *  viable only for a rejected deduction, so an admitted deduction is never
 *  affected. The one-argument guide takes `const T&` to mirror the constructor's
 *  deduction, so an array argument is rejected as the array it is. */
template <typename T>
    requires(!concepts::NothrowCollectionElement<T>)
Repeat(const T&) -> Repeat<concepts_detail::RepeatElementDomainViolation>;

template <typename T, typename Count>
    requires(!concepts::NothrowCollectionElement<T> || !concepts_detail::ProducerCount<Count>)
Repeat(Count, const T&) -> Repeat<concepts_detail::RepeatElementDomainViolation>;

template <typename T>
    requires concepts::NothrowCollectionElement<std::remove_cvref_t<T>>
[[nodiscard]] constexpr auto repeat(T&& value) noexcept -> Repeat<std::remove_cvref_t<T>> {
    return Repeat<std::remove_cvref_t<T>>{std::forward<T>(value)};
}

template <typename T, concepts_detail::ProducerCount Count>
    requires concepts::NothrowCollectionElement<std::remove_cvref_t<T>>
[[nodiscard]] constexpr auto repeat(Count count, T&& value) noexcept -> Repeat<std::remove_cvref_t<T>> {
    return Repeat<std::remove_cvref_t<T>>{count, std::forward<T>(value)};
}

// Diagnostic fallback (REQ-DIAG-009): the value argument is outside the closed
// producer domain. This overload explains the rejection; it never returns a
// value and is never a supported call target.
template <typename T>
    requires(!concepts::NothrowCollectionElement<std::remove_cvref_t<T>>)
constexpr auto repeat([[maybe_unused]] T&& value) noexcept -> concepts_detail::RejectedProducerFactory {
    static_assert(concepts_detail::dependent_false<T>,
                  "cljonic::repeat: this value cannot be repeated. The repeated value must be copyable and must be "
                  "safe to create, store, and destroy without throwing.");
    return {};
}

template <typename T, typename Count>
    requires(!concepts::NothrowCollectionElement<std::remove_cvref_t<T>> || !concepts_detail::ProducerCount<Count>)
constexpr auto repeat([[maybe_unused]] Count count, [[maybe_unused]] T&& value) noexcept
    -> concepts_detail::RejectedProducerFactory {
    static_assert(concepts_detail::dependent_false<T>,
                  "cljonic::repeat: this value cannot be repeated. The repeated value must be copyable and must be "
                  "safe to create, store, and destroy without throwing. The count must be a non-boolean, "
                  "non-character integer type whose nonnegative range fits std::size_t. Use repeat(count, value).");
    return {};
}

} // namespace cljonic

namespace cljonic::concepts_detail {

template <concepts::NothrowCollectionElement T>
struct producer_traits<Repeat<T>> {
    static constexpr bool is_cljonic_producer = true;
    static constexpr producer_kind kind = producer_kind::repeat;
};

template <concepts::NothrowCollectionElement T>
struct contains_floating_point<Repeat<T>> : std::bool_constant<contains_floating_point_v<T>> {};

template <concepts::NothrowCollectionElement T>
struct contains_callable<Repeat<T>> : std::bool_constant<contains_callable_v<T>> {};

template <concepts::NothrowCollectionElement T>
struct contains_standard_range<Repeat<T>> : std::bool_constant<contains_standard_range_v<T>> {};

} // namespace cljonic::concepts_detail
