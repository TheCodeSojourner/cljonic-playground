#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto seq_probe() noexcept -> bool {
  const auto v = Vector<int, 4>{1, 2};
  const auto s = seq(v);
  return count(s) == 2U;
}

} // namespace cljonic::no_heap::probes
