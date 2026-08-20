#pragma once

#include <concepts>
#include <cstddef>

#include <cljonic-vector.hpp>

namespace cljonic {

template <typename element_type, std::size_t capacity_value,
          std::integral index_type>
[[nodiscard]] constexpr auto
valid_index(const Vector<element_type, capacity_value> &vector,
            index_type index) noexcept -> bool {
  if constexpr (std::signed_integral<index_type>) {
    if (index < 0) {
      return false;
    }
  }

  return static_cast<std::size_t>(index) < vector.size();
}

} // namespace cljonic
