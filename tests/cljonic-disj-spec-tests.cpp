#include <catch2/catch_test_macros.hpp>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("Disj free function operations", "[disj]") {
  using cljonic::conj;
  using cljonic::disj;
  using cljonic::Set;

  TRACE_ID("entity-fields.Disj");
  TRACE_ID("invariant.Disj.DisjFreeFunctionSupported");
  TRACE_ID("invariant.Disj.NoHeapAllocation");
  TRACE_ID("invariant.Disj.NoRtti");
  TRACE_ID("invariant.Disj.NoExceptions");
  TRACE_ID("invariant.Disj.SingleThreadedExecutionModel");
  TRACE_ID("invariant.Disj.FunctionalImmutabilityPreserved");
  TRACE_ID("invariant.Disj.ReferentialTransparency");
  TRACE_ID("invariant.Disj.CopyOnModifySemantics");

  constexpr Set<int, 4> s0{};
  constexpr auto s1 = conj(s0, 42);
  constexpr auto s2 = disj(s1, 42);
  STATIC_REQUIRE_FALSE(s2.contains(42));
  STATIC_REQUIRE(s1.contains(42));
}
