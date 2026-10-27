#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto variant_probe() noexcept -> bool {
    // cljonic::Variant storage and alternative-strict equality are non-allocating.
    // No deduced construction here: Variant has no admitted class template argument deduction (its alternatives are
    // never deducible), so every deduced form is redirected to the REQ-DIAG-012 diagnostic. Name the alternatives
    // explicitly, either through the type or the factory; rejected deduction is covered by
    // scripts/check-ctad-diagnostics.py.
    const auto a = Variant<int, long>{1};
    const auto b = Variant<int, long>{1};
    const auto c = Variant<int, long>{1L};
    const auto factory_value = variant<int, long>(1L);
    const auto v = Vector<Variant<int, long>, 4>{a, b};

    return a.index() == 0U && a.holds<int>() && a == b && a != c && factory_value == c && v.count() == 2U &&
           (v(0) == v(1));
}

} // namespace cljonic::no_heap::probes
