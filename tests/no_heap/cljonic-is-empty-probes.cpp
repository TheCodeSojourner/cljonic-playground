#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto is_empty_probe() noexcept -> bool {
    const auto v = Vector<int, 4>{};
    return is_empty(v);
}

} // namespace cljonic::no_heap::probes
