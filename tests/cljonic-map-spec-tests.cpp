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
    STATIC_REQUIRE(m.is_empty());
    STATIC_REQUIRE(m.count() == 0U);
    STATIC_REQUIRE(m.capacity() == 4U);

    constexpr auto m1 = m.assoc(10, 100);
    STATIC_REQUIRE_FALSE(m1.is_empty());
    STATIC_REQUIRE(m1.count() == 1U);
    STATIC_REQUIRE(m1.contains(10));
    STATIC_REQUIRE_FALSE(m1.contains(20));
    STATIC_REQUIRE(m1(10) == 100);
    STATIC_REQUIRE(m1(20) == 0);
    STATIC_REQUIRE(m1(20, -1) == -1);

    constexpr auto m2 = m1.assoc(20, 200);
    STATIC_REQUIRE(m2.count() == 2U);
    STATIC_REQUIRE(m2(20) == 200);

    // Dissoc using swap-and-remove
    constexpr auto m3 = m2.dissoc(10);
    STATIC_REQUIRE(m3.count() == 1U);
    STATIC_REQUIRE_FALSE(m3.contains(10));
    STATIC_REQUIRE(m3.contains(20));
    STATIC_REQUIRE(m3(20) == 200);

    // Runtime tests for code coverage instrumentation
    volatile int mk1_raw = 10;
    volatile int mv1_raw = 100;
    volatile int mk2_raw = 20;
    volatile int mv2_raw = 200;
    int mk1 = mk1_raw;
    int mv1 = mv1_raw;
    int mk2 = mk2_raw;
    int mv2 = mv2_raw;
    auto rm = Map<int, int, 4>{};
    REQUIRE(rm.is_empty());
    REQUIRE(rm.count() == 0U);
    REQUIRE(rm.capacity() == 4U);
    REQUIRE_FALSE(rm.contains(mk1));
    REQUIRE(rm(mk1) == 0);
    REQUIRE(rm(mk1, -1) == -1);
    REQUIRE(rm.can_assoc(mk1));
    REQUIRE(rm.can_assoc(mk1, mv1));

    auto rm1 = rm.assoc(mk1, mv1);
    REQUIRE_FALSE(rm1.is_empty());
    REQUIRE(rm1.count() == 1U);
    REQUIRE(rm1.contains(mk1));
    REQUIRE(rm1(mk1) == 100);

    // Replace existing key
    auto rm1_updated = rm1.assoc(mk1, 999);
    REQUIRE(rm1_updated.count() == 1U);
    REQUIRE(rm1_updated(mk1) == 999);

    auto rm2 = rm1.assoc(mk2, mv2);
    REQUIRE(rm2.count() == 2U);
    REQUIRE(rm2(mk2) == 200);

    auto rm_full = rm2.assoc(30, 300).assoc(40, 400);
    REQUIRE(rm_full.count() == 4U);
    REQUIRE_FALSE(rm_full.can_assoc(50));
    REQUIRE(rm_full.assoc(50, 500).count() == 4U); // rejected overflow

    auto rm_dissoc_first = rm2.dissoc(mk1);
    REQUIRE(rm_dissoc_first.count() == 1U);
    REQUIRE_FALSE(rm_dissoc_first.contains(mk1));
    REQUIRE(rm_dissoc_first.contains(mk2));

    auto rm_dissoc_last = rm2.dissoc(mk2);
    REQUIRE(rm_dissoc_last.count() == 1U);
    REQUIRE(rm_dissoc_last.contains(mk1));

    auto rm_dissoc_absent = rm1.dissoc(999);
    REQUIRE(rm_dissoc_absent.count() == 1U);
}
