#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto map_probe() noexcept -> bool {
    const auto m = Map<int, int, 4>{};
    const auto m1 = m.assoc(1, 10);
    const auto literal = Map{MapEntry<int, int>{1, 10}, MapEntry<int, int>{2, 20}};
    return m1.count() == 1U && m1.contains(1) && m1(1) == 10 && literal.count() == 2U;
}

} // namespace cljonic::no_heap::probes
