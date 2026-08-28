#include <catch2/catch_test_macros.hpp>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("IsEmpty free function operations", "[is_empty]") {
  using cljonic::conj;
  using cljonic::is_empty;
  using cljonic::Queue;
  using cljonic::Vector;

  TRACE_ID("entity-fields.IsEmpty");
  TRACE_ID("invariant.IsEmpty.IsEmptyFreeFunctionSupported");
  TRACE_ID("invariant.IsEmpty.ReturnsBooleanPredicate");
  TRACE_ID("invariant.IsEmpty.NoHeapAllocation");
  TRACE_ID("invariant.IsEmpty.NoRtti");
  TRACE_ID("invariant.IsEmpty.NoExceptions");
  TRACE_ID("invariant.IsEmpty.SingleThreadedExecutionModel");
  TRACE_ID("invariant.IsEmpty.ReferentialTransparency");

  constexpr Vector<int, 4> e{};
  constexpr Vector<int, 4> v{1};
  STATIC_REQUIRE(is_empty(e));
  STATIC_REQUIRE_FALSE(is_empty(v));

  constexpr Queue<int, 4> q{};
  constexpr auto q1 = conj(q, 5);
  STATIC_REQUIRE(is_empty(q));
  STATIC_REQUIRE_FALSE(is_empty(q1));

  // Runtime tests for code coverage instrumentation
  auto rv = Vector<int, 4>{9};
  REQUIRE_FALSE(is_empty(rv));
  auto rev = Vector<int, 4>{};
  REQUIRE(is_empty(rev));
}