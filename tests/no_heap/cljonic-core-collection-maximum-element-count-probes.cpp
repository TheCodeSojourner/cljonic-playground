#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto max_count_probe() noexcept -> bool {
  return CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE == 1000000ULL;
}

} // namespace cljonic::no_heap::probes
