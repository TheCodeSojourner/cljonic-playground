#pragma once

#include <cljonic-next.hpp>

namespace cljonic {

/** \anchor Rest
 * The \b rest free function returns the logical remainder of a \ref Vector "Vector", \ref Map "Map", \ref Set "Set",
 * \ref Queue "Queue", or \ref String "String" as an owning \ref Vector "Vector". It contains every logical element
 * after the first and preserves the input. An empty input returns an empty Vector.
 *
 * Map elements remain value-semantic \ref MapEntryFactory "map_entry" values. Map and Set traversal order is not
 * semantically significant, while Queue retains FIFO order. Unlike \ref Pop "pop", `rest` follows logical traversal
 * order rather than the collection-specific removal end.
 *
 * \b Examples
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 int main() {
   // Compile-time demonstration.
   constexpr auto values = vector(10, 20, 30);
   constexpr auto remainder = rest(values);
   static_assert(count(remainder) == 2);
   static_assert(remainder(0) == 20 && remainder(1) == 30);

   constexpr auto queue_values = queue(4, 5, 6);
   constexpr auto queue_remainder = rest(queue_values);
   static_assert(queue_remainder(0) == 5 && queue_remainder(1) == 6);
   static_assert(count(rest(cljonic::vector<int, 0>())) == 0);

   // Runtime demonstration.
   volatile int first_raw = 40;
   volatile int second_raw = 50;
   volatile int third_raw = 60;
   const int first = first_raw;
   const int second = second_raw;
   const int third = third_raw;
   const auto runtime_values = vector<int, 4>(first, second, third);
   const auto runtime_remainder = rest(runtime_values);
   const auto runtime_queue = queue<int, 4>(first, second, third);
   const auto runtime_queue_remainder = rest(runtime_queue);

   return (count(runtime_remainder) == 2 && runtime_remainder(0) == second &&
           runtime_remainder(1) == third && count(runtime_values) == 3 &&
           runtime_values(0) == first && runtime_queue_remainder(0) == second &&
           runtime_queue_remainder(1) == third)
              ? 0
              : 1;
 }
 ~~~~~
 */
template <concepts_detail::NextSequenceableCollection C>
[[nodiscard]] constexpr auto rest(const C& collection) noexcept -> Vector<typename C::value_type, C::capacity()> {
    return next(collection);
}

template <typename C>
    requires(!concepts_detail::NextSequenceableCollection<C>)
constexpr auto rest([[maybe_unused]] const C& value) noexcept -> void {
    static_assert(concepts_detail::dependent_false<C>,
                  "cljonic::rest: expected a Vector, Map, Set, Queue, or String value.");
}

} // namespace cljonic
