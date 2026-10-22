#include <catch2/catch_test_macros.hpp>

#include <concepts>
#include <type_traits>

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

TEST_CASE("map_entry factory builds one value-semantic key/value pair", "[map-entry][factory]") {
    using cljonic::map_entry;
    using cljonic::MapEntry;

    TRACE_ID("entity-fields.MapEntryFactory");
    TRACE_ID("invariant.MapEntryFactory.SupportsDeducedKeyValueFactory");
    TRACE_ID("invariant.MapEntryFactory.DeducedKeyTypeIsUnqualifiedKey");
    TRACE_ID("invariant.MapEntryFactory.DeducedValueTypeIsUnqualifiedValue");
    TRACE_ID("invariant.MapEntryFactory.SupportsExplicitKeyValueTypeFactory");
    TRACE_ID("invariant.MapEntryFactory.ArityIsExactlyTwo");
    TRACE_ID("invariant.MapEntryFactory.OtherArityIsCompileTimeRejected");
    TRACE_ID("invariant.MapEntryFactory.HasNoCapacityParameter");
    TRACE_ID("invariant.MapEntryFactory.HasNoSpareCapacityForm");
    TRACE_ID("invariant.MapEntryFactory.EveryKeyAndValueSatisfiesMapEntryElementAdmission");
    TRACE_ID("invariant.MapEntryFactory.KeyAndValueConstructSafelyWithoutThrowing");
    TRACE_ID("invariant.MapEntryFactory.PreservesPublicMapEntryTypeAndExistingConstructors");
    TRACE_ID("invariant.MapEntryFactory.ResultMatchesEquivalentMapEntryConstructor");
    TRACE_ID("invariant.MapEntryFactory.ReturnsValueSemanticPair");
    TRACE_ID("invariant.MapEntryFactory.ConstexprNoexceptNonallocating");
    TRACE_ID("invariant.MapEntryFactory.NoRtti");
    TRACE_ID("invariant.MapEntryFactory.NoExceptions");
    TRACE_ID("invariant.MapEntryFactory.ReferentialTransparency");

    constexpr auto deduced = map_entry(1, 100L);
    STATIC_REQUIRE(std::same_as<std::remove_cvref_t<decltype(deduced)>, MapEntry<int, long>>);
    STATIC_REQUIRE(deduced.key == 1);
    STATIC_REQUIRE(deduced.value == 100L);
    STATIC_REQUIRE(deduced == MapEntry{1, 100L});

    constexpr auto explicit_types = map_entry<long, long>(short{2}, short{200});
    STATIC_REQUIRE(std::same_as<std::remove_cvref_t<decltype(explicit_types)>, MapEntry<long, long>>);
    STATIC_REQUIRE(explicit_types.key == 2L);
    STATIC_REQUIRE(explicit_types.value == 200L);

    STATIC_REQUIRE(noexcept(map_entry(3, 300)));
    STATIC_REQUIRE(noexcept(map_entry<long, long>(short{3}, short{300})));

    const auto runtime = map_entry(4, 400);
    CHECK(runtime == MapEntry{4, 400});
    CHECK(runtime.key == 4);
    CHECK(runtime.value == 400);
}
