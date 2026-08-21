#include <catch2/catch_test_macros.hpp>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("Vector construction establishes logical size", "[vector]") {
  using cljonic::Vector;

  TRACE_ID("entity-fields.VectorCollection");
  TRACE_ID("invariant.VectorCollection.CapacityIsNonNegative");
  TRACE_ID("invariant.VectorCollection.LogicalSizeIsNonNegative");
  TRACE_ID("invariant.VectorCollection.LogicalSizeDoesNotExceedCapacity");
  TRACE_ID("invariant.VectorCollection.SupportsLiteralDeducedConstruction");
  TRACE_ID(
      "invariant.VectorCollection.SupportsEmptyExplicitCapacityConstruction");
  TRACE_ID(
      "invariant.VectorCollection.OversizedInitializerIsCompileTimeFailure");
  TRACE_ID("invariant.VectorCollection."
           "OversizedInitializerDiagnosticIdentifiesCapacity");

  constexpr Vector<int, 4> empty{};
  constexpr Vector<int, 4> populated{1, 2};
  constexpr Vector<int, 2> full{1, 2};

  STATIC_REQUIRE(empty.size() == 0U);
  STATIC_REQUIRE(populated.size() == 2U);
  STATIC_REQUIRE(full.size() == 2U);
  STATIC_REQUIRE(Vector<int, 4>::capacity() == 4U);
}

TEST_CASE("Vector indexed access handles valid and invalid indexes",
          "[vector][indexed-access]") {
  using cljonic::Vector;

  TRACE_ID("entity-fields.VectorCollection");
  TRACE_ID("invariant.VectorCollection.SupportsIndexedLookup");
  TRACE_ID("invariant.VectorCollection.SupportsIndexedFallbackLookup");
  TRACE_ID("invariant.VectorCollection.InvalidIndexReturnsDefaultElement");
  TRACE_ID("invariant.VectorCollection.InvalidIndexReturnsSuppliedFallback");

  constexpr Vector<int, 4> values{10, 20};

  STATIC_REQUIRE(values(0U) == 10);
  STATIC_REQUIRE(values(1U) == 20);
  STATIC_REQUIRE(values(2U) == 0);
  STATIC_REQUIRE(values(2U, 99) == 99);
}
