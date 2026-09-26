#include "probes.hpp"

namespace {
[[nodiscard]] auto no_heap_probe() noexcept -> bool {
    using namespace cljonic::no_heap::probes;
    return vector_storage() && vector_capacity() && assoc_probe() && can_assoc_probe() && can_conj_probe() &&
           concepts_probe() && cycle_probe() && config_probe() && conj_probe() && contains_probe() &&
           max_count_probe() && core_probe() && count_probe() && disj_probe() && dissoc_probe() && fits_into_probe() &&
           get_probe() && into_probe() && iterate_probe() && is_empty_probe() && map_entry_probe() && map_probe() &&
           parameters_equal_probe() && peek_probe() && pop_probe() && queue_probe() && range_probe() &&
           repeat_probe() && repeatedly_probe() && producer_storage_probe() && set_probe() && string_probe();
}
} // namespace

auto main() -> int {
    return no_heap_probe() ? 0 : 1;
}
