#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto iterate_probe() noexcept -> bool {
    const auto destination = Vector<int, 4>{};
    const auto step = [](int value) noexcept { return value + 1; };
    const auto sequence = iterate(step, 0);
    const auto copied = sequence;
    const auto result = into(destination, copied);
    // Deduced construction (REQ-DIAG-012): exercises the admitted class template argument deduction path so the
    // redirecting guide is instantiated in the no-heap build as well. The rejected (redirected) deduction is a
    // compile-time error and is covered by scripts/check-ctad-diagnostics.py.
    const auto deduced_sequence = Iterate{0, step};
    const auto deduced_result = into(destination, deduced_sequence);
    return sequence.count() == CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE && !sequence.is_finite() &&
           !fits_into(destination, sequence) && result.count() == destination.capacity() && result(0U) == 0 &&
           result(1U) == 1 && result(2U) == 2 && result(3U) == 3 && deduced_sequence.count() == sequence.count() &&
           !deduced_sequence.is_finite() && deduced_result.count() == result.count() &&
           deduced_result(3U) == result(3U);
}

} // namespace cljonic::no_heap::probes