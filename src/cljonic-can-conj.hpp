#pragma once

#include <cljonic-concepts.hpp>

namespace cljonic {

/** \anchor CanConj
 * \brief Checks whether conj can succeed without capacity overflow.
 *
 * Supported for `Vector`, `Set`, `Map`, `Queue`, and `String`. For `Vector` and `Queue`, the result depends only on
 * remaining capacity; the value does not affect the preflight. For `String`, both remaining capacity and a character
 * that is not NUL and whose unsigned byte value is at most `0x7F` are required. An invalid character returns false
 * even though runtime `conj` replaces it with `.`.
 *
 * For a `Set` element or `Map` key already present, returns true because insertion is a no-op or a value replacement
 * that needs no capacity. An absent element or key returns true only when capacity remains; when full, it returns
 * false.
 *
 * \b Examples
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 int main() {
   // Compile-time demonstration.
   constexpr Queue<int, 4> q_const{};
   static_assert(can_conj(q_const, 1));
   constexpr Queue<int, 1> q_full{1};
   static_assert(!can_conj(q_full, 2));
   constexpr Set<int, 4> s_const{};
   static_assert(can_conj(s_const, 1));
   constexpr String<4> text_const{"Hi"};
   static_assert(can_conj(text_const, '!'));
   constexpr Map<int, int, 4> m_const{MapEntry<int, int>{1, 100}};
   static_assert(can_conj(m_const, MapEntry<int, int>{1, 999}));

   // Runtime demonstration.
   auto q_runtime = Queue<int, 4>{};
   const auto q_ok = can_conj(q_runtime, 1);

   return q_ok ? 0 : 1;
 }
 ~~~~~
 */
namespace concepts_detail {

template <typename C, typename V>
concept CanConjValueAdmissible = concepts::ConjableCollection<C> && requires(const C& collection, const V& value) {
    { collection.can_conj(value) } noexcept -> std::same_as<bool>;
};

} // namespace concepts_detail

template <typename C, typename V>
    requires concepts_detail::CanConjValueAdmissible<C, V>
[[nodiscard]] constexpr auto can_conj(const C& collection,
                                      const V& value) noexcept(noexcept(collection.can_conj(value))) -> bool {
    return collection.can_conj(value);
}

// Diagnostic fallback (REQ-DIAG-009): a cljonic collection or producer that
// does not support conj is rejected at the public boundary.
template <typename C, typename V>
    requires(concepts::CljonicCollection<C> || concepts::CljonicProducer<C>) && (!concepts::ConjableCollection<C>)
[[nodiscard]] constexpr auto can_conj([[maybe_unused]] const C& collection, [[maybe_unused]] const V& value) noexcept
    -> bool {
    static_assert(concepts_detail::dependent_false<C>,
                  "cljonic::can_conj: the first argument must be a Conjable collection -- Vector, Set, Map, Queue, or "
                  "String.");
    return false;
}

template <typename C, typename V>
    requires concepts::ConjableCollection<C> && (!concepts_detail::CanConjValueAdmissible<C, V>)
[[nodiscard]] constexpr auto can_conj([[maybe_unused]] const C& collection, [[maybe_unused]] const V& value) noexcept
    -> bool {
    static_assert(concepts_detail::dependent_false<C, V>,
                  "cljonic::can_conj: value must be admissible for the collection's conj operation.");
    return false;
}

} // namespace cljonic
