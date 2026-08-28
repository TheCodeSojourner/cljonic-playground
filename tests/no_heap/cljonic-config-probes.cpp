#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto config_probe() noexcept -> bool {
  return CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE > 0U;
}

} // namespace cljonic::no_heap::probes
