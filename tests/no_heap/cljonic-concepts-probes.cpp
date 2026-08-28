#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto concepts_probe() noexcept -> bool {
    return concepts::VectorElement<int> && concepts::NothrowVectorElement<int>;
}

} // namespace cljonic::no_heap::probes
