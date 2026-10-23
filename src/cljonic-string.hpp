#pragma once

#include <array>
#include <cstddef>
#include <ranges>
#include <span>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

#include <cljonic-concepts.hpp>
#include <cljonic-config.hpp>
#include <cljonic-core-collection-maximum-element-count.hpp>

namespace cljonic {

/** \anchor String
 * A String is a bounded, ordered collection of ASCII bytes with automatic null termination management. It provides
 * callable lookup with optional fallback values.
 *
 * The \c string factory is the idiomatic way to create String values at runtime or during constant evaluation. Use \c
 * string("Hi") to deduce the capacity from a string literal, \c string<Capacity>(...) to provide an explicit capacity
 * (admitting a string literal, a \c std::string_view, or a non-cljonic char range or view), or \c string<Capacity>()
 * for an empty String. \c String<Capacity> remains the value's type. Direct construction from a span deducing its
 * capacity remains supported.
 *
 * NUL and characters whose unsigned byte value exceeds `0x7F` are rejected at compile time and replaced with `.` at
 * runtime. Construction with content longer than the available capacity is rejected at compile time.
 *
 * Character arguments must be \c char; numeric values do not implicitly become characters. Text imports follow the
 * \ref implicit-conversions "safe implicit conversion rules" and retain character validation.
 *
 \b Examples
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 int main() {
   // The factory infers String<2> from the string literal: capacity counts
   // content characters, excluding the automatic null terminator. The inferred
   // capacity is observed through the callable lookup form.
   [[maybe_unused]] constexpr auto inferred = string("Hi");
   static_assert(
       std::same_as<std::remove_cvref_t<decltype(inferred)>, String<2>>);
   static_assert(inferred(1) == 'i');

   // Explicit capacity permits a partially populated String and an empty String.
   constexpr auto literal = string<10>("Hello");
   [[maybe_unused]] constexpr auto empty = string<8>();

   // String values can be used as a callable function, returning a default-
   // value for invalid indexes or a supplied fallback when provided.
   static_assert(literal(0) == 'H');
   static_assert(literal(99, 'Z') == 'Z');

   // Without a fallback, an invalid lookup returns '\0'.
   static_assert(literal(5) == '\0');

   // Runtime construction supports the same callable lookup and fallback
   // behavior.
   auto runtime = string("Hi");
   const auto first = runtime(0);
   const auto missing = runtime(9, '!');

   // -------------------------------------------------------------------------
   // C++ interoperability: a String supports const traversal and exposes a
   // non-owning std::string_view. Range/view sources are copied into owned
   // storage, retaining only the bounded prefix that fits the capacity. A
   // string_view's size is not part of its type, so view construction requires
   // an explicit capacity.
   // -------------------------------------------------------------------------
   static constexpr auto static_source = std::string_view{"from view"};
   constexpr auto from_static_view = string<16>(static_source);
   static_assert(from_static_view.view() == static_source);

   const auto runtime_source = std::string_view{"runtime view"};
   const auto from_runtime_view = string<16>(runtime_source);
   const auto runtime_view_copy = from_runtime_view.view();

   // A standard view pipeline can use an existing String as its source and
   // Construct another String from the transformed characters.
   const auto uppercase_view =
       runtime | std::views::transform([](char byte) noexcept {
         return byte == 'i' ? 'I' : byte;
       });
   const auto from_pipeline = string<8>(uppercase_view);

   static constexpr char char_source[] = {'s', 'p', 'a', 'n'};
   static constexpr auto span_source = std::span{char_source};
   constexpr auto from_char_span = string<4>(span_source);
   static_assert(from_char_span.view() == std::string_view{"span"});

   static_assert(literal.view() == std::string_view{"Hello"});

   // Use C++ interoperability to sum the bytes in a String.
   auto byte_sum = 0;
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
        "cljonic::String: CapacityValue exceeds "
        "CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT=" CLJONIC_STRINGIFY(CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT));

    constexpr String() noexcept {
        data_[0] = '\0';
    }

    /** Construct from a character array literal. CapacityValue counts content
     * characters; the null terminator is managed separately. */
    template <std::size_t N>
    explicit constexpr String(const char (&arr)[N]) noexcept {
        static_assert(N - 1U <= CapacityValue, "cljonic::String: literal too long for capacity");
        copy_from_source(arr, N - 1U);
    }

    template <typename Source>
        requires std::same_as<std::remove_cvref_t<Source>, std::string_view>
    explicit constexpr String(const Source& source) noexcept {
        if consteval {
            if (source.size() > CapacityValue) {
                rejected_oversized_string_source_at_compile_time();
            }
        }
        copy_from_source(source, std::min<std::size_t>(source.size(), CapacityValue));
    }

    // Copy non-cljonic character ranges into owned storage, retaining the
    // bounded prefix for dynamic extents; statically known extents must fit.
    // A cljonic collection or producer is not a SourceConstruction source
    // (REQ-FN-027A); materialize it via into, preflighted by fits_into.
    template <std::ranges::input_range SourceRange>
        requires(!concepts_detail::is_cljonic_collection_v<SourceRange> &&
                 !concepts_detail::is_cljonic_producer_v<SourceRange> &&
                 !std::same_as<std::remove_cvref_t<SourceRange>, std::string_view> &&
                 std::same_as<std::remove_cvref_t<std::ranges::range_reference_t<SourceRange>>, char>)
    explicit constexpr String(SourceRange&& source) noexcept {
        static_assert(concepts::NothrowRangeConstruction<char, SourceRange>,
                      "cljonic::String: range/view constructor requires char elements and non-throwing traversal");
        static_assert(concepts_detail::static_extent_fits_v<SourceRange, CapacityValue>,
                      "cljonic::String: static-extent range source exceeds String CapacityValue");

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

    // Diagnostic fallback (REQ-DIAG-010): a cljonic collection or producer is
    // not a SourceConstruction source. This overload exists only to explain the
    // rejection -- it produces no value and is never a supported call target.
    template <typename SourceValue>
        requires((concepts_detail::is_cljonic_collection_v<SourceValue> ||
                  concepts_detail::is_cljonic_producer_v<SourceValue>) &&
                 !std::same_as<std::remove_cvref_t<SourceValue>, value_type>)
    explicit constexpr String([[maybe_unused]] SourceValue&& source) noexcept {
        static_assert(concepts_detail::dependent_false<SourceValue>,
                      "cljonic::String: you cannot construct a String from a cljonic collection or producer. A String "
                      "is built from its characters, or from a non-cljonic C++ range or view.");
    }

    [[nodiscard]] static constexpr auto capacity() noexcept -> std::size_t {
        return CapacityValue;
    }

    [[nodiscard]] constexpr auto count() const noexcept -> std::size_t {
        return logical_size_;
    }

    /** Returns whether the character is non-NUL ASCII. */
    [[nodiscard]] static constexpr auto character_is_valid(char value) noexcept -> bool {
        return value != '\0' && static_cast<unsigned char>(value) <= 0x7FU;
    }

    template <typename Value>
        requires(!std::same_as<std::remove_cvref_t<Value>, char>)
    static auto character_is_valid(Value value) -> bool = delete;

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
    template <concepts_detail::IntegerIndexType IndexType>
    [[nodiscard]] constexpr auto contains(IndexType index) const noexcept -> bool {
        const auto normalized_index = concepts_detail::try_normalize_index(index);
        return normalized_index && *normalized_index < logical_size_;
    }

    /** Callable index access with custom fallback value on invalid index. */
    template <concepts_detail::IntegerIndexType IndexType>
    [[nodiscard]] constexpr auto operator()(IndexType index, char fallback = '\0') const noexcept -> char {
        const auto normalized_index = concepts_detail::try_normalize_index(index);
        return normalized_index && *normalized_index < logical_size_ ? data_[*normalized_index] : fallback;
    }

    template <concepts_detail::IntegerIndexType IndexType>
    [[nodiscard]] constexpr auto can_assoc(IndexType index, char value) const noexcept -> bool {
        const auto normalized_index = concepts_detail::try_normalize_index(index);
        return normalized_index && association_index_is_valid(*normalized_index) && character_is_valid(value);
    }

    template <concepts_detail::IntegerIndexType IndexType>
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

    [[nodiscard]] constexpr auto can_conj(char value) const noexcept -> bool {
        return logical_size_ < CapacityValue && character_is_valid(value);
    }

    [[nodiscard]] constexpr auto conj(char value) const noexcept -> String {
        String result = *this;
        if (result.logical_size_ < CapacityValue) {
            result.data_[result.logical_size_] = normalize_byte(value);
            ++result.logical_size_;
            result.data_[result.logical_size_] = '\0';
        }
        return result;
    }

    /** Returns a fresh String with the character at index replaced. Out-of-bounds
     * indices leave copy unchanged. */
    template <concepts_detail::IntegerIndexType IndexType>
    [[nodiscard]] constexpr auto put(IndexType index, char c) const noexcept -> String {
        String result = *this;
        const auto normalized_index = concepts_detail::try_normalize_index(index);
        if (normalized_index && *normalized_index < logical_size_) {
            result.data_[*normalized_index] = c;
        }
        return result;
    }

    template <typename Index, typename Fallback>
        requires(!concepts::SafeImplicitConversion<char, Fallback>)
    auto operator()(Index index, Fallback fallback) const -> char = delete;

    template <typename Index, typename Value>
        requires(!concepts::SafeImplicitConversion<char, Value>)
    auto can_assoc(Index index, Value value) const -> bool = delete;

    template <typename Index, typename Value>
        requires(!concepts::SafeImplicitConversion<char, Value>)
    auto assoc(Index index, Value value) const -> String = delete;

    template <typename Index, typename Value>
        requires(!concepts::SafeImplicitConversion<char, Value>)
    auto put(Index index, Value value) const -> String = delete;

    template <typename Value>
        requires(!concepts::SafeImplicitConversion<char, Value>)
    auto can_conj(Value value) const -> bool = delete;

    template <typename Value>
        requires(!concepts::SafeImplicitConversion<char, Value>)
    auto conj(Value value) const -> String = delete;

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
        if (!character_is_valid(byte)) {
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

namespace concepts_detail {

template <typename Source>
concept StringFactoryTextSource =
    std::same_as<std::remove_cvref_t<Source>, std::string_view> ||
    (std::ranges::input_range<Source> && !is_cljonic_collection_v<std::remove_cvref_t<Source>> &&
     !is_cljonic_producer_v<std::remove_cvref_t<Source>> &&
     std::same_as<std::remove_cvref_t<std::ranges::range_reference_t<Source>>, char>);

template <typename... Args>
concept StringFactoryArguments = sizeof...(Args) == 1 && (StringFactoryTextSource<Args> && ...);

template <std::size_t Capacity, typename... Args>
concept StringFactoryCapacityArguments = Capacity <= CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE &&
                                         sizeof...(Args) <= 1U && (StringFactoryTextSource<Args> && ...);

template <typename Source, typename... Args>
constexpr auto string_factory_reject_text_source() noexcept -> void {
    if constexpr (is_cljonic_collection_v<Source> || is_cljonic_producer_v<Source>) {
        static_assert(dependent_false<Args...>,
                      "cljonic::string: a cljonic collection or producer is not a text source. A String is built "
                      "from a string literal, or from a non-cljonic C++ range or view with an explicit capacity.");
    } else if constexpr (std::same_as<Source, std::string_view>) {
        static_assert(dependent_false<Args...>,
                      "cljonic::string: a string_view's size is not part of its type; provide an explicit "
                      "capacity: string<Capacity>(view).");
    } else {
        static_assert(dependent_false<Args...>,
                      "cljonic::string: the argument must be a string literal, or with an explicit capacity a "
                      "string_view or non-cljonic char range or view.");
    }
}

template <std::size_t Capacity, typename... Args>
constexpr auto string_factory_reject_capacity_call() noexcept -> void {
    if constexpr (Capacity > CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE) {
        static_assert(dependent_false<Args...>,
                      "cljonic::string: capacity exceeds CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT.");
    } else {
        static_assert(dependent_false<Args...>,
                      "cljonic::string: the argument must be a string literal, or with an explicit capacity a "
                      "string_view or non-cljonic char range or view.");
    }
}

template <typename... Args>
constexpr auto string_factory_reject_deduced_call() noexcept -> void {
    using Source = std::remove_cvref_t<std::tuple_element_t<0, std::tuple<Args...>>>;
    if constexpr (sizeof...(Args) > 1U) {
        static_assert(dependent_false<Args...>,
                      "cljonic::string: the string factory takes a single text source; individual characters are "
                      "not admitted.");
    } else {
        string_factory_reject_text_source<Source, Args...>();
    }
}

template <typename... Args>
constexpr auto string_factory_reject_empty_call() noexcept -> void {
    static_assert(dependent_false<Args...>,
                  "cljonic::string: an empty String requires an explicit capacity; write string<Capacity>().");
}

template <typename Array>
inline constexpr bool string_factory_literal_exceeds_maximum_v =
    std::is_array_v<Array> && (std::extent_v<Array> > CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE + 1U);

template <typename Source>
inline constexpr bool string_factory_literal_v =
    std::is_array_v<std::remove_cvref_t<Source>> &&
    std::same_as<std::remove_cv_t<std::remove_extent_t<std::remove_cvref_t<Source>>>, char>;

template <typename... Args>
concept StringFactoryDeducedArguments = sizeof...(Args) == 1 && (string_factory_literal_v<Args> && ...) &&
                                        (!string_factory_literal_exceeds_maximum_v<std::remove_cvref_t<Args>> && ...);

} // namespace concepts_detail

template <typename Literal>
    requires(concepts_detail::string_factory_literal_v<Literal> &&
             !concepts_detail::string_factory_literal_exceeds_maximum_v<std::remove_cvref_t<Literal>>)
[[nodiscard]] constexpr auto string(Literal&& literal) noexcept
    -> String<std::extent_v<std::remove_cvref_t<Literal>> - 1U> {
    return String<std::extent_v<std::remove_cvref_t<Literal>> - 1U>{std::forward<Literal>(literal)};
}

/** Creates an empty bounded String from an explicit capacity. */
template <std::size_t Capacity>
    requires(Capacity <= CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE)
[[nodiscard]] constexpr auto string() noexcept -> String<Capacity> {
    return String<Capacity>{};
}

/** \anchor StringFactory
 * Creates a bounded \ref String "String" from one text source, or an empty String from an explicit capacity.
 *
 * \c string("Hi") deduces the capacity from the string literal's content length, excluding the automatic null
 * terminator. \c string<Capacity>(...) names the capacity: it admits a string literal, a \c std::string_view, or a
 * non-cljonic char range or view, copying the bounded prefix for dynamic extents and rejecting oversized static
 * extents at compile time; \c string<Capacity>() creates an empty String, including zero capacity. An untyped empty
 * call and a variadic individual-character form are rejected at compile time.
 *
 \b Examples
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 int main() {
   // The factory deduces the capacity from the literal's content length.
   constexpr auto deduced = string("Hi");
   static_assert(std::same_as<decltype(deduced), const String<2>>);
   static_assert(deduced(1) == 'i');

   // Explicit capacity admits a partially populated String, an empty String,
   // and text import from a string_view.
   constexpr auto reserved = string<8>("Hello");
   constexpr auto empty = string<8>();
   static constexpr auto view_source = std::string_view{"from view"};
   constexpr auto imported = string<16>(view_source);

   // Runtime construction supports the same observable results.
   const auto runtime = string("Hi");
   const auto runtime_import = string<16>(std::string_view{"runtime"});

   return (deduced(0) == 'H' && reserved.count() == 5U && empty.count() == 0U &&
           imported.view() == "from view" && runtime(1) == 'i' &&
           runtime_import.view() == "runtime")
              ? 0
              : 1;
 }
 ~~~~~
 */
template <std::size_t Capacity, typename Source>
    requires(concepts_detail::StringFactoryCapacityArguments<Capacity, Source>)
[[nodiscard]] constexpr auto string(Source&& source) noexcept -> String<Capacity> {
    return String<Capacity>{std::forward<Source>(source)};
}

template <std::size_t Capacity, typename... Args>
    requires(!concepts_detail::StringFactoryCapacityArguments<Capacity, Args...>)
constexpr auto string([[maybe_unused]] Args&&... args) noexcept -> void {
    if constexpr (Capacity > CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE || sizeof...(Args) == 0U) {
        concepts_detail::string_factory_reject_capacity_call<Capacity, Args...>();
    } else {
        using Source = std::remove_cvref_t<std::tuple_element_t<0, std::tuple<Args...>>>;
        if constexpr (sizeof...(Args) > 1U) {
            static_assert(concepts_detail::dependent_false<Args...>,
                          "cljonic::string: the string factory takes a single text source; individual characters are "
                          "not admitted.");
        } else {
            concepts_detail::string_factory_reject_text_source<Source, Args...>();
        }
    }
}

template <typename... Args>
    requires(!concepts_detail::StringFactoryDeducedArguments<Args...>)
constexpr auto string([[maybe_unused]] Args&&... args) noexcept -> void {
    if constexpr (sizeof...(Args) == 0U) {
        concepts_detail::string_factory_reject_empty_call<Args...>();
    } else if constexpr (sizeof...(Args) > 1U) {
        static_assert(concepts_detail::dependent_false<Args...>,
                      "cljonic::string: the string factory takes a single text source; individual characters are "
                      "not admitted.");
    } else {
        using Source = std::remove_cvref_t<std::tuple_element_t<0, std::tuple<Args...>>>;
        if constexpr (concepts_detail::string_factory_literal_exceeds_maximum_v<Source>) {
            static_assert(concepts_detail::dependent_false<Args...>,
                          "cljonic::string: literal content length exceeds CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT.");
        } else {
            concepts_detail::string_factory_reject_text_source<Source, Args...>();
        }
    }
}

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

template <std::size_t CapacityValue>
struct contains_standard_range<String<CapacityValue>> : std::false_type {};

} // namespace cljonic::concepts_detail
