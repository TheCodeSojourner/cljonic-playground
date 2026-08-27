#include <catch2/catch_test_macros.hpp>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("Peek free function operations", "[peek]") {
  using cljonic::conj;
  using cljonic::peek;
  using cljonic::Queue;

  TRACE_ID("entity-fields.Peek");
  TRACE_ID("invariant.Peek.PeekFreeFunctionSupported");
  TRACE_ID("invariant.Peek.NoHeapAllocation");
  TRACE_ID("invariant.Peek.NoRtti");
  TRACE_ID("invariant.Peek.NoExceptions");
  TRACE_ID("invariant.Peek.SingleThreadedExecutionModel");
  TRACE_ID("invariant.Peek.ReferentialTransparency");

  constexpr Queue<int, 4> q0{};
  constexpr auto q1 = conj(q0, 99);
  STATIC_REQUIRE(peek(q1) == 99);
}
