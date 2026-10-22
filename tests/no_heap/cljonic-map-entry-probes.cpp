#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto map_entry_probe() noexcept -> bool {
    const MapEntry<int, int> me{1, 2};
    const auto factory_me = map_entry(3, 4);
    const auto explicit_me = map_entry<long, long>(3, 4);
    return me.key == 1 && me.value == 2 && factory_me.key == 3 && factory_me.value == 4 && explicit_me.key == 3L &&
           explicit_me.value == 4L;
}

} // namespace cljonic::no_heap::probes
