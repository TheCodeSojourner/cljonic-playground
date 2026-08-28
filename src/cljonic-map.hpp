#pragma once

#include <array>
#include <concepts>
#include <cstddef>
#include <utility>

#include <cljonic-concepts.hpp>
#include <cljonic-config.hpp>
#include <cljonic-core-collection-maximum-element-count.hpp>
#include <cljonic-map-entry.hpp>

namespace cljonic
{

  /** \anchor Map
   * \b Map is a fixed-capacity associative collection backed by contiguous array
   * storage and linear scan lookup with copy-on-modify updates.
   *
   * \b Examples
   * ~~~~~{.cpp}
   * #include <cljonic.hpp>
   *
   * int main() {
   *   using namespace cljonic;
   *
   *   // Compile-time demonstration.
   *   constexpr auto m_const = Map<int, int, 4>{}.assoc(1, 10).assoc(2, 20);
   *   static_assert(m_const.size() == 2U);
   *   static_assert(m_const.contains(1));
   *   static_assert(m_const(1) == 10);
   *   static_assert(m_const(99, -1) == -1);
   *   static_assert(m_const.can_assoc(3));
   *
   *   // Runtime demonstration.
   *   auto m_runtime = Map<int, int, 4>{};
   *   auto m1 = m_runtime.assoc(10, 100);
   *   auto m2 = m1.dissoc(10);
   *
   *   return (m1.contains(10) && !m2.contains(10) && m2.empty()) ? 0 : 1;
   * }
   * ~~~~~
   */
  template <concepts::VectorElement KeyType, concepts::VectorElement ValueType,
            std::size_t CapacityValue>
  class Map
  {
  public:
    using key_type = KeyType;
    using mapped_type = ValueType;
    using value_type = MapEntry<KeyType, ValueType>;

    static_assert(concepts::NothrowVectorElement<KeyType>,
                  "Map key type operations must not throw");
    static_assert(concepts::NothrowVectorElement<ValueType>,
                  "Map value type operations must not throw");
    static_assert(
        CapacityValue <= cljonic::CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE,
        "Map capacity exceeds CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT");

    constexpr Map() noexcept : entries_{}, logical_size_{0} {}

    [[nodiscard]] static constexpr auto capacity() noexcept -> std::size_t
    {
      return CapacityValue;
    }

    [[nodiscard]] constexpr auto size() const noexcept -> std::size_t
    {
      return logical_size_;
    }

    [[nodiscard]] constexpr auto empty() const noexcept -> bool
    {
      return logical_size_ == 0U;
    }

    [[nodiscard]] constexpr auto
    contains(const KeyType &key) const noexcept -> bool
    {
      return find_index(key) < logical_size_;
    }

    [[nodiscard]] constexpr auto
    operator()(const KeyType &key) const noexcept -> ValueType
    {
      const auto idx = find_index(key);
      return (idx < logical_size_) ? entries_[idx].value : ValueType{};
    }

    [[nodiscard]] constexpr auto
    operator()(const KeyType &key,
               const ValueType &fallback) const noexcept -> ValueType
    {
      const auto idx = find_index(key);
      return (idx < logical_size_) ? entries_[idx].value : fallback;
    }

    [[nodiscard]] constexpr auto
    can_assoc(const KeyType &key) const noexcept -> bool
    {
      return contains(key) || (logical_size_ < CapacityValue);
    }

    [[nodiscard]] constexpr auto
    can_assoc(const KeyType &key,
              const ValueType & /*value*/) const noexcept -> bool
    {
      return can_assoc(key);
    }

    [[nodiscard]] constexpr auto
    assoc(const KeyType &key, const ValueType &value) const noexcept -> Map
    {
      Map result = *this;
      const auto idx = result.find_index(key);
      if (idx < result.logical_size_)
      {
        result.entries_[idx].value = value;
      }
      else if (result.logical_size_ < CapacityValue)
      {
        result.entries_[result.logical_size_] = value_type{key, value};
        ++result.logical_size_;
      }
      return result;
    }

    [[nodiscard]] constexpr auto
    dissoc(const KeyType &key) const noexcept -> Map
    {
      Map result = *this;
      const auto idx = result.find_index(key);
      if (idx < result.logical_size_)
      {
        if (idx + 1U < result.logical_size_)
        {
          result.entries_[idx] = result.entries_[result.logical_size_ - 1U];
        }
        --result.logical_size_;
      }
      return result;
    }

  private:
    [[nodiscard]] constexpr auto
    find_index(const KeyType &key) const noexcept -> std::size_t
    {
      for (std::size_t i = 0; i < logical_size_; ++i)
      {
        if (entries_[i].key == key)
        {
          return i;
        }
      }
      return logical_size_;
    }

    std::array<value_type, CapacityValue> entries_{};
    std::size_t logical_size_{0};
  };

} // namespace cljonic
