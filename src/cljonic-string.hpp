#pragma once

#include <array>
#include <cstddef>

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

 int main() {
   using namespace cljonic;

   // Explicit capacity permits an empty String and a partially populated String.
   [[maybe_unused]] constexpr auto empty = String<8>{};
   constexpr auto literal = String<8>{"Hello"};
   static_assert(literal(0) == 'H');
   static_assert(literal(99, 'Z') == 'Z');

   // Out-of-bounds access returns char{} (the ASCII NUL character).
   static_assert(literal(5) == '\0');

   // Capacity-inferred construction is also supported.
   [[maybe_unused]] constexpr auto inferred = String{"Hi"};

   // Runtime construction supports the same callable lookup and fallback
   // behavior.
   auto runtime = String<8>{"Hi"};
   const auto first = runtime(0);
   const auto missing = runtime(9, '!');

   return (first == 'H' && missing == '!') ? 0 : 1;
 }
 ~~~~~
 */
template <std::size_t CapacityValue>
class String {
  public:
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
        logical_size_ = N - 1U;
        for (std::size_t i = 0; i < logical_size_; ++i) {
            data_[i] = normalize_byte(arr[i]);
        }
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

    /** Returns true when index falls within logical bounds (not counting null
     * terminator). Mirrors Clojure contains? over string indices. */
    [[nodiscard]] constexpr auto contains(std::size_t index) const noexcept -> bool {
        return index < logical_size_;
    }

    /** Callable index access returning default-constructed char ('\\0') on
     * invalid index. */
    [[nodiscard]] constexpr auto operator()(std::size_t index) const noexcept -> char {
        return (index < logical_size_) ? data_[index] : '\0';
    }

    /** Callable index access with custom fallback value on invalid index. */
    [[nodiscard]] constexpr auto operator()(std::size_t index, char fallback) const noexcept -> char {
        return (index < logical_size_) ? data_[index] : fallback;
    }

    /** Returns a fresh String with the character at index replaced. Out-of-bounds
     * indices leave copy unchanged. */
    [[nodiscard]] constexpr auto put(std::size_t index, char c) const noexcept -> String {
        String result = *this;
        if (index < logical_size_) {
            result.data_[index] = c;
        }
        return result;
    }

  private:
    [[nodiscard]] static constexpr auto normalize_byte(char byte) noexcept -> char {
        if (byte == '\0' || static_cast<unsigned char>(byte) > 0x7FU) {
            if consteval {
                reject_invalid_literal_byte();
            }
            return '.';
        }
        return byte;
    }

    static void reject_invalid_literal_byte() noexcept;

    // Internal array is CapacityValue+1 to hold null terminator
    std::array<char, CapacityValue + 1> data_{};
    std::size_t logical_size_{0};
};

template <std::size_t N>
String(const char (&)[N]) -> String<N - 1U>;

} // namespace cljonic

namespace cljonic::concepts_detail {

template <std::size_t CapacityValue>
struct collection_traits<String<CapacityValue>> {
    static constexpr bool is_cljonic_collection = true;
    static constexpr collection_kind kind = collection_kind::string;
};

} // namespace cljonic::concepts_detail
