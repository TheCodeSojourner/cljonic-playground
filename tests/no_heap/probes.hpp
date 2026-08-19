#pragma once

namespace cljonic::no_heap::probes {

[[nodiscard]] auto count_on_vector() noexcept -> bool;
[[nodiscard]] auto vector_count() noexcept -> bool;

} // namespace cljonic::no_heap::probes
