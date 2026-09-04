#include <catch2/catch_test_macros.hpp>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("Next free function operations", "[next]") {
    using cljonic::conj;
    using cljonic::next;
    using cljonic::peek;
    using cljonic::Queue;

    TRACE_ID("entity-fields.Next");
    TRACE_ID("invariant.Next.NextFreeFunctionSupported");
    TRACE_ID("invariant.Next.NoHeapAllocation");
    TRACE_ID("invariant.Next.NoRtti");
    TRACE_ID("invariant.Next.NoExceptions");
    TRACE_ID("invariant.Next.SingleThreadedExecutionModel");
    TRACE_ID("invariant.Next.ReferentialTransparency");

    constexpr Queue<int, 4> q{};
    constexpr auto q1 = conj(conj(conj(q, 1), 2), 3);
    constexpr auto q2 = next(q1);
    STATIC_REQUIRE(peek(q2) == 2);
    STATIC_REQUIRE(q1.count() == 3U); // input preserved

    // Runtime tests for code coverage instrumentation
    volatile int v1_raw = 1;
    volatile int v2_raw = 2;
    int v1 = v1_raw;
    int v2 = v2_raw;
    auto rq = Queue<int, 4>{};
    auto rq1 = conj(conj(rq, v1), v2);
    auto rq2 = next(rq1);
    REQUIRE(peek(rq2) == 2);
}