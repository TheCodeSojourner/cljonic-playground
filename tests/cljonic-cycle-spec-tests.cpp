#include "cljonic-test-api.hpp"
#include <catch2/catch_test_macros.hpp>
#include <concepts>

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("Cycle repeats a bounded source value sequence and materializes bounded prefixes", "[cycle]") {
    using cljonic::cycle;
    using cljonic::fits_into;
    using cljonic::into;
    using cljonic::Vector;
    using cljonic::concepts::CljonicCycle;
    using cljonic::concepts::CljonicProducer;
    using cljonic::concepts::SequenceableProducer;

    TRACE_ID("entity-fields.Cycle");
    TRACE_ID("invariant.Cycle.HeaderOnlyDistribution");
    TRACE_ID("invariant.Cycle.NoHeapAllocation");
    TRACE_ID("invariant.Cycle.NoRtti");
    TRACE_ID("invariant.Cycle.NoExceptions");
    TRACE_ID("invariant.Cycle.SingleThreadedExecutionModel");
    TRACE_ID("invariant.Cycle.OwnsRepeatedSequence");
    TRACE_ID("invariant.Cycle.DoesNotOwnResultStorage");
    TRACE_ID("invariant.Cycle.DoesNotBorrowSourceOrCallback");
    TRACE_ID("invariant.Cycle.ReferentialTransparency");
    TRACE_ID("invariant.Cycle.SingleArgumentFormOnly");
    TRACE_ID("invariant.Cycle.IsUnbounded");
    TRACE_ID("invariant.Cycle.ExposesCountAndIsFinite");
    TRACE_ID("invariant.Cycle.CountIsObservableTraversalCap");
    TRACE_ID("invariant.Cycle.ConstTraversalTerminatesAtMaximumCap");
    TRACE_ID("invariant.Cycle.RepeatsSourceSequenceInOrder");
    TRACE_ID("invariant.Cycle.EmptySourceProducesEmptyObservation");
    TRACE_ID("invariant.Cycle.MaterializesAtMostDestinationCapacity");
    TRACE_ID("invariant.Cycle.DoesNotFitIntoDestination");
    TRACE_ID("invariant.Cycle.UnboundedCycleDoesNotSatisfyStableEquality");
    TRACE_ID("invariant.Cycle.UnboundedEqualityFailsAtCompileTime");
    TRACE_ID("invariant.Cycle.DoesNotSatisfyIndexedProducer");
    TRACE_ID("invariant.Cycle.SatisfiesSequenceableProducer");
    TRACE_ID("invariant.Cycle.DoesNotExposeCallableOperator");

    constexpr auto unbounded_cycle = cycle(Vector<int, 3>{1, 2, 3});
    STATIC_REQUIRE(unbounded_cycle.count() == cljonic::CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE);
    STATIC_REQUIRE_FALSE(unbounded_cycle.is_finite());
    constexpr auto bounded_prefix_result = into(Vector<int, 5>{}, unbounded_cycle);
    STATIC_REQUIRE(bounded_prefix_result.count() == 5U);
    STATIC_REQUIRE(bounded_prefix_result(0U) == 1);
    STATIC_REQUIRE(bounded_prefix_result(1U) == 2);
    STATIC_REQUIRE(bounded_prefix_result(2U) == 3);
    STATIC_REQUIRE(bounded_prefix_result(3U) == 1);
    STATIC_REQUIRE(bounded_prefix_result(4U) == 2);
    STATIC_REQUIRE_FALSE(fits_into(Vector<int, 5>{}, unbounded_cycle));

    constexpr auto empty_cycle = cycle(Vector<int, 0>{});
    STATIC_REQUIRE(empty_cycle.count() == cljonic::CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE);
    STATIC_REQUIRE_FALSE(empty_cycle.is_finite());
    constexpr auto empty_prefix_result = into(Vector<int, 3>{}, empty_cycle);
    STATIC_REQUIRE(empty_prefix_result.count() == 0U);
    STATIC_REQUIRE_FALSE(fits_into(Vector<int, 3>{}, empty_cycle));

    const auto runtime_empty_cycle = cycle(Vector<int, 0>{});
    const auto empty_begin = runtime_empty_cycle.begin();
    CHECK(empty_begin == runtime_empty_cycle.end());
    auto exhausted_end = runtime_empty_cycle.end();
    ++exhausted_end;
    CHECK(exhausted_end == runtime_empty_cycle.end());

    STATIC_REQUIRE(CljonicProducer<decltype(unbounded_cycle)>);
    STATIC_REQUIRE(CljonicCycle<decltype(unbounded_cycle)>);
    STATIC_REQUIRE(SequenceableProducer<decltype(unbounded_cycle)>);

    const auto runtime_unbounded_cycle = cycle(Vector<int, 3>{7, 8, 9});
    const auto runtime_prefix_result = into(Vector<int, 2>{}, runtime_unbounded_cycle);
    CHECK(runtime_prefix_result.count() == 2U);
    CHECK(runtime_prefix_result(0U) == 7);
    CHECK(runtime_prefix_result(1U) == 8);
    CHECK_FALSE(fits_into(Vector<int, 2>{}, runtime_unbounded_cycle));
}
