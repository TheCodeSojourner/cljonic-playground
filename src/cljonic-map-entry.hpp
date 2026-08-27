#pragma once

#include <concepts>
#include <cstddef>

namespace cljonic {

/** \anchor MapEntry
 * \b MapEntry is a value-semantic pair representing a single key-value
 * association.
 */
template <typename KeyType, typename ValueType> struct MapEntry {
  KeyType key{};
  ValueType value{};

  [[nodiscard]] constexpr auto
  operator==(const MapEntry &other) const noexcept -> bool {
    return key == other.key && value == other.value;
  }

  template <std::integral IndexType>
  [[nodiscard]] constexpr auto
  valid_index(IndexType index) const noexcept -> bool {
    if constexpr (std::signed_integral<IndexType>) {
      if (index < 0) {
        return false;
      }
    }
    return static_cast<std::size_t>(index) < 2U;
  }
};

} // namespace cljonic
