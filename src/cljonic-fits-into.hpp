#pragma once

#include <cljonic-concepts.hpp>

namespace cljonic {

/** \anchor FitsInto
 * \brief The non-throwing, non-allocating materialization-completeness preflight for \ref Into "into".
 *
 * Reports whether appending the complete \p source to \p destination fits within the destination's remaining capacity,
 * using the same cardinality semantics as `into`. It returns `false` for every unbounded producer, even though its
 * normal const traversal is capped. Scoped to `Vector` destinations in this increment.
 *
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 constexpr Vector<int, 8> destination{};
 constexpr Range<int> source{0, 5};
 static_assert(fits_into(destination, source));
 constexpr Range<int> unbounded{0, 5, 0};
 static_assert(!fits_into(destination, unbounded));
 ~~~~~
 */
template <concepts::CljonicVector Dest, concepts::CljonicSource Source>
[[nodiscard]] constexpr auto fits_into(const Dest& destination, const Source& source) noexcept -> bool {
    if constexpr (concepts::CljonicProducer<Source>) {
        if (!source.is_finite()) {
            return false;
        }
    }
    return source.count() <= (Dest::capacity() - destination.count());
}

} // namespace cljonic
