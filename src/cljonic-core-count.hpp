#pragma once

#include <cljonic-vector.hpp>

namespace cljonic {

/** \anchor Core_Count
 * \b count returns the number of logical elements in \p collection.
 * Applies to all \b CopyOnModifyCollection types: \ref Vector, Set, Map, and
 * String.
 */
template <concepts::VectorElement element_type, std::size_t capacity_value>
[[nodiscard]] constexpr auto
count(const Vector<element_type, capacity_value> &collection) noexcept
    -> std::size_t {
  return collection.logical_size();
}

} // namespace cljonic
