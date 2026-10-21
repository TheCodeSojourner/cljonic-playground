#pragma once

#include <cljonic-concepts.hpp>
#include <utility>

namespace cljonic {

/** \anchor Pop
 * The \b pop free function API yields an updated \ref concepts::CljonicCollection "collection" with the accessible
 * element removed. For a \ref Vector "Vector", it removes the last logical element; for a \ref Queue "Queue", it
 * removes the front element.
 *
 * \b Examples
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 int main() {
   // Compile-time demonstration.
   constexpr auto vector = Vector<int, 4>{1, 2};
   constexpr auto vector_after_pop = pop(vector);
   constexpr auto queue = conj(conj(Queue<int, 4>{}, 1), 2);
   constexpr auto queue_after_pop = pop(queue);
   static_assert(peek(vector_after_pop) == 1);
   static_assert(pop(vector) == vector_after_pop);
   static_assert(peek(queue_after_pop) == 2);
   static_assert(vector.count() == 2);

   // Runtime demonstration.
   auto runtime_vector = Vector<int, 4>{10, 20};
   auto runtime_vector_after_pop = pop(runtime_vector);
   auto runtime_queue = conj(conj(Queue<int, 4>{}, 10), 20);
   auto runtime_queue_after_pop = pop(runtime_queue);

   return (peek(runtime_vector_after_pop) == 10 &&
           pop(runtime_vector) == runtime_vector_after_pop &&
           runtime_vector.count() == 2 && peek(runtime_queue_after_pop) == 20)
              ? 0
              : 1;
 }
 ~~~~~
 */
template <typename C>
    requires concepts::CljonicCollection<C> && requires(const C& value) { value.pop(); }
[[nodiscard]] constexpr auto pop(const C& collection) noexcept {
    return collection.pop();
}

template <typename C>
    requires(!(concepts::CljonicCollection<C> && requires(const C& value) { value.pop(); }))
constexpr auto pop([[maybe_unused]] const C& collection) noexcept -> void {
    static_assert(concepts_detail::dependent_false<C>,
                  "cljonic::pop: the argument must be a cljonic collection with a pop() operation.");
}

} // namespace cljonic
