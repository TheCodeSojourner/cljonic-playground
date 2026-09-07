#pragma once

#include <array>
#include <cstddef>
#include <utility>

#include <cljonic-concepts.hpp>
#include <cljonic-config.hpp>
#include <cljonic-core-collection-maximum-element-count.hpp>
#include <cljonic-map-entry.hpp>

namespace cljonic {

/** \anchor Map
 * \b Map is a fixed-capacity associative collection backed by contiguous array
 * storage and linear scan lookup with copy-on-modify updates. Keys satisfy
 * `NothrowStableEqualityComparable`; values satisfy `NothrowCollectionElement`.
 *
 * \b Examples
 * ~~~~~{.cpp}
 * #include <cljonic.hpp>
 *
 * struct Key {
 *   int id;
 *   friend constexpr bool operator==(const Key&, const Key&) noexcept = default;
 * };
 *
 * struct Value {
 *   int amount = 0;
 *   friend constexpr bool operator==(const Value&, const Value&) noexcept = default;
 * };
 *
 * int main() {
 *   using namespace cljonic;
 *
 *   // An explicit capacity creates an empty bounded map.
 *   constexpr Map<Key, Value, 4> empty{};
 *   constexpr auto first = empty.assoc(Key{1}, Value{10});
 *   constexpr auto replaced = first.assoc(Key{1}, Value{20});
 *   static_assert(first.count() == 1U);
 *   static_assert(replaced.count() == 1U);
 *   static_assert(replaced(Key{1}).amount == 20);
 *   static_assert(!replaced.contains(Key{2}));
 *   static_assert(replaced(Key{2}).amount == 0);
 *   static_assert(replaced(Key{2}, Value{99}).amount == 99);
 *   static_assert(replaced.can_assoc(Key{2}));
 *
 *   // Pack-literal construction folds assoc over each entry in argument
 *   // order; CTAD deduces Map<Key, Value, 2>. A later duplicate key
 *   // replaces an earlier one, matching explicit assoc semantics.
 *   constexpr auto literal = Map{MapEntry<Key, Value>{Key{1}, Value{10}}, MapEntry<Key, Value>{Key{1}, Value{20}}};
 *   static_assert(literal.count() == 1U);
 *   static_assert(literal(Key{1}).amount == 20);
 *
 *   // Map lookup is callable and missing-key access does not insert.
 *   auto runtime = replaced.assoc(Key{2}, Value{30});
 *   const auto missing = runtime(Key{3}, Value{77});
 *   const auto removed = runtime.dissoc(Key{1});
 *
 *   return (runtime.count() == 2U && missing.amount == 77 &&
 *           !removed.contains(Key{1}) && removed.contains(Key{2})) ? 0 : 1;
 * }
 * ~~~~~
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
                      "MapEntry<KeyType, ValueType> without throwing and be "
                      "implicitly convertible to MapEntry<KeyType, ValueType>");

        ((*this = assoc_entry(value_type{std::forward<Args>(args)})), ...);
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

    [[nodiscard]] constexpr auto can_assoc(const KeyType& key, const ValueType& /*value*/) const noexcept -> bool {
        return can_assoc(key);
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

} // namespace cljonic

namespace cljonic::concepts_detail {

template <typename KeyType, typename ValueType, std::size_t CapacityValue>
struct collection_traits<Map<KeyType, ValueType, CapacityValue>> {
    static constexpr bool is_cljonic_collection = true;
    static constexpr collection_kind kind = collection_kind::map;
};

} // namespace cljonic::concepts_detail
