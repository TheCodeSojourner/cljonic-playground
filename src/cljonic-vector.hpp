#pragma once

#include <array>
#include <cstddef>
#include <utility>

#include <cljonic-concepts.hpp>
#include <cljonic-config.hpp>
#include <cljonic-core-collection-maximum-element-count.hpp>

namespace cljonic {

// Spec obligations: VectorCollection and its invariants are implemented by
// this type and covered by the corresponding traceable tests.
// entity-fields.VectorCollection
// invariant.VectorCollection.CapacityIsNonNegative
// invariant.VectorCollection.LogicalSizeIsNonNegative
// invariant.VectorCollection.LogicalSizeDoesNotExceedCapacity
// invariant.VectorCollection.SupportsLiteralDeducedConstruction
// invariant.VectorCollection.SupportsEmptyExplicitCapacityConstruction
// invariant.VectorCollection.OversizedInitializerIsCompileTimeFailure
// invariant.VectorCollection.OversizedInitializerDiagnosticIdentifiesCapacity
// invariant.VectorCollection.SupportsIndexedLookup
// invariant.VectorCollection.SupportsIndexedFallbackLookup
// invariant.VectorCollection.InvalidIndexReturnsDefaultElement
// invariant.VectorCollection.InvalidIndexReturnsSuppliedFallback

/** \anchor Vector
 * \b Vector is a CopyOnModifyCollection with fixed-capacity storage.
 * Construction with more initializers than capacity is a compile-time error.
 *
 * This example covers bounded construction and direct member observation.
 *
 ~~~~~{.cpp}
 #include "cljonic.hpp"
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
   constexpr auto doubles_populated = Vector<double, 3>{1.5, 2.5};
   constexpr auto pixels_populated = Vector{Pixel{1, 2}, Pixel{3, 4}};
   [[maybe_unused]] constexpr auto nested_int_vectors =
       Vector{Vector<int, 2>{1, 2}, Vector<int, 2>{3}};
   [[maybe_unused]] constexpr auto nested_alias_vectors =
       Vector{Inner{4, 5}, Inner{6}};

   constexpr Vector<int, 4> values{10, 20};
   static_assert(values(0U) == 10);
   static_assert(values(2U) == 0);
   static_assert(values(2U, 99) == 99);
   static_assert(std::same_as<decltype(doubles_populated(0U)), double>);
   static_assert(pixels_populated(0U).x == 1);
   static_assert(pixels_populated(1U).y == 4);

   return 0;
 }
 ~~~~~
 */
template <concepts::VectorElement element_type, std::size_t capacity_value>
class Vector {
public:
  using value_type = element_type;

  static_assert(concepts::NothrowVectorElement<element_type>,
                "Vector element storage operations must not throw");

  static_assert(capacity_value <=
                    cljonic::CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE,
                "Vector capacity exceeds "
                "CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT=" CLJONIC_STRINGIFY(
                    CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT));

  template <typename... Args>
  constexpr Vector(Args... args) noexcept(
      concepts::NothrowVectorElement<element_type> &&
      (concepts::NothrowElementConstruction<element_type, Args> && ...))
      : storage_{}, logical_size_{0} {
    static_assert(sizeof...(Args) <= capacity_value,
                  "Vector constructor requires initializer count to be less "
                  "than or equal to capacity");
    static_assert(
        (concepts::NothrowElementConstruction<element_type, Args> && ...),
        "Vector constructor requires all arguments to construct "
        "element_type without throwing and be implicitly "
        "convertible to element_type");

    initialize_storage_if_valid(args...);
  }

  [[nodiscard]] static constexpr auto capacity() noexcept -> std::size_t {
    return capacity_value;
  }

  [[nodiscard]] constexpr auto size() const noexcept -> std::size_t {
    return logical_size_;
  }

  template <std::integral index_type>
  [[nodiscard]] constexpr auto
  operator()(index_type index) const noexcept -> value_type {
    return index_is_valid(index) ? storage_[static_cast<std::size_t>(index)]
                                 : value_type{};
  }

  template <std::integral index_type>
  [[nodiscard]] constexpr auto
  operator()(index_type index,
             const value_type &fallback) const noexcept -> value_type {
    return index_is_valid(index) ? storage_[static_cast<std::size_t>(index)]
                                 : fallback;
  }

private:
  template <std::integral index_type>
  [[nodiscard]] constexpr auto
  index_is_valid(index_type index) const noexcept -> bool {
    if constexpr (std::signed_integral<index_type>) {
      if (index < 0) {
        return false;
      }
    }

    return static_cast<std::size_t>(index) < logical_size_;
  }

  template <typename... Args>
  static constexpr bool constructor_arguments_valid =
      sizeof...(Args) <= capacity_value &&
      concepts::NothrowVectorElement<element_type> &&
      (concepts::NothrowElementConstruction<element_type, Args> && ...);

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
    ((storage_[Indices] = element_type{args}), ...);
  }

  std::array<value_type, capacity_value> storage_{};
  std::size_t logical_size_ = 0;
};

template <typename First, typename... Rest>
Vector(First, Rest...) -> Vector<First, 1 + sizeof...(Rest)>;

} // namespace cljonic
