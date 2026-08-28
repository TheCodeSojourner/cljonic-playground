#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto dissoc_probe() noexcept -> bool {
  const auto m = Map<int, int, 4>{};
  const auto m1 = assoc(m, 1, 10);
  const auto m2 = dissoc(m1, 1);
  return m2.size() == 0U;
}

} // namespace cljonic::no_heap::probes
