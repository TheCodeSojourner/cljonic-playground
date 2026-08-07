#include "poison.hpp"
#include "probes.hpp"
#include <cljonic-core.hpp>

namespace cljonic::no_heap::probes {
[[nodiscard]] auto vector_state_classification() noexcept -> bool {
  Vector<int, 4> values{1, 2, 3};
  return values.state() == vector_state::populated;
}

[[nodiscard]] auto vector_try_push_back() noexcept -> bool {
  Vector<int, 4> values{1, 2, 3};
  const bool push_ok = values.try_push_back(4);
  return push_ok && values.state() == vector_state::at_capacity;
}

} // namespace cljonic::no_heap::probes
