#pragma once

#include <cljonic-concepts.hpp>

namespace cljonic {

/** \anchor Conj
 * \brief Adds an element to a collection according to its type conventions.
 *
 * Appends at the end of a `Vector`, the rear of a `Queue`, inserts into a `Set` unless the element is already present,
 * and associates a `MapEntry` into a `Map` (replacing an existing key's value). `String` supports indexed `assoc`, not
 * `conj`. The result is a distinct collection value; the source is unchanged.
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
   constexpr auto m_const = conj(Map<int, int, 4>{}, MapEntry<int, int>{1, 100});
   static_assert(m_const(1) == 100);

   // Runtime demonstration.
   auto q_runtime = Queue<int, 4>{};
   const auto q1 = conj(q_runtime, 100);

   return (peek(q1) == 100) ? 0 : 1;
 }
 ~~~~~
 */
template <typename C, typename T>
    requires concepts::ConjableCollection<C> && requires(const C& collection, const T& value) {
        { collection.conj(value) } noexcept -> std::same_as<C>;
    }
[[nodiscard]] constexpr auto conj(const C& collection, const T& value) noexcept(noexcept(collection.conj(value))) -> C {
    return collection.conj(value);
}

} // namespace cljonic
