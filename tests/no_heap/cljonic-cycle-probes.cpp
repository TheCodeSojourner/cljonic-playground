#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto cycle_probe() noexcept -> bool {
    const auto destination = Vector<int, 5>{};
    const auto source = Vector<int, 3>{1, 2, 3};
    const auto repeated = cycle(source);
    const auto prefix = into(destination, repeated);
    return repeated.count() == CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE && !repeated.is_finite() &&
           !fits_into(destination, repeated) && prefix.count() == destination.capacity() && prefix(0U) == 1 &&
           prefix(1U) == 2 && prefix(2U) == 3 && prefix(3U) == 1 && prefix(4U) == 2;
}

} // namespace cljonic::no_heap::probes
