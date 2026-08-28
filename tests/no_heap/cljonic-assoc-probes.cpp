#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto assoc_probe() noexcept -> bool {
    const auto m = Map<int, int, 4>{};
    const auto m1 = assoc(m, 1, 100);
    return m1.size() == 1U && m1(1) == 100;
}

} // namespace cljonic::no_heap::probes
