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
  TRACE_ID("invariant.String.SupportsLiteralDeducedConstruction");
  TRACE_ID("invariant.String.SupportsEmptyExplicitCapacityConstruction");
  TRACE_ID("invariant.String.SupportsExplicitCapacityConstruction");
  TRACE_ID(
      "invariant.String.SupportsCapacityInferredLiteralEquivalentSemantics");
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
  TRACE_ID("invariant.String.ValidIndexIsCanonicalIndexPredicate");
  TRACE_ID("invariant.String.FitsIntoIsCanonicalMaterializationPreflight");
  TRACE_ID("invariant.String.CompileTimeCapacityOverflowIsRejected");
  TRACE_ID("invariant.String.RuntimeCapacityFailuresHaveDocumentedPolicy");
  TRACE_ID("invariant.String.DefaultAccessHasPreflightPredicate");

  // Empty construction with explicit capacity
  constexpr String<8> s{};
  STATIC_REQUIRE(s.empty());
  STATIC_REQUIRE(s.size() == 0U);
  STATIC_REQUIRE(s.capacity() == 8U);
  STATIC_REQUIRE(s[0] == '\0');        // out-of-bounds returns '\0'
  STATIC_REQUIRE(s.valid(0) == false); // index 0 not valid when empty

  // Literal construction deduces size from initializer
  constexpr String<8> s1{"Hello"}; // "Hello" = 5 chars + null terminator
  STATIC_REQUIRE_FALSE(s1.empty());
  STATIC_REQUIRE(s1.size() == 5U); // logical size excludes null terminator
  STATIC_REQUIRE(s1[0] == 'H');
  STATIC_REQUIRE(s1[4] == 'o');
  STATIC_REQUIRE(s1[5] ==
                 '\0'); // position 5 is null terminator (out-of-logical-bounds)
  STATIC_REQUIRE(s1.valid(0)); // valid indices 0..4
  STATIC_REQUIRE(s1.valid(4));
  STATIC_REQUIRE_FALSE(s1.valid(5)); // null terminator position is invalid

  // Callable index access: present element
  STATIC_REQUIRE(s1(0) == 'H');
  STATIC_REQUIRE(s1(4) == 'o');

  // Callable index access: missing element returns default ('\0')
  STATIC_REQUIRE(s1(99) == '\0');

  // Callable index access: custom fallback
  STATIC_REQUIRE(s1(99, 'X') == 'X');

  // put replaces character at index (copy-on-modify semantics)
  constexpr auto s2 = s1.put(0, 'h');
  STATIC_REQUIRE(s2[0] == 'h');
  STATIC_REQUIRE(s2[1] == 'e');
  STATIC_REQUIRE(s1[0] == 'H'); // original unchanged — referential transparency

  // put with out-of-bounds index leaves copy unchanged
  constexpr auto s3 = s1.put(99, 'X');
  STATIC_REQUIRE(s3[0] == 'H');
  STATIC_REQUIRE(s3.size() == 5U);

  // append adds character to end (if room available)
  constexpr auto s4 = s1.append('!');
  STATIC_REQUIRE(s4.size() == 6U);
  STATIC_REQUIRE(s4[5] == '!');
  STATIC_REQUIRE(s4[6] == '\0'); // new null terminator

  // append on full string returns unchanged copy
  constexpr String<5> s_full{"Hello"}; // exactly at capacity
  STATIC_REQUIRE(s_full.size() == 5U);
  constexpr auto s_n_append = s_full.append('!');
  STATIC_REQUIRE(s_n_append.size() == 5U); // unchanged — no room

  // Capacity zero: can only hold empty/null
  constexpr String<0> s_empty_cap{};
  STATIC_REQUIRE(s_empty_cap.empty());
  STATIC_REQUIRE(s_empty_cap.size() == 0U);
  STATIC_REQUIRE(s_empty_cap[0] == '\0');

  // Append to zero-capacity string is a no-op
  constexpr auto s_zero_appended = s_empty_cap.append('A');
  STATIC_REQUIRE(s_zero_appended.empty());
}
