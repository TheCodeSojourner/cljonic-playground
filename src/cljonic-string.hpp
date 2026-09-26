#pragma once

#include <array>
#include <cstddef>
#include <ranges>
#include <span>
#include <string_view>

#include <cljonic-concepts.hpp>
#include <cljonic-config.hpp>
#include <cljonic-core-collection-maximum-element-count.hpp>

namespace cljonic {

/** \anchor String
 * \b String is a bounded, ordered collection of ASCII bytes with automatic null termination management. It provides
 * callable lookup with optional fallback values. The way to operate on the collection is through the library's
 * free-function API. Updates return a modified copy without changing the original collection. Construction with
 * content longer than the available capacity is rejected at compile time.
 *
 \b Examples
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 int main() {
   // CTAD infers String<3> from the initializer count.
   [[maybe_unused]] constexpr auto inferred = String{"Hi"};

   // Explicit capacity permits a partially populated String and an empty String.
   constexpr auto literal = String<10>{"Hello"};
   [[maybe_unused]] constexpr auto empty = String<8>{};

   // String values can be used as a callable function, returning a default-
   // value for invalid indexes or a supplied fallback when provided.
   static_assert(literal(0) == 'H');
   static_assert(literal(99, 'Z') == 'Z');

   // Without a fallback, an invalid lookup returns '\0'.
   static_assert(literal(5) == '\0');

   // Runtime construction supports the same callable lookup and fallback
   // behavior.
   auto runtime = String<8>{"Hi"};
   const auto first = runtime(0);
   const auto missing = runtime(9, '!');

   // -------------------------------------------------------------------------
   // C++ interoperability: a String supports const traversal and exposes a
   // non-owning std::string_view. Range/view sources are copied into owned
   // storage, retaining only the bounded prefix that fits the capacity. A
   // string_view's size is not part of its type, so view construction requires
   // an explicit capacity.
   // -------------------------------------------------------------------------
   static constexpr std::string_view static_source{"from view"};
   constexpr auto from_static_view = String<16>{static_source};
   static_assert(from_static_view.view() == static_source);

   const std::string_view runtime_source{"runtime view"};
   const auto from_runtime_view = String<16>{runtime_source};
   const auto runtime_view_copy = from_runtime_view.view();

   // A standard view pipeline can use an existing String as its source and
   // materialize transformed characters into another String.
   const auto uppercase_view = runtime | std::views::transform([](char byte) {
                                 return byte == 'i' ? 'I' : byte;
                               });
   const auto from_pipeline = String<8>{uppercase_view};

   static constexpr char char_source[] = {'s', 'p', 'a', 'n'};
   static constexpr auto span_source{std::span<const char, 4>{char_source}};
   constexpr auto from_char_span = String{span_source};
   static_assert(from_char_span.view() == std::string_view{"span"});

   static_assert(literal.view() == std::string_view{"Hello"});

   // Use C++ interoperability to sum the bytes in a String.
   int byte_sum = 0;
   for (const auto byte : runtime) {
     byte_sum += byte;
   }

   const auto runtime_view = runtime.view();

   return (first == 'H' && missing == '!' && byte_sum == 'H' + 'i' &&
           runtime_view == std::string_view{"Hi"} &&
           runtime_view_copy == runtime_source &&
           from_pipeline.view() == std::string_view{"HI"})
              ? 0
              : 1;
 }
 ~~~~~
 */
template <std::size_t CapacityValue>
class String {
  public:
    using key_type = std::size_t;
    using lookup_type = key_type;
    using association_value_type = char;
    using value_type = char;

    static_assert(
        CapacityValue <= cljonic::CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE,
        "String CapacityValue exceeds "
        "CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT=" CLJONIC_STRINGIFY(CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT));

    constexpr String() noexcept {
        data_[0] = '\0';
    }

    /** Construct from a character array literal. CapacityValue counts content
     * characters; the null terminator is managed separately. */
    template <std::size_t N>
    constexpr String(const char (&arr)[N]) noexcept {
        static_assert(N - 1U <= CapacityValue, "String literal too long for capacity");
        copy_from_source(arr, N - 1U);
    }

    constexpr String(std::string_view source) noexcept {
        if consteval {
            if (source.size() > CapacityValue) {
                rejected_oversized_string_source_at_compile_time();
            }
        }
        copy_from_source(source, std::min<std::size_t>(source.size(), CapacityValue));
    }

    template <std::ranges::input_range SourceRange>
        requires(!std::same_as<std::remove_cvref_t<SourceRange>, String> &&
                 std::same_as<std::remove_cv_t<std::ranges::range_value_t<SourceRange>>, char>)
    constexpr String(SourceRange&& source) noexcept {
        static_assert(concepts_detail::static_extent_fits_v<SourceRange, CapacityValue>,
                      "String static-extent range source exceeds String CapacityValue");

        std::size_t copy_count = 0;
        for (auto&& byte : std::forward<SourceRange>(source)) {
            if (copy_count >= CapacityValue) {
                break;
            }
            data_[copy_count++] = normalize_byte(static_cast<char>(byte));
        }
        logical_size_ = copy_count;
        data_[logical_size_] = '\0';
    }

    [[nodiscard]] static constexpr auto capacity() noexcept -> std::size_t {
        return CapacityValue;
    }

    [[nodiscard]] constexpr auto count() const noexcept -> std::size_t {
        return logical_size_;
    }

    [[nodiscard]] constexpr auto is_empty() const noexcept -> bool {
        return logical_size_ == 0U;
    }

    [[nodiscard]] constexpr auto operator==(const String& other) const noexcept -> bool {
        if (logical_size_ != other.logical_size_) {
            return false;
        }
        for (std::size_t i = 0; i < logical_size_; ++i) {
            if (data_[i] != other.data_[i]) {
                return false;
            }
        }
        return true;
    }

    [[nodiscard]] constexpr auto begin() const noexcept -> const value_type* {
        return data_.data();
    }

    [[nodiscard]] constexpr auto end() const noexcept -> const value_type* {
        return data_.data() + logical_size_;
    }

    [[nodiscard]] constexpr auto view() const noexcept -> std::string_view {
        return {data_.data(), logical_size_};
    }

    /** Returns true when index falls within logical bounds (not counting null
     * terminator). Mirrors Clojure contains? over string indices. */
    template <std::integral IndexType>
    [[nodiscard]] constexpr auto contains(IndexType index) const noexcept -> bool {
        const auto normalized_index = concepts_detail::try_normalize_index(index);
        return normalized_index && *normalized_index < logical_size_;
    }

    /** Callable index access with custom fallback value on invalid index. */
    template <std::integral IndexType>
    [[nodiscard]] constexpr auto operator()(IndexType index, char fallback = '\0') const noexcept -> char {
        const auto normalized_index = concepts_detail::try_normalize_index(index);
        return normalized_index && *normalized_index < logical_size_ ? data_[*normalized_index] : fallback;
    }

    template <std::integral IndexType>
    [[nodiscard]] constexpr auto can_assoc(IndexType index) const noexcept -> bool {
        const auto normalized_index = concepts_detail::try_normalize_index(index);
        return normalized_index && association_index_is_valid(*normalized_index);
    }

    template <std::integral IndexType>
    [[nodiscard]] constexpr auto assoc(IndexType index, char value) const noexcept -> String {
        String result = *this;
        const auto normalized_index = concepts_detail::try_normalize_index(index);
        if (normalized_index && association_index_is_valid(*normalized_index)) {
            result.data_[*normalized_index] = normalize_byte(value);
            if (*normalized_index == logical_size_) {
                ++result.logical_size_;
                result.data_[result.logical_size_] = '\0';
            }
        }
        return result;
    }

    /** Returns a fresh String with the character at index replaced. Out-of-bounds
     * indices leave copy unchanged. */
    template <std::integral IndexType>
    [[nodiscard]] constexpr auto put(IndexType index, char c) const noexcept -> String {
        String result = *this;
        const auto normalized_index = concepts_detail::try_normalize_index(index);
        if (normalized_index && *normalized_index < logical_size_) {
            result.data_[*normalized_index] = c;
        }
        return result;
    }

  private:
    [[nodiscard]] constexpr auto association_index_is_valid(std::size_t index) const noexcept -> bool {
        return index < logical_size_ || (index == logical_size_ && logical_size_ < CapacityValue);
    }

    template <typename Source>
    constexpr void copy_from_source(const Source& source, std::size_t copy_count) noexcept {
        logical_size_ = copy_count;
        for (std::size_t i = 0; i < logical_size_; ++i) {
            data_[i] = normalize_byte(source[i]);
        }
        data_[logical_size_] = '\0';
    }

    [[nodiscard]] static constexpr auto normalize_byte(char byte) noexcept -> char {
        if (byte == '\0' || static_cast<unsigned char>(byte) > 0x7FU) {
            if consteval {
                rejected_invalid_string_byte_at_compile_time();
            }
            return '.';
        }
        return byte;
    }

    static void rejected_invalid_string_byte_at_compile_time() noexcept;
    static void rejected_oversized_string_source_at_compile_time() noexcept;

    // Internal array is CapacityValue+1 to hold null terminator
    std::array<char, CapacityValue + 1> data_{};
    std::size_t logical_size_{0};
};

template <std::size_t N>
String(const char (&)[N]) -> String<N - 1U>;

template <typename SourceElement, std::size_t Extent>
    requires(Extent != std::dynamic_extent)
String(std::span<SourceElement, Extent>) -> String<Extent>;

} // namespace cljonic

namespace cljonic::concepts_detail {

template <std::size_t CapacityValue>
struct collection_traits<String<CapacityValue>> {
    static constexpr bool is_cljonic_collection = true;
    static constexpr collection_kind kind = collection_kind::string;
};

template <std::size_t CapacityValue>
struct contains_floating_point<String<CapacityValue>> : std::false_type {};

template <std::size_t CapacityValue>
struct contains_callable<String<CapacityValue>> : std::false_type {};

} // namespace cljonic::concepts_detail
