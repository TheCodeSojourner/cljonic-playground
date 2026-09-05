#pragma once

#include <array>
#include <cstddef>
#include <utility>

#include <cljonic-concepts.hpp>
#include <cljonic-config.hpp>
#include <cljonic-core-collection-maximum-element-count.hpp>

namespace cljonic {

// Traceability: entity-fields.Vector.
// The complete obligation set is defined in specs/collections/vector.allium,
// covered by matching TRACE_ID tests, and recorded in the traceability snapshot.

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

   friend constexpr bool operator==(const Pixel &lhs,
                                    const Pixel &rhs) noexcept {
     return lhs.x == rhs.x && lhs.y == rhs.y;
   }
 };

 using Inner = Vector<int, 2>;

 int main() {
   // Compile-time / constexpr demonstration.
   [[maybe_unused]] constexpr auto ints_at_capacity = Vector{1, 2, 3};
   [[maybe_unused]] constexpr auto ints_populated = Vector<int, 4>{1, 2};
   [[maybe_unused]] constexpr auto ints_empty = Vector<int, 4>{};
   [[maybe_unused]] constexpr auto nested_int_vectors =
       Vector{Vector<int, 2>{1, 2}, Vector<int, 2>{3}};
   [[maybe_unused]] constexpr auto nested_alias_vectors =
       Vector{Inner{4, 5}, Inner{6}};

   constexpr auto doubles_populated = Vector<double, 3>{1.5, 2.5};
   constexpr auto pixels_populated = Vector{Pixel{1, 2}, Pixel{3, 4}};
   constexpr Vector<int, 4> values{10, 20};

   static_assert(values(0U) == 10);
   static_assert(values(2U) == 0);
   static_assert(values(2U, 99) == 99);
   static_assert(std::same_as<decltype(doubles_populated(0U)), double>);
   static_assert(pixels_populated(0U).x == 1);
   static_assert(pixels_populated(1U).y == 4);

   // Runtime demonstration.
   auto runtime_values = Vector<int, 4>{7, 9};
   const auto fallback = runtime_values(4U, -1);

   auto runtime_pixels = Vector<Pixel, 4>{Pixel{1, 2}, Pixel{3, 4}};
   const auto pixel_value = runtime_pixels(1U);
   const auto pixel_fallback = runtime_pixels(4U, Pixel{99, 99});

   return (fallback == -1 && pixel_value == Pixel{3, 4} &&
           pixel_fallback == Pixel{99, 99})
              ? 0
              : 1;
 }
 ~~~~~
 */
template <concepts::NothrowCollectionElement element_type, std::size_t capacity_value>
class Vector {
  public:
    using value_type = element_type;

    static_assert(
        capacity_value <= cljonic::CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE,
        "Vector capacity_value exceeds "
        "CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT=" CLJONIC_STRINGIFY(CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT));

    template <typename... Args>
    constexpr Vector(Args&&... args) noexcept((concepts::NothrowElementConstruction<element_type, Args> && ...))
        : storage_{}, logical_size_{0} {
        static_assert(sizeof...(Args) <= capacity_value, "Vector initializer count exceeds Vector capacity_value");
        static_assert((concepts::NothrowElementConstruction<element_type, Args> && ...),
                      "Vector constructor requires all arguments to construct "
                      "element_type without throwing and be implicitly "
                      "convertible to element_type");

        initialize_storage_if_valid(std::forward<Args>(args)...);
    }

    [[nodiscard]] static constexpr auto capacity() noexcept -> std::size_t {
        return capacity_value;
    }

    [[nodiscard]] constexpr auto count() const noexcept -> std::size_t {
        return logical_size_;
    }

    template <std::integral index_type>
    [[nodiscard]] constexpr auto operator()(index_type index) const noexcept -> value_type {
        return index_is_valid(index) ? storage_[static_cast<std::size_t>(index)] : value_type{};
    }

    template <std::integral index_type>
    [[nodiscard]] constexpr auto operator()(index_type index, const value_type& fallback) const noexcept -> value_type {
        return index_is_valid(index) ? storage_[static_cast<std::size_t>(index)] : fallback;
    }

    template <std::integral index_type>
    [[nodiscard]] constexpr auto contains(index_type index) const noexcept -> bool {
        return index_is_valid(index);
    }

    [[nodiscard]] constexpr auto is_empty() const noexcept -> bool {
        return logical_size_ == 0U;
    }

  private:
    template <std::integral index_type>
    [[nodiscard]] constexpr auto index_is_valid(index_type index) const noexcept -> bool {
        if constexpr (std::signed_integral<index_type>) {
            if (index < 0) {
                return false;
            }
        }

        return static_cast<std::size_t>(index) < logical_size_;
    }

    template <typename... Args>
    static constexpr bool constructor_arguments_valid =
        sizeof...(Args) <= capacity_value && (concepts::NothrowElementConstruction<element_type, Args> && ...);

    template <typename... Args>
    constexpr void initialize_storage_if_valid(Args&&... args) noexcept {
        if constexpr (constructor_arguments_valid<Args...>) {
            initialize_storage(std::index_sequence_for<Args...>{}, std::forward<Args>(args)...);
            logical_size_ = sizeof...(Args);
        }
    }

    template <std::size_t... Indices, typename... Args>
    constexpr void initialize_storage(std::index_sequence<Indices...>, Args&&... args) noexcept {
        ((storage_[Indices] = element_type{std::forward<Args>(args)}), ...);
    }

    std::array<value_type, capacity_value> storage_{};
    std::size_t logical_size_ = 0;
};

template <typename First, typename... Rest>
Vector(First, Rest...) -> Vector<First, 1 + sizeof...(Rest)>;

} // namespace cljonic

namespace cljonic::concepts_detail {

template <typename ElementType, std::size_t CapacityValue>
struct collection_traits<Vector<ElementType, CapacityValue>> {
    static constexpr bool is_cljonic_collection = true;
    static constexpr collection_kind kind = collection_kind::vector;
};

} // namespace cljonic::concepts_detail
