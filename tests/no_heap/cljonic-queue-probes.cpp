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

    // queue factory: deduced, explicit element type, explicit capacity,
    // typed empty, and FIFO argument order, all through the free-function
    // factory surface.
    const auto factory_deduced = queue(1, 2, 3);
    const auto factory_typed = queue<long>(short{4}, short{5});
    const auto factory_capacity = queue<int, 4>(6);
    const auto factory_empty = queue<int>();
    const auto factory_zero = queue<int, 2>();
    const auto factory_enclosed = queue(literal);
    return q1.count() == 1U && q1.peek() == 1 && cljonic::is_empty(q1.pop()) && literal.count() == 3U &&
           literal.peek() == 10 && imported.count() == 2U && imported.peek() == 3 && imported.begin()[1] == 4 &&
           static_imported.peek() == 8 && view_imported.begin()[1] == 9 && factory_deduced.count() == 3U &&
           factory_deduced.peek() == 1 && factory_deduced.pop().peek() == 2 && factory_typed.peek() == 4L &&
           factory_typed.count() == 2U && factory_capacity.count() == 1U && factory_capacity.capacity() == 4U &&
           factory_capacity.peek() == 6 && cljonic::is_empty(factory_empty) && factory_empty.capacity() == 0U &&
           cljonic::is_empty(factory_zero) && factory_zero.capacity() == 2U && factory_enclosed.count() == 1U &&
           factory_enclosed.peek() == literal;
}

} // namespace cljonic::no_heap::probes
