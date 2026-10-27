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
    volatile int runtime_initial = 0;
    const auto initial_value = static_cast<int>(runtime_initial);
    auto mutable_step = [value = initial_value]() mutable noexcept {
        value += 3;
        return value;
    };
    const auto mutable_producer = repeatedly(2, mutable_step);
    const auto mutable_first = into(Vector<int, 2>{}, mutable_producer);
    const auto mutable_second = into(Vector<int, 2>{}, mutable_producer);
    volatile int runtime_zero_count = 0;
    auto mutable_empty_calls = 0;
    auto mutable_empty_step = [value = initial_value, &mutable_empty_calls]() mutable noexcept {
        ++mutable_empty_calls;
        return ++value;
    };
    const auto mutable_empty = repeatedly(static_cast<int>(runtime_zero_count), mutable_empty_step);
    const auto mutable_empty_result = into(Vector<int, 2>{}, mutable_empty);
    volatile int runtime_shared_initial = 0;
    auto shared_value = static_cast<int>(runtime_shared_initial);
    const auto shared_step = [&shared_value]() noexcept {
        shared_value += 3;
        return shared_value;
    };
    const auto shared_producer = repeatedly(2, shared_step);
    const auto shared_first = into(Vector<int, 2>{}, shared_producer);
    const auto shared_second = into(Vector<int, 2>{}, shared_producer);
    return calls == 0 && negative.is_finite() && negative.count() == 0 && negative_result.count() == 0 &&
           counted.is_finite() && counted.count() == 3U && counted_result.count() == 3U && counted_result(0U) == 5 &&
           counted_result(2U) == 5 && uncounted.count() == CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE &&
           !uncounted.is_finite() && !fits_into(destination, uncounted) &&
           uncounted_result.count() == destination.capacity() && uncounted_result(0U) == 5 &&
           uncounted_result(3U) == 5 && empty_result.count() == 0U && mutable_first(0U) == 3 &&
           mutable_first(1U) == 6 && mutable_second(0U) == 3 && mutable_second(1U) == 6 && shared_first(0U) == 3 &&
           shared_first(1U) == 6 && shared_second(0U) == 9 && shared_second(1U) == 12 &&
           mutable_empty_result.count() == 0U && mutable_empty_calls == 0;
}

} // namespace cljonic::no_heap::probes