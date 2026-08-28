#include <catch2/catch_test_macros.hpp>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("Get free function operations", "[get]") {
  using cljonic::assoc;
  using cljonic::conj;
  using cljonic::get;
  using cljonic::Map;
  using cljonic::Set;
  using cljonic::Vector;

  TRACE_ID("entity-fields.Get");
  TRACE_ID("invariant.Get.GetFreeFunctionSupported");
  TRACE_ID("invariant.Get.SupportsFallbackArgument");
  TRACE_ID("invariant.Get.NoHeapAllocation");
  TRACE_ID("invariant.Get.NoRtti");
  TRACE_ID("invariant.Get.NoExceptions");
  TRACE_ID("invariant.Get.SingleThreadedExecutionModel");
  TRACE_ID("invariant.Get.ReferentialTransparency");

  // Vector indexed lookup + fallback
  constexpr Vector<int, 4> v{10, 20};
  STATIC_REQUIRE(get(v, 0U) == 10);
  STATIC_REQUIRE(get(v, 9U) == 0);      // default element
  STATIC_REQUIRE(get(v, 9U, -1) == -1); // supplied fallback

  // Map key lookup + fallback
  constexpr Map<int, int, 4> m{};
  constexpr auto m1 = assoc(m, 1, 100);
  STATIC_REQUIRE(get(m1, 1) == 100);
  STATIC_REQUIRE(get(m1, 2) == 0);
  STATIC_REQUIRE(get(m1, 2, -1) == -1);

  // Set value lookup + fallback
  constexpr Set<int, 4> s{};
  constexpr auto s1 = conj(s, 5);
  STATIC_REQUIRE(get(s1, 5) == 5);
  STATIC_REQUIRE(get(s1, 8) == 0);
  STATIC_REQUIRE(get(s1, 8, -1) == -1);

  // Runtime tests for code coverage instrumentation
  volatile int idx_raw = 0;
  int idx = idx_raw;
  auto rv = Vector<int, 4>{10, 20};
  REQUIRE(get(rv, idx) == 10);
  REQUIRE(get(rv, 99U, -1) == -1);

  auto rm = Map<int, int, 4>{};
  auto rm1 = assoc(rm, 1, 100);
  REQUIRE(get(rm1, 1) == 100);
  REQUIRE(get(rm1, 2, -1) == -1);

  auto rs = Set<int, 4>{};
  auto rs1 = conj(rs, 5);
  REQUIRE(get(rs1, 5) == 5);
  REQUIRE(get(rs1, 8, -1) == -1);
}