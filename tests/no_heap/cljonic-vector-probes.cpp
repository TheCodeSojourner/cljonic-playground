#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

#include <ranges>

namespace cljonic::no_heap::probes {
[[nodiscard]] auto vector_capacity() noexcept -> bool {
    const Vector<int, 4> values{1, 2, 3};
    const int static_source[] = {8, 9};
    const auto static_imported = Vector<int, 2>{std::span<const int, 2>{static_source}};
    const int source[] = {4, 5, 6, 7};
    const std::span<const int> dynamic_source{source};
    const auto imported = Vector<int, 2>{dynamic_source};
    const auto view_imported = Vector<int, 2>{std::views::all(static_source)};
    return values.capacity() == 4U && values.count() <= values.capacity() && imported.count() == 2U &&
           imported(0) == 4 && imported(1) == 5 && static_imported(0) == 8 && view_imported(1) == 9;
}

} // namespace cljonic::no_heap::probes
