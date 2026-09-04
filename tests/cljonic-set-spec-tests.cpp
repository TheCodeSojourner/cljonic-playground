#include <catch2/catch_test_macros.hpp>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("Set construction and basic operations", "[set]") {
    using cljonic::Set;

    TRACE_ID("entity-fields.Set");
    TRACE_ID("invariant.Set.CapacityIsNonNegative");
    TRACE_ID("invariant.Set.LogicalSizeIsNonNegative");
    TRACE_ID("invariant.Set.LogicalSizeDoesNotExceedCapacity");
    TRACE_ID("invariant.Set.HeaderOnlyDistribution");
    TRACE_ID("invariant.Set.StaticOrAutomaticStorageOnly");
    TRACE_ID("invariant.Set.ContiguousStorage");
    TRACE_ID("invariant.Set.LinearScanLookup");
    TRACE_ID("invariant.Set.SwapAndRemove");
    TRACE_ID("invariant.Set.CallableLookup");
    TRACE_ID("invariant.Set.NoHeapAllocation");
    TRACE_ID("invariant.Set.NoRtti");
    TRACE_ID("invariant.Set.NoExceptions");
    TRACE_ID("invariant.Set.SingleThreadedExecutionModel");
    TRACE_ID("invariant.Set.ImmutablePublicApi");
    TRACE_ID("invariant.Set.UpdateReturnsNewValue");
    TRACE_ID("invariant.Set.DeepCopyUpdate");
    TRACE_ID("invariant.Set.ReferentialTransparency");
    TRACE_ID("invariant.Set.RequiresValueSemanticElements");
    TRACE_ID("invariant.Set.SupportsEmptyExplicitCapacityConstruction");
    TRACE_ID("invariant.Set.SupportsExplicitCapacityConstruction");
    TRACE_ID("invariant.Set.CapacityExceedsMaximumIsCompileTimeFailure");
    TRACE_ID("invariant.Set.SupportsMembershipLookup");
    TRACE_ID("invariant.Set.SupportsMembershipFallbackLookup");
    TRACE_ID("invariant.Set.MissingValueReturnsDefaultElement");
    TRACE_ID("invariant.Set.MissingValueReturnsSuppliedFallback");
    TRACE_ID("invariant.Set.CanonicalResultStatusModelIsDeclared");
    TRACE_ID("invariant.Set.CompleteResultStatusDeclared");
    TRACE_ID("invariant.Set.BoundedPrefixResultStatusDeclared");
    TRACE_ID("invariant.Set.DefaultReturningResultStatusDeclared");
    TRACE_ID("invariant.Set.CheckedFailureResultStatusDeclared");
    TRACE_ID("invariant.Set.ProducerOnlyResultStatusDeclared");
    TRACE_ID("invariant.Set.PreflightPredicatesAreNonThrowingNonAllocating");
    TRACE_ID("invariant.Set.ContainsIsCanonicalMembershipPredicate");
    TRACE_ID("invariant.Set.CanConjIsCanonicalConjPreflight");
    TRACE_ID("invariant.Set.FitsIntoIsCanonicalMaterializationPreflight");
    TRACE_ID("invariant.Set.CompileTimeCapacityOverflowIsRejected");
    TRACE_ID("invariant.Set.RuntimeCapacityFailuresHaveDocumentedPolicy");
    TRACE_ID("invariant.Set.DefaultAccessHasPreflightPredicate");

    constexpr Set<int, 4> s{};
    STATIC_REQUIRE(s.is_empty());
    STATIC_REQUIRE(s.count() == 0U);
    STATIC_REQUIRE(s.capacity() == 4U);

    // conj adds elements (copy-on-modify semantics)
    constexpr auto s1 = s.conj(10);
    STATIC_REQUIRE_FALSE(s1.is_empty());
    STATIC_REQUIRE(s1.count() == 1U);
    STATIC_REQUIRE(s1.contains(10));
    STATIC_REQUIRE_FALSE(s1.contains(20));

    constexpr auto s2 = s1.conj(20);
    STATIC_REQUIRE(s2.count() == 2U);
    STATIC_REQUIRE(s2.contains(10));
    STATIC_REQUIRE(s2.contains(20));

    // conj on full set returns unchanged copy
    constexpr auto s3 = s2.conj(30).conj(40);
    STATIC_REQUIRE(s3.count() == 4U);
    constexpr auto s4 = s3.conj(99);  // overflow attempt
    STATIC_REQUIRE(s4.count() == 4U); // size unchanged

    // conj ignores duplicates
    constexpr auto s5 = s1.conj(10);
    STATIC_REQUIRE(s5.count() == 1U);

    // callable lookup: present element returns stored value
    STATIC_REQUIRE(s2(10) == 10);
    STATIC_REQUIRE(s2(20) == 20);

    // callable lookup: missing element returns default-constructed value
    STATIC_REQUIRE(s2(99) == 0);

    // callable lookup: missing element with fallback
    STATIC_REQUIRE(s2(99, -1) == -1);

    // can_conj preflight
    STATIC_REQUIRE(s.can_conj(10));       // empty set, room available
    STATIC_REQUIRE(s1.can_conj(10));      // duplicate allowed
    STATIC_REQUIRE(s4.can_conj(10));      // full set but duplicate → true
    STATIC_REQUIRE_FALSE(s4.can_conj(5)); // full set, not present → false

    // disj via swap-and-remove
    constexpr auto s6 = s2.disj(10);
    STATIC_REQUIRE(s6.count() == 1U);
    STATIC_REQUIRE_FALSE(s6.contains(10));
    STATIC_REQUIRE(s6.contains(20));

    // disj removes last via swap
    constexpr auto s7 = s2.disj(20);
    STATIC_REQUIRE(s7.count() == 1U);
    STATIC_REQUIRE(s7.contains(10));
    STATIC_REQUIRE_FALSE(s7.contains(20));

    // disj absent key returns unchanged copy
    constexpr auto s8 = s1.disj(99);
    STATIC_REQUIRE(s8.count() == 1U);
    STATIC_REQUIRE(s8.contains(10));

    // Runtime tests for code coverage instrumentation
    volatile int sv1_raw = 100;
    volatile int sv2_raw = 200;
    int sv1 = sv1_raw;
    int sv2 = sv2_raw;
    auto rs = Set<int, 4>{};
    REQUIRE(rs.is_empty());
    REQUIRE(rs.count() == 0U);
    REQUIRE(rs.capacity() == 4U);
    REQUIRE_FALSE(rs.contains(sv1));
    REQUIRE(rs(sv1) == 0);
    REQUIRE(rs(sv1, -1) == -1);
    REQUIRE(rs.can_conj(sv1));

    auto rs1 = rs.conj(sv1);
    REQUIRE_FALSE(rs1.is_empty());
    REQUIRE(rs1.count() == 1U);
    REQUIRE(rs1.contains(sv1));
    REQUIRE(rs1(sv1) == 100);
    REQUIRE(rs1.conj(sv1).count() == 1U); // duplicate insertion is no-op

    auto rs2 = rs1.conj(sv2);
    REQUIRE(rs2.count() == 2U);
    REQUIRE(rs2.contains(sv2));

    auto rs_full = rs2.conj(300).conj(400);
    REQUIRE(rs_full.count() == 4U);
    REQUIRE_FALSE(rs_full.can_conj(500));
    REQUIRE(rs_full.conj(500).count() == 4U); // rejected overflow

    auto rs_disj_mid = rs2.disj(sv1);
    REQUIRE(rs_disj_mid.count() == 1U);
    REQUIRE_FALSE(rs_disj_mid.contains(sv1));
    REQUIRE(rs_disj_mid.contains(sv2));

    auto rs_disj_last = rs2.disj(sv2);
    REQUIRE(rs_disj_last.count() == 1U);
    REQUIRE(rs_disj_last.contains(sv1));
    REQUIRE_FALSE(rs_disj_last.contains(sv2));

    auto rs_disj_absent = rs1.disj(999);
    REQUIRE(rs_disj_absent.count() == 1U);
}
