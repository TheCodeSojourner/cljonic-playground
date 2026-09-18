#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto repeat_probe() noexcept -> bool {
    const auto destination = Vector<int, 4>{};
    const auto finite = Repeat{7, 3U};
    const auto unbounded = Repeat{7};
    const auto finite_result = into(destination, finite);
    const auto prefix_result = into(destination, unbounded);
    return finite.count() == 3U && finite.is_finite() && fits_into(destination, finite) &&
           finite_result.count() == 3U && finite_result(0U) == 7 && finite_result(2U) == 7 &&
           unbounded.count() == CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE && !unbounded.is_finite() &&
           !fits_into(destination, unbounded) && prefix_result.count() == destination.capacity() &&
           prefix_result(0U) == 7 && prefix_result(3U) == 7;
}

} // namespace cljonic::no_heap::probes