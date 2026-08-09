#pragma once

#include <array>
#include <cstddef>
#include <utility>

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
 * \b Vector is a fixed-capacity collection with copy-on-modify behavior.
  * Construction with more initializers than capacity is a compile-time
  * error.
 *
 * This example focuses on collection construction. Free functions that operate
 * on collections are documented with their own headers.
 *
 ~~~~~{.cpp}
 #include "cljonic-core.hpp"
 using namespace cljonic;

 struct Pixel {
   int x;
   int y;
 };

 using Inner = Vector<int, 2>;

 int main() {
   [[maybe_unused]] constexpr auto ints_at_capacity = Vector{1, 2, 3};
   [[maybe_unused]] constexpr auto ints_populated = Vector<int, 4>{1, 2};
   [[maybe_unused]] constexpr auto ints_empty = Vector<int, 4>{};
   [[maybe_unused]] constexpr auto doubles_populated =
       Vector<double, 3>{1.5, 2.5};
   [[maybe_unused]] constexpr auto pixels_populated =
       Vector{Pixel{1, 2}, Pixel{3, 4}};
   [[maybe_unused]] constexpr auto nested_int_vectors =
       Vector{Vector<int, 2>{1, 2}, Vector<int, 2>{3}};
   [[maybe_unused]] constexpr auto nested_alias_vectors =
       Vector{Inner{4, 5}, Inner{6}};

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
  constexpr Vector(Args... args) noexcept(
      (concepts::NothrowConstructible<element_type, Args> && ...))
      : storage_{}, logical_size_{0} {
    static_assert(sizeof...(Args) <= capacity_value,
                  "Vector constructor requires initializer count to be less "
                  "than or equal to capacity");
    static_assert((concepts::ConvertibleToElement<element_type, Args> && ...),
                  "Vector constructor requires all arguments to be "
                  "convertible to element_type");
    static_assert((concepts::NothrowConstructible<element_type, Args> && ...),
                  "Vector constructor requires all arguments to construct "
                  "element_type without throwing");

    initialize_storage_if_valid(args...);
  }

  [[nodiscard]] static constexpr auto capacity() noexcept -> std::size_t {
    return capacity_value;
  }

  [[nodiscard]] static constexpr auto
  collection_maximum_element_count() noexcept -> std::size_t {
    return CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT;
  }

  [[nodiscard]] constexpr auto size() const noexcept -> std::size_t {
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
  template <typename... Args>
  static constexpr bool constructor_arguments_valid =
      sizeof...(Args) <= capacity_value &&
      (concepts::ConvertibleToElement<element_type, Args> && ...) &&
      (concepts::NothrowConstructible<element_type, Args> && ...);

  template <typename... Args>
  constexpr void initialize_storage_if_valid(Args... args) noexcept {
    if constexpr (constructor_arguments_valid<Args...>) {
      initialize_storage(std::index_sequence_for<Args...>{}, args...);
      logical_size_ = sizeof...(Args);
    }
  }

  template <std::size_t... Indices, typename... Args>
  constexpr void initialize_storage(std::index_sequence<Indices...>,
                                    Args... args) noexcept {
    ((storage_[Indices] = static_cast<element_type>(args)), ...);
  }

  std::array<value_type, capacity_value> storage_{};
  std::size_t logical_size_ = 0;
};

template <typename First, typename... Rest>
Vector(First, Rest...) -> Vector<First, 1 + sizeof...(Rest)>;

} // namespace cljonic
