#include <catch2/catch_test_macros.hpp>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("Pop free function operations", "[pop]") {
    using cljonic::conj;
    using cljonic::peek;
    using cljonic::pop;
    using cljonic::Queue;

    TRACE_ID("entity-fields.Pop");
    TRACE_ID("invariant.Pop.PopFreeFunctionSupported");
    TRACE_ID("invariant.Pop.NoHeapAllocation");
    TRACE_ID("invariant.Pop.NoRtti");
    TRACE_ID("invariant.Pop.NoExceptions");
    TRACE_ID("invariant.Pop.SingleThreadedExecutionModel");
    TRACE_ID("invariant.Pop.FunctionalImmutabilityPreserved");
    TRACE_ID("invariant.Pop.ReferentialTransparency");
    TRACE_ID("invariant.Pop.CopyOnModifySemantics");

    constexpr Queue<int, 4> q0{};
    constexpr auto q1 = conj(conj(q0, 1), 2);
    constexpr auto q2 = pop(q1);
    STATIC_REQUIRE(peek(q2) == 2);
    STATIC_REQUIRE(peek(q1) == 1);

    // Runtime tests for code coverage instrumentation
    volatile int v1_raw = 1;
    volatile int v2_raw = 2;
    int v1 = v1_raw;
    int v2 = v2_raw;
    auto rq0 = Queue<int, 4>{};
    auto rq1 = conj(conj(rq0, v1), v2);
    auto rq2 = pop(rq1);
    REQUIRE(peek(rq2) == 2);
    REQUIRE(peek(rq1) == 1);
}
