#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto repeatedly_probe() noexcept -> bool {
    // No deduced construction here: Repeatedly has no admitted class template argument deduction (its element type
    // is never deducible from a step), so every deduced form is redirected to the REQ-DIAG-012 diagnostic. The
    // factory is the only construction surface at runtime; the rejected deduction is a compile-time error covered
    // by scripts/check-ctad-diagnostics.py.
    const auto destination = Vector<int, 4>{};
    volatile int runtime_count = 3;
    volatile int runtime_negative = -2;
    const auto counted = repeatedly(static_cast<int>(runtime_count), []() noexcept { return 5; });
    const auto copied = counted;
    const auto counted_result = into(destination, copied);
    const auto uncounted = repeatedly([]() noexcept { return 5; });
    const auto uncounted_result = into(destination, uncounted);
    const auto empty_result = into(destination, repeatedly(0U, []() noexcept { return 5; }));
    auto calls = 0;
    const auto step = [&calls]() noexcept { return ++calls; };
    const auto negative = repeatedly(static_cast<int>(runtime_negative), step);
    const auto negative_result = into(destination, negative);
    return calls == 0 && negative.is_finite() && negative.count() == 0 && negative_result.count() == 0 &&
           counted.is_finite() && counted.count() == 3U && counted_result.count() == 3U && counted_result(0U) == 5 &&
           counted_result(2U) == 5 && uncounted.count() == CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE &&
           !uncounted.is_finite() && !fits_into(destination, uncounted) &&
           uncounted_result.count() == destination.capacity() && uncounted_result(0U) == 5 &&
           uncounted_result(3U) == 5 && empty_result.count() == 0U;
}

} // namespace cljonic::no_heap::probes