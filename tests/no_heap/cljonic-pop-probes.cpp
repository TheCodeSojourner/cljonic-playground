#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto pop_probe() noexcept -> bool {
    volatile int first_raw = 10;
    volatile int last_raw = 20;
    const int first = first_raw;
    const int last = last_raw;

    const auto vector = Vector<int, 4>{first, last};
    const auto vector_after_pop = pop(vector);
    const auto vector_with_one_element = pop(Vector<int, 4>{first});
    const auto empty_vector = Vector<int, 4>{};
    const auto zero_capacity_vector = Vector<int, 0>{};

    const auto queue = Queue<int, 4>{first, last};
    const auto queue_after_pop = pop(queue);
    const auto queue_with_one_element = pop(Queue<int, 4>{first});
    const auto empty_queue = Queue<int, 4>{};

    return peek(vector_after_pop) == first && vector_after_pop.count() == 1U && peek(vector) == last &&
           vector.count() == 2U && vector_after_pop.capacity() == vector.capacity() &&
           vector_with_one_element.count() == 0U && pop(empty_vector) == empty_vector &&
           pop(zero_capacity_vector) == zero_capacity_vector && peek(queue_after_pop) == last &&
           queue_after_pop.count() == 1U && peek(queue) == first && queue.count() == 2U &&
           queue_after_pop.capacity() == queue.capacity() && queue_with_one_element.count() == 0U &&
           pop(empty_queue) == empty_queue;
}

} // namespace cljonic::no_heap::probes
