#include "probes.hpp"

namespace {
[[nodiscard]] auto no_heap_probe() noexcept -> bool {
  return cljonic::no_heap::probes::count_on_vector() &&
         cljonic::no_heap::probes::vector_count();
}
} // namespace

auto main() -> int { return no_heap_probe() ? 0 : 1; }
