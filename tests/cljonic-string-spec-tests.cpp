#include <catch2/catch_test_macros.hpp>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("String construction and indexed operations", "[string]") {
    using cljonic::String;

    TRACE_ID("entity-fields.String");
    TRACE_ID("invariant.String.CapacityIsNonNegative");
    TRACE_ID("invariant.String.LogicalSizeIsNonNegative");
    TRACE_ID("invariant.String.LogicalSizeDoesNotExceedCapacity");
    TRACE_ID("invariant.String.HeaderOnlyDistribution");
    TRACE_ID("invariant.String.StaticOrAutomaticStorageOnly");
    TRACE_ID("invariant.String.ContiguousStorage");
    TRACE_ID("invariant.String.OrderedAsciiStorage");
    TRACE_ID("invariant.String.NullTerminatorIsUncounted");
    TRACE_ID("invariant.String.InvalidBytesRejectedAtCompileTime");
    TRACE_ID("invariant.String.NoHeapAllocation");
    TRACE_ID("invariant.String.NoRtti");
    TRACE_ID("invariant.String.NoExceptions");
    TRACE_ID("invariant.String.SingleThreadedExecutionModel");
    TRACE_ID("invariant.String.ImmutablePublicApi");
    TRACE_ID("invariant.String.UpdateReturnsNewValue");
    TRACE_ID("invariant.String.DeepCopyUpdate");
    TRACE_ID("invariant.String.ReferentialTransparency");
    TRACE_ID("invariant.String.RequiresNothrowDefaultConstruction");
    TRACE_ID("invariant.String.RequiresNothrowCopyConstruction");
    TRACE_ID("invariant.String.RequiresNothrowCopyAssignment");
    TRACE_ID("invariant.String.RequiresNothrowDestruction");
    TRACE_ID("invariant.String.SupportsLiteralDeducedConstruction");
    TRACE_ID("invariant.String.SupportsEmptyExplicitCapacityConstruction");
    TRACE_ID("invariant.String.SupportsExplicitCapacityConstruction");
    TRACE_ID("invariant.String.SupportsCapacityInferredLiteralEquivalentSemantics");
    TRACE_ID("invariant.String.OversizedInitializerIsCompileTimeFailure");
    TRACE_ID("invariant.String.CapacityExceedsMaximumIsCompileTimeFailure");
    TRACE_ID("invariant.String.SupportsIndexedLookup");
    TRACE_ID("invariant.String.SupportsIndexedFallbackLookup");
    TRACE_ID("invariant.String.InvalidIndexReturnsDefaultElement");
    TRACE_ID("invariant.String.InvalidIndexReturnsSuppliedFallback");
    TRACE_ID("invariant.String.CanonicalResultStatusModelIsDeclared");
    TRACE_ID("invariant.String.CompleteResultStatusDeclared");
    TRACE_ID("invariant.String.BoundedPrefixResultStatusDeclared");
    TRACE_ID("invariant.String.DefaultReturningResultStatusDeclared");
    TRACE_ID("invariant.String.CheckedFailureResultStatusDeclared");
    TRACE_ID("invariant.String.ProducerOnlyResultStatusDeclared");
    TRACE_ID("invariant.String.PreflightPredicatesAreNonThrowingNonAllocating");
    TRACE_ID("invariant.String.ContainsIsCanonicalIndexPredicate");
    TRACE_ID("invariant.String.FitsIntoIsCanonicalMaterializationPreflight");
    TRACE_ID("invariant.String.CompileTimeCapacityOverflowIsRejected");
    TRACE_ID("invariant.String.RuntimeCapacityFailuresHaveDocumentedPolicy");
    TRACE_ID("invariant.String.DefaultAccessHasPreflightPredicate");

    // Empty construction with explicit capacity
    constexpr String<8> s{};
    STATIC_REQUIRE(s.is_empty());
    STATIC_REQUIRE(s.count() == 0U);
    STATIC_REQUIRE(s.capacity() == 8U);
    STATIC_REQUIRE(s(0) == '\0');           // out-of-bounds returns '\0'
    STATIC_REQUIRE(s.contains(0) == false); // index 0 not valid when empty

    // Literal construction deduces size from initializer
    constexpr String<8> s1{"Hello"}; // "Hello" = 5 chars + null terminator
    STATIC_REQUIRE_FALSE(s1.is_empty());
    STATIC_REQUIRE(s1.count() == 5U); // logical size excludes null terminator
    STATIC_REQUIRE(s1(0) == 'H');
    STATIC_REQUIRE(s1(4) == 'o');
    STATIC_REQUIRE(s1(5) == '\0');  // position 5 is null terminator (out-of-logical-bounds)
    STATIC_REQUIRE(s1.contains(0)); // valid indices 0..4
    STATIC_REQUIRE(s1.contains(4));
    STATIC_REQUIRE_FALSE(s1.contains(5)); // null terminator position is invalid

    // Callable index access: present element
    STATIC_REQUIRE(s1(0) == 'H');
    STATIC_REQUIRE(s1(4) == 'o');

    // Callable index access: missing element returns default ('\0')
    STATIC_REQUIRE(s1(99) == '\0');

    // Callable index access: custom fallback
    STATIC_REQUIRE(s1(99, 'X') == 'X');

    // put replaces character at index (copy-on-modify semantics)
    constexpr auto s2 = s1.put(0, 'h');
    STATIC_REQUIRE(s2(0) == 'h');
    STATIC_REQUIRE(s2(1) == 'e');
    STATIC_REQUIRE(s1(0) == 'H'); // original unchanged — referential transparency

    // put with out-of-bounds index leaves copy unchanged
    constexpr auto s3 = s1.put(99, 'X');
    STATIC_REQUIRE(s3(0) == 'H');
    STATIC_REQUIRE(s3.count() == 5U);

    // append adds character to end (if room available)
    constexpr auto s4 = s1.append('!');
    STATIC_REQUIRE(s4.count() == 6U);
    STATIC_REQUIRE(s4(5) == '!');
    STATIC_REQUIRE(s4(6) == '\0'); // new null terminator

    // append on full string returns unchanged copy
    constexpr String<5> s_full{"Hello"}; // exactly at capacity
    STATIC_REQUIRE(s_full.count() == 5U);
    constexpr auto s_n_append = s_full.append('!');
    STATIC_REQUIRE(s_n_append.count() == 5U); // unchanged — no room

    // Capacity zero: can only hold empty/null
    constexpr String<0> s_empty_cap{};
    STATIC_REQUIRE(s_empty_cap.is_empty());
    STATIC_REQUIRE(s_empty_cap.count() == 0U);
    STATIC_REQUIRE(s_empty_cap(0) == '\0');

    // Append to zero-capacity string is a no-op
    constexpr auto s_zero_appended = s_empty_cap.append('A');
    STATIC_REQUIRE(s_zero_appended.is_empty());

    // Runtime tests for code coverage instrumentation
    volatile char c1_raw = 'A';
    volatile char c2_raw = 'B';
    volatile std::size_t idx0_raw = 0;
    char c1 = c1_raw;
    char c2 = c2_raw;
    std::size_t idx0 = idx0_raw;
    auto rs = String<8>{};
    REQUIRE(rs.is_empty());
    REQUIRE(rs.count() == 0U);
    REQUIRE(rs.capacity() == 8U);
    REQUIRE(rs(idx0) == '\0');
    REQUIRE_FALSE(rs.contains(idx0));

    auto rs1 = rs.append(c1).append(c2);
    REQUIRE_FALSE(rs1.is_empty());
    REQUIRE(rs1.count() == 2U);
    REQUIRE(rs1(idx0) == 'A');
    REQUIRE(rs1(1) == 'B');
    REQUIRE(rs1.contains(idx0));
    REQUIRE(rs1.contains(1));
    REQUIRE_FALSE(rs1.contains(2));
    REQUIRE(rs1(idx0) == 'A');
    REQUIRE(rs1(99) == '\0');
    REQUIRE(rs1(99, 'Z') == 'Z');

    auto rs_put = rs1.put(idx0, 'X');
    REQUIRE(rs_put(idx0) == 'X');
    REQUIRE(rs1(idx0) == 'A'); // immutability

    auto rs_put_oob = rs1.put(99, 'X');
    REQUIRE(rs_put_oob.count() == 2U);

    auto rs_full = String<2>{"AB"};
    REQUIRE(rs_full.append('C').count() == 2U);
}
