#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

#include <ranges>

namespace cljonic::no_heap::probes {

[[nodiscard]] auto queue_probe() noexcept -> bool {
    const auto q = Queue<int, 4>{};
    const auto q1 = q.conj(1);
    const auto literal = Queue{10, 20, 30};
    const int static_source[] = {8, 9};
    const auto static_imported = Queue<int, 2>{std::span<const int, 2>{static_source}};
    const int source[] = {3, 4, 5};
    const std::span<const int> dynamic_source{source};
    const auto imported = Queue<int, 2>{dynamic_source};
    const auto view_imported = Queue<int, 2>{std::views::all(static_source)};
    return q1.count() == 1U && q1.peek() == 1 && q1.pop().is_empty() && literal.count() == 3U && literal.peek() == 10 &&
           imported.count() == 2U && imported.peek() == 3 && imported.begin()[1] == 4 && static_imported.peek() == 8 &&
           view_imported.begin()[1] == 9;
}

} // namespace cljonic::no_heap::probes
