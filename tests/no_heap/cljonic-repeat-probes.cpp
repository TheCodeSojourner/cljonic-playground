#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto repeat_probe() noexcept -> bool {
    const auto destination = Vector<int, 4>{};
    volatile int runtime_count = 3;
    volatile int runtime_negative = -2;
    volatile int runtime_value = 7;
    const auto count = runtime_count;
    const auto value = runtime_value;
    const auto finite = Repeat{count, value};
    const auto from_factory = repeat(count, value);
    const auto unbounded = Repeat{value};
    const auto empty = repeat(static_cast<int>(runtime_negative), value);
    const auto empty_constructor = Repeat{static_cast<int>(runtime_negative), value};
    const auto finite_result = into(destination, finite);
    const auto prefix_result = into(destination, unbounded);
    return empty.count() == 0 && empty.is_finite() && empty == empty_constructor &&
           into(destination, empty).count() == 0 && finite == from_factory && finite.count() == 3U &&
           finite.is_finite() && fits_into(destination, finite) && finite_result.count() == 3U &&
           finite_result(0U) == 7 && finite_result(2U) == 7 &&
           unbounded.count() == CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE && !unbounded.is_finite() &&
           !fits_into(destination, unbounded) && prefix_result.count() == destination.capacity() &&
           prefix_result(0U) == 7 && prefix_result(3U) == 7;
}

} // namespace cljonic::no_heap::probes