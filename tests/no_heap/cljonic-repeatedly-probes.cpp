#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto repeatedly_probe() noexcept -> bool {
    const auto destination = Vector<int, 4>{};
    const auto counted = repeatedly(3U, []() noexcept { return 5; });
    const auto copied = counted;
    const auto counted_result = into(destination, copied);
    const auto uncounted = repeatedly([]() noexcept { return 5; });
    const auto uncounted_result = into(destination, uncounted);
    const auto empty_result = into(destination, repeatedly(0U, []() noexcept { return 5; }));
    return counted.is_finite() && counted.count() == 3U && counted_result.count() == 3U && counted_result(0U) == 5 &&
           counted_result(2U) == 5 && uncounted.count() == CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE &&
           !uncounted.is_finite() && !fits_into(destination, uncounted) &&
           uncounted_result.count() == destination.capacity() && uncounted_result(0U) == 5 &&
           uncounted_result(3U) == 5 && empty_result.count() == 0U;
}

} // namespace cljonic::no_heap::probes