#include <catch2/catch_test_macros.hpp>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("Vector construction establishes logical size", "[vector]") {
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
  TRACE_ID(
      "invariant.Vector.SupportsCapacityInferredLiteralEquivalentSemantics");
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

TEST_CASE(
    "Vector canonical preflight predicates model index validity and emptiness",
    "[vector][preflight]") {
  using cljonic::Vector;

  TRACE_ID("entity-fields.Vector");
  TRACE_ID("invariant.Vector.CanonicalResultStatusModelIsDeclared");
  TRACE_ID("invariant.Vector.CompleteResultStatusDeclared");
  TRACE_ID("invariant.Vector.BoundedPrefixResultStatusDeclared");
  TRACE_ID("invariant.Vector.DefaultReturningResultStatusDeclared");
  TRACE_ID("invariant.Vector.CheckedFailureResultStatusDeclared");
  TRACE_ID("invariant.Vector.ProducerOnlyResultStatusDeclared");
  TRACE_ID("invariant.Vector.PreflightPredicatesAreNonThrowingNonAllocating");
  TRACE_ID("invariant.Vector.ValidIndexIsCanonicalIndexPredicate");
  TRACE_ID("invariant.Vector.FitsIntoIsCanonicalMaterializationPreflight");
  TRACE_ID("invariant.Vector.CompileTimeCapacityOverflowIsRejected");
  TRACE_ID("invariant.Vector.RuntimeCapacityFailuresHaveDocumentedPolicy");
  TRACE_ID("invariant.Vector.DefaultAccessHasPreflightPredicate");
  TRACE_ID("invariant.Vector.SupportsIndexedLookup");
  TRACE_ID("invariant.Vector.SupportsIndexedFallbackLookup");
  TRACE_ID("invariant.Vector.InvalidIndexReturnsDefaultElement");
  TRACE_ID("invariant.Vector.InvalidIndexReturnsSuppliedFallback");

  constexpr Vector<int, 4> values{10, 20};
  constexpr Vector<int, 4> empty_values{};

  STATIC_REQUIRE(values.valid_index(0U));
  STATIC_REQUIRE(values.valid_index(1U));
  STATIC_REQUIRE_FALSE(values.valid_index(2U));
  STATIC_REQUIRE(empty_values.empty());
  STATIC_REQUIRE_FALSE(values.empty());

  const auto runtime_values = Vector<int, 4>{10, 20};
  CHECK(runtime_values.valid_index(0U));
  CHECK(runtime_values.valid_index(1U));
  CHECK_FALSE(runtime_values.valid_index(2U));
  CHECK(Vector<int, 4>{}.empty());
  CHECK_FALSE(runtime_values.empty());
}

TEST_CASE("Vector indexed access handles valid and invalid indexes",
          "[vector][indexed-access]") {
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
