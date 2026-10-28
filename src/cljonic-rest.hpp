#pragma once

#include <cljonic-next.hpp>

namespace cljonic {

/** \anchor Rest
 * The \b rest free function returns the logical remainder of a \ref Vector "Vector", \ref Map "Map", \ref Set "Set",
 * \ref Queue "Queue", or \ref String "String" as exactly the result of \ref Next "next": the same collection kind with
 * one less capacity (zero stays zero). It contains every logical element after the first and preserves the input. An
 * empty or singleton input returns an empty value of the result type.
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
   static_assert(first(queue_remainder) == 5 &&
                 first(rest(queue_remainder)) == 6);
   static_assert(count(rest(vector<int, 0>())) == 0);

   // Runtime demonstration.
   const auto runtime_values = vector<int, 4>(40, 50, 60);
   const auto runtime_remainder = rest(runtime_values);
   const auto runtime_queue = queue<int, 4>(40, 50, 60);
   const auto runtime_queue_remainder = rest(runtime_queue);

   return (count(runtime_remainder) == 2 && runtime_remainder(0) == 50 &&
           runtime_remainder(1) == 60 && count(runtime_values) == 3 &&
           runtime_values(0) == 40 && first(runtime_queue_remainder) == 50 &&
           first(rest(runtime_queue_remainder)) == 60)
              ? 0
              : 1;
 }
 ~~~~~
 */
template <concepts_detail::NextSequenceableCollection C>
[[nodiscard]] constexpr auto rest(const C& collection) noexcept -> decltype(next(collection)) {
    return next(collection);
}

template <typename C>
    requires(!concepts_detail::NextSequenceableCollection<C>)
constexpr auto rest([[maybe_unused]] const C& value) noexcept -> void {
    static_assert(concepts_detail::dependent_false<C>,
                  "cljonic::rest: expected a Vector, Map, Set, Queue, or String value.");
}

} // namespace cljonic
