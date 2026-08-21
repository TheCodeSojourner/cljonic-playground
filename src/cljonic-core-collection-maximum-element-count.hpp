#pragma once

#include <cstddef>

#include <cljonic-config.hpp>

namespace cljonic {

/** \anchor Core_CollectionMaximumElementCount
 * \b collection_maximum_element_count returns the configured synthesis
 * ceiling for bounded cljonic collections and semantically infinite
 * producers.
 */
[[nodiscard]] constexpr auto
collection_maximum_element_count() noexcept -> std::size_t {
  return CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT;
}

} // namespace cljonic
