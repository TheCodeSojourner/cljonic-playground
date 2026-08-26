#pragma once

namespace cljonic::no_heap::probes {

[[nodiscard]] auto vector_storage() noexcept -> bool;
[[nodiscard]] auto vector_capacity() noexcept -> bool;

} // namespace cljonic::no_heap::probes
