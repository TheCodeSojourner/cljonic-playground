#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto vector_storage() noexcept -> bool {
    const Vector<int, 4> values{1, 2, 3};
    return values.size() == 3U && values(0U) == 1;
}

} // namespace cljonic::no_heap::probes