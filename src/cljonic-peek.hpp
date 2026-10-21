#pragma once

#include <cljonic-concepts.hpp>
#include <utility>

namespace cljonic {

/** \anchor Peek
 * The \b peek free function API returns the last logical element of a \ref Vector "Vector" or the front element of a
 * \ref Queue "Queue" without removing it. An empty collection returns its element type's default value.
 *
 * \b Examples
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 int main() {
   // Compile-time demonstration.
   constexpr auto vector = Vector<int, 4>{10, 20};
   constexpr auto queue = conj(conj(Queue<int, 4>{}, 30), 40);
   constexpr auto empty_vector = Vector<int, 4>{};
   constexpr auto empty_queue = Queue<int, 4>{};
   static_assert(peek(vector) == 20);
   static_assert(peek(queue) == 30);
   static_assert(peek(empty_vector) == 0);
   static_assert(peek(empty_queue) == 0);

   // Runtime demonstration.
   volatile int vector_first_raw = 11;
   volatile int vector_last_raw = 22;
   volatile int queue_front_raw = 33;
   volatile int queue_back_raw = 44;
   int vector_first = vector_first_raw;
   int vector_last = vector_last_raw;
   int queue_front = queue_front_raw;
   int queue_back = queue_back_raw;
   const auto runtime_vector = Vector<int, 4>{vector_first, vector_last};
   const auto runtime_queue =
       conj(conj(Queue<int, 4>{}, queue_front), queue_back);
   const auto vector_last_value = peek(runtime_vector);
   const auto queue_front_value = peek(runtime_queue);
   const auto empty_runtime_vector = empty(runtime_vector);
   const auto empty_runtime_queue = empty(runtime_queue);

   return (vector_last_value == 22 && queue_front_value == 33 &&
           peek(empty_runtime_vector) == 0 && peek(empty_runtime_queue) == 0 &&
           count(runtime_vector) == 2 && peek(runtime_vector) == 22 &&
           count(runtime_queue) == 2 && peek(runtime_queue) == 33)
              ? 0
              : 1;
 }
 ~~~~~
 */
template <typename C>
    requires concepts::CljonicVector<C> || concepts::CljonicQueue<C>
[[nodiscard]] constexpr auto peek(const C& collection) noexcept -> C::value_type {
    return collection.peek();
}

template <typename C>
    requires(!concepts::CljonicVector<C> && !concepts::CljonicQueue<C>)
constexpr auto peek([[maybe_unused]] const C& collection) noexcept -> void {
    static_assert(concepts_detail::dependent_false<C>, "cljonic::peek: the first argument must be a Vector or Queue.");
}

} // namespace cljonic
