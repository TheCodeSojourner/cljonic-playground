#pragma once

#include <cljonic-concepts.hpp>

namespace cljonic {

namespace concepts_detail {

template <concepts::CljonicSource Source>
[[nodiscard]] constexpr auto materialization_source_is_finite(const Source& source) noexcept -> bool {
    if constexpr (concepts::CljonicProducer<Source>) {
        return source.is_finite();
    } else {
        return true;
    }
}

template <typename Dest, typename Source>
    requires MaterializableSource<Dest, Source>
[[nodiscard]] constexpr auto source_fits_by_insertion(const Dest& destination, const Source& source) noexcept -> bool {
    Dest result = destination;
    for (auto&& item : source) {
        const auto value = typename Dest::value_type(item);
        if (!result.can_conj(value)) {
            return false;
        }
        result = result.conj(value);
    }
    return true;
}

template <typename Dest, typename Source>
[[nodiscard]] constexpr auto source_fits_by_count(const Dest& destination, const Source& source) noexcept -> bool {
    return source.count() <= (Dest::capacity() - destination.count());
}

} // namespace concepts_detail

/** \anchor FitsInto
 * The \b fits_into free function checks whether every value from a cljonic collection or producer can be inserted into
 * a \ref Vector "Vector", \ref Set "Set", \ref Map "Map", \ref Queue "Queue", or \ref String "String".
 *
 * Uses the same insertion rules as \ref Into "into", leaving both input values unchanged. Vector and Queue compare the
 * finite source count to remaining capacity. Set and Map inspect values, accounting for duplicates and matching-key
 * updates in the destination and earlier source values. It returns `false` at the first insertion that needs
 * unavailable capacity, or `true` after the complete finite source is accepted. Every unbounded producer returns
 * `false`, even though its normal const traversal is capped. String compares finite source count to remaining
 * character capacity without evaluating character validity; invalid characters are normalized at runtime by `into`.
 *
 * Finite Set/Map preflight may evaluate producer callbacks. Preflight and `into` traverse independently, without
 * shared caching; callback effects through references or static storage may remain observable. The prediction applies
 * only when the subsequent traversal yields the same values.
 *
 * Source values follow the same \ref implicit-conversions "safe implicit conversion rules" as `into`; Map requires its
 * declared \ref MapEntry "MapEntry" type. External C++ range or view sources are excluded.
 *
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 int main() {
   // Compile-time demonstration.
   constexpr auto destination = vector<int, 8>();
   constexpr auto finite_source = range(0, 5);
   static_assert(fits_into(destination, finite_source));

   constexpr auto unbounded_source = range(0, 5, 0);
   static_assert(!fits_into(destination, unbounded_source));

   constexpr auto small_destination = vector<int, 2>();
   static_assert(!fits_into(small_destination, finite_source));
   static_assert(fits_into(hash_set<int, 2>(1), vector(1, 2, 2, 1)));
   static_assert(fits_into(hash_map<int, int, 1>(1, 10),
                           vector(map_entry(1, 11), map_entry(1, 12))));
   static_assert(!fits_into(queue<int, 2>(9), vector(1, 2)));
   static_assert(fits_into(string<4>("x"), string("ab")));
   static_assert(!fits_into(string<2>("x"), string("ab")));

   // Runtime demonstration.
   const auto runtime_end = 5;
   const auto runtime_destination = vector<int, 8>(1, 2);
   const auto runtime_source = range(0, runtime_end);
   const auto runtime_small_destination = vector<int, 4>();
   const auto runtime_set_source = vector(1, 2, 2);
   const auto runtime_map_source = vector(map_entry(1, 11), map_entry(1, 12));
   const auto runtime_string_source = string("ab");

   return (fits_into(runtime_destination, runtime_source) &&
           !fits_into(runtime_small_destination, runtime_source) &&
           fits_into(hash_set<int, 2>(1), runtime_set_source) &&
           fits_into(hash_map<int, int, 1>(1, 10), runtime_map_source) &&
           fits_into(queue<int, 3>(9), vector(1, 2)) &&
           fits_into(string<4>("x"), runtime_string_source) &&
           !fits_into(string<2>("x"), runtime_string_source))
              ? 0
              : 1;
 }
 ~~~~~
 */
template <typename Dest, typename Source>
    requires concepts_detail::MaterializableSource<Dest, Source>
[[nodiscard]] constexpr auto fits_into(const Dest& destination, const Source& source) noexcept -> bool {
    if (!concepts_detail::materialization_source_is_finite(source)) {
        return false;
    }
    if constexpr (concepts_detail::CountPreflightDestination<Dest>) {
        return concepts_detail::source_fits_by_count(destination, source);
    } else {
        return concepts_detail::source_fits_by_insertion(destination, source);
    }
}

// Diagnostic fallback (REQ-DIAG-009): a call outside the admitted domain names
// the domain instead of listing failed concept candidates. It produces no value.
template <typename Dest, typename Source>
    requires(!concepts_detail::MaterializableSource<Dest, Source>)
[[nodiscard]] constexpr auto fits_into([[maybe_unused]] const Dest& destination,
                                       [[maybe_unused]] const Source& source) noexcept -> bool {
    static_assert(concepts_detail::dependent_false<Dest, Source>,
                  "cljonic::fits_into: expected a Vector, Set, Map, Queue, or String destination and a compatible "
                  "cljonic collection or producer source. Source values must convert safely to the destination's "
                  "element type; a Map requires its declared MapEntry type. External C++ ranges are not supported.");
    return false;
}

} // namespace cljonic
