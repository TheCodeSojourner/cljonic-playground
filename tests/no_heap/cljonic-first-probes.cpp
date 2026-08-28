#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto first_probe() noexcept -> bool {
  const auto v = Vector<int, 4>{10, 20};
  const MapEntry<int, int> me{1, 100};
  return first(v) == 10 && first(me) == 1;
}

} // namespace cljonic::no_heap::probes
