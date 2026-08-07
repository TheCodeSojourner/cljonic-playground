#pragma once

namespace cljonic::no_heap::probes {

[[nodiscard]] auto count_on_vector() noexcept -> bool;
[[nodiscard]] auto vector_state_classification() noexcept -> bool;
[[nodiscard]] auto vector_try_push_back() noexcept -> bool;

} // namespace cljonic::no_heap::probes
