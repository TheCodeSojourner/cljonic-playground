#include <catch2/catch_test_macros.hpp>

#include <cljonic-core.hpp>

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("valid_index probes Vector logical indexes", "[valid-index]") {
  using cljonic::valid_index;
  using cljonic::Vector;

  TRACE_ID("entity-fields.IndexedAccess");
  TRACE_ID("invariant.IndexedAccess.IndexedAccessRequiresVectorSubject");
  TRACE_ID("invariant.IndexedAccess.SupportsValidIndexProbe");
  TRACE_ID("invariant.IndexedAccess.IndexedAccessUsesLogicalSize");
  TRACE_ID("invariant.IndexedAccess.NegativeIntegralIndexIsInvalid");
  TRACE_ID("invariant.IndexedAccess.ValidIndexIsNonThrowing");
  TRACE_ID("invariant.IndexedAccess.ValidIndexIsNonAllocating");
  TRACE_ID("invariant.IndexedAccess.ValidIndexDoesNotInspectElement");
  TRACE_ID("surface-actor.IndexedAccessSurface");
  TRACE_ID("surface-provides.IndexedAccessSurface");

  constexpr Vector<int, 4> values{10, 20};

  STATIC_REQUIRE(valid_index(values, 0U));
  STATIC_REQUIRE(valid_index(values, 1U));
  STATIC_REQUIRE_FALSE(valid_index(values, 2U));
  STATIC_REQUIRE_FALSE(valid_index(values, -1));
  STATIC_REQUIRE(noexcept(valid_index(values, 0U)));
}
