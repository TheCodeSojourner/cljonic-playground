#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto assoc_probe() noexcept -> bool {
    const auto m = Map<int, int, 4>{};
    const auto m1 = assoc(m, 1, 100);
    const auto m2 = assoc(m, 1, 100, 2, 200);
    return m1.count() == 1U && m1(1) == 100 && m2.count() == 2U && m2(2) == 200;
}

} // namespace cljonic::no_heap::probes
