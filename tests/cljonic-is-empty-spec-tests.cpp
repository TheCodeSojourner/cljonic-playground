#include <catch2/catch_test_macros.hpp>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("IsEmpty free function operations", "[is_empty]") {
    using cljonic::conj;
    using cljonic::count;
    using cljonic::is_empty;
    using cljonic::Queue;
    using cljonic::Range;
    using cljonic::Repeat;
    using cljonic::Vector;

    TRACE_ID("entity-fields.IsEmpty");
    TRACE_ID("invariant.IsEmpty.IsEmptyFreeFunctionSupported");
    TRACE_ID("invariant.IsEmpty.IsEmptyDerivedFromZeroCount");
    TRACE_ID("invariant.IsEmpty.NoCollectionIsEmptyMember");
    TRACE_ID("invariant.IsEmpty.ReturnsBooleanPredicate");
    TRACE_ID("invariant.IsEmpty.SupportsSequenceableCollectionDomain");
    TRACE_ID("invariant.IsEmpty.SupportsSequenceableProducerDomain");
    TRACE_ID("invariant.IsEmpty.ZeroCountMatchesEmptyObservation");
    TRACE_ID("invariant.IsEmpty.FollowsProducerCountObservation");
    TRACE_ID("invariant.IsEmpty.RejectsUnsupportedDomainWithTargetedDiagnostic");
    TRACE_ID("invariant.IsEmpty.DiagnosticNamesRejectedOperandAndRequiredSequenceableDomain");
    TRACE_ID("invariant.IsEmpty.NoHeapAllocation");
    TRACE_ID("invariant.IsEmpty.NoRtti");
    TRACE_ID("invariant.IsEmpty.NoExceptions");
    TRACE_ID("invariant.IsEmpty.SingleThreadedExecutionModel");
    TRACE_ID("invariant.IsEmpty.ReferentialTransparency");

    STATIC_REQUIRE(cljonic::concepts::SequenceableCollection<Vector<int, 4>>);
    STATIC_REQUIRE_FALSE(cljonic::concepts::SequenceableCollection<Range<int>>);
    STATIC_REQUIRE_FALSE(cljonic::concepts::SequenceableProducer<Vector<int, 4>>);
    STATIC_REQUIRE(cljonic::concepts::SequenceableProducer<Range<int>>);
    STATIC_REQUIRE(is_empty(Range<int>{0, 0}));
    STATIC_REQUIRE(is_empty(Range<int>{4, 0}));
    STATIC_REQUIRE_FALSE(is_empty(Range<int>{0, 3}));
    STATIC_REQUIRE_FALSE(is_empty(Range<int>{0, 0, 0}));
    STATIC_REQUIRE_FALSE(is_empty(Repeat<int>{1}));

    constexpr Vector<int, 4> e{};
    constexpr Vector<int, 4> v{1};
    STATIC_REQUIRE(is_empty(e));
    STATIC_REQUIRE(is_empty(e) == (count(e) == 0U));
    STATIC_REQUIRE_FALSE(is_empty(v));
    STATIC_REQUIRE(is_empty(v) == (count(v) == 0U));

    constexpr Queue<int, 4> q{};
    constexpr auto q1 = conj(q, 5);
    STATIC_REQUIRE(is_empty(q));
    STATIC_REQUIRE(is_empty(q) == (count(q) == 0U));
    STATIC_REQUIRE_FALSE(is_empty(q1));

    // Runtime tests for code coverage instrumentation
    auto rv = Vector<int, 4>{9};
    REQUIRE_FALSE(is_empty(rv));
    auto rev = Vector<int, 4>{};
    REQUIRE(is_empty(rev));
    REQUIRE(is_empty(Range<int>{0, 0}));
    REQUIRE_FALSE(is_empty(Range<int>{0, 3}));
    REQUIRE_FALSE(is_empty(Repeat<int>{1}));
}