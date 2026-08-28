#include <catch2/catch_test_macros.hpp>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("NotEmpty free function operations", "[not_empty]") {
    using cljonic::count;
    using cljonic::not_empty;
    using cljonic::Vector;

    TRACE_ID("entity-fields.NotEmpty");
    TRACE_ID("invariant.NotEmpty.NotEmptyFreeFunctionSupported");
    TRACE_ID("invariant.NotEmpty.PreservesInput");
    TRACE_ID("invariant.NotEmpty.ReturnsOwningCopy");
    TRACE_ID("invariant.NotEmpty.NoHeapAllocation");
    TRACE_ID("invariant.NotEmpty.NoRtti");
    TRACE_ID("invariant.NotEmpty.NoExceptions");
    TRACE_ID("invariant.NotEmpty.SingleThreadedExecutionModel");
    TRACE_ID("invariant.NotEmpty.ReferentialTransparency");

    constexpr Vector<int, 4> v{1, 2};
    constexpr auto nv = not_empty(v);
    STATIC_REQUIRE(count(nv) == 2U); // owning copy returned
    STATIC_REQUIRE(count(v) == 2U);  // input preserved

    constexpr Vector<int, 4> e{};
    constexpr auto ne = not_empty(e);
    STATIC_REQUIRE(count(ne) == 0U); // empty value returned

    // Runtime tests for code coverage instrumentation
    volatile int v1 = 5;
    auto rv = Vector<int, 4>{v1};
    REQUIRE(count(not_empty(rv)) == 1U);
    auto re = Vector<int, 4>{};
    REQUIRE(count(not_empty(re)) == 0U);
}