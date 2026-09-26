#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto parameters_equal_probe() noexcept -> bool {
    // Producer parameter equality is O(1), constexpr, non-allocating, and
    // never traverses a produced sequence (REQ-FN-014B).
    constexpr auto range_ok = parameters_equal(Range<int>{1, 5, 2}, Range<int>{1, 5, 2}) &&
                              !parameters_equal(Range<int>{1, 5, 2}, Range<int>{1, 5, 3}) &&
                              !parameters_equal(Range<int>{0, 5, 0}, Range<int>{0, 7, 0});
    constexpr auto repeat_ok = parameters_equal(Repeat<int>{7, 3U}, Repeat<int>{7, 3U}) &&
                               !parameters_equal(Repeat<int>{7, 3U}, Repeat<int>{7}) &&
                               !parameters_equal(Repeat<int>{7, 3U}, Repeat<int>{7, 2U});
    constexpr auto cycle_ok = parameters_equal(cycle(Vector<int, 3>{1, 2, 3}), cycle(Vector<int, 3>{1, 2, 3})) &&
                              !parameters_equal(cycle(Vector<int, 3>{1, 2, 3}), cycle(Vector<int, 3>{1, 2, 4}));

    // operator== has identical producer parameter equality semantics.
    constexpr auto operator_eq_ok = (Range<int>{1, 5, 2} == Range<int>{1, 5, 2}) &&
                                    !(Range<int>{1, 5, 2} == Range<int>{1, 5, 3}) &&
                                    (Repeat<int>{7, 3U} == Repeat<int>{7, 3U}) &&
                                    (cycle(Vector<int, 3>{1, 2, 3}) == cycle(Vector<int, 3>{1, 2, 3})) &&
                                    !(cycle(Vector<int, 3>{1, 2, 3}) == cycle(Vector<int, 3>{1, 2, 4}));

    // The three producers admit StableEqualityComparable admission so they may
    // be used as set elements and map keys. A float-bearing Repeat is excluded.
    constexpr auto admission_ok = concepts::StableEqualityComparable<Range<int>> &&
                                  concepts::StableEqualityComparable<Repeat<int>> &&
                                  concepts::StableEqualityComparable<Cycle<Vector<int, 3>>> &&
                                  !concepts::StableEqualityComparable<Repeat<double>> &&
                                  !concepts::StableEqualityComparable<Iterate<int, int (*)(int) noexcept>> &&
                                  !concepts::StableEqualityComparable<Repeatedly<int, int (*)() noexcept>>;

    // Set admission with producers as elements.
    constexpr auto set_ok = Set<Range<int>, 4>{Range<int>{1, 5, 2}, Range<int>{1, 5, 3}}.count() == 2U &&
                            Set<Repeat<int>, 4>{Repeat<int>{7, 3U}, Repeat<int>{8, 2U}}.count() == 2U;

    // Runtime probe defeats constexpr folding so the no-heap artifact truly
    // executes the comparisons at runtime.
    volatile int start_raw = 1;
    const auto start = start_raw;
    volatile int step_raw = 2;
    const auto step = step_raw;
    auto runtime_ok = parameters_equal(Range<int>{start, 5, step}, Range<int>{1, 5, 2}) &&
                      !parameters_equal(Range<int>{start, 5, step + 1}, Range<int>{1, 5, 2}) &&
                      (Range<int>{start, 5, step} == Range<int>{1, 5, 2});

    return range_ok && repeat_ok && cycle_ok && operator_eq_ok && admission_ok && set_ok && runtime_ok;
}

} // namespace cljonic::no_heap::probes