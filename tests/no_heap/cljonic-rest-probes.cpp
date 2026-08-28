#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto rest_probe() noexcept -> bool {
    const auto q = conj(Queue<int, 4>{}, 1);
    const auto r = rest(q);
    return r.size() == 0U;
}

} // namespace cljonic::no_heap::probes
