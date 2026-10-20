#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto get_probe() noexcept -> bool {
    const auto v = Vector<int, 4>{10, 20};
    const auto m = assoc(Map<int, int, 4>{}, 1, 100);
    const auto s = conj(Set<int, 4>{}, 5);
    const auto text = String<4>{"ab"};
    return get(v, 0U) == 10 && get(m, 1) == 100 && get(s, 5) == 5 && get(text, 0U) == 'a';
}

} // namespace cljonic::no_heap::probes
