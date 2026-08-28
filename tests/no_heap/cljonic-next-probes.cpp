#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto next_probe() noexcept -> bool {
  const auto q = Queue<int, 4>{};
  const auto q1 = conj(conj(q, 1), 2);
  const auto q2 = next(q1);
  return peek(q2) == 2;
}

} // namespace cljonic::no_heap::probes
