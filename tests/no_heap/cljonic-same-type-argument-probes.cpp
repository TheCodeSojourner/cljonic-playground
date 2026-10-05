#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

#include <span>

namespace cljonic::no_heap::probes {

[[nodiscard]] auto same_type_argument_probe() noexcept -> bool {
    // Same-Type-Argument Is One Element: a single constructor argument whose
    // type is exactly the element type is pack construction (one element),
    // never source materialization. Materialization remains into, preflighted
    // by fits_into.
    constexpr auto range = Range<int>{0, 3, 1};
    constexpr auto vector_of_one_range = Vector<Range<int>, 4>{range};
    static_assert(vector_of_one_range.count() == 1U);

    constexpr auto inner = Vector<Repeat<int>, 2>{Repeat<int>{9, 2U}, Repeat<int>{}};
    constexpr auto nested = Vector<Vector<Repeat<int>, 2>, 2>{inner};
    static_assert(nested.count() == 1U);

    // Span-of-producers remains source construction (span is not the element
    // type), preserving the RangeViewMaterialization path without heap use.
    static constexpr Range<int> ranges[] = {Range<int>{0, 2, 1}, Range<int>{5, 8, 1}};
    constexpr std::span<const Range<int>, 2> range_span{ranges};
    constexpr auto vector_from_span = Vector<Range<int>, 4>{range_span};
    static_assert(vector_from_span.count() == 2U);

    // Runtime (non-constexpr) forms exercise the same paths with the heap
    // poison active.
    const auto runtime_one = Vector<Range<int>, 4>{Range<int>{1, 4, 1}};
    const auto runtime_span = Vector<Range<int>, 4>{range_span};
    return vector_of_one_range(0U) == range && nested(0U).count() == 2U &&
           vector_from_span(1U) == Range<int>{5, 8, 1} && runtime_one.count() == 1U && runtime_span.count() == 2U;
}

} // namespace cljonic::no_heap::probes