#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto seq_probe() noexcept -> bool {
    volatile int vector_first_raw = 21;
    volatile int vector_last_raw = 42;
    volatile int map_key_raw = 5;
    volatile int map_value_raw = 500;
    volatile int queue_front_raw = 63;
    volatile int queue_middle_raw = 64;
    volatile unsigned int producer_count_raw = 2;
    int vector_first = vector_first_raw;
    int vector_last = vector_last_raw;
    int map_key = map_key_raw;
    int map_value = map_value_raw;
    int queue_front = queue_front_raw;
    int queue_middle = queue_middle_raw;
    unsigned int producer_count = producer_count_raw;

    const auto vector = Vector<int, 4>{vector_first, vector_last};
    const auto map = Map<int, int, 4>{MapEntry<int, int>{map_key, map_value}};
    const auto set = Set<int, 4>{7, 8};
    const auto queue = Queue<int, 4>{queue_front, queue_middle};
    const auto text = String<4>{"AB"};
    const auto bounded_range = Range<int>{0, 5};
    const auto finite_repeat = Repeat<int>{producer_count, 9};
    const auto unbounded_repeat = Repeat<int>{7};
    const auto cycled = Cycle<Vector<int, 2>>{Vector<int, 2>{1, 2}};

    const auto seq_vector = seq(vector);
    const auto seq_map = seq(map);
    const auto seq_set = seq(set);
    const auto seq_queue = seq(queue);
    const auto seq_text = seq(text);
    const auto seq_range = seq(bounded_range);
    const auto seq_finite_repeat = seq(finite_repeat);
    const auto seq_unbounded_repeat = seq(unbounded_repeat);
    const auto seq_cycled = seq(cycled);

    // Input preservation.
    const auto vector_unchanged = equal(vector, Vector<int, 4>{21, 42});
    const auto map_unchanged = count(map) == 1 && first(map) == MapEntry<int, int>{5, 500};

    // Copy independence: mutate the source afterwards; the result must not track it.
    auto mutable_source = Vector<int, 4>{30};
    const auto independent = seq(mutable_source);
    mutable_source = Vector<int, 4>{99};

    return equal(seq_vector, Vector<int, 2>{21, 42}) && vector_unchanged && count(seq_map) == 1 &&
           first(seq_map) == MapEntry<int, int>{5, 500} && map_unchanged && count(seq_set) == 2 && seq_queue(0) == 63 &&
           seq_queue(1) == 64 && seq_text(0) == 'A' && seq_text(1) == 'B' && count(seq_range) == 5 &&
           seq_range(4) == 4 && count(seq_finite_repeat) == 2 && seq_finite_repeat(0) == 9 &&
           seq_finite_repeat(1) == 9 && count(seq_unbounded_repeat) == CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE &&
           first(seq_unbounded_repeat) == 7 && count(seq_cycled) == CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE &&
           seq_cycled(0) == 1 && seq_cycled(1) == 2 && seq_cycled(2) == 1 && equal(independent, Vector<int, 1>{30});
}

} // namespace cljonic::no_heap::probes
