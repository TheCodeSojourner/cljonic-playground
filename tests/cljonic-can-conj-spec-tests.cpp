#include <catch2/catch_test_macros.hpp>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("CanConj free function operations", "[can_conj]") {
    using cljonic::can_conj;
    using cljonic::Queue;
    using cljonic::Set;

    TRACE_ID("entity-fields.CanConj");
    TRACE_ID("invariant.CanConj.CanConjPreflightSupported");
    TRACE_ID("invariant.CanConj.NoHeapAllocation");
    TRACE_ID("invariant.CanConj.NoRtti");
    TRACE_ID("invariant.CanConj.NoExceptions");
    TRACE_ID("invariant.CanConj.SingleThreadedExecutionModel");
    TRACE_ID("invariant.CanConj.ReferentialTransparency");

    constexpr Queue<int, 4> q0{};
    STATIC_REQUIRE(can_conj(q0));

    constexpr Set<int, 4> s0{};
    STATIC_REQUIRE(can_conj(s0, 1));

    // Runtime tests for code coverage instrumentation
    volatile int v_raw = 10;
    int v = v_raw;
    auto rq = Queue<int, 4>{};
    REQUIRE(can_conj(rq));
    auto rs = Set<int, 4>{};
    REQUIRE(can_conj(rs, v));
}
