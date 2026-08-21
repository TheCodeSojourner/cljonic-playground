#include <type_traits>

#include <catch2/catch_test_macros.hpp>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("Range supports the bounded integer producer subset", "[range]")
{
    using cljonic::Range;

    TRACE_ID("entity-fields.RangeSequence");
    TRACE_ID("invariant.RangeSequence.DefaultStartIsZero");
    TRACE_ID("invariant.RangeSequence.DefaultStepIsOne");
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
    TRACE_ID("invariant.RangeSequence.IntegralOnlyBoundsAndSteps");
    TRACE_ID("invariant.RangeSequence.FloatingPointBoundsAndStepsAreRejectedAtCompileTime");
    TRACE_ID("invariant.RangeSequence.ProducerValueNotImplicitMaterialization");
    TRACE_ID("invariant.RangeSequence.MaterializationRequiresExplicitBoundedDestination");
    TRACE_ID("surface-actor.RangeSurface");
    TRACE_ID("surface-provides.RangeSurface");

    constexpr Range by_default{5};
    constexpr Range descending{10, 0, -2};
    constexpr Range ascending{2, 9, 2};
    constexpr Range empty{5, 5, 1};
    constexpr Range away_from_end{5, 10, -1};
    constexpr Range zero_step{3, 9, 0};

    STATIC_REQUIRE(!std::is_invocable_v<decltype(by_default), int>);
    STATIC_REQUIRE(by_default.size() == 5U);
    STATIC_REQUIRE(by_default.begin() == 0);
    STATIC_REQUIRE(by_default.end() == 5);
    STATIC_REQUIRE(descending.size() == 5U);
    STATIC_REQUIRE(descending.begin() == 10);
    STATIC_REQUIRE(descending.end() == 0);
    STATIC_REQUIRE(ascending.size() == 4U);
    STATIC_REQUIRE(ascending.begin() == 2);
    STATIC_REQUIRE(ascending.end() == 9);
    STATIC_REQUIRE(empty.size() == 0U);
    STATIC_REQUIRE(away_from_end.size() == 0U);
    STATIC_REQUIRE(zero_step.size() == CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT);
    STATIC_REQUIRE(zero_step.end() == 9);
    STATIC_REQUIRE(Range{0, 0}.size() == 0U);
    STATIC_REQUIRE(Range{0, 10}.size() == 10U);
    STATIC_REQUIRE(Range{0, 10, 0}.size() == CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT);

    CHECK(by_default.begin() == 0);
    CHECK(descending.begin() == 10);
    CHECK(ascending.end() == 9);
    CHECK(zero_step.size() == CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT);
    CHECK((Range{5, 8, 2}.size()) == 2U);
}
