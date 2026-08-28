#include <catch2/catch_test_macros.hpp>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("Rest free function operations", "[rest]") {
  using cljonic::conj;
  using cljonic::peek;
  using cljonic::Queue;
  using cljonic::rest;

  TRACE_ID("entity-fields.Rest");
  TRACE_ID("invariant.Rest.RestFreeFunctionSupported");
  TRACE_ID("invariant.Rest.NoHeapAllocation");
  TRACE_ID("invariant.Rest.NoRtti");
  TRACE_ID("invariant.Rest.NoExceptions");
  TRACE_ID("invariant.Rest.SingleThreadedExecutionModel");
  TRACE_ID("invariant.Rest.ReferentialTransparency");

  constexpr Queue<int, 4> r0{};
  constexpr auto r1 = conj(conj(r0, 1), 2);
  constexpr auto r2 = rest(r1);
  STATIC_REQUIRE(peek(r2) == 2);
  STATIC_REQUIRE(r1.size() == 2U);

  constexpr auto r3 = rest(r2);
  STATIC_REQUIRE(r3.size() == 0U);

  // Runtime tests for code coverage instrumentation
  volatile int v1_raw = 10;
  int v1 = v1_raw;
  auto rq = Queue<int, 4>{};
  auto rq1 = conj(rq, v1);
  auto rq2 = rest(rq1);
  REQUIRE(rq2.size() == 0U);
}