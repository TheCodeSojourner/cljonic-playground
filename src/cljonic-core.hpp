#ifndef CLJONIC_CORE_HPP
#define CLJONIC_CORE_HPP

/** \mainpage
 * cljonic is a header-only C++23 library for bounded embedded-friendly
 * functional-style programming.
 *
 * Current bootstrap implementation scope is intentionally narrow:
 * - `Vector` with bounded construction, size/state observation, and
 *   `try_push_back`.
 * - `count` for any type satisfying the `Collection` concept.
 *
 * The broader vocabulary and architecture are developed incrementally from this
 * nucleus, with behavior constraints enforced by specs and tests.
 */

/** \anchor Namespace_Core
 * Placeholder namespace for future core free-function organization. Current
 * bootstrap APIs live directly under `cljonic`.
 */

#include <cljonic-concepts.hpp>
#include <cljonic-core-count.hpp>
#include <cljonic-vector.hpp>

namespace cljonic {

namespace core {} // namespace core

} // namespace cljonic

#endif // CLJONIC_CORE_HPP
