#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto queue_probe() noexcept -> bool {
    const auto q = Queue<int, 4>{};
    const auto q1 = q.conj(1);
    const auto literal = Queue{10, 20, 30};
    const auto wrapped = literal.pop().conj(40);
    const auto sequence = seq(wrapped);
    const auto remainder = rest(wrapped);
    return q1.count() == 1U && q1.peek() == 1 && q1.pop().is_empty() && literal.count() == 3U && first(wrapped) == 20 &&
           sequence.count() == 3U && sequence(0U) == 20 && sequence(1U) == 30 && sequence(2U) == 40 &&
           next(wrapped)(0U) == 30 && remainder(0U) == 30;
}

} // namespace cljonic::no_heap::probes
