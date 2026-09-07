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
 *   static_assert(q_const.count() == 2U);
 *   static_assert(q_const.peek() == 10);
 *   static_assert(q_const.can_conj());
 *
 *   // Pack-literal construction folds conj over each argument in order; CTAD
 *   // deduces Queue<int, 3>. Argument order establishes FIFO order.
 *   constexpr auto literal = Queue{10, 20, 30};
 *   static_assert(literal.count() == 3U);
 *   static_assert(literal.peek() == 10);
 *
 *   // Runtime demonstration.
 *   auto q_runtime = Queue<int, 4>{};
 *   auto q1 = q_runtime.conj(100);
 *   auto q2 = q1.pop();
 *
 *   return (q1.peek() == 100 && q2.is_empty()) ? 0 : 1;
 * }
 * ~~~~~
 */
template <concepts::CopyableElement T, std::size_t CapacityValue>
class Queue {
  public:
    using value_type = T;

    static_assert(concepts::NothrowCopyableElement<T>, "Queue element type operations must not throw");
    static_assert(
        CapacityValue <= cljonic::CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE,
        "Queue CapacityValue exceeds "
        "CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT=" CLJONIC_STRINGIFY(CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT));

    template <typename... Args>
    constexpr Queue(Args&&... args) noexcept((concepts::NothrowElementConstruction<T, Args> && ...)) {
        static_assert(sizeof...(Args) <= CapacityValue, "Queue initializer count exceeds Queue CapacityValue");
        static_assert((concepts::NothrowElementConstruction<T, Args> && ...),
                      "Queue constructor requires all arguments to construct "
                      "T without throwing and be implicitly convertible to T");

        ((*this = conj(T{std::forward<Args>(args)})), ...);
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

    /** Returns true when there is room for at least one more element. */
    [[nodiscard]] constexpr auto can_conj() const noexcept -> bool {
        return logical_size_ < CapacityValue;
    }

    /** Adds an element to the back of a copy of the queue (enqueue). Returns an
     * unchanged copy when full. */
    [[nodiscard]] constexpr auto conj(const T& element) const noexcept -> Queue {
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

template <typename First, typename... Rest>
Queue(First, Rest...) -> Queue<First, 1 + sizeof...(Rest)>;

} // namespace cljonic

namespace cljonic::concepts_detail {

template <typename T, std::size_t CapacityValue>
struct collection_traits<Queue<T, CapacityValue>> {
    static constexpr bool is_cljonic_collection = true;
    static constexpr collection_kind kind = collection_kind::queue;
};

} // namespace cljonic::concepts_detail
