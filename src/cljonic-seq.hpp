#pragma once

#include <cljonic-concepts.hpp>
#include <cljonic-config.hpp>
#include <cljonic-vector.hpp>

#include <cstddef>
#include <ranges>
#include <type_traits>

namespace cljonic {

/** \anchor Seq
 * The \b seq free function returns the logical traversal elements of a `Vector`, `Map`, `Set`, `Queue`, or `String`
 * value, or of a supported \ref Range "Range", \ref Repeat "Repeat", \ref Cycle "Cycle", \ref Iterate "Iterate", or
 * \ref Repeatedly "Repeatedly" producer, as an owning, value-semantic \ref Vector "Vector" in traversal order.
 *
 * A \ref Vector "Vector" input returns a by-value copy of itself. A \ref Map "Map" input yields a vector of
 * value-semantic \ref MapEntryFactory "map_entry" values. A \ref Set "Set" or \ref Queue "Queue" input yields a vector
 * of its stored elements. A \ref String "String" input yields a vector of its characters. A producer input
 * materializes its observable traversal into a vector of \b CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE capacity; a
 * bounded producer contributes exactly its count, and an unbounded producer contributes the full observation cap. \ref
 * Map "Map" and \ref Set "Set" traversal order is not semantically significant. The result is independently valid: it
 * uses bounded automatic or static storage and never retains a dependency on the source value. The input is preserved.
 *
 * \ref MapEntryFactory "map_entry" values and external C++ ranges are not supported.
 *
 \b Examples
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 constexpr auto add_one(int value) noexcept -> int { return value + 1; }

 int main() {
   // Compile-time demonstration.
   constexpr auto v_const = vector(1, 2, 3);
   constexpr auto s_const = seq(v_const);
   static_assert(count(s_const) == 3);
   constexpr auto m_const = seq(hash_map(1, 100));
   static_assert(first(m_const) == map_entry(1, 100));
   constexpr auto text_const = seq(string("AB"));
   static_assert(count(text_const) == 2 && first(text_const) == 'A');
   constexpr auto r_const = seq(Range{0, 5});
   static_assert(count(r_const) == 5 && equal(r_const, vector(0, 1, 2, 3, 4)));

   // An unbounded producer materializes its full observation cap.
   constexpr auto i_const = seq(iterate(add_one, 0));
   static_assert(count(i_const) ==
                 CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE);
   static_assert(i_const(0) == 0 && i_const(999) == 999);

   // Runtime demonstration.
   constexpr int runtime_first = 10;
   const auto v_runtime = vector(runtime_first, 20);
   const auto s_runtime = seq(v_runtime);
   const auto q_runtime = seq(queue(5, 6));
   const auto t_runtime = seq(string("CD"));

   // A produced sequence materializes its observed traversal, observable
   // through const range traversal.
   const auto i_runtime =
       seq(iterate([](int value) noexcept { return value + 1; }, 0));
   auto iterate_length = std::size_t{0};
   auto iterate_sum = 0;
   for (const auto value : i_runtime) {
     iterate_sum += value;
     ++iterate_length;
   }

   return (count(s_runtime) == 2 && s_runtime(0) == 10 && s_runtime(1) == 20 &&
           equal(q_runtime, vector(5, 6)) &&
           equal(t_runtime, vector('C', 'D')) &&
           iterate_length == CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE &&
           iterate_sum == 499500)
              ? 0
              : 1;
 }
 ~~~~~
 */
namespace concepts_detail {

template <typename Result, typename C>
[[nodiscard]] constexpr auto materialize_traversal(const C& value) noexcept -> Result {
    auto result = Result{};
    for (auto&& item : value) {
        result = result.conj(typename Result::value_type(item));
    }
    return result;
}

} // namespace concepts_detail

template <concepts::CljonicVector C>
[[nodiscard]] constexpr auto seq(const C& value) noexcept -> C {
    return value;
}

template <concepts::CljonicCollection C>
    requires(!concepts::CljonicVector<C>)
[[nodiscard]] constexpr auto seq(const C& value) noexcept {
    return concepts_detail::materialize_traversal<Vector<typename C::value_type, C::capacity()>>(value);
}

template <concepts::SequenceableProducer C>
[[nodiscard]] constexpr auto seq(const C& value) noexcept {
    return concepts_detail::materialize_traversal<
        Vector<typename C::value_type, CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE>>(value);
}

// Diagnostic fallback (REQ-DIAG-009): a call outside the admitted domain names
// the domain instead of listing failed concept candidates. It produces no value.
template <typename C>
    requires(!concepts::CljonicSource<C>)
constexpr auto seq([[maybe_unused]] const C& value) noexcept -> void {
    static_assert(concepts_detail::dependent_false<C>, "cljonic::seq: expected a Vector, Map, Set, Queue, String, "
                                                       "Range, Repeat, Cycle, Iterate, or Repeatedly value.");
}

} // namespace cljonic
