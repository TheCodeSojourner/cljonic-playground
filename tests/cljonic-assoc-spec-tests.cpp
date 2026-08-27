#include <catch2/catch_test_macros.hpp>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("Assoc free function operations", "[assoc]") {
  using cljonic::assoc;
  using cljonic::Map;

  TRACE_ID("entity-fields.Assoc");
  TRACE_ID("invariant.Assoc.AssocFreeFunctionSupported");
  TRACE_ID("invariant.Assoc.NoHeapAllocation");
  TRACE_ID("invariant.Assoc.NoRtti");
  TRACE_ID("invariant.Assoc.NoExceptions");
  TRACE_ID("invariant.Assoc.SingleThreadedExecutionModel");
  TRACE_ID("invariant.Assoc.FunctionalImmutabilityPreserved");
  TRACE_ID("invariant.Assoc.ReferentialTransparency");
  TRACE_ID("invariant.Assoc.CopyOnModifySemantics");

  constexpr Map<int, int, 4> m0{};
  constexpr auto m1 = assoc(m0, 1, 100);
  STATIC_REQUIRE(m1.contains(1));
  STATIC_REQUIRE(m1(1) == 100);
  STATIC_REQUIRE(m0.size() == 0U);
}
