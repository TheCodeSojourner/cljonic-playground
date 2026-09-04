#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto empty_probe() noexcept -> bool {
    const auto v = Vector<int, 4>{1, 2};
    const auto e = empty(v);
    return e.count() == 0U;
}

} // namespace cljonic::no_heap::probes
