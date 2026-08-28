#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto conj_probe() noexcept -> bool {
  const auto q = Queue<int, 4>{};
  const auto q1 = conj(q, 10);
  const auto s = Set<int, 4>{};
  const auto s1 = conj(s, 20);
  return q1.size() == 1U && s1.count() == 1U;
}

} // namespace cljonic::no_heap::probes
