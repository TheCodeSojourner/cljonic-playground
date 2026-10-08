#pragma once

#include <cljonic-count.hpp>

namespace cljonic {

/** \anchor IsEmpty
 * \brief Returns true when a collection or producer is empty.
 *
 * \b Examples
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 int main() {
   // Compile-time demonstration.
   constexpr auto e_const = Vector<int, 4>{};
   constexpr auto v_const = Vector<int, 4>{1};
   constexpr auto empty_range = Range<int>{0, 0};
   constexpr auto unbounded_repeat = Repeat<int>{7};
   static_assert(is_empty(e_const));
   static_assert(!is_empty(v_const));
   static_assert(is_empty(empty_range));
   static_assert(!is_empty(unbounded_repeat));

   // Runtime demonstration.
   auto v_runtime = Vector<int, 4>{10};
   const auto empty_res = is_empty(v_runtime);

   return (!empty_res) ? 0 : 1;
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
