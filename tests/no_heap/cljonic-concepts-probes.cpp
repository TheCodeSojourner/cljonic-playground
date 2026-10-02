#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

#include <variant>

namespace cljonic::no_heap::probes {

[[nodiscard]] auto concepts_probe() noexcept -> bool {
    // Storage and element capability concepts.
    constexpr auto storage = concepts::CopyableElement<int> && concepts::NothrowCollectionElement<int>;

    // Value capability concepts: stable equality and total ordering without
    // floating-point keys/elements.
    constexpr auto values = concepts::StableEqualityComparable<int> && concepts::TotallyOrdered<int> &&
                            !concepts::StableEqualityComparable<double>;

    // Composite stable equality (REQ-CAP-010): variants over scalars and
    // collections admit stable equality; floating-point and callable components
    // and function-pointer components are rejected.
    using alt = cljonic::Variant<int, long>;
    constexpr auto composite = concepts::StableEqualityComparable<alt> &&
                               concepts::StableEqualityComparable<cljonic::Variant<int, Vector<int, 4>>> &&
                               !concepts::StableEqualityComparable<cljonic::Variant<int, double>> &&
                               !concepts::StableEqualityComparable<cljonic::Variant<int, int (*)(int)>> &&
                               concepts::StableEqualityComparable<Vector<int, 4>> &&
                               !concepts::StableEqualityComparable<Vector<double, 4>> &&
                               !concepts::StableEqualityComparable<MapEntry<int, double>>;

    // Collection equality (REQ-COLL-021) is usable in constant expressions.
    constexpr Vector<int, 4> v{1, 2, 3};
    constexpr Set<int, 4> s{3, 1, 2};
    constexpr Map<int, int, 4> m{MapEntry<int, int>{1, 10}, MapEntry<int, int>{2, 20}};
    constexpr auto equality = v == Vector<int, 4>{1, 2, 3} && s == Set<int, 4>{1, 2, 3} &&
                              m == Map<int, int, 4>{MapEntry<int, int>{2, 20}, MapEntry<int, int>{1, 10}};

    return storage && values && composite && equality;
}

} // namespace cljonic::no_heap::probes
