#pragma once

#include <cljonic-core-valid-index.hpp>

namespace cljonic
{

  /** \anchor Core_Get
   * \b get returns the logical element at \p index when the index is valid.
   * When the index is invalid, the default element is returned.
   *
   ~~~~~{.cpp}
   #include "cljonic.hpp"
   using namespace cljonic;

   int main() {
     const auto values = Vector<int, 4>{10, 20};

     const auto v0 = get(values, 0U);     // 10
     const auto v1 = get(values, 2U);     // 0
     const auto v2 = get(values, 2U, 99); // 99

     return (v0 == 10 && v1 == 0 && v2 == 99) ? 0 : 1;
   }
   ~~~~~
   */
  template <typename element_type, std::size_t capacity_value,
            std::integral index_type>
  [[nodiscard]] constexpr auto
  get(const Vector<element_type, capacity_value> &vector,
      index_type index) noexcept -> element_type
  {
    return valid_index(vector, index) ? vector(index) : element_type{};
  }

  /** \anchor Core_Get_Fallback
   * \b get returns the logical element at \p index when the index is valid.
   * When the index is invalid, \p fallback is returned.
   */
  template <typename element_type, std::size_t capacity_value,
            std::integral index_type>
  [[nodiscard]] constexpr auto
  get(const Vector<element_type, capacity_value> &vector, index_type index,
      const element_type &fallback) noexcept -> element_type
  {
    return valid_index(vector, index) ? vector(index) : fallback;
  }

} // namespace cljonic
