#ifndef CLJONIC_SET_HPP
#define CLJONIC_SET_HPP

#include <array>
#include <concepts>
#include <cstddef>
#include <utility>

#include <cljonic-concepts.hpp>
#include <cljonic-config.hpp>
#include <cljonic-core-collection-maximum-element-count.hpp>

namespace cljonic {

/** \anchor Set
 * \b Set is a fixed-capacity associative sequence collection backed by
 * contiguous array storage with linear-scan lookup and copy-on-modify
 * semantics.
 */
template <concepts::VectorElement T, std::size_t CapacityValue> class Set {
public:
  using value_type = T;

  static_assert(concepts::NothrowVectorElement<T>,
                "Set element type operations must not throw");
  static_assert(
      CapacityValue <= cljonic::CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE,
      "Set capacity exceeds CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT");

  constexpr Set() noexcept : elements_{}, logical_size_{0} {}

  [[nodiscard]] static constexpr auto capacity() noexcept -> std::size_t {
    return CapacityValue;
  }

  [[nodiscard]] constexpr auto size() const noexcept -> std::size_t {
    return logical_size_;
  }

  [[nodiscard]] constexpr auto count() const noexcept -> std::size_t {
    return logical_size_;
  }

  [[nodiscard]] constexpr auto empty() const noexcept -> bool {
    return logical_size_ == 0U;
  }

  [[nodiscard]] constexpr auto
  contains(const T &element) const noexcept -> bool {
    return find_index(element) < logical_size_;
  }

  /** Returns the stored element value when present; otherwise returns a
   * default-constructed value. */
  [[nodiscard]] constexpr auto
  operator()(const T &element) const noexcept -> T {
    const auto idx = find_index(element);
    return (idx < logical_size_) ? elements_[idx] : T{};
  }

  /** Returns the stored element value when present; otherwise returns the
   * supplied fallback. */
  [[nodiscard]] constexpr auto
  operator()(const T &element, const T &fallback) const noexcept -> T {
    const auto idx = find_index(element);
    return (idx < logical_size_) ? elements_[idx] : fallback;
  }

  /** Returns true when the element is already present or the set has room for
   * one more. */
  [[nodiscard]] constexpr auto
  can_conj(const T &element) const noexcept -> bool {
    return contains(element) || (logical_size_ < CapacityValue);
  }

  /** Adds an element to a copy of the set. If the element is already present or
   * capacity is full, returns an unchanged copy. */
  [[nodiscard]] constexpr auto conj(const T &element) const noexcept -> Set {
    Set result = *this;
    if (!result.contains(element) && result.logical_size_ < CapacityValue) {
      result.elements_[result.logical_size_] = element;
      ++result.logical_size_;
    }
    return result;
  }

  /** Removes an element via swap-and-remove if present; always returns a valid
   * copy. */
  [[nodiscard]] constexpr auto disj(const T &element) const noexcept -> Set {
    Set result = *this;
    const auto idx = result.find_index(element);
    if (idx < result.logical_size_) {
      if (idx + 1U < result.logical_size_) {
        result.elements_[idx] = result.elements_[result.logical_size_ - 1U];
      }
      --result.logical_size_;
    }
    return result;
  }

private:
  /** Returns `logical_size_` when the element is absent. */
  [[nodiscard]] constexpr auto
  find_index(const T &element) const noexcept -> std::size_t {
    for (std::size_t i = 0; i < logical_size_; ++i) {
      if (elements_[i] == element) {
        return i;
      }
    }
    return logical_size_;
  }

  std::array<value_type, CapacityValue> elements_{};
  std::size_t logical_size_{0};
};

} // namespace cljonic

#endif // CLJONIC_SET_HPP
