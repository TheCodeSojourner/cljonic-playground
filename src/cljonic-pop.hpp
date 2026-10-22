#pragma once

#include <cljonic-concepts.hpp>
#include <utility>

namespace cljonic {

/** \anchor Pop
 * The \b pop free function API yields an updated \ref concepts::CljonicCollection "collection" with the accessible
 * element removed. For a \ref Vector "Vector", it removes the last logical element; for a \ref Queue "Queue", it
 * removes the front element. An empty collection, including a zero-capacity Vector, is returned unchanged.
 *
 * \b Examples
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 int main() {
   constexpr auto vector = cljonic::vector(1, 2);
   constexpr auto vector_after_pop = pop(vector);
   constexpr auto queue = Queue{1, 2};
   constexpr auto queue_after_pop = pop(queue);
   static_assert(peek(vector_after_pop) == 1);
   static_assert(peek(vector) == 2);
   static_assert(peek(queue_after_pop) == 2);
   static_assert(peek(queue) == 1);
   constexpr auto empty_vector = cljonic::vector<int, 0>();
   constexpr auto empty_queue = Queue<int, 4>{};
   static_assert(pop(empty_vector) == empty_vector);
   static_assert(pop(empty_queue) == empty_queue);

   const int vector_first = 10;
   const int vector_last = 20;
   const int queue_first = 30;
   const int queue_last = 40;
   const auto runtime_vector = cljonic::vector(vector_first, vector_last);
   const auto runtime_vector_after_pop = pop(runtime_vector);
   const auto runtime_queue = Queue{queue_first, queue_last};
   const auto runtime_queue_after_pop = pop(runtime_queue);

   return (peek(runtime_vector_after_pop) == vector_first &&
           peek(runtime_vector) == vector_last &&
           peek(runtime_queue_after_pop) == queue_last &&
           peek(runtime_queue) == queue_first)
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
                  "cljonic::pop: the argument must be a Vector or Queue collection; scalar values, producers, and "
                  "other collection types are not supported.");
}

} // namespace cljonic
