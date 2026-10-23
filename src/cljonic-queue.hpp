#pragma once

#include <array>
#include <cstddef>
#include <iterator>
#include <ranges>
#include <type_traits>
#include <utility>

#include <cljonic-concepts.hpp>
#include <cljonic-config.hpp>
#include <cljonic-core-collection-maximum-element-count.hpp>

namespace cljonic {

/** \anchor Queue
 * A Queue is a bounded FIFO collection. Operations use the library's free-function API.
 *
 * The \c queue factory is the idiomatic way to create Queue values at runtime or during constant evaluation. Use \c
 * queue(e, ...) to infer the element type and capacity, \c queue<T>(e, ...) to select the element type, \c queue<T,
 * Capacity>(e, ...) to provide an explicit capacity, or \c queue<T>() for an empty zero-capacity Queue. \c Queue<T,
 * Capacity> remains the value's type.
 *
 * Direct construction can import a C++ range or view: its elements are copied into the Queue. A cljonic value is not
 * copied as a range/view source. With an explicit element type, a cljonic argument is accepted as one element only
 * when its type exactly matches the declared element type. Without explicit template arguments, a sole cljonic
 * argument determines the element type and yields a one-element collection. Elements may themselves be cljonic
 * collections or producers, including nested cljonic composites.
 *
 * Construction with more values than the available capacity is rejected at compile time. Elements follow the \ref
 * implicit-conversions "safe implicit conversion rules".
 *
 \b Examples
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 int main() {
   // The factory infers Queue<int, 3> from the argument count.
   [[maybe_unused]] constexpr auto ints_at_capacity = queue(1, 2, 3);

   // Explicit capacity permits a partially populated Queue and an empty Queue.
   [[maybe_unused]] constexpr auto ints_populated = queue<int, 4>(1, 2);
   [[maybe_unused]] constexpr auto ints_empty = queue<int, 4>();

   // --------------------------------------------------------------------------
   // C++ interoperability: a Queue supports const traversal and accepts owned
   // materialization from bounded range/view sources without mutating them.
   // --------------------------------------------------------------------------
   static constexpr int source_values[] = {10, 20, 30};
   constexpr auto source_span = std::span{source_values};
   constexpr auto from_span = Queue<int, 4>{source_span};
   static_assert(from_span.begin()[0] == 10);
   static_assert(from_span.begin()[2] == 30);

   constexpr auto from_span_ctad = Queue{source_span};
   static_assert(from_span_ctad.begin()[1] == 20);

   // A standard view pipeline can use an existing Queue as its source and
   // Construct another Queue from the transformed values.
   const auto pipeline_source = queue(1, 2, 3);
   const auto shifted_view =
       pipeline_source |
       std::views::transform([](int value) noexcept { return value + 10; });
   const auto from_pipeline = Queue<int, 4>{shifted_view};

   // Constructing a Queue from a runtime C++ array/span
   int runtime_buffer[] = {100, 200, 300};
   const auto runtime_from_span =
       Queue<int, 4>{std::span<const int>{runtime_buffer, 3}};

   // Use C++ interoperability to sum the elements of the queue.
   auto fifo_sum = 0;
   for (const auto value : runtime_buffer) {
     fifo_sum += value;
   }

   auto from_span_sum = 0;
   for (const auto value : from_span) {
     from_span_sum += value;
   }

   auto runtime_from_span_sum = 0;
   for (const auto value : runtime_from_span) {
     runtime_from_span_sum += value;
   }

   return (fifo_sum == 600 && from_span_sum == 60 &&
           runtime_from_span_sum == 600 && peek(from_pipeline) == 11)
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
        "cljonic::Queue: CapacityValue exceeds "
        "CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT=" CLJONIC_STRINGIFY(CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT));

    template <typename... Args>
    explicit(sizeof...(Args) ==
             1) constexpr Queue(Args&&... args) noexcept((concepts::NothrowElementConstruction<T, Args> && ...)) {
        static_assert(sizeof...(Args) <= CapacityValue,
                      "cljonic::Queue: initializer count exceeds Queue CapacityValue");
        static_assert((concepts::NothrowElementConstruction<T, Args> && ...),
                      "cljonic::Queue: constructor requires all arguments to construct "
                      "T without throwing and convert safely without losing values or changing their meaning");

        ((*this = conj(T(std::forward<Args>(args)))), ...);
    }

    // A cljonic collection or producer is not a SourceConstruction source
    // (REQ-FN-027A); materialize it via into, preflighted by fits_into.
    template <std::ranges::input_range SourceRange>
        requires(!concepts_detail::is_cljonic_collection_v<SourceRange> &&
                 !concepts_detail::is_cljonic_producer_v<SourceRange> &&
                 !std::same_as<std::remove_cvref_t<SourceRange>, value_type>)
    explicit constexpr Queue(SourceRange&& source) noexcept(
        (concepts::NothrowRangeConstruction<value_type, SourceRange>)) {
        static_assert(
            concepts::NothrowRangeConstruction<value_type, SourceRange>,
            "cljonic::Queue: range/view constructor requires safe element conversion and non-throwing traversal");
        static_assert(concepts_detail::static_extent_fits_v<SourceRange, CapacityValue>,
                      "cljonic::Queue: static-extent range source exceeds Queue CapacityValue");

        std::size_t copy_count = 0;
        for (auto&& item : std::forward<SourceRange>(source)) {
            if (copy_count++ >= CapacityValue) {
                break;
            }
            *this = conj(value_type(std::forward<decltype(item)>(item)));
        }
    }

    // Diagnostic fallback (REQ-DIAG-010): a cljonic collection or producer is
    // not a SourceConstruction source. This overload exists only to explain the
    // rejection -- it produces no value and is never a supported call target.
    template <typename SourceValue>
        requires((concepts_detail::is_cljonic_collection_v<SourceValue> ||
                  concepts_detail::is_cljonic_producer_v<SourceValue>) &&
                 !std::same_as<std::remove_cvref_t<SourceValue>, value_type>)
    explicit constexpr Queue([[maybe_unused]] SourceValue&& source) noexcept {
        static_assert(
            concepts_detail::dependent_false<SourceValue>,
            "cljonic::Queue: you cannot construct a Queue from a cljonic collection or producer. A Queue is built from "
            "its individual elements, or from a non-cljonic C++ range or view. To keep it as a single element, write "
            "Queue{...}.");
    }

    [[nodiscard]] static constexpr auto capacity() noexcept -> std::size_t {
        return CapacityValue;
    }

    [[nodiscard]] constexpr auto count() const noexcept -> std::size_t {
        return logical_size_;
    }

    [[nodiscard]] constexpr auto operator==(const Queue& other) const noexcept -> bool
        requires concepts::StableEqualityComparable<T>
    {
        if (logical_size_ != other.logical_size_) {
            return false;
        }
        for (std::size_t i = 0; i < logical_size_; ++i) {
            const auto lhs_index = (head_ + i) % CapacityValue;
            const auto rhs_index = (other.head_ + i) % CapacityValue;
            if (!(elements_[lhs_index] == other.elements_[rhs_index])) {
                return false;
            }
        }
        return true;
    }

    [[nodiscard]] constexpr auto begin() const noexcept -> const_iterator {
        return {this, 0U};
    }

    [[nodiscard]] constexpr auto end() const noexcept -> const_iterator {
        return {this, logical_size_};
    }

    /** Returns true when there is room for one more element; the value does not
     *  affect the result. */
    [[nodiscard]] constexpr auto can_conj([[maybe_unused]] const T& value) const noexcept -> bool {
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

    /** Peeks at the front element without removing it. Returns the
     * default value when empty. */
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

    template <typename Value>
        requires(!concepts::SafeImplicitConversion<T, Value>)
    auto can_conj(const Value& value) const -> bool = delete;

    template <typename Value>
        requires(!concepts::SafeImplicitConversion<T, Value>)
    auto conj(const Value& value) const -> Queue = delete;

  private:
    std::array<value_type, CapacityValue> elements_{};
    std::size_t head_{0};
    std::size_t logical_size_{0};
};

template <typename First, typename... Rest>
Queue(First, Rest...) -> Queue<First, 1 + sizeof...(Rest)>;

// EnclosureConstruction (REQ-FN-027A): a sole Queue argument wraps as one element.
template <typename ElementType, std::size_t CapacityValue>
Queue(Queue<ElementType, CapacityValue>) -> Queue<Queue<ElementType, CapacityValue>, 1>;

template <typename SourceElement, std::size_t Extent>
    requires(Extent != std::dynamic_extent)
Queue(std::span<SourceElement, Extent>) -> Queue<std::remove_cv_t<SourceElement>, Extent>;

namespace concepts_detail {

struct DeducedQueueElementType {};

template <typename RequestedElementType, typename... Args>
struct queue_factory_element_type {
    using type = RequestedElementType;
};

template <typename First, typename... Rest>
struct queue_factory_element_type<DeducedQueueElementType, First, Rest...> {
    using type = std::remove_cvref_t<First>;
};

template <typename RequestedElementType, typename... Args>
using queue_factory_element_t = queue_factory_element_type<RequestedElementType, Args...>::type;

template <typename RequestedElementType, typename... Args>
concept QueueFactoryHasElementType =
    sizeof...(Args) > 0U || !std::same_as<RequestedElementType, DeducedQueueElementType>;

template <typename RequestedElementType, typename... Args>
concept QueueFactoryArguments =
    QueueFactoryHasElementType<RequestedElementType, Args...> &&
    concepts::NothrowCollectionElement<queue_factory_element_t<RequestedElementType, Args...>> &&
    sizeof...(Args) <= CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE &&
    (concepts::NothrowElementConstruction<queue_factory_element_t<RequestedElementType, Args...>, Args> && ...);

template <typename ElementType, std::size_t Capacity, typename... Args>
concept QueueFactoryCapacityArguments =
    concepts::NothrowCollectionElement<ElementType> && Capacity <= CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE &&
    sizeof...(Args) <= Capacity && (concepts::NothrowElementConstruction<ElementType, Args> && ...);

} // namespace concepts_detail

/** Creates a bounded Queue from its arguments, or a typed empty zero-capacity Queue when called as \c queue<T>(). */
template <typename RequestedElementType = concepts_detail::DeducedQueueElementType, typename... Args>
    requires concepts_detail::QueueFactoryArguments<RequestedElementType, Args...>
[[nodiscard]] constexpr auto queue(Args&&... args) noexcept(
    (concepts::NothrowElementConstruction<concepts_detail::queue_factory_element_t<RequestedElementType, Args...>,
                                          Args> &&
     ...)) -> Queue<concepts_detail::queue_factory_element_t<RequestedElementType, Args...>, sizeof...(Args)> {
    using ElementType = concepts_detail::queue_factory_element_t<RequestedElementType, Args...>;
    return Queue<ElementType, sizeof...(Args)>{std::forward<Args>(args)...};
}

template <typename RequestedElementType = concepts_detail::DeducedQueueElementType, typename... Args>
    requires(!concepts_detail::QueueFactoryArguments<RequestedElementType, Args...>)
constexpr auto queue([[maybe_unused]] Args&&... args) noexcept -> void {
    using ElementType = concepts_detail::queue_factory_element_t<RequestedElementType, Args...>;
    if constexpr (!concepts_detail::QueueFactoryHasElementType<RequestedElementType, Args...>) {
        static_assert(concepts_detail::dependent_false<RequestedElementType, Args...>,
                      "cljonic::queue: an empty Queue requires an explicit element type; write queue<T>().");
    } else if constexpr (!concepts::NothrowCollectionElement<ElementType>) {
        static_assert(concepts_detail::dependent_false<RequestedElementType, Args...>,
                      "cljonic::queue: the element type must satisfy NothrowCollectionElement.");
    } else if constexpr (sizeof...(Args) > CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE) {
        static_assert(concepts_detail::dependent_false<RequestedElementType, Args...>,
                      "cljonic::queue: argument count exceeds CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT.");
    } else {
        static_assert(concepts_detail::dependent_false<RequestedElementType, Args...>,
                      "cljonic::queue: every argument must safely construct the element type without throwing.");
    }
}

/** \anchor QueueFactory
 * Creates a bounded \ref Queue "Queue" from individual element arguments.
 *
 * Deduced calls use the first argument's unqualified type and have capacity equal to their argument count, with the
 * first argument becoming the queue front. Explicit element type uses \c queue<T>(...); explicit capacity uses \c
 * queue<T, Capacity>(...), including for an empty queue; \c queue<T>() creates an empty zero-capacity Queue. An
 * untyped empty call is rejected at compile time.
 *
 \b Examples
 ~~~~~{.cpp}
 #include "cljonic.hpp"
 using namespace cljonic;

 int main() {
   // Deduced calls infer the element type from the first argument and the
   // capacity from the argument count. The first argument becomes the front.
   constexpr auto fifo = queue(1, 2, 3);
   static_assert(fifo.capacity() == 3);
   static_assert(fifo.peek() == 1);
   static_assert(fifo.pop().peek() == 2);
   static_assert(fifo.pop().pop().pop() == Queue<int, 3>{});

   // Explicit element type and explicit capacity create a queue with room for
   // later conj operations, including an empty zero-capacity queue.
   constexpr auto reserved = queue<int, 4>();
   constexpr auto zero = queue<int, 0>();

   // The factory also works with runtime values.
   const auto runtime_value = 40;
   const auto runtime_queue = queue(runtime_value, 50);

   // A named queue type keeps the element and capacity contract explicit.
   constexpr auto typed = queue<int, 4>(1, 2, 3);

   return (typed.count() == 3 && typed.peek() == 1 && reserved.count() == 0 &&
           zero.count() == 0 && runtime_queue.peek() == 40)
              ? 0
              : 1;
 }
 ~~~~~
 */
template <typename ElementType, std::size_t Capacity, typename... Args>
    requires concepts_detail::QueueFactoryCapacityArguments<ElementType, Capacity, Args...>
[[nodiscard]] constexpr auto queue(Args&&... args) noexcept((concepts::NothrowElementConstruction<ElementType, Args> &&
                                                             ...)) -> Queue<ElementType, Capacity> {
    return Queue<ElementType, Capacity>{std::forward<Args>(args)...};
}

template <typename ElementType, std::size_t Capacity, typename... Args>
    requires(!concepts_detail::QueueFactoryCapacityArguments<ElementType, Capacity, Args...>)
constexpr auto queue([[maybe_unused]] Args&&... args) noexcept -> void {
    if constexpr (!concepts::NothrowCollectionElement<ElementType>) {
        static_assert(concepts_detail::dependent_false<ElementType, Args...>,
                      "cljonic::queue: the element type must satisfy NothrowCollectionElement.");
    } else if constexpr (Capacity > CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE) {
        static_assert(concepts_detail::dependent_false<ElementType, Args...>,
                      "cljonic::queue: capacity exceeds CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT.");
    } else if constexpr (sizeof...(Args) > Capacity) {
        static_assert(concepts_detail::dependent_false<ElementType, Args...>,
                      "cljonic::queue: capacity must be at least the number of arguments.");
    } else {
        static_assert(concepts_detail::dependent_false<ElementType, Args...>,
                      "cljonic::queue: every argument must safely construct the element type without throwing.");
    }
}

} // namespace cljonic

namespace cljonic::concepts_detail {

template <typename T, std::size_t CapacityValue>
struct collection_traits<Queue<T, CapacityValue>> {
    static constexpr bool is_cljonic_collection = true;
    static constexpr collection_kind kind = collection_kind::queue;
};

template <typename T, std::size_t CapacityValue>
struct contains_floating_point<Queue<T, CapacityValue>> : std::bool_constant<contains_floating_point_v<T>> {};

template <typename T, std::size_t CapacityValue>
struct contains_callable<Queue<T, CapacityValue>> : std::bool_constant<contains_callable_v<T>> {};

template <typename T, std::size_t CapacityValue>
struct contains_standard_range<Queue<T, CapacityValue>> : std::bool_constant<contains_standard_range_v<T>> {};

} // namespace cljonic::concepts_detail
