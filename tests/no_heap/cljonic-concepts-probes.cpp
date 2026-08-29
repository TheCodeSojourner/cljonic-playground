#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto concepts_probe() noexcept -> bool {
    // Storage and element capability concepts.
    constexpr auto storage = concepts::CopyableElement<int> && concepts::NothrowCopyableElement<int> &&
                             concepts::VectorElement<int> && concepts::NothrowVectorElement<int>;

    // Value capability concepts: stable equality and total ordering without
    // floating-point keys/elements.
    constexpr auto values = concepts::StableEqualityComparable<int> && concepts::TotallyOrdered<int> &&
                            !concepts::StableEqualityComparable<double>;

    return storage && values;
}

} // namespace cljonic::no_heap::probes
