#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto queue_probe() noexcept -> bool {
    const auto q = Queue<int, 4>{};
    const auto q1 = q.conj(1);
    return q1.count() == 1U && q1.peek() == 1 && q1.pop().is_empty();
}

} // namespace cljonic::no_heap::probes
