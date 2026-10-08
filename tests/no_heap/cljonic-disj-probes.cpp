#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto disj_probe() noexcept -> bool {
    const auto s = Set{42};
    const auto s1 = disj(s, 42, 42, 99);
    const auto s2 = disj(s1);
    return cljonic::is_empty(s1) && cljonic::is_empty(s2);
}

} // namespace cljonic::no_heap::probes
