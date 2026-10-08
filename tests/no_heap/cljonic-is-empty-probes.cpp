#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto is_empty_probe() noexcept -> bool {
    const auto v = Vector<int, 4>{};
    const auto finite_producer = Range<int>{0, 0};
    const auto unbounded_producer = Range<int>{0, 0, 0};
    const auto finite_repeat = Repeat<int>{1, 0U};
    const auto unbounded_repeat = Repeat<int>{1};
    return is_empty(v) && is_empty(finite_producer) && is_empty(finite_repeat) && !is_empty(unbounded_producer) &&
           !is_empty(unbounded_repeat);
}

} // namespace cljonic::no_heap::probes
