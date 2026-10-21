#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto peek_probe() noexcept -> bool {
    volatile int vector_first_raw = 21;
    volatile int vector_last_raw = 42;
    volatile int queue_front_raw = 63;
    volatile int queue_back_raw = 84;
    int vector_first = vector_first_raw;
    int vector_last = vector_last_raw;
    int queue_front = queue_front_raw;
    int queue_back = queue_back_raw;
    const auto vector = Vector<int, 4>{vector_first, vector_last};
    const auto queue = conj(conj(Queue<int, 4>{}, queue_front), queue_back);

    const auto empty_vector = empty(vector);
    const auto empty_queue = empty(queue);
    const auto zero_capacity_vector = Vector<int, 0>{};
    const auto zero_capacity_queue = Queue<int, 0>{};

    return peek(vector) == 42 && peek(queue) == 63 && peek(empty_vector) == 0 && peek(empty_queue) == 0 &&
           peek(zero_capacity_vector) == 0 && peek(zero_capacity_queue) == 0 && count(vector) == 2 &&
           peek(vector) == 42 && count(queue) == 2 && peek(queue) == 63;
}

} // namespace cljonic::no_heap::probes
