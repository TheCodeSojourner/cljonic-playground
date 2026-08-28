#include <catch2/catch_test_macros.hpp>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("Dissoc free function operations", "[dissoc]") {
  using cljonic::assoc;
  using cljonic::dissoc;
  using cljonic::Map;

  TRACE_ID("entity-fields.Dissoc");
  TRACE_ID("invariant.Dissoc.DissocFreeFunctionSupported");
  TRACE_ID("invariant.Dissoc.NoHeapAllocation");
  TRACE_ID("invariant.Dissoc.NoRtti");
  TRACE_ID("invariant.Dissoc.NoExceptions");
  TRACE_ID("invariant.Dissoc.SingleThreadedExecutionModel");
  TRACE_ID("invariant.Dissoc.FunctionalImmutabilityPreserved");
  TRACE_ID("invariant.Dissoc.ReferentialTransparency");
  TRACE_ID("invariant.Dissoc.CopyOnModifySemantics");

  constexpr Map<int, int, 4> m0{};
  constexpr auto m1 = assoc(m0, 1, 100);
  constexpr auto m2 = dissoc(m1, 1);
  STATIC_REQUIRE_FALSE(m2.contains(1));
  STATIC_REQUIRE(m1.contains(1));

  // Runtime tests for code coverage instrumentation
  volatile int k_raw = 10;
  int k = k_raw;
  auto rm = Map<int, int, 4>{};
  auto rm1 = assoc(rm, k, 100);
  auto rm2 = dissoc(rm1, k);
  REQUIRE_FALSE(rm2.contains(k));
  REQUIRE(rm1.contains(k));
}
