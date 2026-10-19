#include <catch2/catch_test_macros.hpp>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

namespace {
template <typename T>
concept HasDeferredEntryAccess = requires(const T& value) { last(value); } ||
                                 requires(const T& value) { key(value); } || requires(const T& value) { val(value); };

template <typename T>
concept HasFirstAccess = requires(const T& value) { first(value); };

template <typename T>
concept HasSequenceConversion = requires(const T& value) { seq(value); };
} // namespace

TEST_CASE("MapEntry holds value-semantic key/value pair", "[map-entry]") {
    using cljonic::MapEntry;

    TRACE_ID("entity-fields.MapEntry");
    TRACE_ID("invariant.MapEntry.KeyValueCountIsTwo");
    TRACE_ID("invariant.MapEntry.HeaderOnlyDistribution");
    TRACE_ID("invariant.MapEntry.StaticOrAutomaticStorageOnly");
    TRACE_ID("invariant.MapEntry.NoHeapAllocation");
    TRACE_ID("invariant.MapEntry.NoRtti");
    TRACE_ID("invariant.MapEntry.NoExceptions");
    TRACE_ID("invariant.MapEntry.SingleThreadedExecutionModel");
    TRACE_ID("invariant.MapEntry.ImmutablePublicApi");
    TRACE_ID("invariant.MapEntry.ReferentialTransparency");
    TRACE_ID("invariant.MapEntry.RequiresValueSemanticElements");
    TRACE_ID("invariant.MapEntry.RequiresNothrowCollectionElementKeyAndValueAdmission");
    TRACE_ID("invariant.MapEntry.RequiresNothrowDefaultConstruction");
    TRACE_ID("invariant.MapEntry.RequiresNothrowCopyConstruction");
    TRACE_ID("invariant.MapEntry.RequiresNothrowCopyAssignment");
    TRACE_ID("invariant.MapEntry.RequiresNothrowDestruction");
    TRACE_ID("invariant.MapEntry.SupportsNamedKeyFieldObservation");
    TRACE_ID("invariant.MapEntry.SupportsNamedValueFieldObservation");
    TRACE_ID("invariant.MapEntry.SupportsFirstFreeFunction");
    TRACE_ID("invariant.MapEntry.SemanticKeyValLastFreeFunctionsAreDeferred");
    TRACE_ID("invariant.MapEntry.FixedTwoElementSequenceBehaviorIsDeferred");

    constexpr MapEntry<int, int> entry{1, 100};
    STATIC_REQUIRE(entry.key == 1);
    STATIC_REQUIRE(entry.value == 100);
    STATIC_REQUIRE(HasFirstAccess<decltype(entry)>);
    STATIC_REQUIRE(first(entry) == 1);
    STATIC_REQUIRE_FALSE(HasDeferredEntryAccess<decltype(entry)>);
    STATIC_REQUIRE_FALSE(HasSequenceConversion<decltype(entry)>);

    const auto runtime_entry = MapEntry<int, int>{2, 200};
    CHECK(runtime_entry.key == 2);
    CHECK(runtime_entry.value == 200);
}
