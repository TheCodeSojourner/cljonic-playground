#pragma once

#include <array>
#include <concepts>
#include <cstddef>
#include <utility>

#include <cljonic-concepts.hpp>
#include <cljonic-config.hpp>
#include <cljonic-core-collection-maximum-element-count.hpp>

namespace cljonic {

/** \anchor Queue
 * \b Queue is a fixed-capacity FIFO sequence collection backed by contiguous
 * array storage with copy-on-modify semantics.
 *
 * \b Examples
 * ~~~~~{.cpp}
 * #include <cljonic.hpp>
 *
 * int main() {
 *   using namespace cljonic;
 *
 *   // Compile-time demonstration.
 *   constexpr auto q_const = Queue<int, 4>{}.conj(10).conj(20);
 *   static_assert(q_const.size() == 2U);
 *   static_assert(q_const.peek() == 10);
 *   static_assert(q_const.can_conj());
 *
 *   // Runtime demonstration.
 *   auto q_runtime = Queue<int, 4>{};
 *   auto q1 = q_runtime.conj(100);
 *   auto q2 = q1.pop();
 *
 *   return (q1.peek() == 100 && q2.empty()) ? 0 : 1;
 * }
 * ~~~~~
 */
template <concepts::VectorElement T, std::size_t CapacityValue> class Queue {
public:
  using value_type = T;

  static_assert(concepts::NothrowVectorElement<T>,
                "Queue element type operations must not throw");
  static_assert(
      CapacityValue <= cljonic::CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE,
      "Queue capacity exceeds CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT");

  constexpr Queue() noexcept : elements_{}, head_{0}, logical_size_{0} {}

  [[nodiscard]] static constexpr auto capacity() noexcept -> std::size_t {
    return CapacityValue;
  }

  [[nodiscard]] constexpr auto size() const noexcept -> std::size_t {
    return logical_size_;
  }

  [[nodiscard]] constexpr auto empty() const noexcept -> bool {
    return logical_size_ == 0U;
  }

  /** Returns true when there is room for at least one more element. */
  [[nodiscard]] constexpr auto can_conj() const noexcept -> bool {
    return logical_size_ < CapacityValue;
  }

  /** Adds an element to the back of a copy of the queue (enqueue). Returns an
   * unchanged copy when full. */
  [[nodiscard]] constexpr auto conj(const T &element) const noexcept -> Queue {
    Queue result = *this;
    if (result.logical_size_ < CapacityValue) {
      const auto tail = (result.head_ + result.logical_size_) % CapacityValue;
      result.elements_[tail] = element;
      ++result.logical_size_;
    }
    return result;
  }

  /** Peeks at the front element without removing it. Returns
   * default-constructed value when empty. */
  [[nodiscard]] constexpr auto peek() const noexcept -> T {
    return (logical_size_ > 0) ? elements_[head_] : T{};
  }

  /** Removes the front element from a copy of the queue (dequeue). Returns an
   * unchanged copy when empty. */
  [[nodiscard]] constexpr auto pop() const noexcept -> Queue {
    Queue result = *this;
    if (result.logical_size_ > 0) {
      result.head_ = (result.head_ + 1) % CapacityValue;
      --result.logical_size_;
    }
    return result;
  }

private:
  std::array<value_type, CapacityValue> elements_{};
  std::size_t head_{0};
  std::size_t logical_size_{0};
};

} // namespace cljonic
