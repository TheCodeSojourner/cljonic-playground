#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <utility>

#include <cljonic-string.hpp>

namespace cljonic {

namespace concepts_detail {

template <typename T>
concept CharacterValidityInput =
    std::is_integral_v<std::remove_cvref_t<T>> && !std::is_same_v<std::remove_cvref_t<T>, bool>;

template <typename T>
concept CharacterStringInput = std::is_array_v<std::remove_reference_t<T>> &&
                               std::is_same_v<std::remove_cv_t<std::remove_extent_t<std::remove_reference_t<T>>>, char>;

template <typename T>
[[nodiscard]] constexpr auto signed_value_fits_char(T value) noexcept -> bool {
    if constexpr (std::is_signed_v<char>) {
        return static_cast<std::intmax_t>(value) >= std::numeric_limits<char>::min() &&
               static_cast<std::intmax_t>(value) <= std::numeric_limits<char>::max();
    } else {
        return value >= 0 && static_cast<std::uintmax_t>(value) <= std::numeric_limits<char>::max();
    }
}

template <typename T>
[[nodiscard]] constexpr auto unsigned_value_fits_char(T value) noexcept -> bool {
    return static_cast<std::uintmax_t>(value) <= static_cast<std::uintmax_t>(std::numeric_limits<char>::max());
}

template <typename T>
[[nodiscard]] constexpr auto value_fits_char(T value) noexcept -> bool {
    if constexpr (std::is_signed_v<T>) {
        return signed_value_fits_char(value);
    } else {
        return unsigned_value_fits_char(value);
    }
}

} // namespace concepts_detail

/** \anchor CharacterIsValid
 * \brief Checks whether a value converts to a valid String character.
 *
 * Accepts non-`bool` integral values and one-character C-strings. Values outside the range representable by `char`
 * return false without conversion; otherwise the character is valid when it is not NUL and its unsigned byte value is
 * no greater than `0x7F`. Other input types are rejected at compile time with a targeted diagnostic.
 *
 * \b Examples
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 int main() {
   static_assert(character_is_valid('A'));
   static_assert(character_is_valid(65));
   static_assert(character_is_valid("x"));
   static_assert(!character_is_valid(300));
   static_assert(!character_is_valid('\0'));
   static_assert(!character_is_valid(static_cast<char>(0x80)));
   return 0;
 }
 ~~~~~
 */
template <typename T>
    requires concepts_detail::CharacterValidityInput<T>
[[nodiscard]] constexpr auto character_is_valid(T value) noexcept -> bool {
    if (!concepts_detail::value_fits_char(value)) {
        return false;
    }
    return String<0>::character_is_valid(static_cast<char>(value));
}

template <std::size_t N>
[[nodiscard]] constexpr auto character_is_valid(const char (&value)[N]) noexcept -> bool {
    static_assert(N == 2U, "cljonic::character_is_valid: a c-string argument must be a one-character c-string, such as "
                           "\"x\"; the empty string and multi-character strings are not supported.");
    if constexpr (N == 2U) {
        return value[1U] == '\0' && String<1>::character_is_valid(value[0U]);
    }
    return false;
}

template <typename T>
    requires(!concepts_detail::CharacterValidityInput<T> && !concepts_detail::CharacterStringInput<T>)
[[nodiscard]] constexpr auto character_is_valid([[maybe_unused]] T&& value) noexcept -> bool {
    static_assert(concepts_detail::dependent_false<T>,
                  "cljonic::character_is_valid: the value must be an integer character code or a one-character "
                  "c-string, not a bool, a floating-point number, an enumeration, or another type.");
    return false;
}

} // namespace cljonic
