#pragma once

#include <cljonic-count.hpp>

namespace cljonic {

/** \anchor IsEmpty
 * The \b is_empty free function returns true exactly when \ref Count "count" is zero for a collection or producer. It
 * leaves the input unchanged and does not throw or allocate. For an unbounded producer, it checks the configured
 * observation cap, not complete cardinality. Values outside the cljonic collection and producer domain are rejected at
 * compile time.
 *
 * \b Examples
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 int main() {
   constexpr auto empty_values = Vector<int, 2>{};
   constexpr auto values = Vector{1, 2};
   constexpr auto text = String{"Hi"};
   constexpr auto empty_range = Range{2, 2};
   constexpr auto unbounded_repeat = Repeat{7};
   static_assert(is_empty(empty_values));
   static_assert(!is_empty(values));
   static_assert(!is_empty(text));
   static_assert(is_empty(empty_range));
   static_assert(!is_empty(unbounded_repeat));

   const int value = 3;
   const auto runtime_values = Vector{value};
   const auto original_values = runtime_values;
   const auto runtime_empty_range = Range{value, value};
   const auto runtime_range = Range{0, value};
   const auto original_range = runtime_range;
   const auto runtime_repeat = Repeat{value};
   const auto runtime_text = String{"Hi"};

   const auto results_match =
       is_empty(empty_values) && !is_empty(runtime_values) &&
       !is_empty(runtime_text) && is_empty(runtime_empty_range) &&
       !is_empty(runtime_range) && !is_empty(runtime_repeat);
   const auto inputs_preserved =
       equal(runtime_values, original_values) && runtime_range == original_range;
   return results_match && inputs_preserved ? 0 : 1;
 }
 ~~~~~
 */
template <typename C>
    requires concepts::SequenceableCollection<C> || concepts::SequenceableProducer<C>
[[nodiscard]] constexpr auto is_empty(const C& collection) noexcept -> bool {
    return count(collection) == 0U;
}

template <typename C>
    requires(!concepts::SequenceableCollection<C> && !concepts::SequenceableProducer<C>)
constexpr auto is_empty([[maybe_unused]] const C& collection) noexcept -> void {
    static_assert(concepts_detail::dependent_false<C>, "cljonic::is_empty: value must be a collection or producer.");
}

} // namespace cljonic
