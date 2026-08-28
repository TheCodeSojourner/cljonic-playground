#include <catch2/catch_test_macros.hpp>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("CanAssoc free function operations", "[can_assoc]") {
  using cljonic::can_assoc;
  using cljonic::Map;

  TRACE_ID("entity-fields.CanAssoc");
  TRACE_ID("invariant.CanAssoc.CanAssocPreflightSupported");
  TRACE_ID("invariant.CanAssoc.NoHeapAllocation");
  TRACE_ID("invariant.CanAssoc.NoRtti");
  TRACE_ID("invariant.CanAssoc.NoExceptions");
  TRACE_ID("invariant.CanAssoc.SingleThreadedExecutionModel");
  TRACE_ID("invariant.CanAssoc.ReferentialTransparency");

  constexpr Map<int, int, 4> m0{};
  STATIC_REQUIRE(can_assoc(m0, 1, 100));

  // Runtime tests for code coverage instrumentation
  volatile int k_raw = 1;
  volatile int v_raw = 100;
  int k = k_raw;
  int v = v_raw;
  auto rm = Map<int, int, 4>{};
  REQUIRE(can_assoc(rm, k, v));
}
