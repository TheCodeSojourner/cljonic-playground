#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto first_probe() noexcept -> bool {
    const auto v = Vector<int, 4>{10, 20};
    const auto m = Map<int, int, 4>{MapEntry<int, int>{1, 100}};
    const auto s = Set<int, 4>{7};
    const auto q = Queue<int, 4>{5, 6};
    const auto text = String<4>{"AB"};
    const auto e = MapEntry<int, int>{3, 300};
    return first(v) == 10 && first(m) == MapEntry<int, int>{1, 100} && first(s) == 7 && first(q) == 5 &&
           first(text) == 'A' && first(e) == 3;
}

} // namespace cljonic::no_heap::probes
