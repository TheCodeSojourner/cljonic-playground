#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto has_next_probe() noexcept -> bool {
    volatile int raw = 40;
    const int first = raw;
    const int second = first + 1;
    const char characters[]{static_cast<char>('A' + first - 40), 'B', '\0'};

    const auto vector = Vector<int, 4>{first, second};
    const auto map = Map<int, int, 4>{MapEntry{first, second}, MapEntry{second, first}};
    const auto set = Set<int, 4>{first, second};
    const auto queue = Queue<int, 4>{first, second};
    const auto text = String<4>{characters};
    const auto original_vector = vector;
    const auto original_map = map;
    const auto original_set = set;
    const auto original_queue = queue;
    const auto original_text = text;

    const bool multiple_elements =
        has_next(vector) && has_next(map) && has_next(set) && has_next(queue) && has_next(text);
    const bool empty_and_singleton =
        !has_next(Vector<int, 4>{}) && !has_next(Map<int, int, 4>{}) && !has_next(Set<int, 4>{}) &&
        !has_next(Queue<int, 4>{}) && !has_next(String<4>{}) && !has_next(Vector<int, 4>{first}) &&
        !has_next(Map<int, int, 4>{MapEntry{first, second}}) && !has_next(Set<int, 4>{first}) &&
        !has_next(Queue<int, 4>{first}) && !has_next(String<4>{"A"});
    const bool inputs_preserved = equal(vector, original_vector) && equal(map, original_map) &&
                                  equal(set, original_set) && equal(queue, original_queue) &&
                                  equal(text, original_text);
    const bool agrees_with_count = has_next(vector) == (count(vector) > 1) && has_next(map) == (count(map) > 1) &&
                                   has_next(set) == (count(set) > 1) && has_next(queue) == (count(queue) > 1) &&
                                   has_next(text) == (count(text) > 1);
    return multiple_elements && empty_and_singleton && inputs_preserved && agrees_with_count &&
           !has_next(Vector<int, 0>{}) && !has_next(Map<int, int, 0>{}) && !has_next(Set<int, 0>{}) &&
           !has_next(Queue<int, 0>{}) && !has_next(String<0>{});
}

} // namespace cljonic::no_heap::probes
