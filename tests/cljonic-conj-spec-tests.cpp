#include <catch2/catch_test_macros.hpp>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("Conj free function operations", "[conj]") {
    using cljonic::conj;
    using cljonic::Queue;
    using cljonic::Set;
    using cljonic::Vector;

    TRACE_ID("entity-fields.Conj");
    TRACE_ID("invariant.Conj.ConjFreeFunctionSupported");
    TRACE_ID("invariant.Conj.NoHeapAllocation");
    TRACE_ID("invariant.Conj.NoRtti");
    TRACE_ID("invariant.Conj.NoExceptions");
    TRACE_ID("invariant.Conj.SingleThreadedExecutionModel");
    TRACE_ID("invariant.Conj.FunctionalImmutabilityPreserved");
    TRACE_ID("invariant.Conj.ReferentialTransparency");
    TRACE_ID("invariant.Conj.CopyOnModifySemantics");

    constexpr Queue<int, 4> q0{};
    constexpr auto q1 = conj(q0, 10);
    STATIC_REQUIRE(q1.count() == 1U);
    STATIC_REQUIRE(q0.count() == 0U);

    constexpr Set<int, 4> s0{};
    constexpr auto s1 = conj(s0, 1);
    STATIC_REQUIRE(s1.count() == 1U);
    STATIC_REQUIRE(s0.count() == 0U);

    // Runtime tests for code coverage instrumentation
    volatile int qv_raw = 55;
    volatile int sv_raw = 66;
    int qv = qv_raw;
    int sv = sv_raw;
    auto rq = Queue<int, 4>{};
    auto rq1 = conj(rq, qv);
    REQUIRE(rq1.count() == 1U);
    REQUIRE(rq.count() == 0U);

    auto rs = Set<int, 4>{};
    auto rs1 = conj(rs, sv);
    REQUIRE(rs1.count() == 1U);
    REQUIRE(rs.count() == 0U);
}
