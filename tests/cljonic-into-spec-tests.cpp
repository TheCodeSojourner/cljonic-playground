#include <catch2/catch_test_macros.hpp>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("into and fits_into materialize a producer into a bounded destination", "[into][fits-into]") {
    using cljonic::fits_into;
    using cljonic::into;
    using cljonic::Range;
    using cljonic::Vector;

    TRACE_ID("entity-fields.ProducerMaterialization");
    TRACE_ID("invariant.ProducerMaterialization.IntoRequiresExplicitBoundedDestination");
    TRACE_ID("invariant.ProducerMaterialization.IntoReturnsNewDestinationTypedCollection");
    TRACE_ID("invariant.ProducerMaterialization.IntoAppendsAfterDestinationContents");
    TRACE_ID("invariant.ProducerMaterialization.IntoLeavesDestinationUnchanged");
    TRACE_ID("invariant.ProducerMaterialization.IntoLeavesSourceUnchanged");
    TRACE_ID("invariant.ProducerMaterialization.FitsIntoIsNonThrowingNonAllocating");
    TRACE_ID("invariant.ProducerMaterialization.FitsIntoPredictsIntoCompleteness");
    TRACE_ID("invariant.ProducerMaterialization.FitsIntoUsesRemainingDestinationCapacity");
    TRACE_ID("invariant.ProducerMaterialization.FitsIntoAvoidsUnsignedAdditionOverflow");
    TRACE_ID("invariant.ProducerMaterialization.UnboundedSourceDoesNotFitIntoDestination");
    TRACE_ID("invariant.ProducerMaterialization.UnboundedSourceMaterializesBoundedPrefix");
    TRACE_ID("invariant.ProducerMaterialization.FiniteSourceMaterializesCompleteResultWhenItFits");
    TRACE_ID("invariant.ProducerMaterialization.BoundedPrefixPreservesSourceTraversalOrder");
    TRACE_ID("invariant.ProducerMaterialization.CountIsExactForMaterializedCollection");
    TRACE_ID("invariant.ProducerMaterialization.UnboundedProducerCountIsObservableTraversalCap");
    TRACE_ID("invariant.ProducerMaterialization.ComposedProducerCardinalityUsesSaturatingArithmetic");
    TRACE_ID("invariant.ProducerMaterialization.ComposedCardinalityBoundedByCollectionMaximumElementCount");
    TRACE_ID("invariant.ProducerMaterialization.MaterializationSourceDomainIsCljonicSource");

    // A finite source that fits materializes completely, leaving destination and source unchanged.
    constexpr Vector<int, 8> empty_destination{};
    constexpr Range<int> finite_source{0, 5};
    constexpr auto complete_result = into(empty_destination, finite_source);
    STATIC_REQUIRE(complete_result.count() == 5U);
    STATIC_REQUIRE(complete_result(0U) == 0);
    STATIC_REQUIRE(complete_result(4U) == 4);
    STATIC_REQUIRE(empty_destination.count() == 0U);
    STATIC_REQUIRE(finite_source.count() == 5U);
    STATIC_REQUIRE(fits_into(empty_destination, finite_source));

    // An unbounded source materializes a deterministic bounded prefix limited by destination capacity.
    constexpr Range<int> unbounded_source{0, 5, 0};
    STATIC_REQUIRE_FALSE(unbounded_source.is_finite());
    constexpr auto bounded_prefix_result = into(empty_destination, unbounded_source);
    STATIC_REQUIRE(bounded_prefix_result.count() == empty_destination.capacity());
    STATIC_REQUIRE(bounded_prefix_result(0U) == 0);
    STATIC_REQUIRE_FALSE(fits_into(empty_destination, unbounded_source));

    // A finite source that does not fit the destination's remaining capacity is bounded.
    constexpr Vector<int, 3> small_destination{};
    constexpr Range<int> oversized_finite_source{0, 5};
    constexpr auto oversized_result = into(small_destination, oversized_finite_source);
    STATIC_REQUIRE(oversized_result.count() == 3U);
    STATIC_REQUIRE(oversized_result(2U) == 2);
    STATIC_REQUIRE_FALSE(fits_into(small_destination, oversized_finite_source));

    // Materialization appends to an already-populated destination.
    constexpr Vector<int, 8> populated_destination{100, 200};
    constexpr auto appended_result = into(populated_destination, finite_source);
    STATIC_REQUIRE(appended_result.count() == 7U);
    STATIC_REQUIRE(appended_result(0U) == 100);
    STATIC_REQUIRE(appended_result(2U) == 0);
    STATIC_REQUIRE(populated_destination.count() == 2U);
    STATIC_REQUIRE(fits_into(populated_destination, finite_source));

    const auto runtime_destination = Vector<int, 8>{};
    const auto runtime_source = Range<int>{0, 5};
    const auto runtime_result = into(runtime_destination, runtime_source);
    CHECK(runtime_result.count() == 5U);
    CHECK(fits_into(runtime_destination, runtime_source));

    // Runtime coverage for the bounded-prefix break path (defeats constexpr
    // folding so gcov records the branch where the destination fills mid-loop).
    volatile int runtime_small_capacity_end = 5;
    const auto runtime_small_destination = Vector<int, 3>{};
    const auto runtime_oversized_source = Range<int>{0, runtime_small_capacity_end};
    const auto runtime_bounded_result = into(runtime_small_destination, runtime_oversized_source);
    CHECK(runtime_bounded_result.count() == 3U);
    CHECK_FALSE(fits_into(runtime_small_destination, runtime_oversized_source));

    // Composed cardinality (existing destination content plus a nominally huge
    // producer) stays bounded by the destination capacity and the producer's own
    // saturating arithmetic, without size_t overflow.
    volatile int huge_end_raw = 5000;
    const auto populated_destination_for_composition = Vector<int, 4>{1, 2};
    const auto huge_source = Range<int>{0, huge_end_raw};
    const auto composed_result = into(populated_destination_for_composition, huge_source);
    CHECK(composed_result.count() == 4U);
    CHECK_FALSE(fits_into(populated_destination_for_composition, huge_source));
}
