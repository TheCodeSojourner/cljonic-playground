#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto peek_probe() noexcept -> bool {
    const auto q = conj(Queue<int, 4>{}, 42);
    return peek(q) == 42;
}

} // namespace cljonic::no_heap::probes
