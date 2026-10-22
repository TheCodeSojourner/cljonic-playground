#pragma once

#include <cljonic-concepts.hpp>
#include <cstddef>

namespace cljonic {

/** \anchor Count
 * The \b count free function API returns the number of elements in a \ref concepts::SequenceableCollection
 * "collection" or
 *        \ref concepts::SequenceableProducer "producer". For unbounded \ref concepts::SequenceableProducer "producers",
 *        returns \b CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE.
 *
 * \b Examples
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 int main() {
   // Compile-time demonstration.
   constexpr auto v_const = Vector{1, 2, 3};
   constexpr auto m_const = Map{MapEntry{1, 100}};
   constexpr auto q_const = Queue{9};
   constexpr auto r_const = Range{0, 5};
   constexpr auto finite_repeat = Repeat{3, 7};
   constexpr auto unbounded_repeat = Repeat{7};
   constexpr auto unbounded_range = Range{0, 0, 0};
   static_assert(count(v_const) == 3);
   static_assert(count(m_const) == 1);
   static_assert(count(q_const) == 1);
   static_assert(count(r_const) == 5);
   static_assert(count(finite_repeat) == 3);
   static_assert(count(unbounded_repeat) ==
                 CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE);
   static_assert(count(unbounded_range) ==
                 CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE);

   // Runtime demonstration.
   auto v_runtime = Vector{10, 20};
   const auto sz = count(v_runtime);

   return (sz == 2) ? 0 : 1;
 }
 ~~~~~
 */
template <typename C>
    requires concepts::SequenceableCollection<C> || concepts::SequenceableProducer<C>
[[nodiscard]] constexpr auto count(const C& collection) noexcept -> std::size_t {
    return collection.count();
}

template <typename C>
    requires(!concepts::SequenceableCollection<C> && !concepts::SequenceableProducer<C>)
[[nodiscard]] constexpr auto count([[maybe_unused]] const C& collection) noexcept -> std::size_t {
    static_assert(concepts_detail::dependent_false<C>,
                  "cljonic::count: expected a Vector, Map, Set, Queue, String, Range, Repeat, Cycle, Iterate, or "
                  "Repeatedly value.");
    return 0U;
}

} // namespace cljonic
