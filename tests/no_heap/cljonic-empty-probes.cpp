#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto empty_probe() noexcept -> bool {
    const auto v = Vector<int, 4>{1, 2, 3};
    const auto m = Map<int, int, 4>{MapEntry<int, int>{1, 100}};
    const auto s = Set<int, 4>{1};
    const auto q = Queue<int, 4>{1};
    const auto text = String<4>{"AB"};
    return is_empty(empty(v)) && is_empty(empty(m)) && is_empty(empty(s)) && is_empty(empty(q)) &&
           is_empty(empty(text)) && count(v) == 3U && count(m) == 1U && count(s) == 1U && count(q) == 1U &&
           count(text) == 2U;
}

} // namespace cljonic::no_heap::probes
