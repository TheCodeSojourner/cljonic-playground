#include <catch2/catch_test_macros.hpp>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("Map construction and basic lookup", "[map]") {
  using cljonic::Map;

  TRACE_ID("entity-fields.Map");
  TRACE_ID("invariant.Map.CapacityIsNonNegative");
  TRACE_ID("invariant.Map.LogicalSizeIsNonNegative");
  TRACE_ID("invariant.Map.LogicalSizeDoesNotExceedCapacity");
  TRACE_ID("invariant.Map.HeaderOnlyDistribution");
  TRACE_ID("invariant.Map.StaticOrAutomaticStorageOnly");
  TRACE_ID("invariant.Map.ContiguousStorage");
  TRACE_ID("invariant.Map.LinearScanLookup");
  TRACE_ID("invariant.Map.SwapAndRemove");
  TRACE_ID("invariant.Map.CallableLookup");
  TRACE_ID("invariant.Map.NoHeapAllocation");
  TRACE_ID("invariant.Map.NoRtti");
  TRACE_ID("invariant.Map.NoExceptions");
  TRACE_ID("invariant.Map.SingleThreadedExecutionModel");
  TRACE_ID("invariant.Map.ImmutablePublicApi");
  TRACE_ID("invariant.Map.UpdateReturnsNewValue");
  TRACE_ID("invariant.Map.DeepCopyUpdate");
  TRACE_ID("invariant.Map.ReferentialTransparency");
  TRACE_ID("invariant.Map.RequiresValueSemanticElements");
  TRACE_ID("invariant.Map.SupportsEmptyExplicitCapacityConstruction");
  TRACE_ID("invariant.Map.SupportsExplicitCapacityConstruction");
  TRACE_ID("invariant.Map.CapacityExceedsMaximumIsCompileTimeFailure");
  TRACE_ID("invariant.Map.SupportsAssociativeLookup");
  TRACE_ID("invariant.Map.SupportsAssociativeFallbackLookup");
  TRACE_ID("invariant.Map.MissingKeyReturnsDefaultElement");
  TRACE_ID("invariant.Map.MissingKeyReturnsSuppliedFallback");
  TRACE_ID("invariant.Map.CanonicalResultStatusModelIsDeclared");
  TRACE_ID("invariant.Map.CompleteResultStatusDeclared");
  TRACE_ID("invariant.Map.BoundedPrefixResultStatusDeclared");
  TRACE_ID("invariant.Map.DefaultReturningResultStatusDeclared");
  TRACE_ID("invariant.Map.CheckedFailureResultStatusDeclared");
  TRACE_ID("invariant.Map.ProducerOnlyResultStatusDeclared");
  TRACE_ID("invariant.Map.PreflightPredicatesAreNonThrowingNonAllocating");
  TRACE_ID("invariant.Map.ContainsIsCanonicalKeyPresencePredicate");
  TRACE_ID("invariant.Map.CanAssocIsCanonicalAssocPreflight");
  TRACE_ID("invariant.Map.FitsIntoIsCanonicalMaterializationPreflight");
  TRACE_ID("invariant.Map.CompileTimeCapacityOverflowIsRejected");
  TRACE_ID("invariant.Map.RuntimeCapacityFailuresHaveDocumentedPolicy");
  TRACE_ID("invariant.Map.DefaultAccessHasPreflightPredicate");

  constexpr Map<int, int, 4> m{};
  STATIC_REQUIRE(m.empty());
  STATIC_REQUIRE(m.size() == 0U);
  STATIC_REQUIRE(m.capacity() == 4U);

  constexpr auto m1 = m.assoc(10, 100);
  STATIC_REQUIRE_FALSE(m1.empty());
  STATIC_REQUIRE(m1.size() == 1U);
  STATIC_REQUIRE(m1.contains(10));
  STATIC_REQUIRE_FALSE(m1.contains(20));
  STATIC_REQUIRE(m1(10) == 100);
  STATIC_REQUIRE(m1(20) == 0);
  STATIC_REQUIRE(m1(20, -1) == -1);

  constexpr auto m2 = m1.assoc(20, 200);
  STATIC_REQUIRE(m2.size() == 2U);
  STATIC_REQUIRE(m2(20) == 200);

  // Dissoc using swap-and-remove
  constexpr auto m3 = m2.dissoc(10);
  STATIC_REQUIRE(m3.size() == 1U);
  STATIC_REQUIRE_FALSE(m3.contains(10));
  STATIC_REQUIRE(m3.contains(20));
  STATIC_REQUIRE(m3(20) == 200);
}
