#pragma once

#include <cljonic-concepts.hpp>

namespace cljonic {

/** \anchor Conj
 * \brief Adds an element to a collection according to its type conventions.
 *
 * Supported for `Vector`, `Set`, `Map`, `Queue`, and `String`. `Vector` appends at the end. `Queue` enqueues at the
 * rear. `String` appends at its logical count. For `String`, NUL and characters whose unsigned byte value exceeds
 * `0x7F` are invalid; they are rejected at compile time and replaced with `.` at runtime. `Set` adds absent elements;
 * duplicates are no-ops. `Map` associates a `MapEntry`; an existing key's value is replaced. At full capacity,
 * `Vector`, `Queue`, and `String` return unchanged copies. A full `Set` or `Map` also returns an unchanged copy when
 * the element or key is absent. Each operation returns a distinct collection value and preserves the source.
 *
 * \b Examples
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 int main() {
   // Compile-time demonstration.
   constexpr auto q_const = conj(Queue<int, 4>{}, 10);
   constexpr auto s_const = conj(Set<int, 4>{}, 20);
   static_assert(peek(q_const) == 10);
   static_assert(contains(s_const, 20));

   constexpr auto v_const = conj(Vector<int, 4>{10, 20}, 30);
   static_assert(v_const(2) == 30);
   constexpr auto text_const = conj(String<4>{"Hi"}, '!');
   static_assert(text_const(2) == '!');
   constexpr auto m_const = conj(Map<int, int, 4>{}, MapEntry<int, int>{1, 100});
   static_assert(m_const(1) == 100);

   // Runtime demonstration.
   auto q_runtime = Queue<int, 4>{};
   const auto q1 = conj(q_runtime, 100);

   return (peek(q1) == 100) ? 0 : 1;
 }
 ~~~~~
 */
namespace concepts_detail {

template <typename C, typename T>
concept ConjValueAdmissible = concepts::ConjableCollection<C> && requires(const C& collection, const T& value) {
    { collection.conj(value) } noexcept -> std::same_as<C>;
};

} // namespace concepts_detail

template <typename C, typename T>
    requires concepts_detail::ConjValueAdmissible<C, T>
[[nodiscard]] constexpr auto conj(const C& collection, const T& value) noexcept(noexcept(collection.conj(value))) -> C {
    return collection.conj(value);
}

// Diagnostic fallback (REQ-DIAG-009): explain collection and value-domain
// rejections without making either a supported call target.
template <typename C, typename T>
    requires concepts::CljonicCollection<C> && (!concepts::ConjableCollection<C>)
[[nodiscard]] constexpr auto conj([[maybe_unused]] const C& collection, [[maybe_unused]] const T& value) noexcept -> C {
    static_assert(concepts_detail::dependent_false<C>,
                  "cljonic::conj: the first argument must be a Conjable collection -- Vector, Set, Map, Queue, or "
                  "String.");
    return collection;
}

template <typename C, typename T>
    requires concepts::ConjableCollection<C> && (!concepts_detail::ConjValueAdmissible<C, T>)
[[nodiscard]] constexpr auto conj([[maybe_unused]] const C& collection, [[maybe_unused]] const T& value) noexcept -> C {
    static_assert(concepts_detail::dependent_false<C, T>,
                  "cljonic::conj: value is outside this collection's conj domain. Map requires a MapEntry; other "
                  "collections require an accepted element value.");
    return collection;
}

} // namespace cljonic
