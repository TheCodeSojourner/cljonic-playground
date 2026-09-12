#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto map_entry_probe() noexcept -> bool {
    const MapEntry<int, int> me{1, 2};
    return me.key == 1 && me.value == 2 && me.contains(0);
}

} // namespace cljonic::no_heap::probes
