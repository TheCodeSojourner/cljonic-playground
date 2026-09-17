#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto fits_into_probe() noexcept -> bool {
    const auto fitting_destination = Vector<int, 8>{};
    const auto small_destination = Vector<int, 3>{};
    const auto source = Range<int>{0, 5};
    return fits_into(fitting_destination, source) && !fits_into(small_destination, source);
}

} // namespace cljonic::no_heap::probes
