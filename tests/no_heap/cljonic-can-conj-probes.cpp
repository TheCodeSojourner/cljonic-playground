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
    return can_conj(q, 1) && can_conj(s, 1) && can_conj(v, 1) && can_conj(m, MapEntry<int, int>{1, 10}) &&
           can_conj(text, 'B') && !can_conj(text, '\0');
}

} // namespace cljonic::no_heap::probes
