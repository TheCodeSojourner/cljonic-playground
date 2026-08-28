#include <catch2/catch_test_macros.hpp>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("First free function operations", "[first]") {
    using cljonic::first;
    using cljonic::MapEntry;
    using cljonic::Vector;

    TRACE_ID("entity-fields.First");
    TRACE_ID("invariant.First.FirstFreeFunctionSupported");
    TRACE_ID("invariant.First.NoHeapAllocation");
    TRACE_ID("invariant.First.NoRtti");
    TRACE_ID("invariant.First.NoExceptions");
    TRACE_ID("invariant.First.SingleThreadedExecutionModel");
    TRACE_ID("invariant.First.ReferentialTransparency");

    constexpr Vector<int, 4> v{10, 20, 30};
    STATIC_REQUIRE(first(v) == 10);

    constexpr MapEntry<int, int> e{1, 100};
    STATIC_REQUIRE(first(e) == 1);

    // Runtime tests for code coverage instrumentation
    volatile int v1 = 42;
    auto rv = Vector<int, 4>{v1, 20};
    REQUIRE(first(rv) == 42);
    auto me = MapEntry<int, int>{7, 77};
    REQUIRE(first(me) == 7);
}