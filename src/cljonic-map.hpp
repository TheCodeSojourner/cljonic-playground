#pragma once

#include <array>
#include <cstddef>
#include <ranges>
#include <span>
#include <tuple>
#include <utility>

#include <cljonic-concepts.hpp>
#include <cljonic-config.hpp>
#include <cljonic-core-collection-maximum-element-count.hpp>
#include <cljonic-map-entry.hpp>

namespace cljonic {

/** \anchor Map
 * A Map is a bounded, associative collection that maps unique keys to values that provides callable lookup with
 * optional fallback values. The way to operate on the collection is through the library's free-function API.
 * Construction with more entries than the available capacity is rejected at compile time. A non-cljonic C++ range or
 * view constructor parameter is copied into the \ref Map "Map"; a cljonic collection or producer is not a constructor
 * source. To materialize a cljonic collection or producer into a destination \ref Vector "Vector", use \b into.
 *
 Keys, mapped values, and fallbacks follow the \ref implicit-conversions "safe implicit conversion rules".
 Construction from entries requires the exact declared entry type.

 \b Examples
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 struct Key {
   int id;
   friend constexpr bool operator==(const Key &, const Key &) noexcept = default;
 };

 struct Value {
   int amount = 0;
   friend constexpr bool operator==(const Value &,
                                    const Value &) noexcept = default;
 };

 using AccountEntry = MapEntry<Key, Value>;
 using AccountMap = Map<Key, Value, 2>;

 int main() {
   // The hash_map factory accepts alternating keys and values.
   constexpr auto factory_map = hash_map(Key{1}, Value{10}, Key{2}, Value{20});
   static_assert(factory_map(Key{2}).amount == 20);

   // Explicit key, value, and capacity types create an empty Map with room
   // for later associations.
   constexpr auto reserved = hash_map<Key, Value, 4>();
   static_assert(reserved(Key{1}).amount == 0);

   // The factory also works with runtime values.
   const auto runtime_amount = 40;
   const auto runtime_factory = hash_map(Key{4}, Value{runtime_amount});

   // Runtime CTAD deduces Map<Key, Value, 1> from the MapEntry argument.
   auto runtime = Map{AccountEntry{Key{3}, Value{30}}};
   const auto present = runtime(Key{3});
   const auto missing = runtime(Key{4}, Value{77});

   // A standard view pipeline can use an existing Map as its source and
   // materialize the transformed entries into another Map.
   const auto copied_view =
       runtime | std::views::transform([](const AccountEntry &entry) noexcept {
         return AccountEntry{entry.key, Value{entry.value.amount * 2}};
       });
   const auto from_pipeline = AccountMap{copied_view};

   // A named map type and its named entry type make the intended value model
   // explicit. A later duplicate key replaces the earlier value (i.e., the
   // right-most value associated with a duplicate key).
   constexpr auto literal = AccountMap{AccountEntry{Key{1}, Value{10}},
                                       AccountEntry{Key{1}, Value{20}}};

   // A Map is callable for present-key lookup and missing-key fallback.
   static_assert(literal(Key{1}).amount == 20);
   static_assert(literal(Key{2}).amount == 0);
   static_assert(literal(Key{2}, Value{99}).amount == 99);

   // ---------------------------------------------------------------------------
   // C++ interoperability: a Map supports const traversal and exposes a
   // non-owning std::span view. Range/view sources are copied into owned
   // storage, retaining only the bounded prefix that fits the capacity.
   // ---------------------------------------------------------------------------
   static constexpr AccountEntry source_entries[] = {
       AccountEntry{Key{10}, Value{100}}, AccountEntry{Key{20}, Value{200}}};
   constexpr auto source_span = std::span{source_entries};
   constexpr auto from_span = AccountMap{source_span};
   static_assert(from_span(Key{10}).amount == 100);
   static_assert(from_span(Key{20}).amount == 200);

   constexpr auto from_span_ctad = Map{source_span};
   static_assert(from_span_ctad(Key{10}).amount == 100);
   static_assert(from_span_ctad.view().size() == 2);

   // Constructing a Map from a runtime C++ array/span
   AccountEntry runtime_buffer[] = {AccountEntry{Key{100}, Value{1000}},
                                    AccountEntry{Key{200}, Value{2000}}};
   const auto runtime_from_span =
       Map<Key, Value, 4>{std::span<const AccountEntry>{runtime_buffer, 2}};

   // Use C++ interoperability to sum the values in a map.
   auto value_sum = 0;
   for (const auto &entry : runtime) {
     value_sum += entry.value.amount;
   }

   const auto runtime_view = runtime.view();

   return (present.amount == 30 && missing.amount == 77 && value_sum == 30 &&
           runtime_view.size() == 1 && runtime_view[0].value.amount == 30 &&
           factory_map(Key{1}).amount == 10 &&
           runtime_factory(Key{4}).amount == 40 &&
           from_span(Key{10}).amount == 100 &&
           from_pipeline(Key{3}).amount == 60 &&
           runtime_from_span(Key{100}).amount == 1000)
              ? 0
              : 1;
 }
 ~~~~~
 */
template <concepts::NothrowStableEqualityComparable KeyType, concepts::NothrowCollectionElement ValueType,
          std::size_t CapacityValue>
    requires(!concepts_detail::is_std_variant_v<KeyType>)
class Map {
  public:
    using key_type = KeyType;
    using lookup_type = key_type;
    using mapped_type = ValueType;
    using association_value_type = ValueType;
    using value_type = MapEntry<KeyType, ValueType>;

    static_assert(
        CapacityValue <= cljonic::CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE,
        "cljonic::Map: CapacityValue exceeds "
        "CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT=" CLJONIC_STRINGIFY(CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT));

    template <typename... Args>
    explicit(sizeof...(Args) ==
             1) constexpr Map(Args&&... args) noexcept((concepts::NothrowElementConstruction<value_type, Args> &&
                                                        ...)) {
        static_assert(sizeof...(Args) <= CapacityValue, "cljonic::Map: initializer count exceeds Map CapacityValue");
        static_assert(((std::same_as<std::remove_cvref_t<Args>, value_type> &&
                        concepts::NothrowElementConstruction<value_type, Args>) &&
                       ...),
                      "cljonic::Map: constructor requires all arguments to construct "
                      "the exact MapEntry type without throwing");

        ((*this = assoc_entry(value_type{std::forward<Args>(args)})), ...);
    }

    // A cljonic collection or producer is not a SourceConstruction source
    // (REQ-FN-027A); materialize it via into, preflighted by fits_into.
    template <std::ranges::input_range SourceRange>
        requires(!concepts_detail::is_cljonic_collection_v<SourceRange> &&
                 !concepts_detail::is_cljonic_producer_v<SourceRange> &&
                 !std::same_as<std::remove_cvref_t<SourceRange>, value_type>)
    explicit constexpr Map(SourceRange&& source) noexcept(
        (concepts::NothrowRangeConstruction<value_type, SourceRange>)) {
        static_assert(concepts::NothrowRangeConstruction<value_type, SourceRange>,
                      "cljonic::Map: range/view constructor requires each source element to construct "
                      "the exact MapEntry type and requires non-throwing traversal");
        static_assert(concepts_detail::static_extent_fits_v<SourceRange, CapacityValue>,
                      "cljonic::Map: static-extent range source exceeds Map CapacityValue");

        std::size_t copy_count = 0;
        for (auto&& item : std::forward<SourceRange>(source)) {
            if (copy_count++ >= CapacityValue) {
                break;
            }
            *this = assoc_entry(value_type{std::forward<decltype(item)>(item)});
        }
    }

    // Diagnostic fallback (REQ-DIAG-010): a cljonic collection or producer is
    // not a SourceConstruction source. This overload exists only to explain the
    // rejection -- it produces no value and is never a supported call target.
    template <typename SourceValue>
        requires((concepts_detail::is_cljonic_collection_v<SourceValue> ||
                  concepts_detail::is_cljonic_producer_v<SourceValue>) &&
                 !std::same_as<std::remove_cvref_t<SourceValue>, value_type>)
    explicit constexpr Map([[maybe_unused]] SourceValue&& source) noexcept {
        static_assert(concepts_detail::dependent_false<SourceValue>,
                      "cljonic::Map: you cannot construct a Map from a cljonic collection or producer. A Map is built "
                      "from MapEntry elements, or from a non-cljonic C++ range or view. To materialize a cljonic "
                      "collection or producer into a Vector, use into(destination, source); call "
                      "fits_into(destination, source) first to check whether the complete source fits in the Vector's "
                      "remaining capacity.");
    }

    [[nodiscard]] static constexpr auto capacity() noexcept -> std::size_t {
        return CapacityValue;
    }

    [[nodiscard]] constexpr auto count() const noexcept -> std::size_t {
        return logical_size_;
    }

    [[nodiscard]] constexpr auto operator==(const Map& other) const noexcept -> bool
        requires concepts::StableEqualityComparable<KeyType> && concepts::StableEqualityComparable<ValueType>
    {
        if (logical_size_ != other.logical_size_) {
            return false;
        }
        for (std::size_t i = 0; i < logical_size_; ++i) {
            if (!matches_entry(other, i)) {
                return false;
            }
        }
        return true;
    }

    [[nodiscard]] constexpr auto contains(const KeyType& key) const noexcept -> bool {
        return find_index(key) < logical_size_;
    }

    [[nodiscard]] constexpr auto operator()(const KeyType& key, const ValueType& fallback = ValueType{}) const noexcept
        -> ValueType {
        const auto idx = find_index(key);
        return (idx < logical_size_) ? entries_[idx].value : fallback;
    }

    [[nodiscard]] constexpr auto can_assoc(const KeyType& key, [[maybe_unused]] const ValueType& value) const noexcept
        -> bool {
        return contains(key) || (logical_size_ < CapacityValue);
    }

    [[nodiscard]] constexpr auto assoc(const KeyType& key, const ValueType& value) const noexcept -> Map {
        Map result = *this;
        const auto idx = result.find_index(key);
        if (idx < result.logical_size_) {
            result.entries_[idx].value = value;
        } else if (result.logical_size_ < CapacityValue) {
            result.entries_[result.logical_size_] = value_type{key, value};
            ++result.logical_size_;
        }
        return result;
    }

    /** Returns true when the entry's key is present or the map has room for one
     *  more entry. */
    [[nodiscard]] constexpr auto can_conj(const value_type& entry) const noexcept -> bool {
        return contains(entry.key) || (logical_size_ < CapacityValue);
    }

    /** Associates the entry's key with its value on a copy: an existing key
     *  replaces its value without increasing count; an absent key is added only
     *  when capacity remains. Returns an unchanged copy when full. */
    [[nodiscard]] constexpr auto conj(const value_type& entry) const noexcept -> Map {
        return assoc(entry.key, entry.value);
    }

    [[nodiscard]] constexpr auto dissoc(const KeyType& key) const noexcept -> Map {
        Map result = *this;
        const auto idx = result.find_index(key);
        if (idx < result.logical_size_) {
            if (idx + 1U < result.logical_size_) {
                result.entries_[idx] = result.entries_[result.logical_size_ - 1U];
            }
            --result.logical_size_;
        }
        return result;
    }

    [[nodiscard]] constexpr auto begin() const noexcept -> const value_type* {
        return entries_.data();
    }

    [[nodiscard]] constexpr auto end() const noexcept -> const value_type* {
        return entries_.data() + logical_size_;
    }

    [[nodiscard]] constexpr auto view() const noexcept -> std::span<const value_type> {
        return {entries_.data(), logical_size_};
    }

    template <typename Key>
        requires(!concepts::SafeImplicitConversion<KeyType, Key>)
    auto contains(const Key& key) const -> bool = delete;

    template <typename Key, typename Fallback = ValueType>
        requires(!concepts::SafeImplicitConversion<KeyType, Key> ||
                 !concepts::SafeImplicitConversion<ValueType, Fallback>)
    auto operator()(const Key& key, const Fallback& fallback = Fallback{}) const -> ValueType = delete;

    template <typename Key, typename Value>
        requires(!concepts::SafeImplicitConversion<KeyType, Key> || !concepts::SafeImplicitConversion<ValueType, Value>)
    auto can_assoc(const Key& key, const Value& value) const -> bool = delete;

    template <typename Key, typename Value>
        requires(!concepts::SafeImplicitConversion<KeyType, Key> || !concepts::SafeImplicitConversion<ValueType, Value>)
    auto assoc(const Key& key, const Value& value) const -> Map = delete;

    template <typename Entry>
        requires(!std::same_as<std::remove_cvref_t<Entry>, value_type>)
    auto can_conj(const Entry& entry) const -> bool = delete;

    template <typename Entry>
        requires(!std::same_as<std::remove_cvref_t<Entry>, value_type>)
    auto conj(const Entry& entry) const -> Map = delete;

    template <typename Key>
        requires(!concepts::SafeImplicitConversion<KeyType, Key>)
    auto dissoc(const Key& key) const -> Map = delete;

  private:
    [[nodiscard]] constexpr auto find_index(const KeyType& key) const noexcept -> std::size_t {
        for (std::size_t i = 0; i < logical_size_; ++i) {
            if (entries_[i].key == key) {
                return i;
            }
        }
        return logical_size_;
    }

    [[nodiscard]] constexpr auto matches_entry(const Map& other, std::size_t index) const noexcept -> bool {
        const auto idx = other.find_index(entries_[index].key);
        if (idx >= other.logical_size_) {
            return false;
        }
        return entries_[index].value == other.entries_[idx].value;
    }

    [[nodiscard]] constexpr auto assoc_entry(const value_type& entry) const noexcept -> Map {
        return assoc(entry.key, entry.value);
    }

    std::array<value_type, CapacityValue> entries_{};
    std::size_t logical_size_{0};
};

template <typename KeyType, typename ValueType, typename... Rest>
Map(MapEntry<KeyType, ValueType>, Rest...) -> Map<KeyType, ValueType, 1 + sizeof...(Rest)>;

template <typename KeyType, typename ValueType, std::size_t Extent>
    requires(Extent != std::dynamic_extent)
Map(std::span<const MapEntry<KeyType, ValueType>, Extent>) -> Map<KeyType, ValueType, Extent>;

template <typename KeyType, typename ValueType, std::size_t Extent>
    requires(Extent != std::dynamic_extent)
Map(std::span<MapEntry<KeyType, ValueType>, Extent>) -> Map<KeyType, ValueType, Extent>;

namespace concepts_detail {

struct DeducedHashMapKeyType {};
struct DeducedHashMapValueType {};

template <typename RequestedKeyType, typename RequestedValueType, typename... Args>
struct hash_map_factory_types {
    using key_type = RequestedKeyType;
    using value_type = RequestedValueType;
};

template <typename FirstKey, typename FirstValue, typename... Rest>
struct hash_map_factory_types<DeducedHashMapKeyType, DeducedHashMapValueType, FirstKey, FirstValue, Rest...> {
    using key_type = std::remove_cvref_t<FirstKey>;
    using value_type = std::remove_cvref_t<FirstValue>;
};

template <typename RequestedKeyType, typename RequestedValueType, typename... Args>
using hash_map_factory_key_type_t = hash_map_factory_types<RequestedKeyType, RequestedValueType, Args...>::key_type;

template <typename RequestedKeyType, typename RequestedValueType, typename... Args>
using hash_map_factory_value_type_t = hash_map_factory_types<RequestedKeyType, RequestedValueType, Args...>::value_type;

template <typename KeyType, typename ValueType, typename Tuple, std::size_t... PairIndices>
[[nodiscard]] consteval auto
hash_map_factory_pairs_are_nothrow([[maybe_unused]] std::index_sequence<PairIndices...> indices) noexcept -> bool {
    return (concepts::NothrowElementConstruction<KeyType, std::tuple_element_t<2U * PairIndices, Tuple>> && ...) &&
           (concepts::NothrowElementConstruction<ValueType, std::tuple_element_t<(2U * PairIndices) + 1U, Tuple>> &&
            ...);
}

template <typename KeyType, typename ValueType, typename... Args>
concept HashMapFactoryPairArguments = sizeof...(Args) > 0U && sizeof...(Args) % 2U == 0U &&
                                      hash_map_factory_pairs_are_nothrow<KeyType, ValueType, std::tuple<Args...>>(
                                          std::make_index_sequence<sizeof...(Args) / 2U>{});

template <typename KeyType, typename ValueType>
concept HashMapFactoryElementTypes = concepts::NothrowStableEqualityComparable<KeyType> &&
                                     concepts::NothrowCollectionElement<ValueType> && !is_std_variant_v<KeyType>;

template <typename KeyType, typename ValueType, std::size_t Capacity, typename... Args>
concept HashMapFactoryCapacityArguments =
    HashMapFactoryElementTypes<KeyType, ValueType> && Capacity <= CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE &&
    sizeof...(Args) % 2U == 0U && sizeof...(Args) / 2U <= Capacity &&
    sizeof...(Args) / 2U <= CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE &&
    (sizeof...(Args) == 0U || HashMapFactoryPairArguments<KeyType, ValueType, Args...>);

template <typename KeyType, typename ValueType, std::size_t Capacity>
[[nodiscard]] constexpr auto build_hash_map_factory_result(Map<KeyType, ValueType, Capacity> result) noexcept
    -> Map<KeyType, ValueType, Capacity> {
    return result;
}

template <typename KeyType, typename ValueType, std::size_t Capacity, typename KeyArgument, typename ValueArgument,
          typename... Rest>
[[nodiscard]] constexpr auto build_hash_map_factory_result(Map<KeyType, ValueType, Capacity> result, KeyArgument&& key,
                                                           ValueArgument&& value, Rest&&... rest) noexcept
    -> Map<KeyType, ValueType, Capacity> {
    return build_hash_map_factory_result(
        result.assoc(std::forward<KeyArgument>(key), std::forward<ValueArgument>(value)), std::forward<Rest>(rest)...);
}

} // namespace concepts_detail

/** \anchor HashMapFactory
 * Creates a bounded \ref Map "Map" from alternating key/value arguments. Deduced calls use the first pair's
 * unqualified types and have capacity equal to their pair count. Explicit types use \c hash_map<Key, Value>(...);
 * explicit capacity uses \c hash_map<Key, Value, Capacity>(...), including for an empty map.
 */
template <typename RequestedKeyType = concepts_detail::DeducedHashMapKeyType,
          typename RequestedValueType = concepts_detail::DeducedHashMapValueType, typename... Args>
    requires(
        concepts_detail::HashMapFactoryElementTypes<
            concepts_detail::hash_map_factory_key_type_t<RequestedKeyType, RequestedValueType, Args...>,
            concepts_detail::hash_map_factory_value_type_t<RequestedKeyType, RequestedValueType, Args...>> &&
        concepts_detail::HashMapFactoryPairArguments<
            concepts_detail::hash_map_factory_key_type_t<RequestedKeyType, RequestedValueType, Args...>,
            concepts_detail::hash_map_factory_value_type_t<RequestedKeyType, RequestedValueType, Args...>, Args...> &&
        sizeof...(Args) / 2U <= CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE)
[[nodiscard]] constexpr auto hash_map(Args&&... args) noexcept
    -> Map<concepts_detail::hash_map_factory_key_type_t<RequestedKeyType, RequestedValueType, Args...>,
           concepts_detail::hash_map_factory_value_type_t<RequestedKeyType, RequestedValueType, Args...>,
           sizeof...(Args) / 2U> {
    using KeyType = concepts_detail::hash_map_factory_key_type_t<RequestedKeyType, RequestedValueType, Args...>;
    using ValueType = concepts_detail::hash_map_factory_value_type_t<RequestedKeyType, RequestedValueType, Args...>;
    return concepts_detail::build_hash_map_factory_result<KeyType, ValueType, sizeof...(Args) / 2U>(
        Map<KeyType, ValueType, sizeof...(Args) / 2U>{}, std::forward<Args>(args)...);
}

template <typename RequestedKeyType = concepts_detail::DeducedHashMapKeyType,
          typename RequestedValueType = concepts_detail::DeducedHashMapValueType, typename... Args>
    requires(sizeof...(Args) == 0U || sizeof...(Args) % 2U != 0U)
constexpr auto hash_map([[maybe_unused]] Args&&... args) noexcept -> void {
    static_assert(concepts_detail::dependent_false<RequestedKeyType, RequestedValueType, Args...>,
                  "cljonic::hash_map: empty calls require explicit key, value, and capacity types; nonempty calls "
                  "require complete alternating key/value pairs.");
}

template <typename RequestedKeyType = concepts_detail::DeducedHashMapKeyType,
          typename RequestedValueType = concepts_detail::DeducedHashMapValueType, typename... Args>
    requires(sizeof...(Args) > 0U && sizeof...(Args) % 2U == 0U &&
             !concepts_detail::HashMapFactoryElementTypes<
                 concepts_detail::hash_map_factory_key_type_t<RequestedKeyType, RequestedValueType, Args...>,
                 concepts_detail::hash_map_factory_value_type_t<RequestedKeyType, RequestedValueType, Args...>>)
constexpr auto hash_map([[maybe_unused]] Args&&... args) noexcept -> void {
    static_assert(concepts_detail::dependent_false<RequestedKeyType, RequestedValueType, Args...>,
                  "cljonic::hash_map: key and value types must satisfy Map element requirements.");
}

template <typename RequestedKeyType = concepts_detail::DeducedHashMapKeyType,
          typename RequestedValueType = concepts_detail::DeducedHashMapValueType, typename... Args>
    requires(sizeof...(Args) > 0U && sizeof...(Args) % 2U == 0U &&
             concepts_detail::HashMapFactoryElementTypes<
                 concepts_detail::hash_map_factory_key_type_t<RequestedKeyType, RequestedValueType, Args...>,
                 concepts_detail::hash_map_factory_value_type_t<RequestedKeyType, RequestedValueType, Args...>> &&
             sizeof...(Args) / 2U > CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE)
constexpr auto hash_map([[maybe_unused]] Args&&... args) noexcept -> void {
    static_assert(concepts_detail::dependent_false<RequestedKeyType, RequestedValueType, Args...>,
                  "cljonic::hash_map: pair count exceeds CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT.");
}

template <typename RequestedKeyType = concepts_detail::DeducedHashMapKeyType,
          typename RequestedValueType = concepts_detail::DeducedHashMapValueType, typename... Args>
    requires(
        sizeof...(Args) > 0U && sizeof...(Args) % 2U == 0U &&
        concepts_detail::HashMapFactoryElementTypes<
            concepts_detail::hash_map_factory_key_type_t<RequestedKeyType, RequestedValueType, Args...>,
            concepts_detail::hash_map_factory_value_type_t<RequestedKeyType, RequestedValueType, Args...>> &&
        sizeof...(Args) / 2U <= CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE &&
        !concepts_detail::HashMapFactoryPairArguments<
            concepts_detail::hash_map_factory_key_type_t<RequestedKeyType, RequestedValueType, Args...>,
            concepts_detail::hash_map_factory_value_type_t<RequestedKeyType, RequestedValueType, Args...>, Args...>)
constexpr auto hash_map([[maybe_unused]] Args&&... args) noexcept -> void {
    static_assert(concepts_detail::dependent_false<RequestedKeyType, RequestedValueType, Args...>,
                  "cljonic::hash_map: every key and value must safely construct its declared type without "
                  "throwing.");
}

/** Creates a bounded Map with explicit capacity, including an empty Map. */
template <typename KeyType, typename ValueType, std::size_t Capacity, typename... Args>
    requires concepts_detail::HashMapFactoryCapacityArguments<KeyType, ValueType, Capacity, Args...>
[[nodiscard]] constexpr auto hash_map(Args&&... args) noexcept -> Map<KeyType, ValueType, Capacity> {
    return concepts_detail::build_hash_map_factory_result<KeyType, ValueType, Capacity>(
        Map<KeyType, ValueType, Capacity>{}, std::forward<Args>(args)...);
}

template <typename KeyType, typename ValueType, std::size_t Capacity, typename... Args>
    requires(!concepts_detail::HashMapFactoryElementTypes<KeyType, ValueType>)
constexpr auto hash_map([[maybe_unused]] Args&&... args) noexcept -> void {
    static_assert(concepts_detail::dependent_false<KeyType, ValueType, Args...>,
                  "cljonic::hash_map: key and value types must satisfy Map element requirements.");
}

template <typename KeyType, typename ValueType, std::size_t Capacity, typename... Args>
    requires(concepts_detail::HashMapFactoryElementTypes<KeyType, ValueType> &&
             Capacity > CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE)
constexpr auto hash_map([[maybe_unused]] Args&&... args) noexcept -> void {
    static_assert(concepts_detail::dependent_false<KeyType, ValueType, Args...>,
                  "cljonic::hash_map: capacity exceeds CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT.");
}

template <typename KeyType, typename ValueType, std::size_t Capacity, typename... Args>
    requires(concepts_detail::HashMapFactoryElementTypes<KeyType, ValueType> &&
             Capacity <= CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE &&
             (sizeof...(Args) % 2U != 0U || sizeof...(Args) / 2U > Capacity))
constexpr auto hash_map([[maybe_unused]] Args&&... args) noexcept -> void {
    static_assert(concepts_detail::dependent_false<KeyType, ValueType, Args...>,
                  "cljonic::hash_map: arguments must be complete key/value pairs and fit within the selected "
                  "capacity.");
}

template <typename KeyType, typename ValueType, std::size_t Capacity, typename... Args>
    requires(concepts_detail::HashMapFactoryElementTypes<KeyType, ValueType> &&
             Capacity <= CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE && sizeof...(Args) % 2U == 0U &&
             sizeof...(Args) / 2U <= Capacity &&
             sizeof...(Args) / 2U <= CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE && sizeof...(Args) > 0U &&
             !concepts_detail::HashMapFactoryPairArguments<KeyType, ValueType, Args...>)
constexpr auto hash_map([[maybe_unused]] Args&&... args) noexcept -> void {
    static_assert(concepts_detail::dependent_false<KeyType, ValueType, Args...>,
                  "cljonic::hash_map: every key and value must safely construct its declared type without "
                  "throwing.");
}

} // namespace cljonic

namespace cljonic::concepts_detail {

template <typename KeyType, typename ValueType, std::size_t CapacityValue>
struct collection_traits<Map<KeyType, ValueType, CapacityValue>> {
    static constexpr bool is_cljonic_collection = true;
    static constexpr collection_kind kind = collection_kind::map;
};

template <typename KeyType, typename ValueType, std::size_t CapacityValue>
struct contains_floating_point<Map<KeyType, ValueType, CapacityValue>>
    : std::bool_constant<contains_floating_point_v<KeyType> || contains_floating_point_v<ValueType>> {};

template <typename KeyType, typename ValueType, std::size_t CapacityValue>
struct contains_callable<Map<KeyType, ValueType, CapacityValue>>
    : std::bool_constant<contains_callable_v<KeyType> || contains_callable_v<ValueType>> {};

template <typename KeyType, typename ValueType, std::size_t CapacityValue>
struct contains_standard_range<Map<KeyType, ValueType, CapacityValue>>
    : std::bool_constant<contains_standard_range_v<KeyType> || contains_standard_range_v<ValueType>> {};

} // namespace cljonic::concepts_detail
