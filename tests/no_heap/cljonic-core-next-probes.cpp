#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

#include <array>

namespace cljonic::no_heap::probes {

[[nodiscard]] auto next_probe() noexcept -> bool {
    volatile int first_raw = 11;
    volatile int second_raw = 22;
    volatile int third_raw = 33;
    const int first = first_raw;
    const int second = second_raw;
    const int third = third_raw;

    const auto vector = Vector<int, 3>{first, second, third};
    const auto vector_before = vector;
    const auto vector_tail = next(vector);
    const auto queue = Queue<int, 3>{first, second, third};
    const auto queue_before = queue;
    const auto queue_tail = next(queue);
    const auto map = Map<int, int, 2>{MapEntry{first, second}, MapEntry{second, third}};
    const auto map_before = map;
    const auto map_tail = next(map);
    const auto set = Set<int, 3>{first, second, third};
    const auto set_before = set;
    const auto set_tail = next(set);
    volatile char first_char_raw = 'a';
    volatile char second_char_raw = 'b';
    volatile char third_char_raw = 'c';
    const char first_char = first_char_raw;
    const char second_char = second_char_raw;
    const char third_char = third_char_raw;
    const std::array text_input{first_char, second_char, third_char};
    const auto text = String<3>{text_input};
    const auto text_before = text;
    const auto text_tail = next(text);
    const auto finite_repeat = Repeat<int>{3U, first};
    const auto finite_repeat_before = finite_repeat;
    const auto finite_repeat_tail = next(finite_repeat);
    const auto finite_repeat_tail_values = into(Vector<int, 2>{}, finite_repeat_tail);
    const auto unbounded_repeat = Repeat<int>{second};
    const auto unbounded_repeat_tail = next(unbounded_repeat);
    const auto repeated_unbounded_repeat_tail = next(unbounded_repeat_tail);
    const auto unbounded_repeat_tail_values = into(Vector<int, 2>{}, unbounded_repeat_tail);
    const auto repeated_unbounded_repeat_tail_values = into(Vector<int, 2>{}, repeated_unbounded_repeat_tail);
    const auto finite_range = Range<int>{1, 8, 2};
    const auto finite_range_before = finite_range;
    const auto finite_range_tail = next(finite_range);
    const auto finite_range_tail_values = into(Vector<int, 3>{}, finite_range_tail);
    const auto empty_range = Range<int>{5, 0, 1};
    const auto empty_range_tail = next(empty_range);
    const auto unbounded_range = Range<int>{second, third, 0};
    const auto unbounded_range_tail = next(unbounded_range);
    const auto repeated_unbounded_range_tail = next(unbounded_range_tail);
    const auto unbounded_range_tail_values = into(Vector<int, 2>{}, unbounded_range_tail);

    return vector_tail.count() == 2U && vector_tail(0U) == second && vector_tail(1U) == third && vector.count() == 3U &&
           vector(0U) == first && equal(vector, vector_before) && queue_tail.count() == 2U &&
           queue_tail.begin()[0U] == second && queue_tail.begin()[1U] == third && queue.count() == 3U &&
           queue.begin()[0] == first && equal(queue, queue_before) && map_tail.count() == 1U &&
           map_tail.begin()[0U] == map.begin()[1] && map.count() == 2U && map.begin()[0] == MapEntry{first, second} &&
           equal(map, map_before) && set_tail.count() == 2U && set_tail.begin()[0U] == set.begin()[1] &&
           set_tail.begin()[1U] == set.begin()[2] && set.count() == 3U && set.begin()[0] == first &&
           equal(set, set_before) && text_tail.count() == 2U && text_tail(0U) == second_char &&
           text_tail(1U) == third_char && text.count() == 3U && text(0U) == first_char && equal(text, text_before) &&
           count(finite_repeat_tail) == 2U && fits_into(Vector<int, 2>{}, finite_repeat_tail) &&
           finite_repeat_tail_values(0U) == first && finite_repeat_tail_values(1U) == first &&
           finite_repeat == finite_repeat_before && count(finite_repeat) == 3U &&
           !fits_into(Vector<int, 2>{}, unbounded_repeat_tail) &&
           count(unbounded_repeat_tail) == count(unbounded_repeat) &&
           count(repeated_unbounded_repeat_tail) == count(unbounded_repeat) &&
           unbounded_repeat_tail_values(0U) == second && unbounded_repeat_tail_values(1U) == second &&
           repeated_unbounded_repeat_tail_values(0U) == second && repeated_unbounded_repeat_tail_values(1U) == second &&
           finite_range_tail == Range<int>{3, 8, 2} && finite_range_tail_values(0U) == 3 &&
           finite_range_tail_values(1U) == 5 && finite_range_tail_values(2U) == 7 &&
           finite_range == finite_range_before && empty_range_tail == empty_range && is_empty(empty_range_tail) &&
           unbounded_range_tail == unbounded_range && repeated_unbounded_range_tail == unbounded_range &&
           count(unbounded_range_tail) == count(unbounded_range) &&
           !fits_into(Vector<int, 2>{}, unbounded_range_tail) && unbounded_range_tail_values(0U) == second &&
           unbounded_range_tail_values(1U) == second && next(Vector<int, 3>{first}).count() == 0U &&
           next(Map<int, int, 2>{MapEntry{first, second}}).count() == 0U && next(Set<int, 3>{first}).count() == 0U &&
           next(Queue<int, 3>{first}).count() == 0U && next(String<3>{std::array{first_char}}).count() == 0U &&
           next(Vector<int, 0>{}).count() == 0U && next(Vector<int, 4>{}).count() == 0U &&
           next(Map<int, int, 4>{}).count() == 0U && next(Set<int, 4>{}).count() == 0U &&
           next(Queue<int, 4>{}).count() == 0U && next(String<4>{}).count() == 0U;
}

} // namespace cljonic::no_heap::probes
