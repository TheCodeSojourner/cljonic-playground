#include <catch2/catch_test_macros.hpp>

#include <concepts>
#include <type_traits>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("queue factory builds bounded FIFO queues from individual element arguments", "[queue][factory]") {
    using cljonic::Queue;
    using cljonic::queue;

    TRACE_ID("entity-fields.QueueFactory");
    TRACE_ID("invariant.QueueFactory.SupportsDeducedElementFactory");
    TRACE_ID("invariant.QueueFactory.DeducedElementTypeIsUnqualifiedFirstArgument");
    TRACE_ID("invariant.QueueFactory.SupportsExplicitElementTypeFactory");
    TRACE_ID("invariant.QueueFactory.SupportsExplicitCapacityFactory");
    TRACE_ID("invariant.QueueFactory.DeducedCapacityEqualsArgumentCount");
    TRACE_ID("invariant.QueueFactory.ExplicitCapacityDeterminesResultCapacity");
    TRACE_ID("invariant.QueueFactory.ExplicitCapacityFactorySupportsEmptyArguments");
    TRACE_ID("invariant.QueueFactory.ExplicitCapacityEmptyFactorySupportsZeroCapacity");
    TRACE_ID("invariant.QueueFactory.ExplicitCapacityIsAtLeastArgumentCount");
    TRACE_ID("invariant.QueueFactory.ExplicitCapacityIsBoundedByConfiguredMaximum");
    TRACE_ID("invariant.QueueFactory.NonemptyVariadicFormsRequireAtLeastOneArgument");
    TRACE_ID("invariant.QueueFactory.TypedEmptyFactoryReturnsZeroCapacityQueue");
    TRACE_ID("invariant.QueueFactory.EmptyFactoryWithoutExplicitElementTypeIsCompileTimeRejected");
    TRACE_ID("invariant.QueueFactory.UntypedEmptyCallIsCompileTimeRejected");
    TRACE_ID("invariant.QueueFactory.ArgumentCountIsBoundedByConfiguredMaximum");
    TRACE_ID("invariant.QueueFactory.OversizedArgumentCountIsCompileTimeRejected");
    TRACE_ID("invariant.QueueFactory.ElementTypeSatisfiesNothrowCollectionElement");
    TRACE_ID("invariant.QueueFactory.EveryArgumentConstructsSafelyWithoutThrowing");
    TRACE_ID("invariant.QueueFactory.ArgumentsAreAssociatedInFifoArgumentOrder");
    TRACE_ID("invariant.QueueFactory.FirstArgumentBecomesTheQueueFront");
    TRACE_ID("invariant.QueueFactory.SoleCljonicCollectionOrProducerArgumentIsOneElement");
    TRACE_ID("invariant.QueueFactory.PreservesPublicQueueTypeAndExistingConstructors");
    TRACE_ID("invariant.QueueFactory.ReturnsIndependentlyOwningQueue");
    TRACE_ID("invariant.QueueFactory.ConstexprNoexceptNonallocating");
    TRACE_ID("invariant.QueueFactory.NoRtti");
    TRACE_ID("invariant.QueueFactory.NoExceptions");
    TRACE_ID("invariant.QueueFactory.ReferentialTransparency");

    constexpr auto deduced = queue(1, 2, short{3});
    constexpr auto explicit_types = queue<long>(short{1}, short{2});
    constexpr auto explicit_capacity = queue<int, 4>(1, 2);
    constexpr auto empty_zero_capacity = queue<int>();
    constexpr auto reserved_empty = queue<int, 4>();

    STATIC_REQUIRE(std::same_as<decltype(deduced), const Queue<int, 3>>);
    STATIC_REQUIRE(deduced.count() == 3U);
    STATIC_REQUIRE(deduced.peek() == 1);
    STATIC_REQUIRE(deduced.pop().count() == 2U);
    STATIC_REQUIRE(std::same_as<decltype(explicit_types), const Queue<long, 2>>);
    STATIC_REQUIRE(explicit_types.peek() == 1L);
    STATIC_REQUIRE(std::same_as<decltype(explicit_capacity), const Queue<int, 4>>);
    STATIC_REQUIRE(explicit_capacity.count() == 2U);
    STATIC_REQUIRE(explicit_capacity.capacity() == 4U);
    STATIC_REQUIRE(std::same_as<decltype(empty_zero_capacity), const Queue<int, 0>>);
    STATIC_REQUIRE(empty_zero_capacity.count() == 0U);
    STATIC_REQUIRE(empty_zero_capacity.capacity() == 0U);
    STATIC_REQUIRE(reserved_empty.count() == 0U);
    STATIC_REQUIRE(reserved_empty.capacity() == 4U);

    // FIFO order: the first argument becomes the front; conj folds in order.
    STATIC_REQUIRE(explicit_capacity.peek() == 1);
    STATIC_REQUIRE(explicit_capacity.pop().peek() == 2);
    STATIC_REQUIRE(explicit_capacity.conj(3).count() == 3U);

    // A sole cljonic collection argument is one element (enclosure), not a
    // materialized source.
    constexpr auto inner = Queue<int, 2>{1, 2};
    constexpr auto enclosed = queue(inner);
    STATIC_REQUIRE(std::same_as<decltype(enclosed), const Queue<Queue<int, 2>, 1>>);
    STATIC_REQUIRE(enclosed.count() == 1U);
    STATIC_REQUIRE(enclosed.peek() == inner);

    // The public type and its existing constructors remain supported.
    constexpr Queue<int, 4> literal{10, 20, 30};
    STATIC_REQUIRE(literal.peek() == 10);

    // Returns an independently owning queue: modifying the result does not
    // affect the arguments' collections.
    constexpr auto owned = queue(inner, inner);
    STATIC_REQUIRE(owned.count() == 2U);
    STATIC_REQUIRE(owned.peek() == inner);

    // noexcept on admitted calls.
    STATIC_REQUIRE(noexcept(queue(1, 2)));

    // Runtime tests for code coverage instrumentation.
    volatile int runtime_first_raw = 40;
    volatile int runtime_second_raw = 50;
    const int runtime_first = runtime_first_raw;
    const int runtime_second = runtime_second_raw;
    const auto runtime_queue = queue(runtime_first, runtime_second);
    REQUIRE(runtime_queue.count() == 2U);
    REQUIRE(runtime_queue.capacity() == 2U);
    REQUIRE(runtime_queue.peek() == 40);
    REQUIRE(runtime_queue.pop().peek() == 50);

    const auto runtime_reserved = queue<int, 4>(runtime_first);
    REQUIRE(runtime_reserved.count() == 1U);
    REQUIRE(runtime_reserved.capacity() == 4U);

    const auto runtime_typed = queue<long>(short{3}, short{4});
    REQUIRE(std::same_as<decltype(runtime_typed), const Queue<long, 2>>);
    REQUIRE(runtime_typed.peek() == 3L);

    const auto runtime_empty = queue<int>();
    REQUIRE(runtime_empty.count() == 0U);
    REQUIRE(runtime_empty.capacity() == 0U);
}
