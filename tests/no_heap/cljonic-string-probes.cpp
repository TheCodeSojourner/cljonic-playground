#include "cljonic-no-heap-api.hpp"
#include "poison.hpp"
#include "probes.hpp"

namespace cljonic::no_heap::probes {

[[nodiscard]] auto string_probe() noexcept -> bool {
  const auto s = String<8>{"Hello"};
  return s.size() == 5U && s[0] == 'H';
}

} // namespace cljonic::no_heap::probes