#pragma once

#include <concepts>
#include <cstddef>

#include <cljonic-vector.hpp>

namespace cljonic {

/** \anchor Core_ValidIndex
 * \b valid_index reports whether an integral index addresses a logical
 * element in a Vector without inspecting the stored value.
 *
 ~~~~~{.cpp}
 #include "cljonic-core.hpp"
 using namespace cljonic;

 int main() {
   constexpr Vector<int, 4> values{10, 20};

   static_assert(valid_index(values, 1U));
   static_assert(!valid_index(values, 2U));
   static_assert(!valid_index(values, -1));

   return 0;
 }
 ~~~~~
 */
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
