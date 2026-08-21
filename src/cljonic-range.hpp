#pragma once

#include <concepts>
#include <cstddef>
#include <type_traits>

#include <cljonic-concepts.hpp>
#include <cljonic-core-collection-maximum-element-count.hpp>

namespace cljonic {

/** \anchor Range
 * \b Range is a bounded integer producer that represents a sequence from
 * \p start up to but excluding \p end, advancing by \p step. The supported
 * subset is intentionally limited to integral types with explicit defaulting
 * and bounded materialization. \p value_type defaults to \c int, so
 * \c Range{} is equivalent to Clojure's <tt>(range)</tt>.
 */
template <std::integral value_type = int> class Range {
public:
  using element_type = value_type;

  static_assert(std::integral<value_type>,
                "Range requires an integral element type");

  constexpr Range() noexcept
      : start_{0}, end_{collection_maximum_element_count()}, step_{1},
        size_{collection_maximum_element_count()} {}

  constexpr explicit Range(value_type end) noexcept
    requires std::integral<value_type>
      : Range(value_type{0}, end, value_type{1}) {}

  constexpr Range(value_type start, value_type end) noexcept
    requires std::integral<value_type>
      : Range(start, end, value_type{1}) {}

  constexpr Range(value_type start, value_type end, value_type step) noexcept
    requires std::integral<value_type>
      : start_{start}, end_{end}, step_{step},
        size_{compute_size(start, end, step)} {
    if (step == value_type{0}) {
      start_ = start;
      end_ = end;
      step_ = step;
      size_ = collection_maximum_element_count();
    }
  }

  // Reject construction from any argument type that isn't exactly value_type,
  // so mismatched integral/floating-point arguments fail to compile instead
  // of silently truncating via implicit conversion.
  template <typename T>
    requires(!std::same_as<T, value_type>)
  constexpr Range(T) noexcept = delete;

  template <typename T, typename U>
    requires(!std::same_as<T, value_type> || !std::same_as<U, value_type>)
  constexpr Range(T, U) noexcept = delete;

  template <typename T, typename U, typename V>
    requires(!std::same_as<T, value_type> || !std::same_as<U, value_type> ||
             !std::same_as<V, value_type>)
  constexpr Range(T, U, V) noexcept = delete;

  [[nodiscard]] constexpr auto size() const noexcept -> std::size_t {
    return size_;
  }

  [[nodiscard]] constexpr auto empty() const noexcept -> bool {
    return size_ == 0U;
  }

private:
  static constexpr auto compute_size(value_type start, value_type end,
                                     value_type step) noexcept -> std::size_t {
    if (step == value_type{0}) {
      return collection_maximum_element_count();
    }

    if (step > value_type{0}) {
      if (start >= end) {
        return 0U;
      }
      using unsigned_type = std::make_unsigned_t<value_type>;
      const auto delta =
          static_cast<unsigned_type>(end) - static_cast<unsigned_type>(start);
      const auto positive_step = static_cast<unsigned_type>(step);
      const auto computed_size =
          static_cast<std::size_t>(delta / positive_step) +
          (delta % positive_step == 0U ? 0U : 1U);
      return computed_size > collection_maximum_element_count()
                 ? collection_maximum_element_count()
                 : computed_size;
    }

    if (start <= end) {
      return 0U;
    }

    using unsigned_type = std::make_unsigned_t<value_type>;
    const auto magnitude =
        static_cast<unsigned_type>(0) - static_cast<unsigned_type>(step);
    const auto delta =
        static_cast<unsigned_type>(start) - static_cast<unsigned_type>(end);
    const auto computed_size = static_cast<std::size_t>(delta / magnitude) +
                               (delta % magnitude == 0U ? 0U : 1U);
    return computed_size > collection_maximum_element_count()
               ? collection_maximum_element_count()
               : computed_size;
  }

  value_type start_;
  value_type end_;
  value_type step_;
  std::size_t size_;
};

template <std::integral integer_type>
Range(integer_type) -> Range<integer_type>;

template <std::integral integer_type>
Range(integer_type, integer_type) -> Range<integer_type>;

template <std::integral integer_type>
Range(integer_type, integer_type, integer_type) -> Range<integer_type>;

} // namespace cljonic
