#pragma once

#include <array>
#include <cstddef>
#include <type_traits>

#include <concepts.hpp>

#if !defined(CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT)
#define CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT 1000
#endif

#define CLJONIC_STRINGIFY_INNER(x) #x
#define CLJONIC_STRINGIFY(x) CLJONIC_STRINGIFY_INNER(x)

namespace cljonic {

enum class vector_state {
  empty,
  populated,
  at_capacity,
};

namespace detail::vector {
struct vector_observation {
  int capacity_limit;
  int logical_size;
};

[[nodiscard]] constexpr auto
classify_vector(vector_observation observation) noexcept -> vector_state {
  if (observation.logical_size == 0) {
    return vector_state::empty;
  }

  if (observation.logical_size == observation.capacity_limit) {
    return vector_state::at_capacity;
  }

  return vector_state::populated;
}
} // namespace detail::vector

template <concepts::VectorElement element_type, std::size_t capacity_value>
class Vector {
public:
  using value_type = element_type;

  static_assert(capacity_value <= CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT,
                "Vector capacity exceeds "
                "CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT=" CLJONIC_STRINGIFY(
                    CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT));

  template <typename... Args>
    requires(sizeof...(Args) <= capacity_value) &&
                (std::is_convertible_v<Args, element_type> && ...)
  constexpr Vector(Args... args) noexcept(
      (std::is_nothrow_constructible_v<element_type, Args> && ...))
      : storage_{static_cast<element_type>(args)...},
        logical_size_{sizeof...(Args)} {}

  [[nodiscard]] static constexpr auto capacity_limit() noexcept -> std::size_t {
    return capacity_value;
  }

  [[nodiscard]] static constexpr auto
  collection_maximum_element_count() noexcept -> std::size_t {
    return CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT;
  }

  [[nodiscard]] constexpr auto logical_size() const noexcept -> std::size_t {
    return logical_size_;
  }

  [[nodiscard]] constexpr auto state() const noexcept -> vector_state {
    const detail::vector::vector_observation observation{
        static_cast<int>(capacity_value), static_cast<int>(logical_size_)};
    return detail::vector::classify_vector(observation);
  }

  [[nodiscard]] constexpr auto try_push_back(const value_type &value) noexcept(
      concepts::NothrowCopyAssignable<value_type>) -> bool {
    if (logical_size_ >= capacity_value) {
      return false;
    }

    storage_[logical_size_] = value;
    ++logical_size_;
    return true;
  }

private:
  std::array<value_type, capacity_value> storage_{};
  std::size_t logical_size_ = 0;
};

template <typename First, typename... Rest>
Vector(First, Rest...) -> Vector<First, 1 + sizeof...(Rest)>;

template <concepts::VectorElement element_type, std::size_t capacity_value>
[[nodiscard]] constexpr auto
count(const Vector<element_type, capacity_value> &collection) noexcept
    -> std::size_t {
  return collection.logical_size();
}

} // namespace cljonic
