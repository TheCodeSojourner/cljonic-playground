#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto equal_probe() noexcept -> bool {
    // General value equality (REQ-FN-002G): sequential prefix comparison over
    // the supported domain, all constexpr-capable, non-allocating, noexcept.
    constexpr auto scalar_ok = equal(1, 1) && !equal(1, 2);
    constexpr auto sequential_ok =
        equal(Vector<int, 4>{1, 2, 3}, Range<int>{1, 4, 1}) && equal(Vector<int, 4>{1, 2, 3}, Queue<int, 4>{1, 2, 3}) &&
        equal(Vector<int, 4>{1, 1, 1}, Repeat<int>{1, 3U}) &&
        !equal(cycle(Vector<int, 3>{1, 2}), Vector<int, 4>{1, 2, 3}) && equal(Repeat<int>{7}, Repeat<int>{7}) &&
        equal(Vector<int, 4>{}, Range<int>{5, 5, 1});
    constexpr auto map_ok =
        equal(Map<int, int, 4>{MapEntry<int, int>{1, 10}, MapEntry<int, int>{2, 20}},
              Map<int, int, 8>{MapEntry<int, int>{2, 20}, MapEntry<int, int>{1, 10}}) &&
        !equal(Map<int, int, 4>{MapEntry<int, int>{1, 10}}, Map<int, int, 4>{MapEntry<int, int>{1, 11}});
    constexpr auto set_ok =
        equal(Set<int, 4>{3, 1, 2}, Set<int, 8>{1, 2, 3}) && !equal(Set<int, 4>{1, 2}, Set<int, 4>{1, 4});
    constexpr auto string_ok = equal(String<8>{"abc"}, String<16>{"abc"}) && !equal(String<8>{"abc"}, String<8>{"abd"});
    constexpr auto nested_ok =
        equal(Vector<Range<int>, 4>{Range<int>{1, 3, 1}}, Vector<Range<int>, 4>{Range<int>{1, 3, 1}}) &&
        !equal(Vector<Repeat<int>, 2>{Repeat<int>{7, 3U}}, Vector<Repeat<int>, 2>{Repeat<int>{7}});

    // Runtime probe defeats constexpr folding so the no-heap artifact truly
    // executes the comparisons at runtime.
    volatile int first_raw = 1;
    const auto first = first_raw;
    volatile int second_raw = 2;
    const auto second = second_raw;
    auto runtime_ok =
        equal(first, first) && !equal(first, second) &&
        equal(Vector<int, 4>{first, second}, Queue<int, 4>{first, second}) &&
        !equal(Vector<int, 4>{first, second}, Range<int>{first, 3, 1}) &&
        equal(Map<int, int, 4>{MapEntry<int, int>{first, 10}}, Map<int, int, 8>{MapEntry<int, int>{first, 10}}) &&
        equal(Set<int, 4>{first, second}, Set<int, 8>{second, first}) && equal(String<8>{"abc"}, String<16>{"abc"}) &&
        equal(Repeat<int>{first}, Repeat<int>{first});

    return scalar_ok && sequential_ok && map_ok && set_ok && string_ok && nested_ok && runtime_ok;
}

} // namespace cljonic::no_heap::probes