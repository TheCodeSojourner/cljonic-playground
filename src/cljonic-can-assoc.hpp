#pragma once

#include <cljonic-concepts.hpp>

namespace cljonic {

/** \anchor CanAssoc
 * \brief Checks if assoc with a key and a value can succeed without capacity overflow.
 *
 * Supported for `Map` (an existing key can be replaced, or an absent key inserted when capacity remains), `Vector` (an
 * existing index can be replaced, or an index equal to the current count can append when capacity remains), and
 * `String` (the same index and append rule as Vector, with an invalid character returning false). `Set` and `Queue`
 * provide no associative capability and are rejected. This preflight leaves the source unchanged.
 *
 * \b Examples
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 int main() {
   // Compile-time demonstration.
   constexpr Map<int, int, 4> m_const{};
   static_assert(can_assoc(m_const, 1, 10));

   // Runtime demonstration.
   auto m_runtime = Map<int, int, 4>{};
   const auto ok = can_assoc(m_runtime, 2, 20);

   return ok ? 0 : 1;
 }
 ~~~~~
 */
template <typename C, typename K, typename V>
    requires concepts::AssociativeCollection<C> && requires(const C& collection, const K& key, const V& value) {
        { collection.can_assoc(key, value) } noexcept -> std::same_as<bool>;
    }
[[nodiscard]] constexpr auto can_assoc(const C& collection, const K& key,
                                       const V& value) noexcept(noexcept(collection.can_assoc(key, value))) -> bool {
    return collection.can_assoc(key, value);
}

namespace concepts_detail {

// Test key and value admission separately so each rejected operand gets a
// focused diagnostic.
template <typename C, typename K>
concept CanAssocKeyAdmissible = concepts::AssociativeCollection<C> &&
                                requires(const C& collection, const K& key, const C::association_value_type& value) {
                                    { collection.can_assoc(key, value) } noexcept -> std::same_as<bool>;
                                };

template <typename C, typename V>
concept CanAssocValueAdmissible =
    concepts::AssociativeCollection<C> && requires(const C& collection, const C::key_type& key, const V& value) {
        { collection.can_assoc(key, value) } noexcept -> std::same_as<bool>;
    };

} // namespace concepts_detail

// Diagnostic fallbacks (REQ-DIAG-009): one targeted message per rejection mode;
// never a supported call target.
template <typename C, typename K, typename V>
    requires concepts::CljonicCollection<C> && (!concepts::AssociativeCollection<C>)
[[nodiscard]] constexpr auto can_assoc([[maybe_unused]] const C& collection, [[maybe_unused]] const K& key,
                                       [[maybe_unused]] const V& value) noexcept -> bool {
    static_assert(concepts_detail::dependent_false<C>,
                  "cljonic::can_assoc: the first argument must be a Map, a Vector, or a String. A Set or Queue is "
                  "not supported.");
    return false;
}

template <typename C, typename K, typename V>
    requires concepts::AssociativeCollection<C> && (!concepts_detail::CanAssocKeyAdmissible<C, K>)
[[nodiscard]] constexpr auto can_assoc([[maybe_unused]] const C& collection, [[maybe_unused]] const K& key,
                                       [[maybe_unused]] const V& value) noexcept -> bool {
    static_assert(concepts_detail::dependent_false<C>,
                  "cljonic::can_assoc: this key has a type the collection cannot associate. A Map takes a key; a "
                  "Vector or a String takes an integer index.");
    return false;
}

template <typename C, typename K, typename V>
    requires concepts::AssociativeCollection<C> && concepts_detail::CanAssocKeyAdmissible<C, K> &&
             (!concepts_detail::CanAssocValueAdmissible<C, V>)
[[nodiscard]] constexpr auto can_assoc([[maybe_unused]] const C& collection, [[maybe_unused]] const K& key,
                                       [[maybe_unused]] const V& value) noexcept -> bool {
    static_assert(concepts_detail::dependent_false<C>,
                  "cljonic::can_assoc: this value has a type the collection cannot associate. The value must match "
                  "the type stored by the collection.");
    return false;
}

} // namespace cljonic
