#pragma once

#include <cljonic-concepts.hpp>
#include <cstddef>

namespace cljonic {

/** \anchor Count
 * \brief Returns a collection's logical size or a producer's count observation. Collections and finite producers
 * return their exact element count. Unbounded producers return the configured observable traversal cap, not their
 * complete cardinality.
 *
 * \b Examples
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 int main() {
   // Compile-time demonstration.
   constexpr auto v_const = Vector<int, 4>{1, 2, 3};
   constexpr auto m_const = assoc(Map<int, int, 4>{}, 1, 100);
   constexpr auto q_const = conj(Queue<int, 4>{}, 9);
   constexpr Range<int> r_const{0, 5};
   constexpr Repeat<int> finite_repeat{7, 3U};
   constexpr Repeat<int> unbounded_repeat{7};
   constexpr Range<int> unbounded_range{0, 0, 0};
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
   auto v_runtime = Vector<int, 4>{10, 20};
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
