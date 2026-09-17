#pragma once

#include <cljonic-concepts.hpp>

namespace cljonic {

/** \anchor FitsInto
 * \brief The non-throwing, non-allocating materialization-completeness preflight for \ref Into "into".
 *
 * Reports whether appending the complete \p source to \p destination fits within the destination's capacity, using the
 * same cardinality semantics as `into`. Scoped to `Vector` destinations in this increment.
 *
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 constexpr Vector<int, 8> destination{};
 constexpr Range<int> source{0, 5};
 static_assert(fits_into(destination, source));
 ~~~~~
 */
template <concepts::CljonicVector Dest, concepts::CljonicSource Source>
[[nodiscard]] constexpr auto fits_into(const Dest& destination, const Source& source) noexcept -> bool {
    return (destination.count() + source.count()) <= Dest::capacity();
}

} // namespace cljonic
