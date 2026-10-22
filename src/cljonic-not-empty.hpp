#pragma once

#include <cljonic-is-empty.hpp>

namespace cljonic {

/** \anchor NotEmpty
 * The \b not_empty free function returns an independent owning copy of a nonempty collection, or an empty value of the
 * same type when the input is empty. It accepts `Vector`, `Map`, `Set`, `Queue`, and `String`, preserves their type
 * and capacity, and leaves the input unchanged.
 *
 * This is not a boolean emptiness predicate; use \ref IsEmpty "is_empty" for that purpose.
 *
 * \b Examples
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 int main() {
   constexpr auto values = vector(10, 20);
   constexpr auto copied = not_empty(values);
   static_assert(equal(copied, values));
   static_assert(count(copied) == 2);
   static_assert(is_empty(not_empty(vector<int, 0>())));

   const auto entries = Map{MapEntry{1, 100}, MapEntry{2, 200}};
   const auto copied_entries = not_empty(entries);
   const auto empty_values = vector<int, 4>();
   const auto copied_empty = not_empty(empty_values);

   return equal(copied_entries, entries) && count(entries) == 2 &&
                  is_empty(copied_empty) && is_empty(empty_values)
              ? 0
              : 1;
 }
 ~~~~~
 */
template <concepts::SequenceableCollection C>
[[nodiscard]] constexpr auto not_empty(const C& collection) noexcept -> C {
    return is_empty(collection) ? C{} : C{collection};
}

template <typename C>
    requires(!concepts::SequenceableCollection<C>)
constexpr auto not_empty([[maybe_unused]] const C& collection) noexcept -> void {
    static_assert(concepts_detail::dependent_false<C>,
                  "cljonic::not_empty: expected a Vector, Map, Set, Queue, or String value.");
}

} // namespace cljonic
