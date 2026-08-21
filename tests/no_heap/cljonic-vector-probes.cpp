#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {
[[nodiscard]] auto vector_count() noexcept -> bool {
  const Vector<int, 4> values{1, 2, 3};
  return count(values) == 3U;
}

} // namespace cljonic::no_heap::probes
