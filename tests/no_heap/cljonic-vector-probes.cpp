#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {
[[nodiscard]] auto vector_capacity() noexcept -> bool {
    const Vector<int, 4> values{1, 2, 3};
    return values.capacity() == 4U && values.count() <= values.capacity();
}

} // namespace cljonic::no_heap::probes
