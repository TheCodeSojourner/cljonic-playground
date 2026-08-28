#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto pop_probe() noexcept -> bool {
  const auto q = conj(conj(Queue<int, 4>{}, 1), 2);
  const auto q1 = pop(q);
  return peek(q1) == 2;
}

} // namespace cljonic::no_heap::probes
