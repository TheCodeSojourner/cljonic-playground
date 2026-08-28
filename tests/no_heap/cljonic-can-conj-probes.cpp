#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto can_conj_probe() noexcept -> bool {
  const auto q = Queue<int, 4>{};
  const auto s = Set<int, 4>{};
  return can_conj(q) && can_conj(s, 1);
}

} // namespace cljonic::no_heap::probes
