#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

#include <ranges>

namespace cljonic::no_heap::probes {

[[nodiscard]] auto set_probe() noexcept -> bool {
    const auto s = Set<int, 4>{};
    const auto s1 = s.conj(1);
    const auto literal = Set{1, 2, 2, 3};
    const auto zero_capacity = Set<int, 0>{};
    const auto zero_after_conj = zero_capacity.conj(1);
    const auto zero_after_disj = zero_capacity.disj(1);
    const int static_source[] = {8, 9};
    const auto static_imported = Set<int, 2>{std::span<const int, 2>{static_source}};
    const int source[] = {4, 5, 6};
    const std::span<const int> dynamic_source{source};
    const auto imported = Set<int, 2>{dynamic_source};
    const auto view_imported = Set<int, 2>{std::views::all(static_source)};
    return s1.count() == 1U && s1.contains(1) && s1(1) == 1 && literal.count() == 3U && zero_capacity.is_empty() &&
           !zero_capacity.can_conj(1) && zero_after_conj.is_empty() && zero_after_disj.is_empty() &&
           imported.count() == 2U && imported.contains(4) && imported.contains(5) && static_imported.contains(8) &&
           view_imported.contains(9);
}

} // namespace cljonic::no_heap::probes