#include <catch2/catch_test_macros.hpp>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("Vector construction establishes logical size", "[vector]")
{
  using cljonic::Vector;

  TRACE_ID("entity-fields.Vector");
  TRACE_ID("invariant.Vector.CapacityIsNonNegative");
  TRACE_ID("invariant.Vector.LogicalSizeIsNonNegative");
  TRACE_ID("invariant.Vector.LogicalSizeDoesNotExceedCapacity");
  TRACE_ID("invariant.Vector.HeaderOnlyDistribution");
  TRACE_ID("invariant.Vector.StaticOrAutomaticStorageOnly");
  TRACE_ID("invariant.Vector.NoHeapAllocation");
  TRACE_ID("invariant.Vector.NoRtti");
  TRACE_ID("invariant.Vector.NoExceptions");
  TRACE_ID("invariant.Vector.SingleThreadedExecutionModel");
  TRACE_ID("invariant.Vector.ImmutablePublicApi");
  TRACE_ID("invariant.Vector.UpdateReturnsNewValue");
  TRACE_ID("invariant.Vector.DeepCopyUpdate");
  TRACE_ID("invariant.Vector.ReferentialTransparency");
  TRACE_ID("invariant.Vector.RequiresValueSemanticElements");
  TRACE_ID("invariant.Vector.SupportsLiteralDeducedConstruction");
  TRACE_ID("invariant.Vector.SupportsEmptyExplicitCapacityConstruction");
  TRACE_ID("invariant.Vector.SupportsExplicitCapacityConstruction");
  TRACE_ID("invariant.Vector.SupportsCapacityInferredLiteralEquivalentSemantics");
  TRACE_ID("invariant.Vector.OversizedInitializerIsCompileTimeFailure");
  TRACE_ID("invariant.Vector.OversizedInitializerDiagnosticIdentifiesCapacity");
  TRACE_ID("invariant.Vector.CapacityExceedsMaximumIsCompileTimeFailure");

  constexpr Vector<int, 4> empty{};
  constexpr Vector<int, 4> populated{1, 2};
  constexpr Vector<int, 2> full{1, 2};

  STATIC_REQUIRE(empty.size() == 0U);
  STATIC_REQUIRE(populated.size() == 2U);
  STATIC_REQUIRE(full.size() == 2U);
  STATIC_REQUIRE(Vector<int, 4>::capacity() == 4U);

  const auto runtime_values = Vector<int, 4>{1, 2};
  CHECK(runtime_values.size() == 2U);
  CHECK(runtime_values.capacity() == 4U);
}

TEST_CASE("Vector indexed access handles valid and invalid indexes",
          "[vector][indexed-access]")
{
  using cljonic::Vector;

  TRACE_ID("entity-fields.Vector");
  TRACE_ID("invariant.Vector.SupportsIndexedLookup");
  TRACE_ID("invariant.Vector.SupportsIndexedFallbackLookup");
  TRACE_ID("invariant.Vector.InvalidIndexReturnsDefaultElement");
  TRACE_ID("invariant.Vector.InvalidIndexReturnsSuppliedFallback");

  constexpr Vector<int, 4> values{10, 20};

  STATIC_REQUIRE(values(0U) == 10);
  STATIC_REQUIRE(values(1U) == 20);
  STATIC_REQUIRE(values(2U) == 0);
  STATIC_REQUIRE(values(2U, 99) == 99);

  const auto runtime_values = Vector<int, 4>{10, 20};
  CHECK(runtime_values(0U) == 10);
  CHECK(runtime_values(1U) == 20);
  CHECK(runtime_values(2U) == 0);
}
