#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto disj_probe() noexcept -> bool {
  const auto s = Set<int, 4>{};
  const auto s1 = conj(s, 42);
  const auto s2 = disj(s1, 42);
  return s2.count() == 0U;
}

} // namespace cljonic::no_heap::probes
