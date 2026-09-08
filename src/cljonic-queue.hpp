#pragma once

#include <array>
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
 *   // CTAD infers Queue<int, 3> from the initializer count.
 *   [[maybe_unused]] constexpr auto ints_at_capacity = Queue{1, 2, 3};
 *
 *   // Explicit capacity permits a partially populated Queue and an empty Queue.
 *   [[maybe_unused]] constexpr auto ints_populated = Queue<int, 4>{1, 2};
 *   [[maybe_unused]] constexpr auto ints_empty = Queue<int, 4>{};
 *
 *   return 0;
 * }
 * ~~~~~
 */
template <concepts::NothrowCollectionElement T, std::size_t CapacityValue>
class Queue {
  public:
    using value_type = T;

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
