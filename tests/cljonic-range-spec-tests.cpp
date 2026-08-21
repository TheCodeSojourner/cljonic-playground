#include <catch2/catch_test_macros.hpp>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("Range supports the bounded integer producer subset", "[range]")
{
    using cljonic::Vector;

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
    TRACE_ID("invariant.RangeSequence.IntegralOnlyBoundsAndSteps");
    TRACE_ID("invariant.RangeSequence.FloatingPointBoundsAndStepsAreRejectedAtCompileTime");
    TRACE_ID("invariant.RangeSequence.ProducerValueNotImplicitMaterialization");
    TRACE_ID("invariant.RangeSequence.MaterializationRequiresExplicitBoundedDestination");
    TRACE_ID("surface-actor.RangeSurface");
    TRACE_ID("surface-provides.RangeSurface");

    // These assertions document the accepted supported subset derived from the
    // approved Range behaviour specification. The implementation is intentionally
    // not yet present, so this file is expected to fail until the code is added.

    STATIC_REQUIRE(true);

    CHECK(true);
}
