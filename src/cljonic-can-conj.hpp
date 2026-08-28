#ifndef CLJONIC_CAN_CONJ_HPP
#define CLJONIC_CAN_CONJ_HPP

#include <cljonic-concepts.hpp>
#include <utility>

namespace cljonic {

/** \anchor CanConj
 * \brief Checks if conj can succeed without capacity overflow.
 *
 * \b Examples
 * ~~~~~{.cpp}
 * #include <cljonic.hpp>
 *
 * int main() {
 *   using namespace cljonic;
 *
 *   // Compile-time demonstration.
 *   constexpr Queue<int, 4> q_const{};
 *   static_assert(can_conj(q_const));
 *   constexpr Set<int, 4> s_const{};
 *   static_assert(can_conj(s_const, 1));
 *
 *   // Runtime demonstration.
 *   auto q_runtime = Queue<int, 4>{};
 *   auto s_runtime = Set<int, 4>{};
 *   const auto q_ok = can_conj(q_runtime);
 *   const auto s_ok = can_conj(s_runtime, 10);
 *
 *   return (q_ok && s_ok) ? 0 : 1;
 * }
 * ~~~~~
 */
template <typename C, typename... Args>
[[nodiscard]] constexpr auto can_conj(const C& collection, Args&&... args) noexcept -> bool {
    return collection.can_conj(std::forward<Args>(args)...);
}

} // namespace cljonic

#endif // CLJONIC_CAN_CONJ_HPP
