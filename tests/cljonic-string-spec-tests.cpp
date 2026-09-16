#include <catch2/catch_test_macros.hpp>

#include <array>
#include <limits>
#include <ranges>
#include <span>
#include <string_view>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("String construction and indexed operations", "[string]") {
    using cljonic::assoc;
    using cljonic::get;
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
    TRACE_ID("invariant.String.RuntimeInvalidBytesReplacedWithPeriod");
    TRACE_ID("invariant.String.ConstCharSpanInvalidBytesAreReplacedWithPeriod");
    TRACE_ID("invariant.String.ConstCharSpanRuntimeExtentOverflowUsesBoundedPrefix");
    TRACE_ID("invariant.String.ConstCharSpanSourceCopyIsOwned");
    TRACE_ID("invariant.String.ConstCharSpanSourceDoesNotBorrowLifetime");
    TRACE_ID("invariant.String.ConstCharSpanSourceIsNonAllocating");
    TRACE_ID("invariant.String.ConstCharSpanSourceIsNonThrowing");
    TRACE_ID("invariant.String.ConstCharSpanStaticExtentOverflowIsCompileTimeFailure");
    TRACE_ID("invariant.String.RuntimeNullByteIsInvalidInStringViewSource");
    TRACE_ID("invariant.String.StringViewCompileTimeOverflowIsRejected");
    TRACE_ID("invariant.String.StringViewImportExcludesNullTerminator");
    TRACE_ID("invariant.String.StringViewRuntimeOverflowUsesBoundedPrefix");
    TRACE_ID("invariant.String.StringViewSourceCopyIsOwned");
    TRACE_ID("invariant.String.StringViewSourceDoesNotBorrowLifetime");
    TRACE_ID("invariant.String.StringViewSourceIsNonAllocating");
    TRACE_ID("invariant.String.StringViewSourceIsNonThrowing");
    TRACE_ID("invariant.String.SupportsConstCharSpanSourceConstruction");
    TRACE_ID("invariant.String.SupportsStdStringViewSourceConstruction");
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
    TRACE_ID("invariant.String.SupportsDefaultFallbackLookup");
    TRACE_ID("invariant.String.AcceptsIntegralIndexTypes");
    TRACE_ID("invariant.String.SupportsLookupCapability");
    TRACE_ID("invariant.String.SupportsAssociativeCapability");
    TRACE_ID("invariant.String.SeqableLifecycleIsDeferred");
    TRACE_ID("invariant.String.SupportsAssocUpdate");
    TRACE_ID("invariant.String.SupportsCanAssocPreflight");
    TRACE_ID("invariant.String.AssocReplacesExistingCharacter");
    TRACE_ID("invariant.String.AssocAppendsAtLogicalCount");
    TRACE_ID("invariant.String.InvalidAssocIndexReturnsUnchangedString");
    TRACE_ID("invariant.String.NullTerminatorIsNotAssociationIndex");
    TRACE_ID("invariant.String.CanAssocTrueForExistingOrAppendIndex");
    TRACE_ID("invariant.String.CanAssocFalseForInvalidOrFullAppendIndex");
    TRACE_ID("invariant.String.AssocCharacterValueIsValidated");
    TRACE_ID("invariant.String.SupportsGetFreeFunction");
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
    STATIC_REQUIRE_FALSE(s1.contains(-1));
    STATIC_REQUIRE_FALSE(s1.contains(std::numeric_limits<unsigned long long>::max()));
    STATIC_REQUIRE(get(s1, 0U) == 'H');
    STATIC_REQUIRE(get(s1, 99U) == '\0');
    STATIC_REQUIRE(get(s1, 99U, 'X') == 'X');

    constexpr String<1> ascii_limit{"\x7F"};
    STATIC_REQUIRE(ascii_limit.count() == 1U);
    STATIC_REQUIRE(ascii_limit(0) == '\x7F');

    // Callable index access: present element
    STATIC_REQUIRE(s1(0) == 'H');
    STATIC_REQUIRE(s1(4) == 'o');

    // Callable index access: missing element returns default ('\0')
    STATIC_REQUIRE(s1(99) == '\0');

    // Callable index access: custom fallback
    STATIC_REQUIRE(s1(99, 'X') == 'X');
    STATIC_REQUIRE(s1(-1) == '\0');
    STATIC_REQUIRE(s1(-1, 'X') == 'X');
    STATIC_REQUIRE(s1(std::numeric_limits<unsigned long long>::max(), 'X') == 'X');

    // put replaces character at index (copy-on-modify semantics)
    constexpr auto s2 = s1.put(0, 'h');
    STATIC_REQUIRE(s2(0) == 'h');
    STATIC_REQUIRE(s2(1) == 'e');
    STATIC_REQUIRE(s1(0) == 'H'); // original unchanged — referential transparency

    // put with out-of-bounds index leaves copy unchanged
    constexpr auto s3 = s1.put(99, 'X');
    STATIC_REQUIRE(s3(0) == 'H');
    STATIC_REQUIRE(s3.count() == 5U);
    constexpr auto s4 = s1.put(-1, 'X');
    STATIC_REQUIRE(s4.view() == s1.view());

    // Capacity zero: can only hold empty/null
    constexpr String<0> s_empty_cap{};
    STATIC_REQUIRE(s_empty_cap.is_empty());
    STATIC_REQUIRE(s_empty_cap.count() == 0U);
    STATIC_REQUIRE(s_empty_cap(0) == '\0');

    // Runtime tests for code coverage instrumentation
    volatile std::size_t idx0_raw = 0;
    std::size_t idx0 = idx0_raw;
    auto rs = String<8>{};
    REQUIRE(rs.is_empty());
    REQUIRE(rs.count() == 0U);
    REQUIRE(rs.capacity() == 8U);
    REQUIRE(rs(idx0) == '\0');
    REQUIRE_FALSE(rs.contains(idx0));

    auto rs1 = String<8>{"AB"};
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

    const auto rs_assoc_append = assoc(rs1, 2U, 'C');
    REQUIRE(rs_assoc_append.view() == std::string_view{"ABC"});
    REQUIRE(rs1.view() == std::string_view{"AB"});

    auto rs_put_oob = rs1.put(99, 'X');
    REQUIRE(rs_put_oob.count() == 2U);

    const char invalid_runtime_bytes[] = {'A', '\0', static_cast<char>(0x80), '\0'};
    auto rs_invalid = String<3>{invalid_runtime_bytes};
    REQUIRE(rs_invalid.count() == 3U);
    REQUIRE(rs_invalid(0) == 'A');
    REQUIRE(rs_invalid(1) == '.');
    REQUIRE(rs_invalid(2) == '.');
}

TEST_CASE("String imports from std::string_view without allocating or mutating the source", "[string][interop]") {
    using cljonic::String;

    static constexpr std::string_view static_source{"ABCD"};
    constexpr auto from_static_view = String<8>{static_source};
    STATIC_REQUIRE(from_static_view.count() == 4U);
    STATIC_REQUIRE(from_static_view(0) == 'A');
    STATIC_REQUIRE(from_static_view(3) == 'D');

    const std::string_view runtime_source{"XYZ"};
    const auto from_runtime_view = String<2>{runtime_source};
    REQUIRE(from_runtime_view.count() == 2U);
    REQUIRE(from_runtime_view(0) == 'X');
    REQUIRE(from_runtime_view(1) == 'Y');
    REQUIRE(runtime_source[0] == 'X');
    REQUIRE(runtime_source[2] == 'Z');

    const auto bounded_prefix = String<3>{std::string_view{"ABCD"}};
    REQUIRE(bounded_prefix.count() == 3U);
    REQUIRE(bounded_prefix(0) == 'A');
    REQUIRE(bounded_prefix(1) == 'B');
    REQUIRE(bounded_prefix(2) == 'C');

    const char invalid_bytes[] = {'A', '\0', static_cast<char>(0x80), 'B'};
    const std::string_view invalid_source{invalid_bytes, 4U};
    const auto normalized = String<4>{invalid_source};
    REQUIRE(normalized.count() == 4U);
    REQUIRE(normalized(0) == 'A');
    REQUIRE(normalized(1) == '.');
    REQUIRE(normalized(2) == '.');
    REQUIRE(normalized(3) == 'B');
}

TEST_CASE("String imports from const char spans without allocating or mutating the source", "[string][interop]") {
    using cljonic::String;

    static constexpr char static_bytes[] = {'A', 'B', 'C', 'D'};
    constexpr std::span<const char, 4> static_source{static_bytes};
    constexpr auto from_static_span = String{static_source};
    STATIC_REQUIRE(std::same_as<decltype(from_static_span), const String<4>>);
    STATIC_REQUIRE(from_static_span.view() == std::string_view{"ABCD"});

    char runtime_bytes[] = {'X', 'Y', 'Z', 'W'};
    std::span mutable_source{runtime_bytes};
    const auto ctad_from_mutable_span = String{mutable_source};
    STATIC_REQUIRE(std::same_as<std::remove_cvref_t<decltype(ctad_from_mutable_span)>, String<4>>);
    REQUIRE(ctad_from_mutable_span.view() == std::string_view{"XYZW"});

    const std::span<const char> runtime_source{runtime_bytes, 4U};
    const auto from_runtime_span_fit = String<4>{runtime_source};
    REQUIRE(from_runtime_span_fit.view() == std::string_view{"XYZW"});
    const auto from_runtime_span = String<2>{runtime_source};
    REQUIRE(from_runtime_span.view() == std::string_view{"XY"});
    REQUIRE(runtime_source[0] == 'X');
    REQUIRE(runtime_source[3] == 'W');

    const char invalid_bytes[] = {'A', '\0', static_cast<char>(0x80), 'B'};
    const std::span<const char> invalid_source{invalid_bytes, 4U};
    const auto normalized = String<4>{invalid_source};
    REQUIRE(normalized.view() == std::string_view{"A..B"});
}

TEST_CASE("String accepts bounded character ranges and views", "[string][interop][range]") {
    using cljonic::String;

    const std::array<char, 4> source{{'A', 'B', 'C', 'D'}};
    const auto from_static_array = String<4>{source};
    const auto from_static_span = String<4>{std::span<const char, 4>{source}};
    REQUIRE(from_static_array.view() == std::string_view{"ABCD"});
    REQUIRE(from_static_span.view() == std::string_view{"ABCD"});

    const std::span<const char> dynamic_source{source};
    const auto copied = String<3>{dynamic_source};
    REQUIRE(copied.view() == std::string_view{"ABC"});

    const auto mapped =
        std::views::transform(source, [](char byte) { return byte == 'B' ? static_cast<char>(0x80) : byte; });
    const auto normalized = String<4>{mapped};
    REQUIRE(normalized.view() == std::string_view{"A.CD"});
    const auto normalized_bounded = String<3>{mapped};
    REQUIRE(normalized_bounded.view() == std::string_view{"A.CD"}.substr(0, 3));
}
