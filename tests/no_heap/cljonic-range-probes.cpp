#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto range_probe() noexcept -> bool {
    const auto ascending = Range<int>{0, 5};
    const auto descending = Range<int>{5, 0, -1};
    const auto unbounded = Range<int>{0, 5, 0};
    const auto huge = Range<long>{1000000000L};
    return ascending.count() == 5U && ascending.contains(3U) && descending.count() == 5U && descending.contains(0U) &&
           unbounded.count() == CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE && unbounded.contains(0U) &&
           huge.contains(CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE - 1U) &&
           !huge.contains(CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE);
}

} // namespace cljonic::no_heap::probes
