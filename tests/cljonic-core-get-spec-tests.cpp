#include <catch2/catch_test_macros.hpp>

#include <cljonic-core.hpp>

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("get returns default or fallback for invalid Vector indexes",
          "[get][vector]") {
  using cljonic::Vector;

  TRACE_ID("entity-fields.GetAccess");
  TRACE_ID("invariant.GetAccess.GetAccessRequiresIndexedVectorSubject");
  TRACE_ID("invariant.GetAccess.SupportsDefaultElementAccess");
  TRACE_ID("invariant.GetAccess.SupportsFallbackAccess");
  TRACE_ID("invariant.GetAccess.DefaultValueMatchesCollectionDefaultElement");
  TRACE_ID(
      "invariant.GetAccess.FallbackValueReplacesDefaultValueOnlyWhenInvalid");
  TRACE_ID("invariant.GetAccess.ValidIndexProbeIsAuthoritative");
  TRACE_ID("invariant.GetAccess.NegativeIndexIsInvalid");
  TRACE_ID("invariant.GetAccess.AccessIsNonThrowing");
  TRACE_ID("invariant.GetAccess.AccessIsNonAllocating");
  TRACE_ID("surface-actor.GetSurface");
  TRACE_ID("surface-provides.GetSurface");

  constexpr Vector<int, 4> values{10, 20};

  STATIC_REQUIRE(values(0U) == 10);
  STATIC_REQUIRE(values(1U) == 20);
  STATIC_REQUIRE(values(2U) == 0);
  STATIC_REQUIRE(values(2U, 99) == 99);
  STATIC_REQUIRE(values(-1) == 0);

  CHECK(values(2U) == 0);
  CHECK(values(2U, 99) == 99);
  CHECK(values(-1) == 0);
}
