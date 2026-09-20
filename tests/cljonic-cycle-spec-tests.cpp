#include "cljonic-test-api.hpp"
#include <catch2/catch_test_macros.hpp>
#include <concepts>

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("Cycle repeats a bounded source value sequence and materializes bounded prefixes", "[cycle]") {
    using cljonic::cycle;
    using cljonic::fits_into;
    using cljonic::into;
    using cljonic::Range;
    using cljonic::Vector;
    using cljonic::concepts::CljonicCycle;
    using cljonic::concepts::CljonicProducer;
    using cljonic::concepts::CljonicSource;
    using cljonic::concepts::ConstInputRange;
    using cljonic::concepts::NothrowConstInputRange;
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
    TRACE_ID("invariant.Cycle.AcceptsFiniteOrUnboundedSequenceSource");
    TRACE_ID("invariant.Cycle.SourceSatisfiesConstInputRange");
    TRACE_ID("invariant.Cycle.SourceSatisfiesNothrowConstInputRange");
    TRACE_ID("invariant.Cycle.StoresIndependentOwnedSourceCopy");
    TRACE_ID("invariant.Cycle.ProducerSourceCopyIsParametersAndState");
    TRACE_ID("invariant.Cycle.IsUnbounded");
    TRACE_ID("invariant.Cycle.ExposesCountAndIsFinite");
    TRACE_ID("invariant.Cycle.CountIsObservableTraversalCap");
    TRACE_ID("invariant.Cycle.ConstTraversalTerminatesAtMaximumCap");
    TRACE_ID("invariant.Cycle.RepeatsSourceSequenceInOrder");
    TRACE_ID("invariant.Cycle.FiniteSourceRestartsAfterCompletePass");
    TRACE_ID("invariant.Cycle.UnboundedSourcePreservesObservablePrefix");
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
    STATIC_REQUIRE(ConstInputRange<Vector<int, 3>>);
    STATIC_REQUIRE(ConstInputRange<Range<int>>);
    STATIC_REQUIRE(NothrowConstInputRange<Vector<int, 3>>);
    STATIC_REQUIRE(NothrowConstInputRange<Range<int>>);
    STATIC_REQUIRE(NothrowConstInputRange<decltype(unbounded_cycle)>);
    STATIC_REQUIRE(CljonicSource<Vector<int, 3>>);
    STATIC_REQUIRE(CljonicSource<Range<int>>);
    STATIC_REQUIRE(SequenceableProducer<decltype(unbounded_cycle)>);

    const auto runtime_unbounded_cycle = cycle(Vector<int, 3>{7, 8, 9});
    const auto runtime_prefix_result = into(Vector<int, 2>{}, runtime_unbounded_cycle);
    CHECK(runtime_prefix_result.count() == 2U);
    CHECK(runtime_prefix_result(0U) == 7);
    CHECK(runtime_prefix_result(1U) == 8);
    CHECK_FALSE(fits_into(Vector<int, 2>{}, runtime_unbounded_cycle));

    const auto runtime_restarted_cycle = cycle(Vector<int, 3>{4, 5, 6});
    const auto runtime_restarted_prefix = into(Vector<int, 5>{}, runtime_restarted_cycle);
    CHECK(runtime_restarted_prefix(0U) == 4);
    CHECK(runtime_restarted_prefix(1U) == 5);
    CHECK(runtime_restarted_prefix(2U) == 6);
    CHECK(runtime_restarted_prefix(3U) == 4);
    CHECK(runtime_restarted_prefix(4U) == 5);

    constexpr auto finite_source_cycle = cycle(Range{1, 4});
    STATIC_REQUIRE(finite_source_cycle.count() == cljonic::CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE);
    STATIC_REQUIRE_FALSE(finite_source_cycle.is_finite());
    constexpr auto finite_source_prefix = into(Vector<int, 5>{}, finite_source_cycle);
    STATIC_REQUIRE(finite_source_prefix.count() == 5U);
    STATIC_REQUIRE(finite_source_prefix(0U) == 1);
    STATIC_REQUIRE(finite_source_prefix(1U) == 2);
    STATIC_REQUIRE(finite_source_prefix(2U) == 3);
    STATIC_REQUIRE(finite_source_prefix(3U) == 1);
    STATIC_REQUIRE(finite_source_prefix(4U) == 2);

    constexpr auto unbounded_source_cycle = cycle(Range{0, 0, 0});
    constexpr auto unbounded_source_prefix = into(Vector<int, 4>{}, unbounded_source_cycle);
    STATIC_REQUIRE(unbounded_source_prefix.count() == 4U);
    STATIC_REQUIRE(unbounded_source_prefix(0U) == 0);
    STATIC_REQUIRE(unbounded_source_prefix(1U) == 0);
    STATIC_REQUIRE(unbounded_source_prefix(2U) == 0);
    STATIC_REQUIRE(unbounded_source_prefix(3U) == 0);
}
