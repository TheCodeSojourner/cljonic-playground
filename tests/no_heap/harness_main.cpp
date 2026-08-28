#include "probes.hpp"

namespace {
[[nodiscard]] auto no_heap_probe() noexcept -> bool {
  using namespace cljonic::no_heap::probes;
  return vector_storage() && vector_capacity() && assoc_probe() &&
         can_assoc_probe() && can_conj_probe() && concepts_probe() &&
         config_probe() && conj_probe() && max_count_probe() && core_probe() &&
         count_probe() && disj_probe() && dissoc_probe() && empty_probe() &&
         first_probe() && get_probe() && is_empty_probe() &&
         map_entry_probe() && map_probe() && next_probe() &&
         not_empty_probe() && peek_probe() && pop_probe() && queue_probe() &&
         rest_probe() && seq_probe() && set_probe() && string_probe();
}
} // namespace

auto main() -> int { return no_heap_probe() ? 0 : 1; }
