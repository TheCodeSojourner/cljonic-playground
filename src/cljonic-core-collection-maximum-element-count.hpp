#pragma once

#include <cstddef>

#include <cljonic-config.hpp>

namespace cljonic {

/** \anchor CollectionMaximumElementCountValue
 * \b CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE is a constexpr constant
 * that captures the configured synthesis ceiling for bounded cljonic
 * collections and semantically infinite producers. This type-safe constant
 * can be used in compile-time expressions, static assertions, and constexpr
 * functions while remaining fully overridable via the
 * CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT macro.
 */
constexpr std::size_t CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE = CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT;

} // namespace cljonic
