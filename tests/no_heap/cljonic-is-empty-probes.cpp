#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

namespace {

[[nodiscard]] auto collections_are_observed(int value) noexcept -> bool {
    const auto v = Vector<int, 1>{value};
    const auto m = Map<int, int, 1>{MapEntry<int, int>{value, value}};
    const auto s = Set<int, 1>{value};
    const auto q = Queue<int, 1>{value};
    const char source[]{static_cast<char>('A' + value), '\0'};
    const auto text = String<1>{source};
    const auto original_v = v;
    const auto original_m = m;
    const auto original_s = s;
    const auto original_q = q;
    const auto original_text = text;
    return is_empty(Vector<int, 1>{}) && !is_empty(v) && is_empty(Map<int, int, 1>{}) && !is_empty(m) &&
           is_empty(Set<int, 1>{}) && !is_empty(s) && is_empty(Queue<int, 1>{}) && !is_empty(q) &&
           is_empty(String<1>{}) && !is_empty(text) && equal(v, original_v) && equal(m, original_m) &&
           equal(s, original_s) && equal(q, original_q) && equal(text, original_text) &&
           is_empty(v) == (count(v) == 0) && is_empty(m) == (count(m) == 0) && is_empty(s) == (count(s) == 0) &&
           is_empty(q) == (count(q) == 0) && is_empty(text) == (count(text) == 0);
}

[[nodiscard]] auto finite_producers_are_observed(int value, std::size_t zero) noexcept -> bool {
    const auto vacant_range = Range{value, value};
    const auto finite_range = Range{0, value};
    const auto descending_range = Range{value, 0, -1};
    const auto vacant_repeat = Repeat{value, zero};
    const auto finite_repeat = Repeat{value, std::size_t{2}};
    const auto step = [value] noexcept { return value; };
    const auto vacant_repeatedly = repeatedly(zero, step);
    const auto finite_repeatedly = repeatedly(std::size_t{2}, step);
    const auto original_range = finite_range;
    const auto original_repeat = finite_repeat;
    const auto original_repeatedly = finite_repeatedly;
    return is_empty(vacant_range) && !is_empty(finite_range) && !is_empty(descending_range) &&
           is_empty(Range{value, 0}) && is_empty(vacant_repeat) && !is_empty(finite_repeat) &&
           is_empty(vacant_repeatedly) && !is_empty(finite_repeatedly) &&
           is_empty(finite_range) == (count(finite_range) == 0) &&
           is_empty(finite_repeat) == (count(finite_repeat) == 0) &&
           is_empty(finite_repeatedly) == (count(finite_repeatedly) == 0) && finite_range == original_range &&
           finite_repeat == original_repeat && count(finite_repeatedly) == count(original_repeatedly) &&
           *finite_repeatedly.begin() == *original_repeatedly.begin();
}

[[nodiscard]] auto unbounded_producers_are_observed(int value) noexcept -> bool {
    const auto range = Range{value, value, 0};
    const auto repeat_value = Repeat{value};
    const auto cycle_value = cycle(Vector<int, 1>{value});
    const auto iterate_value = iterate([](int item) noexcept { return item + 1; }, value);
    const auto repeatedly_value = repeatedly([value] noexcept { return value; });
    const auto original_range = range;
    const auto original_repeat = repeat_value;
    const auto original_cycle = cycle_value;
    const auto original_iterate = iterate_value;
    const auto original_repeatedly = repeatedly_value;
    const auto cap_is_zero = CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE == 0;
    return is_empty(range) == cap_is_zero && is_empty(repeat_value) == cap_is_zero &&
           is_empty(cycle_value) == cap_is_zero && is_empty(iterate_value) == cap_is_zero &&
           is_empty(repeatedly_value) == cap_is_zero && is_empty(cycle(Range{value, value})) == cap_is_zero &&
           is_empty(range) == (count(range) == 0) && is_empty(repeat_value) == (count(repeat_value) == 0) &&
           is_empty(cycle_value) == (count(cycle_value) == 0) &&
           is_empty(iterate_value) == (count(iterate_value) == 0) &&
           is_empty(repeatedly_value) == (count(repeatedly_value) == 0) && range == original_range &&
           repeat_value == original_repeat && cycle_value == original_cycle &&
           count(iterate_value) == count(original_iterate) && *iterate_value.begin() == *original_iterate.begin() &&
           count(repeatedly_value) == count(original_repeatedly) &&
           *repeatedly_value.begin() == *original_repeatedly.begin();
}

} // namespace

[[nodiscard]] auto is_empty_probe() noexcept -> bool {
    volatile int raw = 3;
    const int value = raw;
    volatile std::size_t raw_zero = 0;
    const std::size_t zero = raw_zero;
    return collections_are_observed(value) && finite_producers_are_observed(value, zero) &&
           unbounded_producers_are_observed(value) && is_empty(Vector<int, 0>{}) && is_empty(Map<int, int, 0>{}) &&
           is_empty(Set<int, 0>{}) && is_empty(Queue<int, 0>{}) && is_empty(String<0>{});
}

} // namespace cljonic::no_heap::probes
