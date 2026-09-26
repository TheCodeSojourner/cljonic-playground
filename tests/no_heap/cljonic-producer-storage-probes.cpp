#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

namespace {

struct AddOne {
    constexpr AddOne() noexcept = default;
    constexpr auto operator()(int value) const noexcept -> int {
        return value + 1;
    }
};

struct Generate {
    constexpr Generate() noexcept = default;
    constexpr auto operator()() const noexcept -> int {
        return 42;
    }
};

} // namespace

[[nodiscard]] auto producer_storage_probe() noexcept -> bool {
    // Default-constructed producers are valid and satisfy NothrowCollectionElement.
    constexpr auto range_ok =
        concepts::NothrowCollectionElement<Range<int>> && Range<int>{}.is_finite() && Range<int>{}.count() == 0U;
    constexpr auto repeat_ok =
        concepts::NothrowCollectionElement<Repeat<int>> && Repeat<int>{}.is_finite() && Repeat<int>{}.count() == 0U;
    constexpr auto repeatedly_ok = concepts::NothrowCollectionElement<Repeatedly<int, Generate>> &&
                                   Repeatedly<int, Generate>{}.is_finite() && Repeatedly<int, Generate>{}.count() == 0U;
    constexpr auto iterate_ok =
        concepts::NothrowCollectionElement<Iterate<int, AddOne>> && !Iterate<int, AddOne>{}.is_finite();
    constexpr auto cycle_ok =
        concepts::NothrowCollectionElement<Cycle<Vector<int, 4>>> && !Cycle<Vector<int, 4>>{}.is_finite();

    // Producers stored in storage-only positions.
    const auto storage = Vector<Repeat<int>, 4>{Repeat<int>{}, Repeat<int>{7, 3U}}.count() == 2U &&
                         Map<int, Repeat<int>, 4>{MapEntry<int, Repeat<int>>{1, Repeat<int>{7, 2U}}}(1).count() == 2U;

    return range_ok && repeat_ok && repeatedly_ok && iterate_ok && cycle_ok && storage;
}

} // namespace cljonic::no_heap::probes