#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto into_probe() noexcept -> bool {
    const auto destination = Vector<int, 8>{};
    const auto source = Range<int>{0, 5};
    const auto result = into(destination, source);

    const auto vector_source = Vector<short, 4>{short{1}, short{2}, short{3}};
    const auto populated = Vector<long long, 4>{99LL, 98LL};
    const auto vector_result = into(populated, vector_source);
    const auto queue_source = Queue<int, 4>{10, 20, 30}.pop();
    const auto queue_result = into(Vector<int, 4>{99}, queue_source);
    const auto string_source = String<4>{"ab"};
    const auto string_result = into(Vector<char, 4>{'x'}, string_source);

    const auto set_source = Set<int, 4>{3, 1, 2}.disj(1);
    const auto set_result = into(Vector<int, 4>{99}, set_source);
    auto set_index = std::size_t{1};
    for (const auto item : set_source) {
        if (set_result(set_index++) != item) {
            return false;
        }
    }

    using Entry = MapEntry<int, int>;
    const auto map_source = Map<int, int, 4>{Entry{3, 30}, Entry{1, 10}, Entry{2, 20}}.dissoc(1);
    const auto map_result = into(Vector<Entry, 4>{Entry{9, 90}}, map_source);
    auto map_index = std::size_t{1};
    for (const auto& entry : map_source) {
        if (map_result(map_index++) != entry) {
            return false;
        }
    }

    const auto full = Vector<int, 2>{7, 8};
    const auto full_result = into(full, source);
    const auto empty_result = into(Vector<int, 4>{}, Vector<int, 4>{});
    const auto unchanged_result = into(full, Vector<int, 4>{});
    const auto unbounded_result = into(Vector<int, 3>{9}, Range<int>{0, 5, 0});
    return result.count() == 5U && result(0U) == 0 && result(4U) == 4 && vector_result.count() == 4U &&
           vector_result(0U) == 99LL && vector_result(1U) == 98LL && vector_result(2U) == 1LL &&
           vector_result(3U) == 2LL && populated.count() == 2U && vector_source.count() == 3U &&
           queue_result.count() == 3U && queue_result(0U) == 99 && queue_result(1U) == 20 && queue_result(2U) == 30 &&
           queue_source.count() == 2U && queue_source.peek() == 20 && string_result.count() == 3U &&
           string_result(0U) == 'x' && string_result(1U) == 'a' && string_result(2U) == 'b' &&
           string_source.count() == 2U && set_result.count() == 3U && set_result(0U) == 99 &&
           set_source.count() == 2U && map_result.count() == 3U && map_result(0U) == Entry{9, 90} &&
           map_source.count() == 2U && full_result.count() == 2U && full_result(0U) == 7 && full_result(1U) == 8 &&
           empty_result.count() == 0U && unchanged_result.count() == 2U && unchanged_result(0U) == 7 &&
           unchanged_result(1U) == 8 && unbounded_result.count() == 3U && unbounded_result(0U) == 9 &&
           unbounded_result(1U) == 0 && unbounded_result(2U) == 0;
}

} // namespace cljonic::no_heap::probes
