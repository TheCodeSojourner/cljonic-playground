#pragma once

#include <array>
#include <cstddef>
#include <type_traits>

#include <cljonic-concepts.hpp>

#if !defined(CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT)
#define CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT 1000
#endif

#define CLJONIC_STRINGIFY_INNER(x) #x
#define CLJONIC_STRINGIFY(x) CLJONIC_STRINGIFY_INNER(x)

namespace cljonic {

/** \anchor vector_state
 * Observable classification of a \ref Vector instance's fill relative to its
 * fixed capacity.
 */
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

/** \anchor Vector
 * \b Vector is a fixed-capacity sequential \b CopyOnModifyCollection backed by
 \c std::array.
 * No heap allocation occurs; construction with more initializers than \p
 capacity_value is a
 * compile-time error.  State is observable via \ref vector_state.
 *
 * Out-of-bounds access via \c get is not yet implemented; \c try_push_back
 returns \c false
 * (\b UnchangedValueReturn) when the collection is at capacity.
 *
 ~~~~~{.cpp}
 #include "cljonic-vector.hpp"
 using namespace cljonic;

 int main()
 {
     // CTAD: capacity deduced from initializer count
     const auto v0{Vector{1, 2, 3}};       // Vector<int, 3>, at_capacity
     const auto v1{Vector<int, 4>{1, 2}};  // explicit capacity 4, populated
     const auto v2{Vector<int, 4>{}};      // explicit capacity 4, empty

     // count() returns current logical size
     const auto n = count(v0);  // n == 3

     // try_push_back returns false at capacity; collection unchanged
     Vector<int, 2> v3{1, 2};
     const bool ok = v3.try_push_back(3);  // ok == false

     // Compiler error: initializer count exceeds capacity
     // const auto bad{Vector<int, 2>{1, 2, 3}};

     // Compiler error: capacity exceeds
 CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT
     // const auto big{Vector<int, 1001>{}};

     return 0;
 }
 ~~~~~
 */
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

} // namespace cljonic
