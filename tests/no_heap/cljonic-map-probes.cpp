#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

#include <concepts>
#include <ranges>
#include <type_traits>
#include <variant>

namespace cljonic::no_heap::probes {

[[nodiscard]] auto map_probe() noexcept -> bool {
    const auto m = Map<int, int, 4>{};
    const auto m1 = m.assoc(1, 10);
    const auto literal = Map{MapEntry<int, int>{1, 10}, MapEntry<int, int>{2, 20}};
    volatile int factory_key_raw = 8;
    volatile int factory_value_raw = 80;
    const int factory_key = factory_key_raw;
    const int factory_value = factory_value_raw;
    const auto factory = hash_map(factory_key, factory_value, 9, 90);
    const auto factory_typed = hash_map<long, long>(factory_key, factory_value);
    const auto factory_spare = hash_map<int, int, 4>(factory_key, factory_value);
    const auto factory_reserved_empty = hash_map<int, int, 4>();
    const auto factory_zero = hash_map<int, int, 0>();
    const auto factory_duplicate = hash_map(1, 10, 1, 20);
    static_assert(std::same_as<std::remove_cvref_t<decltype(factory)>, Map<int, int, 2>>);
    static_assert(std::same_as<std::remove_cvref_t<decltype(factory_typed)>, Map<long, long, 1>>);
    static_assert(std::same_as<std::remove_cvref_t<decltype(factory_spare)>, Map<int, int, 4>>);
    static_assert(std::same_as<std::remove_cvref_t<decltype(factory_reserved_empty)>, Map<int, int, 4>>);
    static_assert(std::same_as<std::remove_cvref_t<decltype(factory_zero)>, Map<int, int, 0>>);
    const MapEntry<int, int> static_source[] = {{6, 60}, {7, 70}};
    const auto static_imported = Map<int, int, 2>{std::span<const MapEntry<int, int>, 2>{static_source}};
    const MapEntry<int, int> source[] = {{3, 30}, {4, 40}, {5, 50}};
    const std::span<const MapEntry<int, int>> dynamic_source{source};
    const auto imported = Map<int, int, 2>{dynamic_source};
    const auto view_imported = Map<int, int, 2>{std::views::all(static_source)};

    // Alternative-strict variant map keys (REQ-CAP-010).
    using alt = cljonic::Variant<int, long>;
    const auto variant_map = Map<alt, int, 4>{MapEntry<alt, int>{alt{1}, 100}};
    const auto variant_hit = variant_map(alt{1}) == 100;
    const auto variant_miss = variant_map(alt{1L}, -1) == -1;

    return m1.count() == 1U && m1.contains(1) && m1(1) == 10 && literal.count() == 2U && factory.count() == 2U &&
           factory(factory_key) == factory_value && factory(9) == 90 && factory_typed.count() == 1U &&
           factory_typed(8L) == 80L && factory_spare.count() == 1U && factory_spare(factory_key) == factory_value &&
           factory_reserved_empty.count() == 0U && factory_zero.count() == 0U && factory_duplicate.count() == 1U &&
           factory_duplicate(1) == 20 && imported.count() == 2U && imported(3) == 30 && imported(4) == 40 &&
           static_imported(6) == 60 && view_imported(7) == 70 && variant_hit && variant_miss;
}

} // namespace cljonic::no_heap::probes
