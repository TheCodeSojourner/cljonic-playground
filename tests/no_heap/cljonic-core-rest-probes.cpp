#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

#include <array>

namespace cljonic::no_heap::probes {

[[nodiscard]] auto rest_probe() noexcept -> bool {
    volatile int first_raw = 11;
    volatile int second_raw = 22;
    volatile int third_raw = 33;
    const int first = first_raw;
    const int second = second_raw;
    const int third = third_raw;

    const auto vector = Vector<int, 3>{first, second, third};
    const auto vector_rest = rest(vector);
    const auto queue = Queue<int, 3>{first, second, third};
    const auto queue_rest = rest(queue);
    const auto map = Map<int, int, 3>{MapEntry{first, second}, MapEntry{second, third}, MapEntry{third, first}};
    const auto map_rest = rest(map);
    const auto set = Set<int, 3>{first, second, third};
    const auto set_rest = rest(set);

    volatile char first_char_raw = 'a';
    volatile char second_char_raw = 'b';
    volatile char third_char_raw = 'c';
    const char first_char = first_char_raw;
    const char second_char = second_char_raw;
    const char third_char = third_char_raw;
    const std::array chars{first_char, second_char, third_char};
    const auto text = String<3>{chars};
    const auto text_rest = rest(text);

    const auto empty_vector = Vector<int, 4>{};
    const auto empty_map = Map<int, int, 4>{};
    const auto empty_set = Set<int, 4>{};
    const auto empty_queue = Queue<int, 4>{};
    const auto empty_text = String<4>{};
    const auto zero_capacity_vector = Vector<int, 0>{};

    const auto repeat_rest = rest(repeat(3, second));
    const auto range_rest = rest(Range<int>{first, third, 1});
    const auto cycle_rest = rest(cycle(vector));
    const auto iterate_rest = rest(iterate([](int value) noexcept { return value + 1; }, first));
    const auto repeatedly_rest = rest(repeatedly(3, []() noexcept { return 7; }));

    return vector_rest.count() == 2U && vector_rest(0U) == second && vector_rest(1U) == third && vector.count() == 3U &&
           vector(0U) == first && vector_rest.capacity() + 1U == vector.capacity() && queue_rest.count() == 2U &&
           queue_rest.begin()[0U] == second && queue_rest.begin()[1U] == third && queue.count() == 3U &&
           queue.begin()[0] == first && queue_rest.capacity() + 1U == queue.capacity() && map_rest.count() == 2U &&
           map_rest.begin()[0U] == map.begin()[1] && map_rest.begin()[1U] == map.begin()[2] && map.count() == 3U &&
           map.begin()[0] == MapEntry{first, second} && map_rest.capacity() + 1U == map.capacity() &&
           set_rest.count() == 2U && set_rest.begin()[0U] == set.begin()[1] && set_rest.begin()[1U] == set.begin()[2] &&
           set.count() == 3U && set.begin()[0] == first && set_rest.capacity() + 1U == set.capacity() &&
           text_rest.count() == 2U && text_rest(0U) == second_char && text_rest(1U) == third_char &&
           text.count() == 3U && text(0U) == first_char && text_rest.capacity() + 1U == text.capacity() &&
           rest(Vector<int, 4>{first}).count() == 0U && rest(Map<int, int, 4>{MapEntry{first, second}}).count() == 0U &&
           rest(Set<int, 4>{first}).count() == 0U && rest(Queue<int, 4>{first}).count() == 0U &&
           rest(String<4>{std::array{first_char}}).count() == 0U && rest(empty_vector).count() == 0U &&
           rest(empty_map).count() == 0U && rest(empty_set).count() == 0U && rest(empty_queue).count() == 0U &&
           rest(empty_text).count() == 0U && rest(zero_capacity_vector).count() == 0U && repeat_rest.count() == 2U &&
           range_rest.count() == 21U && cycle_rest.count() == cycle(vector).count() && iterate_rest.count() > 0U &&
           repeatedly_rest.count() == 2U;
}

} // namespace cljonic::no_heap::probes
