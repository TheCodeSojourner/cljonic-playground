#pragma once

#include <cljonic-vector.hpp>

namespace cljonic {

/** \anchor Core_Count
 * \b count returns the number of logical elements in \p collection.
 *
 ~~~~~{.cpp}
 #include "cljonic-core.hpp"
 using namespace cljonic;

 int main() {
   const auto empty = Vector<int, 4>{};
   const auto populated = Vector{1, 2};

   const auto n0 = count(empty);     // 0
   const auto n1 = count(populated); // 2

   return (n0 == 0 && n1 == 2) ? 0 : 1;
 }
 ~~~~~
 */
template <concepts::VectorElement element_type, std::size_t capacity_value>
[[nodiscard]] constexpr auto
count(const Vector<element_type, capacity_value> &collection) noexcept
    -> std::size_t {
  return collection.size();
}

} // namespace cljonic
