#pragma once

#include <array>
#include <cstddef>
#include <utility>

#include <cljonic-concepts.hpp>
#include <cljonic-config.hpp>
#include <cljonic-core-collection-maximum-element-count.hpp>

namespace cljonic {

/** \anchor Vector
 * \b Vector is a bounded, ordered collection that provides callable lookup with optional fallback values. The way to
 * operate on the collection is through the library's free-function API. Updates return a modified copy without
 * changing the original collection. Construction with more initializers than the available capacity is rejected at
 * compile time.
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

 struct CategoryArgument {};

 struct CategoryElement {
   int category = 0;

   constexpr CategoryElement() noexcept = default;
   constexpr CategoryElement(const CategoryArgument &) noexcept : category(1) {}
   constexpr CategoryElement(CategoryArgument &&) noexcept : category(2) {}
   constexpr CategoryElement(const CategoryElement &) noexcept = default;
   constexpr auto operator=(const CategoryElement &) noexcept
       -> CategoryElement & = default;
 };

 using Inner = Vector<int, 2>;

 int main() {
   // CTAD infers Vector<int, 3> from the initializer count.
   [[maybe_unused]] constexpr auto ints_at_capacity = Vector{1, 2, 3};

   // Explicit capacity permits a partially populated Vector and an empty Vector.
   [[maybe_unused]] constexpr auto ints_populated = Vector<int, 4>{1, 2};
   [[maybe_unused]] constexpr auto ints_empty = Vector<int, 4>{};

   // Vector values can be nested, including through an explicit type alias.
   [[maybe_unused]] constexpr auto nested_int_vectors =
       Vector{Vector<int, 2>{1, 2}, Vector<int, 2>{3}};
   [[maybe_unused]] constexpr auto nested_alias_vectors =
       Vector{Inner{4, 5}, Inner{6}};

   // User-defined values and constructor argument categories are supported.
   constexpr auto doubles_populated = Vector<double, 3>{1.5, 2.5};
   constexpr auto pixels_populated = Vector{Pixel{1, 2}, Pixel{3, 4}};
   constexpr Vector<int, 4> values{10, 20};
   constexpr CategoryArgument category_argument{};
   constexpr Vector<CategoryElement, 1> lvalue_constructed{category_argument};
   constexpr Vector<CategoryElement, 1> rvalue_constructed{CategoryArgument{}};

   static_assert(values(0) == 10);
   static_assert(values(2) == 0);
   static_assert(values(2, 99) == 99);
   static_assert(values(-1) == 0);
   static_assert(values(-1, 99) == 99);
   static_assert(std::same_as<decltype(doubles_populated(0)), double>);
   static_assert(pixels_populated(0).x == 1);
   static_assert(pixels_populated(1).y == 4);
   static_assert(lvalue_constructed(0).category == 1);
   static_assert(rvalue_constructed(0).category == 2);

   // Without a fallback, an invalid lookup returns value_type{}; Pixel's
   // default-constructed int members are zero.
   static_assert(pixels_populated(-1).x == 0);
   static_assert(pixels_populated(-1).y == 0);

   // A Vector is callable with an index: valid indices select stored values;
   // an invalid index, including a negative signed index, returns the default
   // value_type{} or the supplied fallback. For int, value_type{} is zero.
   auto runtime_values = Vector<int, 4>{7, 9};
   const auto fallback = runtime_values(4, -1);
   const auto negative_default = runtime_values(-1);
   const auto negative_fallback = runtime_values(-1, 99);

   // Pixel equality validates the runtime result for a user-defined value type.
   auto runtime_pixels = Vector<Pixel, 4>{Pixel{1, 2}, Pixel{3, 4}};
   const auto pixel_value = runtime_pixels(1);
   const auto pixel_fallback = runtime_pixels(4, Pixel{99, 99});

   return (fallback == -1 && negative_default == 0 && negative_fallback == 99 &&
           pixel_value == Pixel{3, 4} && pixel_fallback == Pixel{99, 99})
              ? 0
              : 1;
 }
 ~~~~~
 */
template <concepts::NothrowCollectionElement ElementType, std::size_t CapacityValue>
class Vector {
  public:
    using value_type = ElementType;

    static_assert(
        CapacityValue <= cljonic::CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE,
        "Vector CapacityValue exceeds "
        "CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT=" CLJONIC_STRINGIFY(CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT));

    template <typename... Args>
    constexpr Vector(Args&&... args) noexcept((concepts::NothrowElementConstruction<ElementType, Args> && ...)) {
        static_assert(sizeof...(Args) <= CapacityValue, "Vector initializer count exceeds Vector CapacityValue");
        static_assert((concepts::NothrowElementConstruction<ElementType, Args> && ...),
                      "Vector constructor requires all arguments to construct "
                      "ElementType without throwing and be implicitly "
                      "convertible to ElementType");

        initialize_storage_if_valid(std::forward<Args>(args)...);
    }

    [[nodiscard]] static constexpr auto capacity() noexcept -> std::size_t {
        return CapacityValue;
    }

    [[nodiscard]] constexpr auto count() const noexcept -> std::size_t {
        return logical_size_;
    }

    template <std::integral IndexType>
    [[nodiscard]] constexpr auto operator()(IndexType index) const noexcept -> value_type {
        return index_is_valid(index) ? storage_[static_cast<std::size_t>(index)] : value_type{};
    }

    template <std::integral IndexType>
    [[nodiscard]] constexpr auto operator()(IndexType index, const value_type& fallback) const noexcept -> value_type {
        return index_is_valid(index) ? storage_[static_cast<std::size_t>(index)] : fallback;
    }

    template <std::integral IndexType>
    [[nodiscard]] constexpr auto contains(IndexType index) const noexcept -> bool {
        return index_is_valid(index);
    }

    [[nodiscard]] constexpr auto is_empty() const noexcept -> bool {
        return logical_size_ == 0U;
    }

  private:
    template <std::integral IndexType>
    [[nodiscard]] constexpr auto index_is_valid(IndexType index) const noexcept -> bool {
        if constexpr (std::signed_integral<IndexType>) {
            if (index < 0) {
                return false;
            }
        }

        return static_cast<std::size_t>(index) < logical_size_;
    }

    template <typename... Args>
    static constexpr bool constructor_arguments_valid =
        sizeof...(Args) <= CapacityValue && (concepts::NothrowElementConstruction<ElementType, Args> && ...);

    template <typename... Args>
    constexpr void initialize_storage_if_valid(Args&&... args) noexcept {
        if constexpr (constructor_arguments_valid<Args...>) {
            initialize_storage(std::index_sequence_for<Args...>{}, std::forward<Args>(args)...);
            logical_size_ = sizeof...(Args);
        }
    }

    template <std::size_t... Indices, typename... Args>
    constexpr void initialize_storage(std::index_sequence<Indices...>, Args&&... args) noexcept {
        ((storage_[Indices] = ElementType{std::forward<Args>(args)}), ...);
    }

    std::array<value_type, CapacityValue> storage_{};
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
