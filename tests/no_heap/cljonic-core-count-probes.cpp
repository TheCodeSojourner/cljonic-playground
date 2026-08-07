#include "poison.hpp"
#include "probes.hpp"
#include <cljonic-core.hpp>

namespace cljonic::no_heap::probes {

[[nodiscard]] auto count_on_vector() noexcept -> bool {
  Vector<int, 4> values{1, 2, 3};
  return count(values) == 3;
}

} // namespace cljonic::no_heap::probes
