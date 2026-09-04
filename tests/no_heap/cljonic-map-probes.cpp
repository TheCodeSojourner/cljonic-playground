#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto map_probe() noexcept -> bool {
    const auto m = Map<int, int, 4>{};
    const auto m1 = m.assoc(1, 10);
    return m1.count() == 1U && m1.contains(1) && m1(1) == 10;
}

} // namespace cljonic::no_heap::probes
