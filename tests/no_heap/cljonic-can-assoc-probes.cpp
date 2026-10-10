#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto can_assoc_probe() noexcept -> bool {
    const auto m = Map<int, int, 4>{};
    const auto v = Vector<int, 4>{1};
    const auto text = String<4>{"a"};
    return can_assoc(m, 1, 100, 2, 200) && can_assoc(v, 0, 2, 1, 3) && can_assoc(text, 1, 'b', 0, 'A');
}

} // namespace cljonic::no_heap::probes
