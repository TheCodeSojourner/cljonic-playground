#pragma once

#include <cljonic-concepts.hpp>

namespace cljonic {

/** \anchor Into
 * The \b into free function inserts values from a cljonic collection or producer into a copy of a \ref Vector
 * "Vector", \ref Set "Set", \ref Map "Map", \ref Queue "Queue", or \ref String "String".
 *
 * Returns the destination's type, leaving both \p destination and \p source unchanged. Vector and Queue append at the
 * end or rear. Set ignores duplicate values. Map accepts \ref MapEntry "MapEntry" values of its declared entry type
 * and replaces matching keys; the last processed entry for a key wins. String appends characters in traversal order.
 * Invalid characters fail constant evaluation; at runtime they are replaced with `'.'` and traversal continues.
 *
 * Values are processed in source traversal order. When full, a Set accepts an existing value as a no-op and a Map
 * accepts a \ref MapEntry "MapEntry" with an existing key as a value replacement. Vector and Queue cannot accept more
 * values when full. String also stops at capacity, independently of character validity. Processing stops before the
 * first value that needs unavailable capacity, without skipping it to process later values. An unbounded source
 * remains limited to its configured observable traversal cap, even if Set or Map insertion does not increase the
 * result count for duplicates or matching keys. Use \ref FitsInto "fits_into" beforehand when complete materialization
 * matters.
 *
 * Source values must follow the destination's \ref implicit-conversions "safe implicit conversion rules". External C++
 * range or view sources are not supported.
 *
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 int main() {
   // Compile-time demonstration.
   constexpr auto destination = vector<int, 8>(100, 200);
   constexpr auto source = Range{0, 5};
   constexpr auto result = into(destination, source);
   static_assert(equal(result, vector(100, 200, 0, 1, 2, 3, 4)));
   static_assert(equal(destination, vector(100, 200)));
   static_assert(count(source) == 5);

   constexpr auto prefix = into(vector<int, 4>(100, 200), source);
   static_assert(equal(prefix, vector(100, 200, 0, 1)));
   constexpr auto distinct = into(hash_set<int, 2>(1), vector(1, 2, 2, 1));
   static_assert(equal(distinct, hash_set(1, 2)));
   constexpr auto entries =
       vector(map_entry(1, 11), map_entry(2, 20), map_entry(1, 12));
   constexpr auto mapped = into(hash_map<int, int, 2>(1, 10), entries);
   static_assert(get(mapped, 1) == 12 && get(mapped, 2) == 20);
   constexpr auto queued = into(queue<int, 3>(9), vector(1, 2, 3));
   static_assert(equal(queued, queue(9, 1, 2)));
   constexpr auto text = into(string<4>("x"), string("ab"));
   static_assert(equal(text, string("xab")));

   // Runtime demonstration.
   const auto runtime_end = 5;
   const auto runtime_destination = vector<int, 8>(100, 200);
   const auto runtime_source = Range{0, runtime_end};
   const auto runtime_result = into(runtime_destination, runtime_source);
   const auto runtime_prefix = into(vector<int, 4>(100, 200), runtime_source);
   const auto invalid_byte = '\x80';
   const auto runtime_text =
       into(string<4>("x"), vector('a', invalid_byte, 'b'));

   return (equal(runtime_result, vector(100, 200, 0, 1, 2, 3, 4)) &&
           equal(runtime_prefix, vector(100, 200, 0, 1)) &&
           equal(runtime_text, string("xa.b")) &&
           equal(runtime_destination, vector(100, 200)) &&
           count(runtime_source) == 5)
              ? 0
              : 1;
 }
 ~~~~~
 */
template <typename Dest, typename Source>
    requires concepts_detail::MaterializableSource<Dest, Source>
[[nodiscard]] constexpr auto into(const Dest& destination, const Source& source) noexcept -> Dest {
    Dest result = destination;
    for (auto&& item : source) {
        const auto value = typename Dest::value_type(item);
        if (!concepts_detail::materialization_has_capacity(result, value)) {
            break;
        }
        result = result.conj(value);
    }
    return result;
}

// Diagnostic fallback (REQ-DIAG-009): a call outside the admitted domain names
// the domain instead of listing failed concept candidates. It produces no value.
template <typename Dest, typename Source>
    requires(!concepts_detail::MaterializableSource<Dest, Source>)
constexpr auto into([[maybe_unused]] const Dest& destination, [[maybe_unused]] const Source& source) noexcept -> void {
    static_assert(
        concepts_detail::dependent_false<Dest, Source>,
        "cljonic::into: expected a Vector, Set, Map, Queue, or String destination and a compatible cljonic "
        "collection or producer source. Source values must convert safely to the destination's element type; a "
        "Map requires its declared MapEntry type. External C++ ranges are not supported.");
}

} // namespace cljonic
