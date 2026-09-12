#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

#include <ranges>

namespace cljonic::no_heap::probes {

[[nodiscard]] auto map_probe() noexcept -> bool {
    const auto m = Map<int, int, 4>{};
    const auto m1 = m.assoc(1, 10);
    const auto literal = Map{MapEntry<int, int>{1, 10}, MapEntry<int, int>{2, 20}};
    const MapEntry<int, int> static_source[] = {{6, 60}, {7, 70}};
    const auto static_imported = Map<int, int, 2>{std::span<const MapEntry<int, int>, 2>{static_source}};
    const MapEntry<int, int> source[] = {{3, 30}, {4, 40}, {5, 50}};
    const std::span<const MapEntry<int, int>> dynamic_source{source};
    const auto imported = Map<int, int, 2>{dynamic_source};
    const auto view_imported = Map<int, int, 2>{std::views::all(static_source)};
    return m1.count() == 1U && m1.contains(1) && m1(1) == 10 && literal.count() == 2U && imported.count() == 2U &&
           imported(3) == 30 && imported(4) == 40 && static_imported(6) == 60 && view_imported(7) == 70;
}

} // namespace cljonic::no_heap::probes
