#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto set_probe() noexcept -> bool {
    const auto s = Set<int, 4>{};
    const auto s1 = s.conj(1);
    const auto literal = Set{1, 2, 2, 3};
    const auto zero_capacity = Set<int, 0>{};
    const auto zero_after_conj = zero_capacity.conj(1);
    const auto zero_after_disj = zero_capacity.disj(1);
    return s1.count() == 1U && s1.contains(1) && s1(1) == 1 && literal.count() == 3U && zero_capacity.is_empty() &&
           !zero_capacity.can_conj(1) && zero_after_conj.is_empty() && zero_after_disj.is_empty();
}

} // namespace cljonic::no_heap::probes