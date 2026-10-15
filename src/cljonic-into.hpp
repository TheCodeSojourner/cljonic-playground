#pragma once

#include <cljonic-concepts.hpp>

namespace cljonic {

/** \anchor Into
 * The \b into free function API appends elements from a \ref concepts::CljonicSource "source" to a \ref Vector
 * "Vector", up to its remaining
 *        capacity.
 *
 * Returns an updated destination-typed collection with source elements appended after the destination's existing
 * logical contents, leaving both \p destination and \p source unchanged. An unbounded or oversized source produces a
 * deterministic bounded prefix limited by the destination's remaining capacity; a finite source that fits materializes
 * completely. Scoped to \ref Vector "Vector" destinations in this increment.
 *
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 constexpr Vector<int, 8> destination{100, 200};
 constexpr Range<int> source{0, 5};
 constexpr auto result = into(destination, source);
 static_assert(count(result) == 7U);
 static_assert(result(2U) == 0);
 ~~~~~
 */
template <concepts::CljonicVector Dest, concepts::CljonicSource Source>
[[nodiscard]] constexpr auto into(const Dest& destination, const Source& source) noexcept -> Dest {
    Dest result = destination;
    for (auto&& item : source) {
        const auto index = result.count();
        if (!result.can_assoc(index, item)) {
            break;
        }
        result = result.assoc(index, item);
    }
    return result;
}

} // namespace cljonic
