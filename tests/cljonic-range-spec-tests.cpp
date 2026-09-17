#include <catch2/catch_test_macros.hpp>
#include <concepts>
#include <limits>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("Range construction and bounded arithmetic behavior", "[range]") {
    using cljonic::Range;
    using cljonic::Vector;
    using cljonic::concepts::CljonicCollection;
    using cljonic::concepts::CljonicProducer;
    using cljonic::concepts::CljonicRange;
    using cljonic::concepts::CljonicSource;
    using cljonic::concepts::IndexedProducer;
    using cljonic::concepts::SequenceableProducer;
    using cljonic::concepts::StableEqualityComparable;

    TRACE_ID("entity-fields.Range");
    TRACE_ID("invariant.Range.HeaderOnlyDistribution");
    TRACE_ID("invariant.Range.NoHeapAllocation");
    TRACE_ID("invariant.Range.NoRtti");
    TRACE_ID("invariant.Range.NoExceptions");
    TRACE_ID("invariant.Range.SingleThreadedExecutionModel");
    TRACE_ID("invariant.Range.OwnsItsParameters");
    TRACE_ID("invariant.Range.DoesNotOwnResultStorage");
    TRACE_ID("invariant.Range.DoesNotBorrowSourceOrCallback");
    TRACE_ID("invariant.Range.ReferentialTransparency");
    TRACE_ID("invariant.Range.DefaultStartIsZero");
    TRACE_ID("invariant.Range.DefaultStepIsOne");
    TRACE_ID("invariant.Range.StartIsInclusive");
    TRACE_ID("invariant.Range.EndIsExclusive");
    TRACE_ID("invariant.Range.ZeroStepProducesInfiniteRepetitionOfStart");
    TRACE_ID("invariant.Range.ZeroStepTakesPrecedenceOverEmptyRange");
    TRACE_ID("invariant.Range.NonzeroStepAwayFromEndIsEmptyRange");
    TRACE_ID("invariant.Range.EqualStartAndEndWithNonzeroStepIsEmptyRange");
    TRACE_ID("invariant.Range.EffectiveSizeIsConservativeMaximumWithoutTraversal");
    TRACE_ID("invariant.Range.SaturatingCardinalityBoundedByCollectionMaximumElementCount");
    TRACE_ID("invariant.Range.SpanArithmeticAvoidsSignedOverflow");
    TRACE_ID("invariant.Range.UnboundedRangeDoesNotSatisfyStableEquality");
    TRACE_ID("invariant.Range.UnboundedEqualityFailsAtCompileTime");
    TRACE_ID("invariant.Range.ContainsIsAuthoritativeAvailableIndexPredicate");
    TRACE_ID("invariant.Range.ContainsIsConstantTime");
    TRACE_ID("invariant.Range.DoesNotExposeCallableOperator");
    TRACE_ID("invariant.Range.GetAndLookupAreExcluded");
    TRACE_ID("invariant.Range.FreeFunctionObservationIsCanonicalAccessPath");
    TRACE_ID("invariant.Range.StartEndStepMemberAccessorsAreNoncanonical");

    // Range is Indexed but not IFn: no callable operator() exists, unlike Vector/Map/Set.
    STATIC_REQUIRE_FALSE(std::invocable<Range<int>, std::size_t>);

    // Default start 0 / step 1, inclusive start, exclusive end.
    constexpr Range<int> default_start_and_step{5};
    STATIC_REQUIRE(default_start_and_step.count() == 5U);
    STATIC_REQUIRE(default_start_and_step.contains(0U));
    STATIC_REQUIRE(default_start_and_step.contains(4U));
    STATIC_REQUIRE_FALSE(default_start_and_step.contains(5U));

    constexpr Range<int> explicit_start_end{2, 5};
    STATIC_REQUIRE(explicit_start_end.count() == 3U);
    STATIC_REQUIRE(explicit_start_end.contains(0U));
    STATIC_REQUIRE(explicit_start_end.contains(2U));
    STATIC_REQUIRE_FALSE(explicit_start_end.contains(3U));

    // Zero step is an infinite repetition of start, bounded by the system maximum.
    constexpr Range<int> zero_step{0, 5, 0};
    STATIC_REQUIRE(zero_step.count() == cljonic::CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE);
    STATIC_REQUIRE(zero_step.contains(0U));
    STATIC_REQUIRE(zero_step.contains(cljonic::CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE - 1U));
    STATIC_REQUIRE_FALSE(zero_step.contains(cljonic::CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE));

    // Zero step with equal start and end still takes precedence over empty-range behavior.
    constexpr Range<int> zero_step_equal_start_end{3, 3, 0};
    STATIC_REQUIRE(zero_step_equal_start_end.count() == cljonic::CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE);
    STATIC_REQUIRE(zero_step_equal_start_end.contains(0U));

    // A nonzero step moving away from end produces an empty range.
    constexpr Range<int> away_from_end{5, 0, 1};
    STATIC_REQUIRE(away_from_end.count() == 0U);
    STATIC_REQUIRE_FALSE(away_from_end.contains(0U));

    // Equal start and end with a nonzero step produces an empty range.
    constexpr Range<int> equal_nonzero_step{3, 3, 1};
    STATIC_REQUIRE(equal_nonzero_step.count() == 0U);
    STATIC_REQUIRE_FALSE(equal_nonzero_step.contains(0U));

    // Negative step traverses downward, still exclusive at end.
    constexpr Range<int> negative_step{5, 0, -1};
    STATIC_REQUIRE(negative_step.count() == 5U);
    STATIC_REQUIRE(negative_step.contains(0U));
    STATIC_REQUIRE(negative_step.contains(4U));
    STATIC_REQUIRE_FALSE(negative_step.contains(5U));

    // contains is authoritative index-in-range, independent of the step cadence's spacing.
    constexpr Range<int> stepped{0, 10, 2};
    STATIC_REQUIRE(stepped.contains(4U));
    STATIC_REQUIRE_FALSE(stepped.contains(5U));

    // contains is O(1) over the available bounded prefix, even for a huge range.
    constexpr Range<long> huge{1000000000L};
    STATIC_REQUIRE(huge.count() == cljonic::CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE);
    STATIC_REQUIRE(huge.contains(cljonic::CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE - 1U));
    STATIC_REQUIRE_FALSE(huge.contains(cljonic::CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE));

    constexpr auto int_min = std::numeric_limits<int>::min();
    constexpr auto int_max = std::numeric_limits<int>::max();

    // Extreme endpoints use unsigned distance arithmetic without signed overflow.
    constexpr Range<int> full_signed_span{int_min, int_max};
    STATIC_REQUIRE(full_signed_span.count() == cljonic::CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE);

    // Extreme negative steps use unsigned magnitude arithmetic without signed overflow.
    constexpr Range<int> extreme_negative_step{1, int_min, int_min};
    STATIC_REQUIRE(extreme_negative_step.count() == 2U);

    // Iteration does not advance past the final available element.
    constexpr Vector<int, 2> edge_destination{};
    constexpr auto single_edge_result = into(edge_destination, Range<int>{int_max - 1, int_max, 2});
    STATIC_REQUIRE(single_edge_result.count() == 1U);
    STATIC_REQUIRE(single_edge_result(0U) == int_max - 1);

    // Range participates in the producer nominal domain, not the collection domain.
    STATIC_REQUIRE(CljonicRange<Range<int>>);
    STATIC_REQUIRE(CljonicProducer<Range<int>>);
    STATIC_REQUIRE(CljonicSource<Range<int>>);
    STATIC_REQUIRE(SequenceableProducer<Range<int>>);
    STATIC_REQUIRE(IndexedProducer<Range<int>>);
    STATIC_REQUIRE_FALSE(CljonicCollection<Range<int>>);
    STATIC_REQUIRE_FALSE(CljonicRange<Vector<int, 4>>);
    STATIC_REQUIRE(CljonicSource<Vector<int, 4>>);

    // Range never satisfies equality, regardless of whether a particular instance is finite.
    STATIC_REQUIRE_FALSE(StableEqualityComparable<Range<int>>);

    const auto runtime_range = Range<int>{0, 3};
    CHECK(runtime_range.count() == 3U);
    CHECK(runtime_range.contains(1U));

    // Runtime coverage for zero-step, ascending, descending, empty, and saturated
    // branches (defeats constexpr folding so gcov records each branch).
    volatile int zero_step_raw = 0;
    const auto runtime_zero_step = Range<int>{0, 5, zero_step_raw};
    CHECK(runtime_zero_step.count() == cljonic::CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE);
    CHECK(runtime_zero_step.contains(0U));

    volatile int ascending_start_raw = 0;
    volatile int ascending_end_raw = 5;
    const auto runtime_ascending = Range<int>{ascending_start_raw, ascending_end_raw};
    CHECK(runtime_ascending.count() == 5U);
    CHECK(runtime_ascending.contains(3U));
    CHECK_FALSE(runtime_ascending.contains(5U));

    volatile int empty_ascending_start_raw = 5;
    const auto runtime_empty_ascending = Range<int>{empty_ascending_start_raw, empty_ascending_start_raw, 1};
    CHECK(runtime_empty_ascending.count() == 0U);
    CHECK_FALSE(runtime_empty_ascending.contains(0U));

    volatile int descending_start_raw = 5;
    volatile int descending_end_raw = 0;
    volatile int descending_step_raw = -1;
    const auto runtime_descending = Range<int>{descending_start_raw, descending_end_raw, descending_step_raw};
    CHECK(runtime_descending.count() == 5U);
    CHECK(runtime_descending.contains(0U));
    CHECK_FALSE(runtime_descending.contains(5U));

    volatile int empty_descending_start_raw = 0;
    volatile int empty_descending_end_raw = 5;
    const auto runtime_empty_descending = Range<int>{empty_descending_start_raw, empty_descending_end_raw, -1};
    CHECK(runtime_empty_descending.count() == 0U);
    CHECK_FALSE(runtime_empty_descending.contains(0U));

    volatile int oversized_end_raw = 2000;
    const auto runtime_oversized = Range<int>{0, oversized_end_raw};
    CHECK(runtime_oversized.count() == cljonic::CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE);
    CHECK(runtime_oversized.contains(cljonic::CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE - 1U));
    CHECK_FALSE(runtime_oversized.contains(cljonic::CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE));
}
