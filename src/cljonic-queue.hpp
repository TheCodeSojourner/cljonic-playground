#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <iterator>
#include <type_traits>
#include <utility>

#include <cljonic-concepts.hpp>
#include <cljonic-config.hpp>
#include <cljonic-core-collection-maximum-element-count.hpp>

namespace cljonic {

/** \anchor Queue
 * \b Queue is a bounded FIFO collection. Updates return a modified copy without changing the original collection. The
 * way to operate on the collection is through the library's free-function API. Construction with more values than the
 * available capacity is rejected at compile time.
 *
 \b Examples
 ~~~~~{.cpp}
 #include "cljonic.hpp"

 int main() {
   using namespace cljonic;

   // CTAD infers Queue<int, 3> from the initializer count.
   [[maybe_unused]] constexpr auto ints_at_capacity = Queue{1, 2, 3};

   // Explicit capacity permits a partially populated Queue and an empty Queue.
   [[maybe_unused]] constexpr auto ints_populated = Queue<int, 4>{1, 2};
   [[maybe_unused]] constexpr auto ints_empty = Queue<int, 4>{};

   // ---------------------------------------------------------------------
   // C++ interoperability: a Queue exposes const logical traversal, and
   // can be constructed from a read-only std::span without mutating the
   // source data.
   // ---------------------------------------------------------------------
   static constexpr int source_values[] = {10, 20, 30};
   constexpr std::span source_span{source_values};
   constexpr auto from_span = Queue<int, 4>{source_span};
   static_assert(from_span.begin()[0] == 10);
   static_assert(from_span.begin()[2] == 30);

   constexpr auto from_span_ctad = Queue{source_span};
   static_assert(from_span_ctad.begin()[1] == 20);

   // Constructing a Queue from a runtime C++ array/span
   int runtime_buffer[] = {100, 200, 300};
   const auto runtime_from_span =
       Queue<int, 4>{std::span<const int>{runtime_buffer, 3}};

   // Const C++ interoperability uses begin()/end() for logical FIFO traversal.
   constexpr auto wrapped = Queue<int, 4>{2, 3, 4, 5};
   static_assert(wrapped.begin()[0] == 2);
   static_assert(wrapped.begin()[1] == 3);
   static_assert(wrapped.begin()[2] == 4);
   static_assert(wrapped.begin()[3] == 5);

   // Use C++ interoperability to sum the elements of the queue.
   int fifo_sum = 0;
   for (const auto value : wrapped) {
     fifo_sum += value;
   }

   int from_span_sum = 0;
   for (const auto value : from_span) {
     from_span_sum += value;
   }

   int runtime_from_span_sum = 0;
   for (const auto value : runtime_from_span) {
     runtime_from_span_sum += value;
   }

   return (fifo_sum == 14 && from_span_sum == 60 && runtime_from_span_sum == 600)
              ? 0
              : 1;
 }
 ~~~~~
 */
template <concepts::NothrowCollectionElement T, std::size_t CapacityValue>
class Queue {
  public:
    using value_type = T;

    class const_iterator {
      public:
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::forward_iterator_tag;
        using iterator_concept = std::forward_iterator_tag;

        constexpr const_iterator() noexcept = default;

        [[nodiscard]] constexpr auto operator*() const noexcept -> const value_type& {
            return queue_->elements_[(queue_->head_ + offset_) % CapacityValue];
        }

        [[nodiscard]] constexpr auto operator[](difference_type index) const noexcept -> const value_type& {
            return queue_->elements_[(queue_->head_ + offset_ + static_cast<std::size_t>(index)) % CapacityValue];
        }

        constexpr auto operator++() noexcept -> const_iterator& {
            ++offset_;
            return *this;
        }

        constexpr auto operator++(int) noexcept -> const_iterator {
            const_iterator result = *this;
            ++(*this);
            return result;
        }

        [[nodiscard]] friend constexpr auto operator==(const const_iterator&, const const_iterator&) noexcept
            -> bool = default;

      private:
        friend class Queue;

        constexpr const_iterator(const Queue* queue, std::size_t offset) noexcept : queue_(queue), offset_(offset) {
        }

        const Queue* queue_{nullptr};
        std::size_t offset_{0};
    };

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

    template <typename SourceElement, std::size_t Extent>
    constexpr Queue(std::span<const SourceElement, Extent> source) noexcept {
        static_assert(std::same_as<std::remove_cvref_t<SourceElement>, value_type>,
                      "Queue span constructor requires a matching element type");
        if constexpr (Extent != std::dynamic_extent) {
            static_assert(Extent <= CapacityValue, "Queue span source exceeds Queue CapacityValue");
        }

        const auto copy_count = std::min<std::size_t>(source.size(), CapacityValue);
        for (std::size_t index = 0; index < copy_count; ++index) {
            *this = conj(value_type{source[index]});
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

    [[nodiscard]] constexpr auto begin() const noexcept -> const_iterator {
        return {this, 0U};
    }

    [[nodiscard]] constexpr auto end() const noexcept -> const_iterator {
        return {this, logical_size_};
    }

    /** Returns true when there is room for at least one more element. */
    [[nodiscard]] constexpr auto can_conj() const noexcept -> bool {
        return logical_size_ < CapacityValue;
    }

    /** Adds an element to the back of a copy of the queue (enqueue). Returns an
     * unchanged copy when full. */
    [[nodiscard]] constexpr auto conj(const T& element) const noexcept -> Queue {
        Queue result = *this;
        if constexpr (CapacityValue > 0U) {
            if (result.logical_size_ < CapacityValue) {
                const auto tail = (result.head_ + result.logical_size_) % CapacityValue;
                result.elements_[tail] = element;
                ++result.logical_size_;
            }
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
        if constexpr (CapacityValue > 0U) {
            if (result.logical_size_ > 0) {
                result.head_ = (result.head_ + 1) % CapacityValue;
                --result.logical_size_;
            }
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

template <typename SourceElement, std::size_t Extent>
    requires(Extent != std::dynamic_extent)
Queue(std::span<const SourceElement, Extent>) -> Queue<std::remove_cv_t<SourceElement>, Extent>;

template <typename SourceElement, std::size_t Extent>
    requires(Extent != std::dynamic_extent)
Queue(std::span<SourceElement, Extent>) -> Queue<std::remove_cv_t<SourceElement>, Extent>;

} // namespace cljonic

namespace cljonic::concepts_detail {

template <typename T, std::size_t CapacityValue>
struct collection_traits<Queue<T, CapacityValue>> {
    static constexpr bool is_cljonic_collection = true;
    static constexpr collection_kind kind = collection_kind::queue;
};

} // namespace cljonic::concepts_detail
