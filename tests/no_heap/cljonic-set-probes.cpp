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

    volatile int factory_value_raw = 8;
    volatile short factory_other_raw = 80;
    const int factory_value = factory_value_raw;
    const short factory_other = factory_other_raw;
    const auto factory = hash_set(factory_value, factory_other, 9);
    const auto factory_typed = hash_set<long>(factory_value, factory_other);
    const auto factory_spare = hash_set<int, 4>(factory_value, factory_other);
    const auto factory_reserved_empty = hash_set<int, 4>();
    const auto factory_zero = hash_set<int, 0>();
    const auto factory_duplicate = hash_set(factory_value, factory_value);
    static_assert(std::same_as<std::remove_cvref_t<decltype(factory)>, Set<int, 3>>);
    static_assert(std::same_as<std::remove_cvref_t<decltype(factory_typed)>, Set<long, 2>>);
    static_assert(std::same_as<std::remove_cvref_t<decltype(factory_spare)>, Set<int, 4>>);
    static_assert(std::same_as<std::remove_cvref_t<decltype(factory_reserved_empty)>, Set<int, 4>>);
    static_assert(std::same_as<std::remove_cvref_t<decltype(factory_zero)>, Set<int, 0>>);

    return s1.count() == 1U && s1.contains(1) && s1(1) == 1 && literal.count() == 3U &&
           cljonic::is_empty(zero_capacity) && !zero_capacity.can_conj(1) && cljonic::is_empty(zero_after_conj) &&
           cljonic::is_empty(zero_after_disj) && imported.count() == 2U && imported.contains(4) &&
           imported.contains(5) && static_imported.contains(8) && view_imported.contains(9) && factory.count() == 3U &&
           factory.contains(factory_value) && factory.contains(factory_other) && factory.contains(9) &&
           factory_typed.count() == 2U && factory_typed(8L) == 8L && factory_spare.count() == 2U &&
           factory_spare.contains(factory_value) && factory_reserved_empty.count() == 0U &&
           factory_zero.count() == 0U && factory_duplicate.count() == 1U && factory_duplicate(factory_value) == 8;
}

} // namespace cljonic::no_heap::probes