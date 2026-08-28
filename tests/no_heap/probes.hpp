#pragma once

namespace cljonic::no_heap::probes {

[[nodiscard]] auto vector_storage() noexcept -> bool;
[[nodiscard]] auto vector_capacity() noexcept -> bool;
[[nodiscard]] auto assoc_probe() noexcept -> bool;
[[nodiscard]] auto can_assoc_probe() noexcept -> bool;
[[nodiscard]] auto can_conj_probe() noexcept -> bool;
[[nodiscard]] auto concepts_probe() noexcept -> bool;
[[nodiscard]] auto config_probe() noexcept -> bool;
[[nodiscard]] auto conj_probe() noexcept -> bool;
[[nodiscard]] auto max_count_probe() noexcept -> bool;
[[nodiscard]] auto core_probe() noexcept -> bool;
[[nodiscard]] auto count_probe() noexcept -> bool;
[[nodiscard]] auto disj_probe() noexcept -> bool;
[[nodiscard]] auto dissoc_probe() noexcept -> bool;
[[nodiscard]] auto empty_probe() noexcept -> bool;
[[nodiscard]] auto first_probe() noexcept -> bool;
[[nodiscard]] auto get_probe() noexcept -> bool;
[[nodiscard]] auto is_empty_probe() noexcept -> bool;
[[nodiscard]] auto map_entry_probe() noexcept -> bool;
[[nodiscard]] auto map_probe() noexcept -> bool;
[[nodiscard]] auto next_probe() noexcept -> bool;
[[nodiscard]] auto not_empty_probe() noexcept -> bool;
[[nodiscard]] auto peek_probe() noexcept -> bool;
[[nodiscard]] auto pop_probe() noexcept -> bool;
[[nodiscard]] auto queue_probe() noexcept -> bool;
[[nodiscard]] auto rest_probe() noexcept -> bool;
[[nodiscard]] auto seq_probe() noexcept -> bool;
[[nodiscard]] auto set_probe() noexcept -> bool;
[[nodiscard]] auto string_probe() noexcept -> bool;

} // namespace cljonic::no_heap::probes
