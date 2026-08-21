#include <catch2/catch_test_macros.hpp>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("count is available for a type with logical size", "[count]") {
  struct Collection {
    constexpr auto size() const noexcept -> std::size_t { return 3U; }
  } collection;

  STATIC_REQUIRE(cljonic::count(collection) == 3U);
}

TEST_CASE("count returns Vector logical cardinality", "[count][vector]") {
  using cljonic::count;
  using cljonic::Vector;

  TRACE_ID("entity-fields.CountRequest");
  TRACE_ID("entity-fields.Collection");
  TRACE_ID("invariant.CountRequest.CountRequiresCollectionSubject");
  TRACE_ID("invariant.Collection.LogicalSizeIsNonNegative");
  TRACE_ID("invariant.CountRequest.ResolvedCountRequiresLogicalSizeMatch");
  TRACE_ID("invariant.CountRequest.ResolvedCountRequiresEmptyReturnsZero");
  TRACE_ID("invariant.CountRequest.ResolvedCountRequiresNonNegativeResult");
  TRACE_ID("rule-failure.CountCollectionCardinality.1");
  TRACE_ID("rule-failure.CountCollectionCardinality.2");
  TRACE_ID("rule-failure.CountCollectionCardinality.3");
  TRACE_ID("rule-success.CountCollectionCardinality");
  TRACE_ID("surface-actor.CountSurface");
  TRACE_ID("surface-provides.CountSurface");

  constexpr Vector<int, 4> empty{};
  constexpr Vector<int, 4> populated{1, 2};
  constexpr Vector<int, 2> at_capacity{1, 2};

  STATIC_REQUIRE(count(empty) == 0U);
  STATIC_REQUIRE(count(populated) == 2U);
  STATIC_REQUIRE(count(at_capacity) == 2U);
  CHECK(count(empty) == 0U);
  CHECK(count(populated) == 2U);
  CHECK(count(at_capacity) == 2U);
}
