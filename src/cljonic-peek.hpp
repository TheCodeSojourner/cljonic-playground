#pragma once

#include <cljonic-concepts.hpp>
#include <utility>

namespace cljonic {

/** \anchor Peek
 * The \b peek free function returns the last logical element of a \ref Vector "Vector" or the front element of a \ref
 * Queue "Queue" without removing it.
 *
 * An empty collection returns its element type's default value.
 *
 * \b Examples
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 int main() {
   // Compile-time demonstration.
   constexpr auto vector = cljonic::vector(10, 20);
   constexpr auto q = queue(30, 40);
   constexpr auto empty_vector = cljonic::vector<int, 4>();
   constexpr auto empty_queue = queue<int, 4>();
   static_assert(peek(vector) == 20);
   static_assert(peek(q) == 30);
   static_assert(peek(empty_vector) == 0);
   static_assert(peek(empty_queue) == 0);

   // Runtime demonstration.
   const auto vector_first = 11;
   const auto vector_last = 22;
   const auto queue_front = 33;
   const auto queue_back = 44;
   const auto runtime_vector = cljonic::vector(vector_first, vector_last);
   const auto runtime_queue = queue(queue_front, queue_back);
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
