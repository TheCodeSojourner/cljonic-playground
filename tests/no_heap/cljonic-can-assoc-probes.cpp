#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto can_assoc_probe() noexcept -> bool {
    const auto m = Map<int, int, 4>{};
    return can_assoc(m, 1, 100);
}

} // namespace cljonic::no_heap::probes
