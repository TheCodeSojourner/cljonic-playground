#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto fits_into_probe() noexcept -> bool {
    const auto fitting_destination = Vector<int, 8>{};
    const auto small_destination = Vector<int, 3>{};
    const auto source = Range<int>{0, 5};
    const auto duplicates = Vector<int, 4>{1, 2, 2, 1};
    using Entry = MapEntry<int, int>;
    const auto updates = Vector<Entry, 3>{Entry{1, 10}, Entry{1, 11}, Entry{1, 12}};
    const auto set_destination = Set<int, 2>{1};
    const auto map_destination = Map<int, int, 1>{Entry{1, 0}};
    return fits_into(fitting_destination, source) && !fits_into(small_destination, source) &&
           fits_into(Queue<int, 8>{}, source) && !fits_into(Queue<int, 3>{}, source) &&
           fits_into(set_destination, duplicates) && !fits_into(Set<int, 1>{1}, duplicates) &&
           fits_into(map_destination, updates) && !fits_into(map_destination, Vector<Entry, 1>{Entry{2, 20}}) &&
           fits_into(String<4>{"x"}, String<4>{"ab"}) && !fits_into(String<2>{"x"}, String<4>{"ab"}) &&
           !fits_into(set_destination, repeat(1)) && !fits_into(map_destination, repeat(Entry{1, 1})) &&
           fits_into(Set<int, 0>{}, Vector<int, 0>{}) && fits_into(Map<int, int, 0>{}, Vector<Entry, 0>{});
}

} // namespace cljonic::no_heap::probes
