#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto range_probe() noexcept -> bool {
    const auto ascending = Range<int>{0, 5};
    const auto descending = Range<int>{5, 0, -1};
    const auto unbounded = Range<int>{0, 5, 0};
    const auto huge = Range<long>{1000000000L};
    // Deduced constructions (REQ-DIAG-012), one per supported arity: these exercise the admitted class template
    // argument deduction path so its redirecting guides are instantiated in the no-heap build as well. The
    // rejected (redirected) deduction is a compile-time error and is covered by scripts/check-ctad-diagnostics.py.
    const auto deduced_ascending = Range{0, 5};
    const auto deduced_descending = Range{5, 0, -1};
    const auto deduced_huge = Range{1000000000L};
    const auto factory_end = range(5);
    const auto factory_bounds = range(0, 5);
    const auto factory_step = range(5, 0, -1);
    const auto factory_empty = range<int>();
    const auto factory_typed_end = range<long>(5);
    const auto factory_typed_bounds = range<long>(0, 5);
    const auto factory_typed_step = range<long>(5, 0, -1);
    return ascending.count() == 5U && ascending.contains(3U) && descending.count() == 5U && descending.contains(0U) &&
           unbounded.count() == CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE && unbounded.contains(0U) &&
           huge.contains(CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE - 1U) &&
           !huge.contains(CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE) &&
           deduced_ascending.count() == ascending.count() && deduced_ascending.contains(3U) &&
           deduced_descending.count() == descending.count() && deduced_descending.contains(0U) &&
           deduced_huge.count() == huge.count() && deduced_huge.contains(huge.count() - 1U) &&
           factory_end == ascending && factory_bounds == ascending && factory_step == descending &&
           count(factory_empty) == 0U && count(factory_typed_end) == 5U && count(factory_typed_bounds) == 5U &&
           count(factory_typed_step) == 5U;
}

} // namespace cljonic::no_heap::probes
