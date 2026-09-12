#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto count_probe() noexcept -> bool {
    const auto v = Vector<int, 4>{1, 2, 3};
    return count(v) == 3U;
}

} // namespace cljonic::no_heap::probes
