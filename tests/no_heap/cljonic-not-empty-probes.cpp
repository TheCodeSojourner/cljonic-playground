#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto not_empty_probe() noexcept -> bool {
  const auto v = Vector<int, 4>{1};
  const auto nv = not_empty(v);
  return count(nv) == 1U;
}

} // namespace cljonic::no_heap::probes
