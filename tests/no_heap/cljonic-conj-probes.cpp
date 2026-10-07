#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto conj_probe() noexcept -> bool {
    const auto q = Queue<int, 4>{};
    const auto q1 = conj(q, 10);
    const auto s = Set<int, 4>{};
    const auto s1 = conj(s, 20);
    const auto v = Vector<int, 4>{1};
    const auto v1 = conj(v, 2);
    const auto m = Map<int, int, 4>{};
    const auto m1 = conj(m, MapEntry<int, int>{1, 10});
    const auto text = String<4>{"A"};
    const auto text1 = conj(text, 'B');
    return q1.count() == 1U && s1.count() == 1U && v1.count() == 2U && v1(1) == 2 && m1.count() == 1U && m1(1) == 10 &&
           text1.count() == 2U && text1(1) == 'B';
}

} // namespace cljonic::no_heap::probes
