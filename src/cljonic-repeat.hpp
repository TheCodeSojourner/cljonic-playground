#pragma once

#include <concepts>
#include <cstddef>
#include <type_traits>
#include <utility>

#include <cljonic-concepts.hpp>
#include <cljonic-core-collection-maximum-element-count.hpp>

namespace cljonic {

/** \anchor Repeat
 * \b Repeat is a producer that owns one value and yields copies of it. `repeat(value)` is unbounded, while
 * `repeat(value, count)` yields exactly \p count values.
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
   constexpr auto finite = Repeat{7, 3U};
   constexpr auto pixels = Repeat{Pixel{4, 9}, 2U};

   // An uncounted Repeat is an unbounded producer.
   [[maybe_unused]] constexpr auto unbounded = Repeat{7};

   // -----------------------------------------------------------------------
   // C++ interoperability: Repeat supports const traversal.
   // -----------------------------------------------------------------------
   auto runtime_finite = Repeat{-1, 2U};

   int finite_sum = 0;
   for (const auto value : finite) {
     finite_sum += value;
   }

   int pixel_sum = 0;
   for (const auto pixel : pixels) {
     pixel_sum += pixel.x + pixel.y;
   }

   int runtime_sum = 0;
   for (const auto value : runtime_finite) {
     runtime_sum += value;
   }

   return (finite_sum == 21) && (pixel_sum == 26) && (runtime_sum == -2);
 }
 ~~~~~
 */
template <concepts::NothrowCollectionElement T>
class Repeat {
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

    constexpr explicit Repeat(T value) noexcept : value_(std::move(value)), count_(0U), is_finite_(false) {
    }

    constexpr Repeat(T value, std::size_t count) noexcept : value_(std::move(value)), count_(count), is_finite_(true) {
    }

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

  private:
    T value_;
    std::size_t count_;
    bool is_finite_;
};

template <typename T>
    requires concepts::NothrowCollectionElement<std::remove_cvref_t<T>>
[[nodiscard]] constexpr auto repeat(T&& value) noexcept -> Repeat<std::remove_cvref_t<T>> {
    return Repeat<std::remove_cvref_t<T>>{std::forward<T>(value)};
}

template <typename T>
    requires concepts::NothrowCollectionElement<std::remove_cvref_t<T>>
[[nodiscard]] constexpr auto repeat(T&& value, std::size_t count) noexcept -> Repeat<std::remove_cvref_t<T>> {
    return Repeat<std::remove_cvref_t<T>>{std::forward<T>(value), count};
}

} // namespace cljonic

namespace cljonic::concepts_detail {

template <concepts::NothrowCollectionElement T>
struct producer_traits<Repeat<T>> {
    static constexpr bool is_cljonic_producer = true;
    static constexpr producer_kind kind = producer_kind::repeat;
};

} // namespace cljonic::concepts_detail
