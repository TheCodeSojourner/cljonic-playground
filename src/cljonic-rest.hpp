#pragma once

#include <cljonic-next.hpp>

namespace cljonic {

/** \anchor Rest
 * The \b rest free function is an alias for \ref Next "next" over the same supported input domain. It returns exactly
 * the result of `next(value)`, preserving its result type, logical tail, input, producer observation policy, and
 * callback timing and effects.
 *
 * For collections, the result has the same collection kind with one less capacity (zero stays zero), and contains
 * every logical element after the first. Map elements remain value-semantic \ref MapEntryFactory "map_entry" values.
 * Map and Set traversal order is not semantically significant, while Queue retains FIFO order. Unlike \ref Pop "pop",
 * `rest` follows logical traversal order rather than the collection-specific removal end.
 *
 * For producers, `rest` preserves all of `next`'s producer-specific tail behavior, including the eager callback timing
 * of \ref Iterate "Iterate" and \ref Repeatedly "Repeatedly".
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

   constexpr auto repeated_remainder = rest(repeat(3, 7));
   static_assert(count(repeated_remainder) == 2);
   constexpr auto range_remainder = rest(Range{0, 3});
   constexpr auto range_values = into(Vector<int, 2>{}, range_remainder);
   static_assert(range_values(0) == 1 && range_values(1) == 2);
   constexpr auto cycle_remainder = rest(cycle(vector(10, 20, 30)));
   constexpr auto cycle_values = into(Vector<int, 3>{}, cycle_remainder);
   static_assert(cycle_values(0) == 20 && cycle_values(1) == 30 &&
                 cycle_values(2) == 10);
   constexpr auto iterate_remainder =
       rest(iterate([](int value) noexcept { return value + 1; }, 1));
   constexpr auto iterate_values = into(Vector<int, 1>{}, iterate_remainder);
   static_assert(iterate_values(0) == 2);
   constexpr auto repeatedly_remainder =
       rest(repeatedly(3, []() noexcept { return 7; }));
   static_assert(count(repeatedly_remainder) == 2);

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
template <concepts_detail::NextSequenceable C>
[[nodiscard]] constexpr auto rest(const C& value) noexcept -> decltype(next(value)) {
    return next(value);
}

template <typename C>
    requires(!concepts_detail::NextSequenceable<C>)
constexpr auto rest([[maybe_unused]] const C& value) noexcept -> void {
    static_assert(concepts_detail::dependent_false<C>,
                  "cljonic::rest: expected a Vector, Map, Set, Queue, String, Range, Repeat, Iterate, Repeatedly, or "
                  "supported Cycle value.");
}

} // namespace cljonic
