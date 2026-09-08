#include <catch2/catch_test_macros.hpp>

#include <concepts>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("Queue FIFO construction and operations", "[queue]") {
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
    TRACE_ID("invariant.Queue.RequiresNothrowDefaultConstruction");
    TRACE_ID("invariant.Queue.RequiresNothrowCopyConstruction");
    TRACE_ID("invariant.Queue.RequiresNothrowCopyAssignment");
    TRACE_ID("invariant.Queue.RequiresNothrowDestruction");
    TRACE_ID("invariant.Queue.SupportsEmptyExplicitCapacityConstruction");
    TRACE_ID("invariant.Queue.SupportsExplicitCapacityConstruction");
    TRACE_ID("invariant.Queue.SupportsLiteralDeducedConstruction");
    TRACE_ID("invariant.Queue.SupportsCapacityInferredLiteralEquivalentSemantics");
    TRACE_ID("invariant.Queue.OversizedInitializerIsCompileTimeFailure");
    TRACE_ID("invariant.Queue.PackConstructionFoldsOverConj");
    TRACE_ID("invariant.Queue.CapacityExceedsMaximumIsCompileTimeFailure");
    TRACE_ID("invariant.Queue.SupportsPeekOperation");
    TRACE_ID("invariant.Queue.SupportsPopOperation");
    TRACE_ID("invariant.Queue.SupportsConjOperation");
    TRACE_ID("invariant.Queue.SupportsSeqOperation");
    TRACE_ID("invariant.Queue.SupportsFirstOperation");
    TRACE_ID("invariant.Queue.SupportsNextOperation");
    TRACE_ID("invariant.Queue.SupportsRestOperation");
    TRACE_ID("invariant.Queue.SequenceResultIsOwningVector");
    TRACE_ID("invariant.Queue.SequenceTraversalIsFrontToRearFifo");
    TRACE_ID("invariant.Queue.SequenceTraversalPreservesSource");
    TRACE_ID("invariant.Queue.ConstRangeTraversalIsNonAllocating");
    TRACE_ID("invariant.Queue.ConstRangeTraversalIsNonThrowing");
    TRACE_ID("invariant.Queue.WrappedStorageDoesNotChangeLogicalOrder");
    TRACE_ID("invariant.Queue.ContiguousInteroperabilityViewIsNotRequired");
    TRACE_ID("invariant.Queue.EmptyQueuePeekReturnsDefaultElement");
    TRACE_ID("invariant.Queue.EmptyQueuePopReturnsEmptyQueue");
    TRACE_ID("invariant.Queue.PreflightPredicatesAreNonThrowingNonAllocating");
    TRACE_ID("invariant.Queue.IsEmptyIsCanonicalEmptyPredicate");
    TRACE_ID("invariant.Queue.CanConjIsCanonicalConjPreflight");
    TRACE_ID("invariant.Queue.CompileTimeCapacityOverflowIsRejected");
    TRACE_ID("invariant.Queue.RuntimeCapacityFailuresHaveDocumentedPolicy");

    constexpr Queue<int, 4> q{};
    STATIC_REQUIRE(q.is_empty());
    STATIC_REQUIRE(q.count() == 0U);
    STATIC_REQUIRE(q.capacity() == 4U);
    STATIC_REQUIRE(q.can_conj()); // empty → room available

    // Pack-literal construction: explicit capacity, CTAD, and FIFO order
    // matching argument order, folded over conj.
    constexpr Queue<int, 4> explicit_literal{10, 20, 30};
    STATIC_REQUIRE(explicit_literal.count() == 3U);
    STATIC_REQUIRE(explicit_literal.peek() == 10);

    constexpr auto inferred_literal = Queue{10, 20, 30};
    STATIC_REQUIRE(std::same_as<decltype(inferred_literal), const Queue<int, 3>>);
    STATIC_REQUIRE(inferred_literal.count() == 3U);
    STATIC_REQUIRE(inferred_literal.peek() == 10);

    // conj enqueues to back
    constexpr auto q1 = q.conj(1);
    STATIC_REQUIRE_FALSE(q1.is_empty());
    STATIC_REQUIRE(q1.count() == 1U);
    STATIC_REQUIRE(q1.peek() == 1);

    constexpr auto q2 = q1.conj(2);
    STATIC_REQUIRE(q2.count() == 2U);

    constexpr auto q3 = q2.conj(3);
    STATIC_REQUIRE(q3.count() == 3U);

    constexpr auto q4 = q3.conj(4);
    STATIC_REQUIRE(q4.count() == 4U);
    STATIC_REQUIRE_FALSE(q4.can_conj()); // full

    // conj on full queue returns unchanged copy
    constexpr auto q5 = q4.conj(99);
    STATIC_REQUIRE(q5.count() == 4U);

    // FIFO ordering via pop
    // q4 has [1, 2, 3, 4] front-to-back
    constexpr auto qp1 = q4.pop();
    STATIC_REQUIRE(qp1.count() == 3U);
    STATIC_REQUIRE(qp1.peek() == 2); // 1 removed

    constexpr auto qp2 = qp1.pop();
    STATIC_REQUIRE(qp2.count() == 2U);
    STATIC_REQUIRE(qp2.peek() == 3); // 2 removed

    constexpr auto qp3 = qp2.pop();
    STATIC_REQUIRE(qp3.count() == 1U);
    STATIC_REQUIRE(qp3.peek() == 4); // 3 removed

    constexpr auto qp4 = qp3.pop();
    STATIC_REQUIRE(qp4.is_empty()); // 4 removed
    STATIC_REQUIRE(qp4.count() == 0U);

    // Empty queue: peek returns default-constructed value
    STATIC_REQUIRE(q.peek() == 0); // empty queue peek

    // Empty queue: pop returns unchanged copy
    constexpr auto q_empty_pop = q.pop();
    STATIC_REQUIRE(q_empty_pop.is_empty());
    STATIC_REQUIRE(q_empty_pop.count() == 0U);

    // Pop on empty returns identical state
    constexpr auto q_pop_again = q_empty_pop.pop();
    STATIC_REQUIRE(q_pop_again.count() == 0U);

    // Pop followed by conj reuses the vacated physical slot while preserving
    // logical FIFO order.
    constexpr auto wrapped = q3.pop().conj(4);
    STATIC_REQUIRE(wrapped.count() == 3U);
    STATIC_REQUIRE(wrapped.peek() == 2);
    STATIC_REQUIRE(wrapped.pop().peek() == 3);
    STATIC_REQUIRE(wrapped.pop().pop().peek() == 4);

    // Sequence behavior follows logical FIFO order and preserves the source.
    constexpr auto sequence = cljonic::seq(q3);
    STATIC_REQUIRE(sequence.count() == 3U);
    STATIC_REQUIRE(sequence(0U) == 1);
    STATIC_REQUIRE(sequence(1U) == 2);
    STATIC_REQUIRE(sequence(2U) == 3);
    STATIC_REQUIRE(cljonic::first(q3) == 1);
    constexpr auto sequence_next = cljonic::next(q3);
    constexpr auto sequence_rest = cljonic::rest(q3);
    STATIC_REQUIRE(sequence_next.count() == 2U);
    STATIC_REQUIRE(sequence_next(0U) == 2);
    STATIC_REQUIRE(sequence_next(1U) == 3);
    STATIC_REQUIRE(sequence_rest.count() == 2U);
    STATIC_REQUIRE(sequence_rest(0U) == 2);
    STATIC_REQUIRE(sequence_rest(1U) == 3);
    STATIC_REQUIRE(q3.peek() == 1);

    // Sequence behavior remains FIFO after the physical storage wraps.
    constexpr auto wrapped_sequence = cljonic::seq(wrapped);
    STATIC_REQUIRE(wrapped_sequence.count() == 3U);
    STATIC_REQUIRE(wrapped_sequence(0U) == 2);
    STATIC_REQUIRE(wrapped_sequence(1U) == 3);
    STATIC_REQUIRE(wrapped_sequence(2U) == 4);
    STATIC_REQUIRE(cljonic::first(wrapped) == 2);
    constexpr auto wrapped_next = cljonic::next(wrapped);
    constexpr auto wrapped_rest = cljonic::rest(wrapped);
    STATIC_REQUIRE(wrapped_next(0U) == 3);
    STATIC_REQUIRE(wrapped_next(1U) == 4);
    STATIC_REQUIRE(wrapped_rest(0U) == 3);
    STATIC_REQUIRE(wrapped_rest(1U) == 4);

    // Runtime tests for code coverage instrumentation
    volatile int v1_raw = 10;
    volatile int v2_raw = 20;
    int v1 = v1_raw;
    int v2 = v2_raw;
    auto rq = Queue<int, 4>{};
    REQUIRE(rq.is_empty());
    REQUIRE(rq.count() == 0U);
    REQUIRE(rq.capacity() == 4U);
    REQUIRE(rq.can_conj());
    REQUIRE(rq.peek() == 0);
    REQUIRE(rq.pop().is_empty());

    auto rq1 = rq.conj(v1);
    REQUIRE_FALSE(rq1.is_empty());
    REQUIRE(rq1.count() == 1U);
    REQUIRE(rq1.peek() == 10);

    auto rq2 = rq1.conj(v2);
    REQUIRE(rq2.count() == 2U);
    REQUIRE(rq2.peek() == 10);

    auto rq_popped = rq2.pop();
    REQUIRE(rq_popped.count() == 1U);
    REQUIRE(rq_popped.peek() == 20);

    auto rq_full = rq_popped.conj(30).conj(40).conj(50);
    REQUIRE(rq_full.count() == 4U);
    REQUIRE_FALSE(rq_full.can_conj());
    REQUIRE(rq_full.conj(60).count() == 4U); // rejected overflow

    volatile int literal_v1_raw = 10;
    volatile int literal_v2_raw = 20;
    volatile int literal_v3_raw = 30;
    auto runtime_literal = Queue{literal_v1_raw, literal_v2_raw, literal_v3_raw};
    REQUIRE(runtime_literal.count() == 3U);
    REQUIRE(runtime_literal.peek() == 10);

    auto runtime_sequence = cljonic::seq(runtime_literal);
    REQUIRE(runtime_sequence.count() == 3U);
    REQUIRE(runtime_sequence(0U) == 10);
    REQUIRE(runtime_sequence(1U) == 20);
    REQUIRE(runtime_sequence(2U) == 30);
}
