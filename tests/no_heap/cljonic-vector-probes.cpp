#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

#include <ranges>
#include <utility>

namespace cljonic::no_heap::probes {
namespace {

struct FactoryMoveProbe {
    int value = 0;
    bool* move_observed = nullptr;

    constexpr FactoryMoveProbe() noexcept = default;
    constexpr explicit FactoryMoveProbe(int initial_value, bool* observer = nullptr) noexcept
        : value(initial_value), move_observed(observer) {
    }
    constexpr FactoryMoveProbe(const FactoryMoveProbe&) noexcept = default;
    constexpr FactoryMoveProbe(FactoryMoveProbe&& other) noexcept : value(other.value) {
        if (other.move_observed != nullptr) {
            *other.move_observed = true;
        }
    }
    constexpr auto operator=(const FactoryMoveProbe&) noexcept -> FactoryMoveProbe& = default;
};

} // namespace

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
    const auto factory_empty = vector<int>();
    const auto factory_zero_capacity = vector<int, 0>();
    const auto factory_reserved_empty = vector<int, 4>();
    const auto factory_deduced = vector(stack_first, stack_last);
    const auto factory_explicit = vector<long>(stack_first, stack_last);
    const auto factory_reserved = vector<int, 4>(stack_first, stack_last);
    volatile short narrow_raw = 7;
    const short narrow_value = narrow_raw;
    const auto factory_widened = vector(stack_first, narrow_value);
    const auto factory_nested = vector(factory_deduced);
    const auto factory_producer = vector(repeat(2, stack_first));
    FactoryMoveProbe lvalue{stack_first};
    const auto factory_copied = vector(lvalue);
    const bool lvalue_preserved = lvalue.value == stack_first;
    lvalue.value = stack_last;
    bool move_observed = false;
    FactoryMoveProbe rvalue{stack_last, &move_observed};
    const auto factory_moved = vector(std::move(rvalue));
    FactoryMoveProbe reserved_lvalue{stack_first};
    const auto factory_reserved_copy = vector<FactoryMoveProbe, 4>(reserved_lvalue);
    const bool reserved_lvalue_preserved = reserved_lvalue.value == stack_first;
    reserved_lvalue.value = stack_last;
    bool reserved_move_observed = false;
    FactoryMoveProbe reserved_rvalue{stack_last, &reserved_move_observed};
    const auto factory_reserved_move = vector<FactoryMoveProbe, 4>(std::move(reserved_rvalue));
    const auto member_popped = stack_values.pop();
    const auto function_popped = pop(stack_values);
    const auto empty_stack_values = Vector<int, 4>{stack_last}.pop();
    const auto zero_capacity_stack_values = Vector<int, 0>{};
    volatile int zero_capacity_probe_raw = 0;
    const int zero_capacity_probe = zero_capacity_probe_raw;
    volatile int negative_index_raw = -1;
    const int negative_index = negative_index_raw;
    const auto replaced = assoc(stack_values, zero_capacity_probe, stack_last);
    const auto appended = assoc(stack_values, 2, stack_first);
    const auto full = conj(appended, stack_last);
    const bool updates_ok =
        can_assoc(stack_values, 0, stack_last) && can_assoc(stack_values, 2, stack_first) &&
        !can_assoc(stack_values, negative_index, stack_first) && !can_assoc(stack_values, 3, stack_first) &&
        replaced(0) == stack_last && appended.count() == 3U && appended(2) == stack_first &&
        stack_values(0) == stack_first && stack_values.count() == 2U &&
        assoc(stack_values, negative_index, stack_first) == stack_values && can_conj(appended, stack_last) &&
        full.count() == 4U && !can_assoc(full, 4, stack_first) && assoc(full, 4, stack_first) == full &&
        !can_conj(full, stack_first) && conj(full, stack_first) == full;
    const bool lookup_and_interop_ok =
        contains(stack_values, 0) && !contains(stack_values, negative_index) && !contains(stack_values, 2) &&
        stack_values(negative_index) == 0 && stack_values(negative_index, stack_last) == stack_last &&
        stack_values(2, stack_last) == stack_last && stack_values.view().size() == 2U &&
        stack_values.begin()[0] == stack_first && stack_values.end() - stack_values.begin() == 2;
    return values.capacity() == 4U && values.count() <= values.capacity() && factory_empty.count() == 0U &&
           factory_empty.capacity() == 0U && factory_zero_capacity.count() == 0U &&
           factory_zero_capacity.capacity() == 0U && factory_reserved_empty.count() == 0U &&
           factory_reserved_empty.capacity() == 4U && factory_deduced.count() == 2U &&
           factory_deduced.capacity() == 2U && factory_deduced(0U) == stack_first &&
           factory_deduced(1U) == stack_last && factory_explicit.count() == 2U && factory_explicit.capacity() == 2U &&
           factory_reserved.count() == 2U && factory_reserved.capacity() == 4U && factory_reserved(0U) == stack_first &&
           factory_reserved(1U) == stack_last && factory_explicit(0U) == stack_first &&
           factory_explicit(1U) == stack_last && factory_widened.count() == 2U && factory_widened(1) == narrow_value &&
           factory_nested.count() == 1U && factory_nested(0)(1) == stack_last && factory_producer.count() == 1U &&
           count(factory_producer(0)) == 2U && factory_producer(0) == repeat(2, stack_first) && lvalue_preserved &&
           factory_copied(0).value == stack_first && move_observed && factory_moved(0).value == stack_last &&
           reserved_lvalue_preserved && factory_reserved_copy.count() == 1U &&
           factory_reserved_copy(0).value == stack_first && reserved_move_observed &&
           factory_reserved_move.count() == 1U && factory_reserved_move(0).value == stack_last &&
           imported.count() == 2U && imported(0) == 4 && imported(1) == 5 && static_imported(0) == 8 &&
           view_imported(1) == 9 && stack_values.peek() == 42 && peek(stack_values) == 42 &&
           member_popped.count() == 1U && member_popped.peek() == 21 && function_popped.peek() == 21 &&
           stack_values.count() == 2U && stack_values.capacity() == member_popped.capacity() &&
           empty_stack_values.peek() == 0 && empty_stack_values.pop().count() == 0U && updates_ok &&
           lookup_and_interop_ok &&
           (zero_capacity_probe != 0 ||
            (zero_capacity_stack_values.peek() == 0 && zero_capacity_stack_values.pop().count() == 0U));
}

} // namespace cljonic::no_heap::probes
