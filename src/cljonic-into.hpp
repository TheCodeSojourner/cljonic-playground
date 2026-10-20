#pragma once

#include <cljonic-concepts.hpp>

namespace cljonic {

/** \anchor Into
 * The \b into free function API appends elements from a \ref concepts::CljonicSource "source" to a \ref Vector
 * "Vector", up to its remaining capacity.
 *
 * Returns an updated destination-typed collection with source elements appended after the destination's existing
 * logical contents, leaving both \p destination and \p source unchanged. An unbounded or oversized source produces a
 * deterministic bounded prefix limited by the destination's remaining capacity; a finite source that fits materializes
 * completely. Use \b fits_into beforehand to check whether the complete source fits. The destination must be a \ref
 * Vector "Vector". Other collection types are supported only as sources. Source elements must follow the destination's
 * \ref ImplicitConversions "safe implicit conversion rules".
 *
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 int main() {
   // Compile-time demonstration.
   constexpr Vector<int, 8> destination{100, 200};
   constexpr Range<int> source{0, 5};
   constexpr auto result = into(destination, source);
   static_assert(equal(result, Vector<int, 8>{100, 200, 0, 1, 2, 3, 4}));
   static_assert(equal(destination, Vector<int, 8>{100, 200}));
   static_assert(count(source) == 5);

   constexpr auto prefix = into(Vector<int, 4>{100, 200}, source);
   static_assert(equal(prefix, Vector<int, 4>{100, 200, 0, 1}));

   // Runtime demonstration.
   volatile int runtime_end = 5;
   const auto runtime_destination = Vector<int, 8>{100, 200};
   const auto runtime_source = Range<int>{0, runtime_end};
   const auto runtime_result = into(runtime_destination, runtime_source);
   const auto runtime_prefix = into(Vector<int, 4>{100, 200}, runtime_source);

   return (equal(runtime_result, Vector<int, 8>{100, 200, 0, 1, 2, 3, 4}) &&
           equal(runtime_prefix, Vector<int, 4>{100, 200, 0, 1}) &&
           equal(runtime_destination, Vector<int, 8>{100, 200}) &&
           count(runtime_source) == 5)
              ? 0
              : 1;
 }
 ~~~~~
 */
template <concepts::CljonicVector Dest, concepts::CljonicSource Source>
    requires concepts_detail::MaterializableSource<Dest, Source>
[[nodiscard]] constexpr auto into(const Dest& destination, const Source& source) noexcept -> Dest {
    Dest result = destination;
    for (auto&& item : source) {
        const auto index = result.count();
        if (!result.can_assoc(index, item)) {
            break;
        }
        result = result.assoc(index, item);
    }
    return result;
}

// Diagnostic fallback (REQ-DIAG-009): a call outside the admitted domain names
// the domain instead of listing failed concept candidates. It produces no value.
template <typename Dest, typename Source>
    requires(!concepts_detail::MaterializableSource<Dest, Source>)
constexpr auto into([[maybe_unused]] const Dest& destination, [[maybe_unused]] const Source& source) noexcept -> void {
    static_assert(concepts_detail::dependent_false<Dest, Source>,
                  "cljonic::into: expected a Vector destination and a cljonic collection or producer source whose "
                  "elements convert safely to the Vector's element type.");
}

} // namespace cljonic
