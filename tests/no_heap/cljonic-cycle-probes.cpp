#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto cycle_probe() noexcept -> bool {
    const auto destination = Vector<int, 5>{};
    const auto source = Vector<int, 3>{1, 2, 3};
    const auto repeated = cycle(source);
    const auto prefix = into(destination, repeated);
    const auto finite_source_cycle = cycle(Range{1, 4});
    const auto finite_source_prefix = into(Vector<int, 5>{}, finite_source_cycle);
    const auto unbounded_source_cycle = cycle(Range{0, 0, 0});
    const auto unbounded_source_prefix = into(Vector<int, 4>{}, unbounded_source_cycle);
    return repeated.count() == CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE && !repeated.is_finite() &&
           !fits_into(destination, repeated) && prefix.count() == destination.capacity() && prefix(0U) == 1 &&
           prefix(1U) == 2 && prefix(2U) == 3 && prefix(3U) == 1 && prefix(4U) == 2 &&
           finite_source_prefix.count() == 5U && finite_source_prefix(0U) == 1 && finite_source_prefix(1U) == 2 &&
           finite_source_prefix(2U) == 3 && finite_source_prefix(3U) == 1 && finite_source_prefix(4U) == 2 &&
           unbounded_source_prefix.count() == 4U && unbounded_source_prefix(0U) == 0 &&
           unbounded_source_prefix(3U) == 0;
}

} // namespace cljonic::no_heap::probes
