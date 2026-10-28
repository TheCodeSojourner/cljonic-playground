#pragma once

#include <cljonic-count.hpp>
#include <cljonic-next.hpp>

namespace cljonic {

/** \anchor HasNext
 * The \b has_next free function checks whether a \ref Next "next" call would return a nonempty logical tail.
 *
 * It returns true when a supported \ref Vector "Vector", \ref Map "Map", \ref Set "Set", \ref Queue "Queue", or \ref
 * String "String" contains more than one logical element. Empty and single-element collections return false. The
 * predicate observes the count without constructing the result of \ref Next "next"; it preserves the input. Map and
 * Set traversal order does not affect the result. Other inputs are rejected at compile time.
 *
 * \b Examples
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 int main() {
   // Compile-time demonstration.
   constexpr auto values = vector(10, 20, 30);
   constexpr auto singleton = queue(10);
   constexpr auto empty_values = vector<int, 0>();
   static_assert(has_next(values));
   static_assert(!has_next(singleton));
   static_assert(!has_next(empty_values));

   // Runtime demonstration.
   const int first = 40;
   const int second = first + 1;
   const auto runtime_values = vector<int, 4>(first, second);
   const auto runtime_singleton = hash_set(first);

   return (has_next(runtime_values) && !has_next(runtime_singleton) &&
           count(runtime_values) == 2 && runtime_values(0) == first)
              ? 0
              : 1;
 }
 ~~~~~
 */
template <concepts_detail::NextSequenceableCollection C>
[[nodiscard]] constexpr auto has_next(const C& collection) noexcept -> bool {
    return count(collection) > 1U;
}

template <typename C>
    requires(!concepts_detail::NextSequenceableCollection<C>)
constexpr auto has_next([[maybe_unused]] const C& value) noexcept -> void {
    static_assert(concepts_detail::dependent_false<C>,
                  "cljonic::has_next: expected a Vector, Map, Set, Queue, or String value.");
}

} // namespace cljonic
