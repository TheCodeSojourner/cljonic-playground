#include <catch2/catch_test_macros.hpp>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("Disj free function operations", "[disj]") {
    using cljonic::contains;
    using cljonic::disj;
    using cljonic::equal;
    using cljonic::is_empty;
    using cljonic::Set;

    TRACE_ID("entity-fields.Disj");
    TRACE_ID("invariant.Disj.DisjFreeFunctionSupported");
    TRACE_ID("invariant.Disj.NoHeapAllocation");
    TRACE_ID("invariant.Disj.NoRtti");
    TRACE_ID("invariant.Disj.NoExceptions");
    TRACE_ID("invariant.Disj.SingleThreadedExecutionModel");
    TRACE_ID("invariant.Disj.FunctionalImmutabilityPreserved");
    TRACE_ID("invariant.Disj.ReferentialTransparency");
    TRACE_ID("invariant.Disj.CopyOnModifySemantics");
    TRACE_ID("invariant.Disj.ZeroOrMoreExactValues");
    TRACE_ID("invariant.Disj.RemovesValuesLeftToRight");
    TRACE_ID("invariant.Disj.ZeroValuesReturnUnchangedCopy");
    TRACE_ID("invariant.Disj.AbsentValuesAreNoOps");
    TRACE_ID("invariant.Disj.DuplicateValuesAreAllowed");
    TRACE_ID("invariant.Disj.UnsupportedDomainsAreRejected");

    constexpr auto s0 = Set{42};
    constexpr auto s1 = disj(s0, 42);
    STATIC_REQUIRE_FALSE(contains(s1, 42));
    STATIC_REQUIRE(contains(s0, 42));

    constexpr auto populated = Set{1, 2, 3};
    constexpr auto unchanged = disj(populated);
    constexpr auto removed = disj(populated, 1, 99, 2, 1);
    STATIC_REQUIRE(equal(unchanged, populated));
    STATIC_REQUIRE(is_empty(disj(removed, 3)));
    STATIC_REQUIRE(contains(removed, 3));
    STATIC_REQUIRE_FALSE(contains(removed, 1));
    STATIC_REQUIRE_FALSE(contains(removed, 2));

    // Runtime tests for code coverage instrumentation
    volatile int v_raw = 77;
    const int v = v_raw;
    const auto rs0 = Set{v};
    const auto rs1 = disj(rs0, v);
    REQUIRE_FALSE(contains(rs1, v));
    REQUIRE(contains(rs0, v));
}
