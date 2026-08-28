#include <catch2/catch_test_macros.hpp>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("Seq free function operations", "[seq]") {
    using cljonic::conj;
    using cljonic::count;
    using cljonic::Queue;
    using cljonic::seq;
    using cljonic::Set;
    using cljonic::Vector;

    TRACE_ID("entity-fields.Seq");
    TRACE_ID("invariant.Seq.SeqFreeFunctionSupported");
    TRACE_ID("invariant.Seq.ProducesOwningVector");
    TRACE_ID("invariant.Seq.NoHeapAllocation");
    TRACE_ID("invariant.Seq.NoRtti");
    TRACE_ID("invariant.Seq.NoExceptions");
    TRACE_ID("invariant.Seq.SingleThreadedExecutionModel");
    TRACE_ID("invariant.Seq.ReferentialTransparency");

    constexpr Vector<int, 4> v{1, 2, 3};
    constexpr auto sv = seq(v);
    STATIC_REQUIRE(count(sv) == 3U);

    constexpr Set<int, 4> s{};
    constexpr auto s1 = conj(s, 5);
    constexpr auto ss = seq(s1);
    STATIC_REQUIRE(count(ss) == 1U);

    // Runtime tests for code coverage instrumentation
    volatile int v1_raw = 99;
    int v1 = v1_raw;
    auto rv = Vector<int, 4>{v1};
    auto rsv = seq(rv);
    REQUIRE(count(rsv) == 1U);
}