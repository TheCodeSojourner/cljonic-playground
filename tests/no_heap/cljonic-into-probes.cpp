#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto into_probe() noexcept -> bool {
    const auto destination = Vector<int, 8>{};
    const auto source = Range<int>{0, 5};
    const auto result = into(destination, source);
    return result.count() == 5U && result(0U) == 0 && result(4U) == 4;
}

} // namespace cljonic::no_heap::probes
