#include <catch2/catch_test_macros.hpp>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("Count free function operations", "[count]") {
  using cljonic::assoc;
  using cljonic::conj;
  using cljonic::count;
  using cljonic::Map;
  using cljonic::Queue;
  using cljonic::Set;
  using cljonic::Vector;

  TRACE_ID("entity-fields.Count");
  TRACE_ID("invariant.Count.CountFreeFunctionSupported");
  TRACE_ID("invariant.Count.NoHeapAllocation");
  TRACE_ID("invariant.Count.NoRtti");
  TRACE_ID("invariant.Count.NoExceptions");
  TRACE_ID("invariant.Count.SingleThreadedExecutionModel");
  TRACE_ID("invariant.Count.ReferentialTransparency");
  TRACE_ID("invariant.Count.ReturnsLogicalSize");

  constexpr Vector<int, 4> v{1, 2, 3};
  STATIC_REQUIRE(count(v) == 3U);

  constexpr Set<int, 4> s{};
  constexpr auto s1 = conj(s, 10);
  STATIC_REQUIRE(count(s1) == 1U);

  constexpr Map<int, int, 4> m{};
  constexpr auto m1 = assoc(m, 1, 2);
  STATIC_REQUIRE(count(m1) == 1U);

  constexpr Queue<int, 4> q{};
  constexpr auto q1 = conj(q, 7);
  STATIC_REQUIRE(count(q1) == 1U);

  STATIC_REQUIRE(count(s) == 0U); // empty input
  STATIC_REQUIRE(count(m) == 0U);
  STATIC_REQUIRE(count(q) == 0U);
  // Runtime tests for code coverage instrumentation
  volatile int val_raw = 42;
  int val = val_raw;
  auto rv = Vector<int, 4>{1, 2};
  REQUIRE(count(rv) == 2U);
  auto rs = Set<int, 4>{};
  REQUIRE(count(conj(rs, val)) == 1U);
}