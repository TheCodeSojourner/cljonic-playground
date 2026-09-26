#pragma once

#include <cljonic-concepts.hpp>

namespace cljonic {

/** \anchor MapEntry
 * \b MapEntry is a value-semantic pair representing a single key-value association. Keys must support reliable,
 * non-throwing equality comparison and non-throwing storage operations. Values must be default-constructible,
 * copyable, assignable, and destructible without throwing so the entry remains safe to copy and update in bounded
 * collections.
 *
 * \b Examples
 * ~~~~~{.cpp}
 * #include "cljonic.hpp"
 *
 * int main() {
 *   using namespace cljonic;
 *
 *   // Compile-time demonstration.
 *   constexpr MapEntry<int, int> e_const{1, 100};
 *   static_assert(e_const == MapEntry<int, int>{1, 100});
 *
 *   // Runtime demonstration.
 *   auto e_runtime = MapEntry<int, int>{2, 200};
 *   const auto equal_runtime = e_runtime == MapEntry<int, int>{2, 200};
 *
 *   return equal_runtime ? 0 : 1;
 * }
 * ~~~~~
 */
template <concepts::NothrowStableEqualityComparable KeyType, concepts::NothrowCollectionElement ValueType>
struct MapEntry {
    using key_type = KeyType;
    using value_type = ValueType;
    using mapped_type = ValueType;

    KeyType key{};
    ValueType value{};

    [[nodiscard]] constexpr auto operator==(const MapEntry& other) const noexcept -> bool
        requires concepts::StableEqualityComparable<KeyType> && concepts::StableEqualityComparable<ValueType>
    {
        return key == other.key && value == other.value;
    }
};

} // namespace cljonic

namespace cljonic::concepts_detail {

// MapEntry inherits the recursive component analysis of the closed cljonic
// value domain: it admits stable equality only when both its key and value
// components admit stable equality (REQ-CAP-010).
template <typename KeyType, typename ValueType>
struct contains_floating_point<cljonic::MapEntry<KeyType, ValueType>>
    : std::bool_constant<contains_floating_point_v<KeyType> || contains_floating_point_v<ValueType>> {};

template <typename KeyType, typename ValueType>
struct contains_callable<cljonic::MapEntry<KeyType, ValueType>>
    : std::bool_constant<contains_callable_v<KeyType> || contains_callable_v<ValueType>> {};

} // namespace cljonic::concepts_detail
