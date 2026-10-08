#pragma once

#include <cljonic-count.hpp>

namespace cljonic {

/** \anchor IsEmpty
 * \brief Returns true when the collection has no active elements.
 *
 * \b Examples
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 int main() {
   // Compile-time demonstration.
   constexpr auto e_const = Vector<int, 4>{};
   constexpr auto v_const = Vector<int, 4>{1};
   static_assert(is_empty(e_const));
   static_assert(!is_empty(v_const));

   // Runtime demonstration.
   auto v_runtime = Vector<int, 4>{10};
   const auto empty_res = is_empty(v_runtime);

   return (!empty_res) ? 0 : 1;
 }
 ~~~~~
 */
template <concepts::SequenceableCollection C>
[[nodiscard]] constexpr auto is_empty(const C& collection) noexcept -> bool {
    return count(collection) == 0U;
}

} // namespace cljonic
