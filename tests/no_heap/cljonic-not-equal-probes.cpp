#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto not_equal_probe() noexcept -> bool {
    volatile int first_raw = 1;
    const auto first = first_raw;
    volatile int second_raw = 2;
    const auto second = second_raw;
    volatile int third_raw = 3;
    const auto third = third_raw;
    volatile char character_raw = 'a';
    const auto character = character_raw;

    const auto scalar_ok = !not_equal(first) && not_equal(first, second) && !not_equal(first, first) &&
                           not_equal(first, second, first) && !not_equal(first, first, first);

    const auto vector = Vector<int, 4>{first, second};
    const auto sequential_ok =
        not_equal(vector, Range<int>{first, second, 1}) && !not_equal(vector, Range<int>{first, third, 1}) &&
        !not_equal(vector, Queue<int, 4>{first, second}) && not_equal(vector, Repeat<int>{first}) &&
        !not_equal(Repeat<int>{first}, Repeat<int>{first}) && not_equal(Repeat<int>{first}, Repeat<int>{second});

    const auto map = Map<int, int, 4>{MapEntry<int, int>{first, second}};
    const auto map_ok = !not_equal(map, Map<int, int, 8>{MapEntry<int, int>{first, second}}) &&
                        not_equal(map, Map<int, int, 4>{MapEntry<int, int>{first, first}});

    const auto set = Set<int, 4>{first, second};
    const auto set_ok = !not_equal(set, Set<int, 8>{second, first}) && not_equal(set, Set<int, 4>{first, third});

    const char same_text[] = {character, 'b', '\0'};
    const char different_text[] = {character, 'c', '\0'};
    const auto string = String<2>{same_text};
    const auto string_ok = !not_equal(string, String<3>{same_text}) && not_equal(string, String<2>{different_text});

    const auto variant_ok = not_equal(Variant<int, long>{first}, Variant<int, long>{second});
    const auto nested_ok = not_equal(Vector<Variant<int, long>, 2>{Variant<int, long>{first}},
                                     Vector<Variant<int, long>, 2>{Variant<int, long>{second}});
    const auto inputs_preserved = !not_equal(vector, Vector<int, 4>{first, second}) &&
                                  !not_equal(map, Map<int, int, 4>{MapEntry<int, int>{first, second}}) &&
                                  !not_equal(set, Set<int, 4>{first, second}) &&
                                  !not_equal(string, String<2>{same_text}) &&
                                  !not_equal(Variant<int, long>{first}, Variant<int, long>{first});

    return scalar_ok && sequential_ok && map_ok && set_ok && string_ok && variant_ok && nested_ok && inputs_preserved;
}

} // namespace cljonic::no_heap::probes
