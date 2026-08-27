#include <catch2/catch_test_macros.hpp>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("Queue FIFO construction and operations", "[queue]")
{
    using cljonic::Queue;

    TRACE_ID("entity-fields.Queue");
    TRACE_ID("invariant.Queue.CapacityIsNonNegative");
    TRACE_ID("invariant.Queue.LogicalSizeIsNonNegative");
    TRACE_ID("invariant.Queue.LogicalSizeDoesNotExceedCapacity");
    TRACE_ID("invariant.Queue.HeaderOnlyDistribution");
    TRACE_ID("invariant.Queue.StaticOrAutomaticStorageOnly");
    TRACE_ID("invariant.Queue.ContiguousStorage");
    TRACE_ID("invariant.Queue.FifoOrdering");
    TRACE_ID("invariant.Queue.NoHeapAllocation");
    TRACE_ID("invariant.Queue.NoRtti");
    TRACE_ID("invariant.Queue.NoExceptions");
    TRACE_ID("invariant.Queue.SingleThreadedExecutionModel");
    TRACE_ID("invariant.Queue.ImmutablePublicApi");
    TRACE_ID("invariant.Queue.UpdateReturnsNewValue");
    TRACE_ID("invariant.Queue.DeepCopyUpdate");
    TRACE_ID("invariant.Queue.ReferentialTransparency");
    TRACE_ID("invariant.Queue.RequiresValueSemanticElements");
    TRACE_ID("invariant.Queue.SupportsEmptyExplicitCapacityConstruction");
    TRACE_ID("invariant.Queue.SupportsExplicitCapacityConstruction");
    TRACE_ID("invariant.Queue.CapacityExceedsMaximumIsCompileTimeFailure");
    TRACE_ID("invariant.Queue.SupportsPeekOperation");
    TRACE_ID("invariant.Queue.SupportsPopOperation");
    TRACE_ID("invariant.Queue.SupportsConjOperation");
    TRACE_ID("invariant.Queue.EmptyQueuePeekReturnsDefaultElement");
    TRACE_ID("invariant.Queue.EmptyQueuePopReturnsEmptyQueue");
    TRACE_ID("invariant.Queue.CanonicalResultStatusModelIsDeclared");
    TRACE_ID("invariant.Queue.CompleteResultStatusDeclared");
    TRACE_ID("invariant.Queue.BoundedPrefixResultStatusDeclared");
    TRACE_ID("invariant.Queue.DefaultReturningResultStatusDeclared");
    TRACE_ID("invariant.Queue.CheckedFailureResultStatusDeclared");
    TRACE_ID("invariant.Queue.ProducerOnlyResultStatusDeclared");
    TRACE_ID("invariant.Queue.PreflightPredicatesAreNonThrowingNonAllocating");
    TRACE_ID("invariant.Queue.IsEmptyIsCanonicalEmptyPredicate");
    TRACE_ID("invariant.Queue.CanConjIsCanonicalConjPreflight");
    TRACE_ID("invariant.Queue.FitsIntoIsCanonicalMaterializationPreflight");
    TRACE_ID("invariant.Queue.CompileTimeCapacityOverflowIsRejected");
    TRACE_ID("invariant.Queue.RuntimeCapacityFailuresHaveDocumentedPolicy");
    TRACE_ID("invariant.Queue.DefaultAccessHasPreflightPredicate");

    constexpr Queue<int, 4> q{};
    STATIC_REQUIRE(q.empty());
    STATIC_REQUIRE(q.size() == 0U);
    STATIC_REQUIRE(q.capacity() == 4U);
    STATIC_REQUIRE(q.can_conj()); // empty → room available

    // conj enqueues to back
    constexpr auto q1 = q.conj(1);
    STATIC_REQUIRE_FALSE(q1.empty());
    STATIC_REQUIRE(q1.size() == 1U);
    STATIC_REQUIRE(q1.peek() == 1);

    constexpr auto q2 = q1.conj(2);
    STATIC_REQUIRE(q2.size() == 2U);

    constexpr auto q3 = q2.conj(3);
    STATIC_REQUIRE(q3.size() == 3U);

    constexpr auto q4 = q3.conj(4);
    STATIC_REQUIRE(q4.size() == 4U);
    STATIC_REQUIRE_FALSE(q4.can_conj()); // full

    // conj on full queue returns unchanged copy
    constexpr auto q5 = q4.conj(99);
    STATIC_REQUIRE(q5.size() == 4U);

    // FIFO ordering via pop
    // q4 has [1, 2, 3, 4] front-to-back
    constexpr auto qp1 = q4.pop();
    STATIC_REQUIRE(qp1.size() == 3U);
    STATIC_REQUIRE(qp1.peek() == 2); // 1 removed

    constexpr auto qp2 = qp1.pop();
    STATIC_REQUIRE(qp2.size() == 2U);
    STATIC_REQUIRE(qp2.peek() == 3); // 2 removed

    constexpr auto qp3 = qp2.pop();
    STATIC_REQUIRE(qp3.size() == 1U);
    STATIC_REQUIRE(qp3.peek() == 4); // 3 removed

    constexpr auto qp4 = qp3.pop();
    STATIC_REQUIRE(qp4.empty()); // 4 removed
    STATIC_REQUIRE(qp4.size() == 0U);

    // Empty queue: peek returns default-constructed value
    STATIC_REQUIRE(q.peek() == 0); // empty queue peek

    // Empty queue: pop returns unchanged copy
    constexpr auto q_empty_pop = q.pop();
    STATIC_REQUIRE(q_empty_pop.empty());
    STATIC_REQUIRE(q_empty_pop.size() == 0U);

    // Pop on empty returns identical state
    constexpr auto q_pop_again = q_empty_pop.pop();
    STATIC_REQUIRE(q_pop_again.size() == 0U);
}
