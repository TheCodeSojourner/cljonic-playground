#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto can_conj_probe() noexcept -> bool {
    const auto q = Queue<int, 4>{};
    const auto s = Set<int, 4>{};
    const auto v = Vector<int, 4>{};
    const auto m = Map<int, int, 4>{};
    const auto text = String<4>{"A"};
    return can_conj(q) && can_conj(q, 1, 2) && can_conj(s, 1, 2) && can_conj(v, 1, 2) &&
           can_conj(m, MapEntry<int, int>{1, 10}, MapEntry<int, int>{2, 20}) && can_conj(text, 'B', 'C') &&
           !can_conj(text, '\0');
}

} // namespace cljonic::no_heap::probes
