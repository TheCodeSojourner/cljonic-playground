#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

#include <ranges>

namespace cljonic::no_heap::probes {

[[nodiscard]] auto string_probe() noexcept -> bool {
    const auto s = String<8>{"Hello"};
    const char static_source[] = {'A', 'B'};
    const auto static_imported = String<2>{std::span<const char, 2>{static_source}};
    const char source[] = {'W', 'o', 'r', 'l', 'd'};
    const auto imported = String<3>{std::views::all(source)};
    return s.count() == 5U && s(0) == 'H' && imported.view() == "Wor" && static_imported.view() == "AB";
}

} // namespace cljonic::no_heap::probes