#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto implicit_conversions_probe() noexcept -> bool {
    volatile short input = 7;
    const short key = input;
    const auto map = assoc(Map<long long, double, 2>{}, key, key);
    const auto set = Set<long long, 2>{key};
    const auto vector = into(Vector<long long, 2>{}, repeat(key, 2U));
    return contains(map, key) && get(map, key) == 7.0 && is_empty(dissoc(map, key)) && is_empty(disj(set, key)) &&
           get(vector, 1) == key;
}

} // namespace cljonic::no_heap::probes
