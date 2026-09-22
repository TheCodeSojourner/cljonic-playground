#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto iterate_probe() noexcept -> bool {
    const auto destination = Vector<int, 4>{};
    const auto sequence = iterate([](int value) noexcept { return value + 1; }, 0);
    const auto copied = sequence;
    const auto result = into(destination, copied);
    return sequence.count() == CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE && !sequence.is_finite() &&
           !fits_into(destination, sequence) && result.count() == destination.capacity() && result(0U) == 0 &&
           result(1U) == 1 && result(2U) == 2 && result(3U) == 3;
}

} // namespace cljonic::no_heap::probes