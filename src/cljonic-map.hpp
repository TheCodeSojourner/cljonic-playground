#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <span>

#include <cljonic-concepts.hpp>
#include <cljonic-config.hpp>
#include <cljonic-core-collection-maximum-element-count.hpp>
#include <cljonic-map-entry.hpp>

namespace cljonic {

/** \anchor Map
 * \b Map is a bounded, associative collection that maps unique keys to values. It provides callable lookup with
 * optional fallback values. The way to operate on the collection is through the library's free-function API. Updates
 * return a modified copy without changing the original collection. Construction with more entries than the available
 * capacity is rejected at compile time.
 *
 \b Examples
 ~~~~~{.cpp}
 #include "cljonic.hpp"

 struct Key {
   int id;
   friend constexpr bool operator==(const Key &, const Key &) noexcept = default;
 };

 struct Value {
   int amount = 0;
   friend constexpr bool operator==(const Value &,
                                    const Value &) noexcept = default;
 };

 int main() {
   using namespace cljonic;

   using AccountEntry = MapEntry<Key, Value>;
   using AccountMap = Map<Key, Value, 2>;

   // A named map type and its named entry type make the intended value model
   // explicit. A later duplicate key replaces the earlier value (i.e., the
   // right-most value associated with a duplicate key).
   constexpr auto literal = AccountMap{AccountEntry{Key{1}, Value{10}},
                                       AccountEntry{Key{1}, Value{20}}};

   // A Map is callable for present-key lookup and missing-key fallback.
   static_assert(literal(Key{1}).amount == 20);
   static_assert(literal(Key{2}).amount == 0);
   static_assert(literal(Key{2}, Value{99}).amount == 99);

   // Runtime CTAD deduces Map<Key, Value, 1> from the MapEntry argument.
   auto runtime = Map{AccountEntry{Key{3}, Value{30}}};
   const auto present = runtime(Key{3});
   const auto missing = runtime(Key{4}, Value{77});

   // ---------------------------------------------------------------------
   // C++ interoperability: a Map exposes const logical traversal, a
   // non-owning contiguous standard view, and can be constructed from a
   // read-only std::span without mutating the source data.
   // ---------------------------------------------------------------------
   static constexpr AccountEntry source_entries[] = {
       AccountEntry{Key{10}, Value{100}}, AccountEntry{Key{20}, Value{200}}};
   constexpr std::span source_span{source_entries};
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

   // Const C++ interoperability exposes MapEntry values through a range and
   // a non-owning contiguous standard view.
   static_assert(literal.begin()->value.amount == 20);
   static_assert(literal.view().size() == 1);

   // Use C++ interoperability to sum the values in a map.
   int value_sum = 0;
   for (const auto &entry : runtime) {
     value_sum += entry.value.amount;
   }

   const auto runtime_view = runtime.view();

   return (present.amount == 30 && missing.amount == 77 && value_sum == 30 &&
           runtime_view.size() == 1 && runtime_view[0].value.amount == 30 &&
           from_span(Key{10}).amount == 100 &&
           runtime_from_span(Key{100}).amount == 1000)
              ? 0
              : 1;
 }
 ~~~~~
 */
template <concepts::NothrowStableEqualityComparable KeyType, concepts::NothrowCollectionElement ValueType,
          std::size_t CapacityValue>
class Map {
  public:
    using key_type = KeyType;
    using lookup_type = key_type;
    using mapped_type = ValueType;
    using value_type = MapEntry<KeyType, ValueType>;

    static_assert(
        CapacityValue <= cljonic::CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE,
        "Map CapacityValue exceeds "
        "CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT=" CLJONIC_STRINGIFY(CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT));

    template <typename... Args>
    constexpr Map(Args&&... args) noexcept((concepts::NothrowElementConstruction<value_type, Args> && ...)) {
        static_assert(sizeof...(Args) <= CapacityValue, "Map initializer count exceeds Map CapacityValue");
        static_assert((concepts::NothrowElementConstruction<value_type, Args> && ...),
                      "Map constructor requires all arguments to construct "
                      "MapEntry without throwing and be implicitly convertible to MapEntry");

        ((*this = assoc_entry(value_type{std::forward<Args>(args)})), ...);
    }

    template <typename SourceElement, std::size_t Extent>
    constexpr Map(std::span<const SourceElement, Extent> source) noexcept {
        static_assert(concepts::NothrowElementConstruction<value_type, SourceElement>,
                      "Map span constructor requires SourceElement to construct "
                      "MapEntry without throwing and be implicitly convertible to MapEntry");
        if constexpr (Extent != std::dynamic_extent) {
            static_assert(Extent <= CapacityValue, "Map span source exceeds Map CapacityValue");
        }

        const auto copy_count = std::min<std::size_t>(source.size(), CapacityValue);
        for (std::size_t index = 0; index < copy_count; ++index) {
            *this = assoc_entry(value_type{source[index]});
        }
    }

    [[nodiscard]] static constexpr auto capacity() noexcept -> std::size_t {
        return CapacityValue;
    }

    [[nodiscard]] constexpr auto count() const noexcept -> std::size_t {
        return logical_size_;
    }

    [[nodiscard]] constexpr auto is_empty() const noexcept -> bool {
        return logical_size_ == 0U;
    }

    [[nodiscard]] constexpr auto contains(const KeyType& key) const noexcept -> bool {
        return find_index(key) < logical_size_;
    }

    [[nodiscard]] constexpr auto operator()(const KeyType& key) const noexcept -> ValueType {
        const auto idx = find_index(key);
        return (idx < logical_size_) ? entries_[idx].value : ValueType{};
    }

    [[nodiscard]] constexpr auto operator()(const KeyType& key, const ValueType& fallback) const noexcept -> ValueType {
        const auto idx = find_index(key);
        return (idx < logical_size_) ? entries_[idx].value : fallback;
    }

    [[nodiscard]] constexpr auto can_assoc(const KeyType& key) const noexcept -> bool {
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

  private:
    [[nodiscard]] constexpr auto find_index(const KeyType& key) const noexcept -> std::size_t {
        for (std::size_t i = 0; i < logical_size_; ++i) {
            if (entries_[i].key == key) {
                return i;
            }
        }
        return logical_size_;
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

} // namespace cljonic

namespace cljonic::concepts_detail {

template <typename KeyType, typename ValueType, std::size_t CapacityValue>
struct collection_traits<Map<KeyType, ValueType, CapacityValue>> {
    static constexpr bool is_cljonic_collection = true;
    static constexpr collection_kind kind = collection_kind::map;
};

} // namespace cljonic::concepts_detail
