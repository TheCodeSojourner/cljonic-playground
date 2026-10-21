#pragma once

#include <cljonic-concepts.hpp>

namespace cljonic {

/** \anchor FitsInto
 * The \b fits_into free function checks whether the complete contents of a cljonic collection or producer fit within
 * the remaining capacity of a \ref Vector "Vector".
 *
 * The destination must be a Vector, and the source must be a cljonic collection or producer whose elements convert
 * safely to the Vector's element type. Reports whether appending the complete \p source to \p destination fits within
 * the destination's remaining capacity, using the same cardinality semantics as `into`. It returns `false` for every
 * unbounded producer, even though its normal const traversal is capped. Source elements must follow the same \ref
 * implicit-conversions "safe implicit conversion rules" as \ref Into "into".
 *
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 int main() {
   // Compile-time demonstration.
   constexpr Vector<int, 8> destination{};
   constexpr Range<int> finite_source{0, 5};
   static_assert(fits_into(destination, finite_source));

   constexpr Range<int> unbounded_source{0, 5, 0};
   static_assert(!fits_into(destination, unbounded_source));

   constexpr Vector<int, 2> small_destination{};
   static_assert(!fits_into(small_destination, finite_source));

   // Runtime demonstration.
   volatile int runtime_end = 5;
   const auto runtime_destination = Vector<int, 8>{1, 2};
   const auto runtime_source = Range<int>{0, runtime_end};
   const auto runtime_small_destination = Vector<int, 4>{};

   return (fits_into(runtime_destination, runtime_source) &&
           !fits_into(runtime_small_destination, runtime_source))
              ? 0
              : 1;
 }
 ~~~~~
 */
template <concepts::CljonicVector Dest, concepts::CljonicSource Source>
    requires concepts_detail::MaterializableSource<Dest, Source>
[[nodiscard]] constexpr auto fits_into(const Dest& destination, const Source& source) noexcept -> bool {
    if constexpr (concepts::CljonicProducer<Source>) {
        if (!source.is_finite()) {
            return false;
        }
    }
    return source.count() <= (Dest::capacity() - destination.count());
}

// Diagnostic fallback (REQ-DIAG-009): a call outside the admitted domain names
// the domain instead of listing failed concept candidates. It produces no value.
template <typename Dest, typename Source>
    requires(!concepts_detail::MaterializableSource<Dest, Source>)
[[nodiscard]] constexpr auto fits_into([[maybe_unused]] const Dest& destination,
                                       [[maybe_unused]] const Source& source) noexcept -> bool {
    static_assert(concepts_detail::dependent_false<Dest, Source>,
                  "cljonic::fits_into: expected a Vector destination and a cljonic collection or producer source whose "
                  "elements convert safely to the Vector's element type.");
    return false;
}

} // namespace cljonic
