#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto contains_probe() noexcept -> bool {
    using cljonic::assoc;
    using cljonic::conj;
    using cljonic::contains;
    using cljonic::Map;
    using cljonic::Set;
    using cljonic::String;
    using cljonic::Vector;

    const auto m = assoc(Map<int, int, 4>{}, 1, 100);
    const auto s = conj(Set<int, 4>{}, 5);
    const auto v = Vector<int, 4>{10, 20, 30};
    const auto st = String<8>{"abc"};

    return contains(m, 1) && !contains(m, 2) && contains(s, 5) && !contains(s, 8) && contains(v, 0U) &&
           !contains(v, 3U) && contains(st, 1U) && !contains(st, 3U);
}

} // namespace cljonic::no_heap::probes