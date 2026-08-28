#pragma once

#include <concepts>
#include <cstddef>

namespace cljonic {

/** \anchor MapEntry
 * \b MapEntry is a value-semantic pair representing a single key-value
 * association.
 *
 * \b Examples
 * ~~~~~{.cpp}
 * #include <cljonic.hpp>
 *
 * int main() {
 *   using namespace cljonic;
 *
 *   // Compile-time demonstration.
 *   constexpr MapEntry<int, int> e_const{1, 100};
 *   static_assert(e_const.key == 1);
 *   static_assert(e_const.value == 100);
 *   static_assert(e_const.valid_index(0));
 *   static_assert(e_const.valid_index(1));
 *   static_assert(!e_const.valid_index(2));
 *
 *   // Runtime demonstration.
 *   auto e_runtime = MapEntry<int, int>{2, 200};
 *   const auto k = e_runtime.key;
 *   const auto v = e_runtime.value;
 *   const auto ok = e_runtime.valid_index(0);
 *
 *   return (k == 2 && v == 200 && ok) ? 0 : 1;
 * }
 * ~~~~~
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
