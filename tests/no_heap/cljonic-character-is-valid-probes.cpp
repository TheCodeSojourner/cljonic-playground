#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

#include <limits>

namespace cljonic::no_heap::probes {

[[nodiscard]] auto character_is_valid_probe() noexcept -> bool {
    return character_is_valid(65) && !character_is_valid(std::numeric_limits<unsigned long long>::max());
}

} // namespace cljonic::no_heap::probes