#pragma once

#include <array>
#include <cstddef>
#include <ranges>
#include <span>
#include <utility>

#include <cljonic-concepts.hpp>
#include <cljonic-config.hpp>
#include <cljonic-core-collection-maximum-element-count.hpp>

namespace cljonic {

/** \anchor Vector
 * A Vector is a bounded, ordered collection that provides callable lookup with optional fallback values. Other
 * operations use the library's free-function API.
 *
 * The \c vector factory is the idiomatic way to create Vector values, at runtime or during constant evaluation. Use \c
 * vector(args...) to infer the element type and capacity, \c vector<T>(args...) to select an element type, or \c
 * vector<T, Capacity>(args...) to provide an explicit capacity, including for an empty vector. \c Vector<T, Capacity>
 * remains the value's type.
 *
 * Direct construction is useful when importing a C++ range or view: its elements are copied into the Vector. A cljonic
 * value is not copied as a range/view source. With an explicit element type, a cljonic argument is accepted as one
 * element only when its type exactly matches the declared element type. Without explicit template arguments, a sole
 * cljonic argument determines the element type and yields a one-element collection. Elements may themselves be cljonic
 * collections or producers, including nested cljonic composites.
 *
 * Construction with more initializers than the available capacity is rejected at compile time. Element construction,
 * updates, and fallbacks follow the \ref implicit-conversions "safe implicit conversion rules".
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

 int main() {
   // The factory is the idiomatic way to create vectors.
   constexpr auto ints_empty = vector<int>();
   constexpr auto ints_at_capacity = vector(1, 2, 3);

   // Explicit capacity permits a partially populated or empty vector.
   constexpr auto ints_populated = vector<int, 4>(1, 2);
   constexpr auto ints_reserved = vector<int, 4>();
   static_assert(ints_empty(0, 99) == 99);
   static_assert(ints_at_capacity(2) == 3);
   static_assert(ints_populated(1) == 2);
   static_assert(ints_reserved(0, 99) == 99);

   // Vectors can be nested.
   constexpr auto nested_int_vectors = vector(vector(1, 2), vector<int, 2>(3));
   static_assert(nested_int_vectors(1)(0) == 3);

   // An explicit element type and user-defined values are supported.
   constexpr auto doubles_populated = vector<double>(1.5f, 2.5f);
   constexpr auto pixels_populated = vector(Pixel{1, 2}, Pixel{3, 4});
   constexpr auto values = vector<int, 4>(10, 20);

   // Vector values can be used as a callable function, returning a default-
   // value for invalid indexes or a supplied fallback when provided.
   static_assert(values(0) == 10);
   static_assert(values(2) == 0);
   static_assert(values(2, 99) == 99);
   static_assert(values(-1) == 0);
   static_assert(values(-1, 99) == 99);
   static_assert(doubles_populated(0) == 1.5);
   static_assert(pixels_populated(0).x == 1);
   static_assert(pixels_populated(1).y == 4);

   // Without a fallback, an invalid lookup returns value_type{}; Pixel's int
   // members have a default value of zero.
   static_assert(pixels_populated(-1).x == 0);
   static_assert(pixels_populated(-1).y == 0);

   // Pixel equality validates the runtime result for a user-defined value type.
   volatile int runtime_first_raw = 7;
   const int runtime_first = runtime_first_raw;
   auto runtime_pixels = vector(Pixel{1, 2}, Pixel{runtime_first - 4, 4});
   const auto pixel_value = runtime_pixels(1);
   const auto pixel_fallback = runtime_pixels(4, Pixel{99, 99});

   // Runtime construction supports the same callable lookup and fallback
   // behavior.
   auto runtime_values = vector(runtime_first, runtime_first + 2);
   const auto fallback = runtime_values(4, -1);
   const auto negative_default = runtime_values(-1);
   const auto negative_fallback = runtime_values(-1, 99);

   // A standard view pipeline can use an existing Vector as its source and
   // Construct another Vector from the transformed values.
   const auto doubled_view =
       runtime_values |
       std::views::transform([](int value) noexcept { return value * 2; });
   const auto from_pipeline = Vector<int, 4>{doubled_view};

   // -----------------------------------------------------------------------
   // C++ interoperability: a Vector supports const traversal and exposes a
   // non-owning std::span view. Range/view sources are copied into owned
   // storage, retaining only the bounded prefix that fits the capacity.
   // -----------------------------------------------------------------------
   static constexpr int source_values[] = {11, 22, 33, 44};
   constexpr auto source_span = std::span{source_values};
   constexpr auto from_span = Vector<int, 4>{source_span};
   static_assert(from_span(1) == 22);
   constexpr auto from_span_ctad = Vector{source_span};
   static_assert(from_span_ctad(2) == 33);
   constexpr auto from_span_overallocated = Vector<int, 40>{source_span};
   static_assert(from_span_overallocated(20, -11) == -11);

   // Constructing a Vector from a runtime C++ array/span
   int runtime_buffer[] = {100, 200, 300};
   const auto runtime_from_span =
       Vector<int, 4>{std::span<const int>{runtime_buffer, 3}};

   constexpr auto interop_values = vector(10, 20, 30);
   static_assert(interop_values.begin()[1] == 20);
   static_assert(interop_values.view().size() == 3);

   // Use C++ interoperability to sum the values in a vector
   auto range_sum = 0;
   for (const auto value : runtime_values) {
     range_sum += value;
   }

   const auto runtime_view = runtime_values.view();

   return (fallback == -1 && negative_default == 0 && negative_fallback == 99 &&
           values(1) == 20 && pixel_value == Pixel{3, 4} &&
           pixel_fallback == Pixel{99, 99} && range_sum == 16 &&
           runtime_view.size() == 2 && runtime_view[0] == 7 &&
           from_pipeline(0) == 14 && from_pipeline(1) == 18 &&
           runtime_from_span(0) == 100 && runtime_from_span(1) == 200)
              ? 0
              : 1;
 }
 ~~~~~
 */
template <concepts::NothrowCollectionElement ElementType, std::size_t CapacityValue>
class Vector {
  public:
    using key_type = std::size_t;
    using lookup_type = key_type;
    using association_value_type = ElementType;
    using value_type = ElementType;

    static_assert(
        CapacityValue <= cljonic::CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE,
        "cljonic::Vector: CapacityValue exceeds "
        "CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT=" CLJONIC_STRINGIFY(CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT));

    template <typename... Args>
    explicit(sizeof...(Args) ==
             1) constexpr Vector(Args&&... args) noexcept((concepts::NothrowElementConstruction<ElementType, Args> &&
                                                           ...)) {
        static_assert(sizeof...(Args) <= CapacityValue,
                      "cljonic::Vector: initializer count exceeds Vector CapacityValue");
        static_assert(
            (concepts::NothrowElementConstruction<ElementType, Args> && ...),
            "cljonic::Vector: constructor requires all arguments to construct "
            "ElementType without throwing and convert safely without losing values or changing their meaning");

        initialize_storage_if_valid(std::forward<Args>(args)...);
    }

    // A cljonic collection or producer is not a SourceConstruction source
    // (REQ-FN-027A): it is admissible only as one element of exactly its own
    // type (the pack constructor) or enclosed via EnclosureConstruction;
    // materializing it is the role of into, preflighted by fits_into. A lone
    // argument that is exactly ElementType likewise stays with the pack
    // constructor.
    template <std::ranges::input_range SourceRange>
        requires(!concepts_detail::is_cljonic_collection_v<SourceRange> &&
                 !concepts_detail::is_cljonic_producer_v<SourceRange> &&
                 !std::same_as<std::remove_cvref_t<SourceRange>, ElementType>)
    explicit constexpr Vector(SourceRange&& source) noexcept(
        (concepts::NothrowRangeConstruction<ElementType, SourceRange>)) {
        static_assert(concepts::NothrowRangeConstruction<ElementType, SourceRange>,
                      "cljonic::Vector: range/view constructor requires each source element to "
                      "convert safely to ElementType and requires non-throwing traversal");
        static_assert(concepts_detail::static_extent_fits_v<SourceRange, CapacityValue>,
                      "cljonic::Vector: static-extent range source exceeds Vector CapacityValue");

        std::size_t copy_count = 0;
        for (auto&& item : std::forward<SourceRange>(source)) {
            if (copy_count >= CapacityValue) {
                break;
            }
            storage_[copy_count++] = value_type(std::forward<decltype(item)>(item));
        }
        logical_size_ = copy_count;
    }

    // Diagnostic fallback (REQ-DIAG-010): a cljonic collection or producer is
    // not a SourceConstruction source. This overload exists only to explain the
    // rejection -- it produces no value and is never a supported call target.
    template <typename SourceValue>
        requires((concepts_detail::is_cljonic_collection_v<SourceValue> ||
                  concepts_detail::is_cljonic_producer_v<SourceValue>) &&
                 !std::same_as<std::remove_cvref_t<SourceValue>, value_type>)
    explicit constexpr Vector([[maybe_unused]] SourceValue&& source) noexcept {
        static_assert(
            concepts_detail::dependent_false<SourceValue>,
            "cljonic::Vector: you cannot construct a Vector from a cljonic collection or producer. A Vector is built "
            "from its individual elements, or from a non-cljonic C++ range or view. To keep it as a single element, "
            "write vector(...).");
    }

    [[nodiscard]] static constexpr auto capacity() noexcept -> std::size_t {
        return CapacityValue;
    }

    [[nodiscard]] constexpr auto count() const noexcept -> std::size_t {
        return logical_size_;
    }

    /** \cond INTERNAL */
    [[nodiscard]] constexpr auto peek() const noexcept -> value_type {
        return logical_size_ == 0U ? value_type{} : storage_[logical_size_ - 1U];
    }

    [[nodiscard]] constexpr auto pop() const noexcept -> Vector {
        Vector result = *this;
        if (result.logical_size_ > 0U) {
            --result.logical_size_;
        }
        return result;
    }
    /** \endcond */

    template <concepts_detail::IntegerIndexType IndexType>
    [[nodiscard]] constexpr auto operator()(IndexType index, const value_type& fallback = value_type{}) const noexcept
        -> value_type {
        const auto normalized_index = concepts_detail::try_normalize_index(index);
        return normalized_index && *normalized_index < logical_size_ ? storage_[*normalized_index] : fallback;
    }

    template <concepts_detail::IntegerIndexType IndexType>
    [[nodiscard]] constexpr auto contains(IndexType index) const noexcept -> bool {
        const auto normalized_index = concepts_detail::try_normalize_index(index);
        return normalized_index && *normalized_index < logical_size_;
    }

    template <concepts_detail::IntegerIndexType IndexType>
    [[nodiscard]] constexpr auto can_assoc(IndexType index, [[maybe_unused]] const value_type& value) const noexcept
        -> bool {
        const auto normalized_index = concepts_detail::try_normalize_index(index);
        return normalized_index && association_index_is_valid(*normalized_index);
    }

    template <concepts_detail::IntegerIndexType IndexType>
    [[nodiscard]] constexpr auto assoc(IndexType index, const value_type& value) const noexcept -> Vector {
        Vector result = *this;
        const auto normalized_index = concepts_detail::try_normalize_index(index);
        if (normalized_index && association_index_is_valid(*normalized_index)) {
            result.storage_[*normalized_index] = value;
            if (*normalized_index == logical_size_) {
                ++result.logical_size_;
            }
        }
        return result;
    }

    /** Returns true when there is room for one more element; the value does not
     *  affect the result. */
    [[nodiscard]] constexpr auto can_conj([[maybe_unused]] const value_type& value) const noexcept -> bool {
        return logical_size_ < CapacityValue;
    }

    /** Appends the value at the end (the highest logical index) of a copy when
     *  capacity remains; returns an unchanged copy when full. */
    [[nodiscard]] constexpr auto conj(const value_type& value) const noexcept -> Vector {
        Vector result = *this;
        if (result.logical_size_ < CapacityValue) {
            result.storage_[result.logical_size_] = value;
            ++result.logical_size_;
        }
        return result;
    }

    [[nodiscard]] constexpr auto operator==(const Vector& other) const noexcept -> bool
        requires concepts::StableEqualityComparable<ElementType>
    {
        if (logical_size_ != other.logical_size_) {
            return false;
        }
        for (std::size_t i = 0; i < logical_size_; ++i) {
            if (!(storage_[i] == other.storage_[i])) {
                return false;
            }
        }
        return true;
    }

    [[nodiscard]] constexpr auto begin() const noexcept -> const value_type* {
        return storage_.data();
    }

    [[nodiscard]] constexpr auto end() const noexcept -> const value_type* {
        return view().subspan(logical_size_).data();
    }

    [[nodiscard]] constexpr auto view() const noexcept -> std::span<const value_type> {
        return {storage_.data(), logical_size_};
    }

    template <typename Index, typename Fallback>
        requires(!concepts::SafeImplicitConversion<value_type, Fallback>)
    auto operator()(Index index, const Fallback& fallback) const -> value_type = delete;

    template <typename Index, typename Value>
        requires(!concepts::SafeImplicitConversion<value_type, Value>)
    auto can_assoc(Index index, const Value& value) const -> bool = delete;

    template <typename Index, typename Value>
        requires(!concepts::SafeImplicitConversion<value_type, Value>)
    auto assoc(Index index, const Value& value) const -> Vector = delete;

    template <typename Value>
        requires(!concepts::SafeImplicitConversion<value_type, Value>)
    auto can_conj(const Value& value) const -> bool = delete;

    template <typename Value>
        requires(!concepts::SafeImplicitConversion<value_type, Value>)
    auto conj(const Value& value) const -> Vector = delete;

  private:
    [[nodiscard]] constexpr auto association_index_is_valid(std::size_t index) const noexcept -> bool {
        return index < logical_size_ || (index == logical_size_ && logical_size_ < CapacityValue);
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
    constexpr void initialize_storage(std::index_sequence<Indices...> indices, Args&&... args) noexcept {
        (void)indices;
        ((storage_[Indices] = ElementType(std::forward<Args>(args))), ...);
    }

    std::array<value_type, CapacityValue> storage_{};
    std::size_t logical_size_ = 0;
};

namespace concepts_detail {

struct DeducedVectorElementType {};

template <typename RequestedElementType, typename... Args>
struct vector_factory_element_type {
    using type = RequestedElementType;
};

template <typename First, typename... Rest>
struct vector_factory_element_type<DeducedVectorElementType, First, Rest...> {
    using type = std::remove_cvref_t<First>;
};

template <typename RequestedElementType, typename... Args>
using vector_factory_element_t = vector_factory_element_type<RequestedElementType, Args...>::type;

template <typename RequestedElementType, typename... Args>
concept VectorFactoryHasElementType =
    sizeof...(Args) > 0U || !std::same_as<RequestedElementType, DeducedVectorElementType>;

template <typename RequestedElementType, typename... Args>
concept VectorFactoryArguments =
    VectorFactoryHasElementType<RequestedElementType, Args...> &&
    concepts::NothrowCollectionElement<vector_factory_element_t<RequestedElementType, Args...>> &&
    sizeof...(Args) <= CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE &&
    (concepts::NothrowElementConstruction<vector_factory_element_t<RequestedElementType, Args...>, Args> && ...);

template <typename ElementType, std::size_t Capacity, typename... Args>
concept VectorFactoryCapacityArguments =
    concepts::NothrowCollectionElement<ElementType> && Capacity <= CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE &&
    sizeof...(Args) <= Capacity && (concepts::NothrowElementConstruction<ElementType, Args> && ...);

} // namespace concepts_detail

/** Creates a bounded Vector from its arguments, or a typed empty Vector when called as \c vector<T>(). */
template <typename RequestedElementType = concepts_detail::DeducedVectorElementType, typename... Args>
    requires concepts_detail::VectorFactoryArguments<RequestedElementType, Args...>
[[nodiscard]] constexpr auto vector(Args&&... args) noexcept(
    (concepts::NothrowElementConstruction<concepts_detail::vector_factory_element_t<RequestedElementType, Args...>,
                                          Args> &&
     ...)) -> Vector<concepts_detail::vector_factory_element_t<RequestedElementType, Args...>, sizeof...(Args)> {
    using ElementType = concepts_detail::vector_factory_element_t<RequestedElementType, Args...>;
    return Vector<ElementType, sizeof...(Args)>{std::forward<Args>(args)...};
}

template <typename RequestedElementType = concepts_detail::DeducedVectorElementType, typename... Args>
    requires(!concepts_detail::VectorFactoryArguments<RequestedElementType, Args...>)
constexpr auto vector([[maybe_unused]] Args&&... args) noexcept -> void {
    using ElementType = concepts_detail::vector_factory_element_t<RequestedElementType, Args...>;
    if constexpr (!concepts_detail::VectorFactoryHasElementType<RequestedElementType, Args...>) {
        static_assert(concepts_detail::dependent_false<RequestedElementType, Args...>,
                      "cljonic::vector: an empty Vector requires an explicit element type; write vector<T>().");
    } else if constexpr (!concepts::NothrowCollectionElement<ElementType>) {
        static_assert(concepts_detail::dependent_false<RequestedElementType, Args...>,
                      "cljonic::vector: the element type must satisfy NothrowCollectionElement.");
    } else if constexpr (sizeof...(Args) > CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE) {
        static_assert(concepts_detail::dependent_false<RequestedElementType, Args...>,
                      "cljonic::vector: argument count exceeds CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT.");
    } else {
        static_assert(concepts_detail::dependent_false<RequestedElementType, Args...>,
                      "cljonic::vector: every argument must safely construct the element type without throwing.");
    }
}

/** Creates a bounded Vector with an explicit capacity, including for an empty argument pack. */
template <typename ElementType, std::size_t Capacity, typename... Args>
    requires concepts_detail::VectorFactoryCapacityArguments<ElementType, Capacity, Args...>
[[nodiscard]] constexpr auto vector(Args&&... args) noexcept((concepts::NothrowElementConstruction<ElementType, Args> &&
                                                              ...)) -> Vector<ElementType, Capacity> {
    return Vector<ElementType, Capacity>{std::forward<Args>(args)...};
}

template <typename ElementType, std::size_t Capacity, typename... Args>
    requires(!concepts_detail::VectorFactoryCapacityArguments<ElementType, Capacity, Args...>)
constexpr auto vector([[maybe_unused]] Args&&... args) noexcept -> void {
    if constexpr (!concepts::NothrowCollectionElement<ElementType>) {
        static_assert(concepts_detail::dependent_false<ElementType, Args...>,
                      "cljonic::vector: the element type must satisfy NothrowCollectionElement.");
    } else if constexpr (Capacity > CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE) {
        static_assert(concepts_detail::dependent_false<ElementType, Args...>,
                      "cljonic::vector: capacity exceeds CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT.");
    } else if constexpr (sizeof...(Args) > Capacity) {
        static_assert(concepts_detail::dependent_false<ElementType, Args...>,
                      "cljonic::vector: capacity must be at least the number of arguments.");
    } else {
        static_assert(concepts_detail::dependent_false<ElementType, Args...>,
                      "cljonic::vector: every argument must safely construct the element type without throwing.");
    }
}

template <typename First, typename... Rest>
Vector(First, Rest...) -> Vector<First, 1 + sizeof...(Rest)>;

// EnclosureConstruction (REQ-FN-027A): a sole Vector argument wraps as one
// element rather than being copied, mirroring a Clojure literal ([v]).
template <typename ElementType, std::size_t CapacityValue>
Vector(Vector<ElementType, CapacityValue>) -> Vector<Vector<ElementType, CapacityValue>, 1>;

template <typename SourceElement, std::size_t Extent>
    requires(Extent != std::dynamic_extent)
Vector(std::span<SourceElement, Extent>) -> Vector<std::remove_cv_t<SourceElement>, Extent>;

} // namespace cljonic

namespace cljonic::concepts_detail {

template <typename ElementType, std::size_t CapacityValue>
struct collection_traits<Vector<ElementType, CapacityValue>> {
    static constexpr bool is_cljonic_collection = true;
    static constexpr collection_kind kind = collection_kind::vector;
};

template <typename ElementType, std::size_t CapacityValue>
struct contains_floating_point<Vector<ElementType, CapacityValue>>
    : std::bool_constant<contains_floating_point_v<ElementType>> {};

template <typename ElementType, std::size_t CapacityValue>
struct contains_callable<Vector<ElementType, CapacityValue>> : std::bool_constant<contains_callable_v<ElementType>> {};

template <typename ElementType, std::size_t CapacityValue>
struct contains_standard_range<Vector<ElementType, CapacityValue>>
    : std::bool_constant<contains_standard_range_v<ElementType>> {};

} // namespace cljonic::concepts_detail
