#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto set_probe() noexcept -> bool {
    const auto s = Set<int, 4>{};
    const auto s1 = s.conj(1);
    const auto literal = Set{1, 2, 2, 3};
    return s1.count() == 1U && s1.contains(1) && s1(1) == 1 && literal.count() == 3U;
}

} // namespace cljonic::no_heap::probes