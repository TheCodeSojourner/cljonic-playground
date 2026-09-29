#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto not_equal_probe() noexcept -> bool {
    const auto v = Vector<int, 4>{1, 2};
    const auto w = Vector<int, 4>{1, 3};
    return not_equal(v, w) && !not_equal(v, v);
}

} // namespace cljonic::no_heap::probes
