#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

#include <ranges>

namespace cljonic::no_heap::probes {
[[nodiscard]] auto vector_capacity() noexcept -> bool {
    const Vector<int, 4> values{1, 2, 3};
    const int static_source[] = {8, 9};
    const auto static_imported = Vector<int, 2>{std::span<const int, 2>{static_source}};
    const int source[] = {4, 5, 6, 7};
    const std::span<const int> dynamic_source{source};
    const auto imported = Vector<int, 2>{dynamic_source};
    const auto view_imported = Vector<int, 2>{std::views::all(static_source)};
    volatile int stack_first_raw = 21;
    volatile int stack_last_raw = 42;
    const int stack_first = stack_first_raw;
    const int stack_last = stack_last_raw;
    const auto stack_values = Vector<int, 4>{stack_first, stack_last};
    const auto member_popped = stack_values.pop();
    const auto function_popped = pop(stack_values);
    const auto empty_stack_values = Vector<int, 4>{stack_last}.pop();
    const auto zero_capacity_stack_values = Vector<int, 0>{};
    volatile int zero_capacity_probe_raw = 0;
    const int zero_capacity_probe = zero_capacity_probe_raw;
    return values.capacity() == 4U && values.count() <= values.capacity() && imported.count() == 2U &&
           imported(0) == 4 && imported(1) == 5 && static_imported(0) == 8 && view_imported(1) == 9 &&
           stack_values.peek() == 42 && peek(stack_values) == 42 && member_popped.count() == 1U &&
           member_popped.peek() == 21 && function_popped.peek() == 21 && stack_values.count() == 2U &&
           stack_values.capacity() == member_popped.capacity() && empty_stack_values.peek() == 0 &&
           empty_stack_values.pop().count() == 0U &&
           (zero_capacity_probe != 0 ||
            (zero_capacity_stack_values.peek() == 0 && zero_capacity_stack_values.pop().count() == 0U));
}

} // namespace cljonic::no_heap::probes
