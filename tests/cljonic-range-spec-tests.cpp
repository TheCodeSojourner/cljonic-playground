#include <type_traits>

#include <catch2/catch_test_macros.hpp>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("Range supports the bounded integer producer subset", "[range]")
{
    using cljonic::collection_maximum_element_count;
    using cljonic::count;
    using cljonic::Range;
    using cljonic::valid_index;

    TRACE_ID("entity-fields.RangeSequence");
    TRACE_ID("invariant.RangeSequence.DefaultStartIsZero");
    TRACE_ID("invariant.RangeSequence.DefaultStepIsOne");
    TRACE_ID("invariant.RangeSequence.DefaultValueTypeIsInt");
    TRACE_ID("invariant.RangeSequence.SingleArgumentFormSupported");
    TRACE_ID("invariant.RangeSequence.TwoArgumentFormSupported");
    TRACE_ID("invariant.RangeSequence.ThreeArgumentFormSupported");
    TRACE_ID("invariant.RangeSequence.InclusiveStart");
    TRACE_ID("invariant.RangeSequence.ExclusiveEnd");
    TRACE_ID("invariant.RangeSequence.PositiveStepSupported");
    TRACE_ID("invariant.RangeSequence.NegativeStepSupported");
    TRACE_ID("invariant.RangeSequence.AwayFromEndStepYieldsEmptyRange");
    TRACE_ID("invariant.RangeSequence.EqualStartAndEndNonZeroStepIsEmpty");
    TRACE_ID("invariant.RangeSequence.ZeroStepCaseIsDocumentedSpecialCase");
    TRACE_ID("invariant.RangeSequence.ZeroStepIsSemanticallyInfinite");
    TRACE_ID("invariant.RangeSequence.SemanticallyInfiniteFormsUseSynthesisCap");
    TRACE_ID("invariant.RangeSequence.SynthesisCapIsNotTrueCardinality");
    TRACE_ID("invariant.RangeSequence.MaterializationNeverExceedsSynthesisCap");
    TRACE_ID("invariant.RangeSequence.OversizedFiniteFormsUseBoundedPrefix");
    TRACE_ID("invariant.RangeSequence."
             "OversizedFiniteFormsAdjustEffectiveExclusiveEnd");
    TRACE_ID(
        "invariant.RangeSequence.EffectiveSizeIsAuthoritativeIterationBound");
    TRACE_ID("invariant.RangeSequence.CanonicalObservationUsesFreeFunctions");
    TRACE_ID("invariant.RangeSequence.RangeMemberAccessorsAreNotRequired");
    TRACE_ID("invariant.RangeSequence.GetIsExcludedFromRangeContract");
    TRACE_ID("invariant.RangeSequence.ValidIndexIsSupported");
    TRACE_ID("invariant.RangeSequence.ValidIndexUsesEffectiveBoundedSize");
    TRACE_ID("invariant.RangeSequence.NegativeSignedIndexIsInvalid");
    TRACE_ID("invariant.RangeSequence.IntegralOnlyBoundsAndSteps");
    TRACE_ID("invariant.RangeSequence."
             "FloatingPointBoundsAndStepsAreRejectedAtCompileTime");
    TRACE_ID("invariant.RangeSequence.ProducerValueNotImplicitMaterialization");
    TRACE_ID("invariant.RangeSequence."
             "MaterializationRequiresExplicitBoundedDestination");
    TRACE_ID("surface-actor.RangeSurface");
    TRACE_ID("surface-provides.RangeSurface");

    constexpr Range by_default{5};
    constexpr Range<int> unbounded_default{};
    constexpr Range bare_default{};
    constexpr Range descending{10, 0, -2};
    constexpr Range ascending{2, 9, 2};
    constexpr Range empty{5, 5, 1};
    constexpr Range away_from_end{5, 10, -1};
    constexpr Range zero_step{3, 9, 0};
    constexpr Range zero_step_equal_end{0, 0, 0};
    constexpr Range bounded_prefix{0, 10, 1};

    STATIC_REQUIRE(!std::is_invocable_v<decltype(by_default), int>);
    // Range producers are non-invocable and don't expose member accessors;
    // bounded size is authoritative and accessed only through count().
    STATIC_REQUIRE(count(unbounded_default) ==
                   cljonic::CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE);
    STATIC_REQUIRE(std::is_same_v<decltype(bare_default), const Range<int>>);
    STATIC_REQUIRE(count(bare_default) ==
                   cljonic::CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE);
    STATIC_REQUIRE(collection_maximum_element_count() ==
                   cljonic::CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE);
    STATIC_REQUIRE(count(by_default) == 5U);
    STATIC_REQUIRE(count(descending) == 5U);
    STATIC_REQUIRE(count(ascending) == 4U);
    STATIC_REQUIRE(count(empty) == 0U);
    STATIC_REQUIRE(count(away_from_end) == 0U);
    STATIC_REQUIRE(count(zero_step) ==
                   cljonic::CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE);
    STATIC_REQUIRE(count(zero_step_equal_end) ==
                   cljonic::CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE);
    STATIC_REQUIRE(count(Range{0, 0}) == 0U);
    STATIC_REQUIRE(count(Range{0, 10}) == 10U);
    STATIC_REQUIRE(count(Range{0, 10, 0}) ==
                   cljonic::CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE);
    STATIC_REQUIRE(count(Range{5, 0, -2}) == 3U);
    STATIC_REQUIRE(count(Range{4, 10, -2}) == 0U);
    STATIC_REQUIRE(
        count(
            Range{0, static_cast<int>(
                         cljonic::CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE +
                         1)}) == collection_maximum_element_count());
    STATIC_REQUIRE(valid_index(bounded_prefix, 0));
    STATIC_REQUIRE(valid_index(bounded_prefix, 9));
    STATIC_REQUIRE_FALSE(valid_index(bounded_prefix, 10));
    STATIC_REQUIRE_FALSE(valid_index(bounded_prefix, -1));
    STATIC_REQUIRE(noexcept(valid_index(bounded_prefix, 0)));

    CHECK(count(by_default) == 5U);
    CHECK(count(descending) == 5U);
    CHECK(count(ascending) == 4U);
    CHECK(count(zero_step) ==
          cljonic::CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE);
    CHECK(count(zero_step_equal_end) ==
          cljonic::CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE);
    CHECK(count(Range{5, 8, 2}) == 2U);
    CHECK(count(Range{5, 0, -2}) == 3U);
    CHECK(count(Range{4, 10, -2}) == 0U);
    CHECK(valid_index(bounded_prefix, 0));
    CHECK(valid_index(bounded_prefix, 9));
    CHECK_FALSE(valid_index(bounded_prefix, 10));
    CHECK_FALSE(valid_index(bounded_prefix, -1));
}
