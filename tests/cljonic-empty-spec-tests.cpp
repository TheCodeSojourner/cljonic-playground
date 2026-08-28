#include <catch2/catch_test_macros.hpp>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("Empty free function operations", "[empty]") {
  using cljonic::conj;
  using cljonic::empty;
  using cljonic::is_empty;
  using cljonic::Queue;
  using cljonic::Set;
  using cljonic::Vector;

  TRACE_ID("entity-fields.Empty");
  TRACE_ID("invariant.Empty.EmptyFreeFunctionSupported");
  TRACE_ID("invariant.Empty.ReturnsEmptySameType");
  TRACE_ID("invariant.Empty.NoHeapAllocation");
  TRACE_ID("invariant.Empty.NoRtti");
  TRACE_ID("invariant.Empty.NoExceptions");
  TRACE_ID("invariant.Empty.SingleThreadedExecutionModel");
  TRACE_ID("invariant.Empty.ReferentialTransparency");
  TRACE_ID("invariant.Empty.PreservesInput");

  constexpr Vector<int, 4> v{1, 2};
  constexpr auto ev = empty(v);
  STATIC_REQUIRE(is_empty(ev));
  STATIC_REQUIRE(ev.size() == 0U);

  constexpr Set<int, 4> s{};
  constexpr auto es = empty(s);
  STATIC_REQUIRE(is_empty(es));

  constexpr Queue<int, 4> q{};
  constexpr auto eq = empty(q);
  STATIC_REQUIRE(is_empty(eq));

  STATIC_REQUIRE(v.size() == 2U); // input preserved

  // Runtime tests for code coverage instrumentation
  auto rv = Vector<int, 4>{1, 2};
  auto rev = empty(rv);
  REQUIRE(rev.size() == 0U);
}