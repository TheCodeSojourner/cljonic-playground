#pragma once

#include <array>
#include <concepts>
#include <cstddef>
#include <utility>

#include <cljonic-concepts.hpp>
#include <cljonic-config.hpp>
#include <cljonic-core-collection-maximum-element-count.hpp>

namespace cljonic
{

  /** \anchor String
   * \b String is a fixed-capacity ASCII string backed by contiguous array storage
   * with automatic null termination and copy-on-modify semantics.
   *
   * \b Examples
   * ~~~~~{.cpp}
   * #include <cljonic.hpp>
   *
   * int main() {
   *   using namespace cljonic;
   *
   *   // Compile-time demonstration.
   *   constexpr String<8> s_const{"Hello"};
   *   static_assert(s_const.size() == 5U);
   *   static_assert(s_const[0] == 'H');
   *   static_assert(s_const(0) == 'H');
   *   static_assert(s_const(99, 'Z') == 'Z');
   *   static_assert(s_const.valid(0));
   *
   *   // Runtime demonstration.
   *   auto s_runtime = String<8>{"Hi"};
   *   auto s1 = s_runtime.append('!');
   *   auto s2 = s1.put(0, 'h');
   *
   *   return (s1.size() == 3U && s2[0] == 'h') ? 0 : 1;
   * }
   * ~~~~~
   */
  template <std::size_t CapacityValue>
  class String
  {
  public:
    using value_type = char;

    static_assert(
        CapacityValue <= cljonic::CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE,
        "String capacity exceeds CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT");

    constexpr String() noexcept : data_{}, logical_size_{0} { data_[0] = '\0'; }

    /** Construct from character array literal. Capacity must accommodate N-1
     * chars plus null terminator. */
    template <std::size_t N>
    constexpr String(const char (&arr)[N]) noexcept : data_{}, logical_size_{0}
    {
      static_assert(N - 1U <= CapacityValue,
                    "String literal too long for capacity");
      logical_size_ = N - 1U;
      for (std::size_t i = 0; i < logical_size_; ++i)
      {
        data_[i] = arr[i];
      }
      data_[logical_size_] = '\0';
    }

    [[nodiscard]] static constexpr auto capacity() noexcept -> std::size_t
    {
      return CapacityValue;
    }

    [[nodiscard]] constexpr auto size() const noexcept -> std::size_t
    {
      return logical_size_;
    }

    [[nodiscard]] constexpr auto empty() const noexcept -> bool
    {
      return logical_size_ == 0U;
    }

    /** Returns true when index falls within logical bounds (not counting null
     * terminator). */
    [[nodiscard]] constexpr auto valid(std::size_t index) const noexcept -> bool
    {
      return index < logical_size_;
    }

    /** Index access: returns '\\0' for out-of-bounds indices. */
    [[nodiscard]] constexpr auto
    operator[](std::size_t index) const noexcept -> char
    {
      return (index < logical_size_) ? data_[index] : '\0';
    }

    /** Callable index access returning default-constructed char ('\\0') on
     * invalid index. */
    [[nodiscard]] constexpr auto
    operator()(std::size_t index) const noexcept -> char
    {
      return (index < logical_size_) ? data_[index] : '\0';
    }

    /** Callable index access with custom fallback value on invalid index. */
    [[nodiscard]] constexpr auto
    operator()(std::size_t index, char fallback) const noexcept -> char
    {
      return (index < logical_size_) ? data_[index] : fallback;
    }

    /** Returns a fresh String with the character at index replaced. Out-of-bounds
     * indices leave copy unchanged. */
    [[nodiscard]] constexpr auto put(std::size_t index,
                                     char c) const noexcept -> String
    {
      String result = *this;
      if (index < logical_size_)
      {
        result.data_[index] = c;
      }
      return result;
    }

    /** Appends a character to the end of a copy of the string if room is
     * available. */
    [[nodiscard]] constexpr auto append(char c) const noexcept -> String
    {
      String result = *this;
      if (result.logical_size_ < CapacityValue)
      {
        result.data_[result.logical_size_] = c;
        ++result.logical_size_;
        result.data_[result.logical_size_] = '\0';
      }
      return result;
    }

  private:
    // Internal array is CapacityValue+1 to hold null terminator
    std::array<char, CapacityValue + 1> data_{};
    std::size_t logical_size_{0};
  };

} // namespace cljonic
