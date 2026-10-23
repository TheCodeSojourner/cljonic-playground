#pragma once

#include <cljonic-concepts.hpp>
#include <type_traits>
#include <utility>

namespace cljonic {

namespace concepts_detail {

template <typename C, typename... Values>
concept DisjAdmissible = concepts::CljonicSet<C> && requires { typename C::value_type; } &&
                         (concepts::SafeImplicitConversion<typename C::value_type, Values> && ...);

} // namespace concepts_detail

/** \anchor Disj
 * The \b disj free function removes zero or more elements from a \ref Set "Set". Values are removed from left to
 * right; absent or repeated values are no-ops after any prior removal. Calling `disj(set)` with no values returns an
 * unchanged copy.
 *
 * `disj(set, values...)` accepts the \ref Set "Set"'s element type and safe numeric widening conversions. A conversion
 * must preserve every source value.
 *
 * \b Examples
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 int main() {
   // Compile-time demonstration.
   constexpr auto s0_const = hash_set(42, 7, 8);
   constexpr auto s1_const = disj(s0_const, 42);
   constexpr auto s2_const = disj(s1_const, 7, 8, 7);
   constexpr auto s3_const = disj(hash_set(1, 3), 1, 3, 1);
   constexpr auto s4_const = disj(s2_const);
   static_assert(!contains(s1_const, 42));
   static_assert(is_empty(s2_const));
   static_assert(is_empty(s3_const));
   static_assert(equal(s4_const, s2_const));

   // Runtime demonstration.
   auto s0_runtime = hash_set(99);
   auto s1_runtime = disj(s0_runtime, 99);

   return (!contains(s1_runtime, 99) && is_empty(s1_runtime)) ? 0 : 1;
 }
 ~~~~~
 */
template <typename C, typename... Values>
    requires concepts_detail::DisjAdmissible<C, Values...>
[[nodiscard]] constexpr auto disj(const C& collection, const Values&... values) noexcept -> C {
    C result = collection;
    ((result = result.disj(values)), ...);
    return result;
}

// Diagnostic fallback (REQ-DIAG-009): rejected collection and value domains
// are explained at the free-function boundary without accepting the call.
template <typename C, typename... Values>
    requires(!concepts_detail::DisjAdmissible<C, Values...>)
[[nodiscard]] constexpr auto disj([[maybe_unused]] const C& collection,
                                  [[maybe_unused]] const Values&... values) noexcept {
    if constexpr (!concepts::CljonicSet<C>) {
        static_assert(concepts_detail::dependent_false<C>, "cljonic::disj: the first argument must be a Set.");
    } else {
        static_assert(concepts_detail::dependent_false<C, Values...>,
                      "cljonic::disj: every value to remove must convert safely to the set's element type, without "
                      "losing values or changing its meaning.");
    }
}

} // namespace cljonic
