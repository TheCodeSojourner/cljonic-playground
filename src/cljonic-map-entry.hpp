#pragma once

#include <type_traits>
#include <utility>

#include <cljonic-concepts.hpp>

namespace cljonic {

/** \anchor MapEntry
 * A MapEntry is a value-semantic pair representing a single key-value association. Its fields are public, but MapEntry
 * is not a C++ aggregate.
 *
 * Use the \ref MapEntryFactory "map_entry" factory as the idiomatic creation surface at runtime or during constant
 * evaluation. A MapEntry is a fixed pair, so the factory has no capacity parameter.
 *
 * Keys must support reliable, non-throwing equality comparison and non-throwing storage operations. Values must have a
 * default value and be copyable, assignable, and destructible without throwing, so the entry remains safe to copy and
 * update in bounded collections. Keys and values may themselves be cljonic collections or producers, including nested
 * cljonic composites, provided they satisfy their respective constraints.
 *
 * Key and value construction follows the \ref implicit-conversions "safe implicit conversion rules".
 *
 * \b Examples
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 int main() {
   // Compile-time demonstration.
   constexpr auto m_const = hash_map(1, 100, 2, 200);
   static_assert(first(m_const) == map_entry(1, 100) ||
                 first(m_const) == map_entry(2, 200));

   // Explicit key and value types are available when deduction is not wanted.
   constexpr auto e_explicit = map_entry<long, long>(1, 100);

   // Runtime demonstration: a Map is created with hash_map and its entries,
   // each a MapEntry value, can be traversed, compared, and added.
   const auto m_runtime = hash_map<int, int, 4>(1, 100, 2, 200);

   auto value_sum = 0;
   for (const auto &entry : m_runtime) {
     value_sum += entry.value;
   }

   const auto m_fuller = conj(m_runtime, map_entry(3, 300));

   return (value_sum == 300 && m_fuller(3) == 300 && e_explicit.key == 1) ? 0
                                                                          : 1;
 }
 ~~~~~
 */
template <concepts::NothrowStableEqualityComparable KeyType, concepts::NothrowCollectionElement ValueType>
struct MapEntry {
    using key_type = KeyType;
    using value_type = ValueType;
    using mapped_type = ValueType;

    KeyType key{};
    ValueType value{};

    constexpr MapEntry() noexcept = default;

    template <typename Key>
        requires concepts::NothrowElementConstruction<KeyType, Key>
    constexpr explicit MapEntry(Key&& entry_key) noexcept : key(std::forward<Key>(entry_key)) {
    }

    template <typename Key, typename Value>
        requires concepts::NothrowElementConstruction<KeyType, Key> &&
                     concepts::NothrowElementConstruction<ValueType, Value>
    constexpr MapEntry(Key&& entry_key, Value&& entry_value) noexcept
        : key(std::forward<Key>(entry_key)), value(std::forward<Value>(entry_value)) {
    }

    [[nodiscard]] constexpr auto operator==(const MapEntry& other) const noexcept -> bool
        requires concepts::StableEqualityComparable<KeyType> && concepts::StableEqualityComparable<ValueType>
    {
        return key == other.key && value == other.value;
    }
};

template <typename Key, typename Value>
MapEntry(Key, Value) -> MapEntry<Key, Value>;

} // namespace cljonic

namespace cljonic::concepts_detail {

template <typename KeyType, typename ValueType>
inline constexpr bool is_cljonic_map_entry_v<cljonic::MapEntry<KeyType, ValueType>> = true;

// MapEntry inherits the recursive component analysis of the closed cljonic
// value domain: it admits stable equality only when both its key and value
// components admit stable equality (REQ-CAP-010).
template <typename KeyType, typename ValueType>
struct contains_floating_point<cljonic::MapEntry<KeyType, ValueType>>
    : std::bool_constant<contains_floating_point_v<KeyType> || contains_floating_point_v<ValueType>> {};

template <typename KeyType, typename ValueType>
struct contains_callable<cljonic::MapEntry<KeyType, ValueType>>
    : std::bool_constant<contains_callable_v<KeyType> || contains_callable_v<ValueType>> {};

template <typename KeyType, typename ValueType>
struct contains_standard_range<cljonic::MapEntry<KeyType, ValueType>>
    : std::bool_constant<contains_standard_range_v<KeyType> || contains_standard_range_v<ValueType>> {};

struct DeducedMapEntryKeyType {};
struct DeducedMapEntryValueType {};

template <typename RequestedKeyType, typename RequestedValueType, typename... Args>
struct map_entry_factory_types {
    using key_type = RequestedKeyType;
    using value_type = RequestedValueType;
};

template <typename KeyArgument, typename ValueArgument>
struct map_entry_factory_types<DeducedMapEntryKeyType, DeducedMapEntryValueType, KeyArgument, ValueArgument> {
    using key_type = std::remove_cvref_t<KeyArgument>;
    using value_type = std::remove_cvref_t<ValueArgument>;
};

template <typename RequestedKeyType, typename RequestedValueType, typename... Args>
using map_entry_factory_key_type_t = map_entry_factory_types<RequestedKeyType, RequestedValueType, Args...>::key_type;

template <typename RequestedKeyType, typename RequestedValueType, typename... Args>
using map_entry_factory_value_type_t =
    map_entry_factory_types<RequestedKeyType, RequestedValueType, Args...>::value_type;

template <typename KeyType, typename ValueType, typename... Args>
struct map_entry_factory_arguments_are_nothrow : std::false_type {};

template <typename KeyType, typename ValueType, typename KeyArgument, typename ValueArgument>
struct map_entry_factory_arguments_are_nothrow<KeyType, ValueType, KeyArgument, ValueArgument>
    : std::bool_constant<concepts::NothrowElementConstruction<KeyType, KeyArgument> &&
                         concepts::NothrowElementConstruction<ValueType, ValueArgument>> {};

template <typename KeyType, typename ValueType>
concept MapEntryFactoryElementTypes =
    concepts::NothrowStableEqualityComparable<KeyType> && concepts::NothrowCollectionElement<ValueType>;

template <typename KeyType, typename ValueType, typename... Args>
concept MapEntryFactoryArguments = map_entry_factory_arguments_are_nothrow<KeyType, ValueType, Args...>::value;

} // namespace cljonic::concepts_detail

namespace cljonic {

/** \anchor MapEntryFactory
 * Creates a \ref MapEntry "MapEntry" from exactly one key and one value.
 *
 * Deduced calls use the unqualified types of the key and value arguments. Explicit types use \c map_entry<Key,
 * Value>(...). A MapEntry is a fixed pair, so unlike the collection factories there is no capacity parameter and no
 * spare-capacity form.
 */
template <typename RequestedKeyType = concepts_detail::DeducedMapEntryKeyType,
          typename RequestedValueType = concepts_detail::DeducedMapEntryValueType, typename... Args>
    requires(
        concepts_detail::MapEntryFactoryElementTypes<
            concepts_detail::map_entry_factory_key_type_t<RequestedKeyType, RequestedValueType, Args...>,
            concepts_detail::map_entry_factory_value_type_t<RequestedKeyType, RequestedValueType, Args...>> &&
        sizeof...(Args) == 2U &&
        concepts_detail::MapEntryFactoryArguments<
            concepts_detail::map_entry_factory_key_type_t<RequestedKeyType, RequestedValueType, Args...>,
            concepts_detail::map_entry_factory_value_type_t<RequestedKeyType, RequestedValueType, Args...>, Args...>)
[[nodiscard]] constexpr auto map_entry(Args&&... args) noexcept
    -> MapEntry<concepts_detail::map_entry_factory_key_type_t<RequestedKeyType, RequestedValueType, Args...>,
                concepts_detail::map_entry_factory_value_type_t<RequestedKeyType, RequestedValueType, Args...>> {
    using KeyType = concepts_detail::map_entry_factory_key_type_t<RequestedKeyType, RequestedValueType, Args...>;
    using ValueType = concepts_detail::map_entry_factory_value_type_t<RequestedKeyType, RequestedValueType, Args...>;
    return MapEntry<KeyType, ValueType>{std::forward<Args>(args)...};
}

template <typename RequestedKeyType = concepts_detail::DeducedMapEntryKeyType,
          typename RequestedValueType = concepts_detail::DeducedMapEntryValueType, typename... Args>
    requires(sizeof...(Args) != 2U)
constexpr auto map_entry([[maybe_unused]] Args&&... args) noexcept -> void {
    static_assert(concepts_detail::dependent_false<RequestedKeyType, RequestedValueType, Args...>,
                  "cljonic::map_entry: requires exactly one key argument and one value argument.");
}

template <typename RequestedKeyType = concepts_detail::DeducedMapEntryKeyType,
          typename RequestedValueType = concepts_detail::DeducedMapEntryValueType, typename... Args>
    requires(sizeof...(Args) == 2U &&
             !concepts_detail::MapEntryFactoryElementTypes<
                 concepts_detail::map_entry_factory_key_type_t<RequestedKeyType, RequestedValueType, Args...>,
                 concepts_detail::map_entry_factory_value_type_t<RequestedKeyType, RequestedValueType, Args...>>)
constexpr auto map_entry([[maybe_unused]] Args&&... args) noexcept -> void {
    static_assert(concepts_detail::dependent_false<RequestedKeyType, RequestedValueType, Args...>,
                  "cljonic::map_entry: key and value types must satisfy MapEntry element requirements.");
}

template <typename RequestedKeyType = concepts_detail::DeducedMapEntryKeyType,
          typename RequestedValueType = concepts_detail::DeducedMapEntryValueType, typename... Args>
    requires(
        sizeof...(Args) == 2U &&
        concepts_detail::MapEntryFactoryElementTypes<
            concepts_detail::map_entry_factory_key_type_t<RequestedKeyType, RequestedValueType, Args...>,
            concepts_detail::map_entry_factory_value_type_t<RequestedKeyType, RequestedValueType, Args...>> &&
        !concepts_detail::MapEntryFactoryArguments<
            concepts_detail::map_entry_factory_key_type_t<RequestedKeyType, RequestedValueType, Args...>,
            concepts_detail::map_entry_factory_value_type_t<RequestedKeyType, RequestedValueType, Args...>, Args...>)
constexpr auto map_entry([[maybe_unused]] Args&&... args) noexcept -> void {
    static_assert(concepts_detail::dependent_false<RequestedKeyType, RequestedValueType, Args...>,
                  "cljonic::map_entry: the key and value must safely construct their declared types without "
                  "throwing.");
}

} // namespace cljonic
