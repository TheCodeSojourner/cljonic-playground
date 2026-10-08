#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto count_probe() noexcept -> bool {
    const auto v = Vector<int, 4>{1, 2, 3};
    const auto finite_range = Range{0, 5};
    const auto unbounded_repeat = Repeat{7};
    return count(v) == 3U && count(finite_range) == 5U &&
           count(unbounded_repeat) == CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE;
}

} // namespace cljonic::no_heap::probes
