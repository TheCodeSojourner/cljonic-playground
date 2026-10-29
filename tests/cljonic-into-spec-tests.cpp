#include <catch2/catch_test_macros.hpp>

#include <span>
#include <string_view>
#include <type_traits>
#include <utility>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("into and fits_into materialize a producer into a bounded destination", "[into][fits-into]") {
    using cljonic::fits_into;
    using cljonic::into;
    using cljonic::Range;
    using cljonic::Vector;

    TRACE_ID("entity-fields.ProducerMaterialization");
    TRACE_ID("invariant.ProducerMaterialization.IntoRequiresExplicitBoundedDestination");
    TRACE_ID("invariant.ProducerMaterialization.IntoRequiresSupportedCollectionDestination");
    STATIC_REQUIRE(cljonic::concepts_detail::MaterializableSource<cljonic::String<4>, cljonic::String<4>>);
    STATIC_REQUIRE(
        cljonic::concepts_detail::MaterializableSource<cljonic::Map<int, int, 4>, cljonic::Map<int, int, 4>>);
    STATIC_REQUIRE(cljonic::concepts_detail::MaterializableSource<cljonic::Set<int, 4>, Range<int>>);
    STATIC_REQUIRE(cljonic::concepts_detail::MaterializableSource<cljonic::Queue<int, 4>, Range<int>>);
    TRACE_ID("invariant.ProducerMaterialization.IntoReturnsNewDestinationTypedCollection");
    TRACE_ID("invariant.ProducerMaterialization.IntoVectorAndQueueAppendAfterDestinationContents");
    TRACE_ID("invariant.ProducerMaterialization.IntoLeavesDestinationUnchanged");
    TRACE_ID("invariant.ProducerMaterialization.IntoLeavesSourceUnchanged");
    TRACE_ID("invariant.ProducerMaterialization.FitsIntoIsNonThrowingNonAllocating");
    TRACE_ID("invariant.ProducerMaterialization.FitsIntoPredictsIntoCompleteness");
    TRACE_ID("invariant.ProducerMaterialization.FitsIntoUsesRemainingDestinationCapacity");
    TRACE_ID("invariant.ProducerMaterialization.FitsIntoAvoidsUnsignedAdditionOverflow");
    TRACE_ID("invariant.ProducerMaterialization.FitsIntoRequiresSupportedCollectionDestination");
    TRACE_ID("invariant.ProducerMaterialization.UnboundedSourceDoesNotFitIntoDestination");
    TRACE_ID("invariant.ProducerMaterialization.UnboundedSourceMaterializesBoundedPrefix");
    TRACE_ID("invariant.ProducerMaterialization.FiniteSourceMaterializesCompleteResultWhenItFits");
    TRACE_ID("invariant.ProducerMaterialization.BoundedPrefixPreservesSourceTraversalOrder");
    TRACE_ID("invariant.ProducerMaterialization.CountIsExactForMaterializedCollection");
    TRACE_ID("invariant.ProducerMaterialization.UnboundedProducerCountIsObservableTraversalCap");
    TRACE_ID("invariant.ProducerMaterialization.ComposedProducerCardinalityUsesSaturatingArithmetic");
    TRACE_ID("invariant.ProducerMaterialization.ComposedCardinalityBoundedByCollectionMaximumElementCount");
    TRACE_ID("invariant.ProducerMaterialization.MaterializationSourceDomainIsCljonicSource");
    TRACE_ID("invariant.ProducerMaterialization.CljonicSourcesAreDirectlyConsumableByFreeFunctions");
    TRACE_ID("invariant.ProducerMaterialization.IntoIsExplicitOwningMaterializationPath");
    TRACE_ID("invariant.ProducerMaterialization.IntoRejectsUnsupportedDomainWithTargetedDiagnostic");
    // Rejected calls select the diagnostic overload; compile-fail checks verify
    // the targeted message. Admission itself is the named concept.
    STATIC_REQUIRE_FALSE(cljonic::concepts_detail::MaterializableSource<Vector<int, 4>, int>);
    STATIC_REQUIRE(cljonic::concepts_detail::MaterializableSource<Vector<int, 4>, Range<int>>);

    TRACE_ID("invariant.ProducerMaterialization.IntoDoesNotAllocateOrThrow");
    STATIC_REQUIRE(noexcept(into(Vector<int, 4>{}, Range<int>{0, 1})));

    // A finite source that fits materializes completely, leaving destination and source unchanged.
    constexpr Vector<int, 8> empty_destination{};
    constexpr Range<int> finite_source{0, 5};
    constexpr auto complete_result = into(empty_destination, finite_source);
    STATIC_REQUIRE(complete_result.count() == 5U);
    STATIC_REQUIRE(complete_result(0U) == 0);
    STATIC_REQUIRE(complete_result(4U) == 4);
    STATIC_REQUIRE(empty_destination.count() == 0U);
    STATIC_REQUIRE(finite_source.count() == 5U);
    STATIC_REQUIRE(fits_into(empty_destination, finite_source));

    // An unbounded source materializes a deterministic bounded prefix limited by destination capacity.
    constexpr Range<int> unbounded_source{0, 5, 0};
    STATIC_REQUIRE_FALSE(unbounded_source.is_finite());
    constexpr auto bounded_prefix_result = into(empty_destination, unbounded_source);
    STATIC_REQUIRE(bounded_prefix_result.count() == empty_destination.capacity());
    STATIC_REQUIRE(bounded_prefix_result(0U) == 0);
    STATIC_REQUIRE_FALSE(fits_into(empty_destination, unbounded_source));

    // A finite source that does not fit the destination's remaining capacity is bounded.
    constexpr Vector<int, 3> small_destination{};
    constexpr Range<int> oversized_finite_source{0, 5};
    constexpr auto oversized_result = into(small_destination, oversized_finite_source);
    STATIC_REQUIRE(oversized_result.count() == 3U);
    STATIC_REQUIRE(oversized_result(2U) == 2);
    STATIC_REQUIRE_FALSE(fits_into(small_destination, oversized_finite_source));

    // Materialization appends to an already-populated destination.
    constexpr Vector<int, 8> populated_destination{100, 200};
    constexpr auto appended_result = into(populated_destination, finite_source);
    STATIC_REQUIRE(appended_result.count() == 7U);
    STATIC_REQUIRE(appended_result(0U) == 100);
    STATIC_REQUIRE(appended_result(2U) == 0);
    STATIC_REQUIRE(populated_destination.count() == 2U);
    STATIC_REQUIRE(fits_into(populated_destination, finite_source));

    const auto runtime_destination = Vector<int, 8>{};
    const auto runtime_source = Range<int>{0, 5};
    const auto runtime_result = into(runtime_destination, runtime_source);
    CHECK(runtime_result.count() == 5U);
    CHECK(fits_into(runtime_destination, runtime_source));

    // Runtime coverage for the bounded-prefix break path (defeats constexpr
    // folding so gcov records the branch where the destination fills mid-loop).
    volatile int runtime_small_capacity_end = 5;
    const auto runtime_small_destination = Vector<int, 3>{};
    const auto runtime_oversized_source = Range<int>{0, runtime_small_capacity_end};
    const auto runtime_bounded_result = into(runtime_small_destination, runtime_oversized_source);
    CHECK(runtime_bounded_result.count() == 3U);
    CHECK_FALSE(fits_into(runtime_small_destination, runtime_oversized_source));

    // Composed cardinality (existing destination content plus a nominally huge
    // producer) stays bounded by the destination capacity and the producer's own
    // saturating arithmetic, without size_t overflow.
    volatile int huge_end_raw = 5000;
    const auto populated_destination_for_composition = Vector<int, 4>{1, 2};
    const auto huge_source = Range<int>{0, huge_end_raw};
    const auto composed_result = into(populated_destination_for_composition, huge_source);
    CHECK(composed_result.count() == 4U);
    CHECK_FALSE(fits_into(populated_destination_for_composition, huge_source));
}

TEST_CASE("into materializes each collection source without changing its inputs", "[into]") {
    using namespace cljonic;

    TRACE_ID("invariant.ProducerMaterialization.MaterializationSourceDomainIsCljonicSource");
    TRACE_ID("invariant.ProducerMaterialization.IntoReturnsNewDestinationTypedCollection");
    TRACE_ID("invariant.ProducerMaterialization.IntoVectorAndQueueAppendAfterDestinationContents");
    TRACE_ID("invariant.ProducerMaterialization.IntoLeavesDestinationUnchanged");
    TRACE_ID("invariant.ProducerMaterialization.IntoLeavesSourceUnchanged");
    TRACE_ID("invariant.ProducerMaterialization.BoundedPrefixPreservesSourceTraversalOrder");

    SECTION("Vector source safely widens into the destination element type") {
        constexpr Vector<short, 4> source{short{1}, short{2}};
        constexpr Vector<long long, 5> destination{99LL};
        constexpr auto result = into(destination, source);
        STATIC_REQUIRE(std::is_same_v<decltype(into(destination, source)), Vector<long long, 5>>);
        STATIC_REQUIRE(equal(result, Vector<long long, 5>{99LL, 1LL, 2LL}));
        STATIC_REQUIRE(equal(source, Vector<short, 4>{short{1}, short{2}}));
        STATIC_REQUIRE(equal(destination, Vector<long long, 5>{99LL}));
        STATIC_REQUIRE(noexcept(into(destination, source)));
    }

    SECTION("Queue source uses front-to-back logical traversal") {
        const auto source = Queue<int, 4>{10, 20, 30}.pop();
        const auto destination = Vector<int, 5>{99};
        const auto result = into(destination, source);
        CHECK(equal(result, Vector<int, 5>{99, 20, 30}));
        CHECK(equal(source, Queue<int, 4>{20, 30}));
        CHECK(equal(destination, Vector<int, 5>{99}));
    }

    SECTION("String source contributes characters rather than its terminator") {
        constexpr String<4> source{"ab"};
        constexpr Vector<char, 5> destination{'x'};
        constexpr auto result = into(destination, source);
        STATIC_REQUIRE(equal(result, Vector<char, 5>{'x', 'a', 'b'}));
        STATIC_REQUIRE(equal(source, String<4>{"ab"}));
        STATIC_REQUIRE(equal(destination, Vector<char, 5>{'x'}));
        STATIC_REQUIRE(noexcept(into(destination, source)));
    }

    SECTION("Set source preserves its observable traversal without defining a sorted order") {
        const auto source = Set<int, 4>{3, 1, 2}.disj(1);
        const auto before = source;
        const auto destination = Vector<int, 5>{99};
        const auto result = into(destination, source);
        REQUIRE(result.count() == destination.count() + source.count());
        CHECK(result(0U) == 99);
        auto index = destination.count();
        for (const auto item : source) {
            CHECK(result(index++) == item);
        }
        CHECK(equal(source, before));
        CHECK(equal(destination, Vector<int, 5>{99}));
    }

    SECTION("Map source contributes entries in its observable traversal order") {
        using Entry = MapEntry<int, int>;
        const auto source = Map<int, int, 4>{Entry{3, 30}, Entry{1, 10}, Entry{2, 20}}.dissoc(1);
        const auto before = source;
        const auto destination = Vector<Entry, 5>{Entry{9, 90}};
        const auto result = into(destination, source);
        STATIC_REQUIRE(std::is_same_v<decltype(into(destination, source)), Vector<Entry, 5>>);
        REQUIRE(result.count() == destination.count() + source.count());
        CHECK(result(0U) == Entry{9, 90});
        auto index = destination.count();
        for (const auto& entry : source) {
            CHECK(result(index++) == entry);
        }
        CHECK(equal(source, before));
        CHECK(equal(destination, Vector<Entry, 5>{Entry{9, 90}}));
    }
}

TEST_CASE("into handles empty sources and remaining destination capacity", "[into]") {
    using namespace cljonic;

    TRACE_ID("invariant.ProducerMaterialization.IntoVectorAndQueueAppendAfterDestinationContents");
    TRACE_ID("invariant.ProducerMaterialization.FiniteSourceMaterializesCompleteResultWhenItFits");
    TRACE_ID("invariant.ProducerMaterialization.BoundedPrefixPreservesSourceTraversalOrder");
    TRACE_ID("invariant.ProducerMaterialization.IntoLeavesDestinationUnchanged");
    TRACE_ID("invariant.ProducerMaterialization.IntoLeavesSourceUnchanged");

    const auto source = Vector<int, 4>{1, 2, 3};
    const auto full = Vector<int, 2>{7, 8};
    CHECK(equal(into(full, source), full));
    CHECK(equal(into(full, Vector<int, 4>{}), full));
    CHECK(equal(into(Vector<int, 4>{9}, Vector<int, 4>{}), Vector<int, 4>{9}));
    CHECK(equal(into(Vector<int, 4>{}, Vector<int, 4>{}), Vector<int, 4>{}));
    CHECK(equal(into(Vector<int, 3>{9, 8}, source), Vector<int, 3>{9, 8, 1}));
    CHECK(equal(into(Vector<int, 3>{}, source), Vector<int, 3>{1, 2, 3}));
    CHECK(equal(full, Vector<int, 2>{7, 8}));
    CHECK(equal(source, Vector<int, 4>{1, 2, 3}));
}

TEST_CASE("materialization admits only compatible collection destinations and cljonic sources", "[into][fits-into]") {
    using namespace cljonic;
    using Entry = MapEntry<int, int>;

    TRACE_ID("invariant.ProducerMaterialization.MaterializationRejectsProducerDestinations");
    TRACE_ID("invariant.ProducerMaterialization.MaterializationRejectsExternalCppSources");
    TRACE_ID("invariant.ProducerMaterialization.IntoAndFitsIntoShareCompatibleSourceDomain");
    TRACE_ID("invariant.ProducerMaterialization.SourceReferenceObeysSafeNothrowInsertionRules");
    TRACE_ID("invariant.ProducerMaterialization.MapSourceRequiresCompatibleNominalMapEntries");
    TRACE_ID("invariant.ProducerMaterialization.FitsIntoRejectsUnsupportedDomainWithTargetedDiagnostic");
    STATIC_REQUIRE_FALSE(concepts_detail::MaterializableSource<int, Range<int>>);
    STATIC_REQUIRE_FALSE(concepts_detail::MaterializableSource<Range<int>, Range<int>>);
    STATIC_REQUIRE(concepts_detail::MaterializableSource<String<4>, Vector<char, 4>>);
    STATIC_REQUIRE_FALSE(concepts_detail::MaterializableSource<Vector<int, 4>, std::span<const int>>);
    STATIC_REQUIRE_FALSE(concepts_detail::MaterializableSource<Queue<char, 4>, std::string_view>);
    STATIC_REQUIRE_FALSE(concepts_detail::MaterializableSource<Vector<short, 4>, Range<int>>);
    STATIC_REQUIRE_FALSE(concepts_detail::MaterializableSource<Set<short, 4>, Range<int>>);
    STATIC_REQUIRE_FALSE(concepts_detail::MaterializableSource<Queue<short, 4>, Range<int>>);
    STATIC_REQUIRE_FALSE(concepts_detail::MaterializableSource<Map<int, int, 4>, Vector<int, 4>>);
    STATIC_REQUIRE_FALSE(concepts_detail::MaterializableSource<Map<int, int, 4>, Vector<std::pair<int, int>, 4>>);
    STATIC_REQUIRE_FALSE(concepts_detail::MaterializableSource<Map<long, int, 4>, Vector<Entry, 4>>);
    STATIC_REQUIRE(concepts_detail::MaterializableSource<Map<int, int, 4>, Vector<Entry, 4>>);
    STATIC_REQUIRE(concepts_detail::MaterializableSource<Set<long long, 4>, Vector<short, 4>>);
    STATIC_REQUIRE(concepts_detail::MaterializableSource<Queue<long long, 4>, Range<int>>);
    STATIC_REQUIRE(concepts_detail::MaterializableSource<Set<char, 4>, String<4>>);
}

TEST_CASE("String materialization normalizes invalid runtime characters and continues", "[into][fits-into]") {
    using namespace cljonic;

    TRACE_ID("invariant.ProducerMaterialization.IntoStringNormalizesInvalidRuntimeCharactersAndContinues");
    TRACE_ID("invariant.ProducerMaterialization.IntoStringRejectsInvalidCharactersDuringConstantEvaluation");
    TRACE_ID("invariant.ProducerMaterialization.FiniteStringPreflightUsesRemainingCapacity");

    constexpr auto constexpr_result = into(String<4>{"x"}, String<4>{"ab"});
    constexpr auto constexpr_invalid = Vector<char, 1>{static_cast<char>(0x80)};
    STATIC_REQUIRE(constexpr_result.view() == std::string_view{"xab"});
    STATIC_REQUIRE(fits_into(String<4>{"x"}, String<4>{"ab"}));
    STATIC_REQUIRE(fits_into(String<1>{}, constexpr_invalid));
    STATIC_REQUIRE_FALSE(fits_into(String<2>{"x"}, String<4>{"ab"}));
    STATIC_REQUIRE(noexcept(into(String<4>{}, String<4>{})));
    STATIC_REQUIRE(noexcept(fits_into(String<4>{}, String<4>{})));

    volatile unsigned char invalid_byte = 0x80U;
    const auto source = Vector<char, 5>{'a', static_cast<char>(invalid_byte), 'b', '\0', 'c'};
    const auto normalized = into(String<5>{}, source);
    CHECK(normalized.view() == std::string_view{"a.b.c"});
    CHECK(fits_into(String<5>{}, source));

    const auto partial = into(String<4>{"x"}, source);
    CHECK(partial.view() == std::string_view{"xa.b"});
    CHECK_FALSE(fits_into(String<4>{"x"}, source));
    CHECK(equal(source, Vector<char, 5>{'a', static_cast<char>(invalid_byte), 'b', '\0', 'c'}));
}

TEST_CASE("into and fits_into preserve queue FIFO order and remaining capacity", "[into][fits-into]") {
    using namespace cljonic;

    TRACE_ID("invariant.ProducerMaterialization.IntoVectorAndQueueAppendAfterDestinationContents");
    TRACE_ID("invariant.ProducerMaterialization.FiniteVectorAndQueuePreflightUsesSourceCount");
    TRACE_ID("invariant.ProducerMaterialization.MaterializationAndPreflightAreConstexprWhenSupported");
    constexpr auto destination = Queue<long long, 4>{9LL, 8LL, 7LL}.pop().pop();
    constexpr auto source = Vector<short, 4>{short{1}, short{2}};
    constexpr auto result = into(destination, source);
    STATIC_REQUIRE(std::is_same_v<std::remove_cv_t<decltype(result)>, Queue<long long, 4>>);
    STATIC_REQUIRE(equal(result, Queue<long long, 4>{7LL, 1LL, 2LL}));
    STATIC_REQUIRE(equal(destination, Queue<long long, 4>{7LL}));
    STATIC_REQUIRE(fits_into(destination, source));
    STATIC_REQUIRE(noexcept(into(destination, source)));
    STATIC_REQUIRE(noexcept(fits_into(destination, source)));
    STATIC_REQUIRE(equal(into(Queue<int, 0>{}, Range{0, 2}), Queue<int, 0>{}));
    STATIC_REQUIRE(fits_into(Queue<int, 0>{}, Vector<int, 0>{}));
    STATIC_REQUIRE_FALSE(fits_into(Queue<int, 0>{}, Range{0, 1}));

    volatile int runtime_end = 3;
    const auto runtime_source = Range{0, runtime_end};
    CHECK(equal(into(destination, runtime_source), Queue<long long, 4>{7LL, 0LL, 1LL, 2LL}));
    CHECK(fits_into(destination, runtime_source));
    const auto full = Queue<int, 2>{9, 8};
    CHECK(equal(into(full, runtime_source), full));
    CHECK_FALSE(fits_into(full, runtime_source));
    CHECK(fits_into(full, Vector<int, 0>{}));
    CHECK_FALSE(fits_into(destination, repeat(1)));
    CHECK(equal(into(Queue<int, 3>{9}, repeat(1)), Queue<int, 3>{9, 1, 1}));
}

TEST_CASE("Set materialization deduplicates before measuring capacity", "[into][fits-into]") {
    using namespace cljonic;

    TRACE_ID("invariant.ProducerMaterialization.IntoSetInsertsDistinctValues");
    TRACE_ID("invariant.ProducerMaterialization.IntoSetAcceptsDuplicateValuesAsNoOpsWhenFull");
    TRACE_ID("invariant.ProducerMaterialization.IntoStopsBeforeFirstCapacityFailureWithoutSkipping");
    TRACE_ID("invariant.ProducerMaterialization.FiniteSetAndMapPreflightAccountsForExistingAndPriorSourceValues");
    TRACE_ID("invariant.ProducerMaterialization.FitsIntoPreservesInputValues");
    constexpr auto destination = Set<long long, 2>{1LL};
    constexpr auto source = Vector<short, 5>{short{1}, short{2}, short{2}, short{1}, short{2}};
    constexpr auto result = into(destination, source);
    STATIC_REQUIRE(equal(result, Set<long long, 2>{1LL, 2LL}));
    STATIC_REQUIRE(fits_into(destination, source));
    STATIC_REQUIRE(equal(destination, Set<long long, 2>{1LL}));
    STATIC_REQUIRE(source.count() == 5U);
    STATIC_REQUIRE(noexcept(into(destination, source)));
    STATIC_REQUIRE(noexcept(fits_into(destination, source)));
    STATIC_REQUIRE(fits_into(Set<int, 0>{}, Vector<int, 0>{}));
    STATIC_REQUIRE_FALSE(fits_into(Set<int, 0>{}, Vector<int, 1>{1}));
    STATIC_REQUIRE(into(Set<int, 0>{}, Vector<int, 1>{1}).count() == 0U);
    STATIC_REQUIRE(equal(into(Set<char, 2>{'a'}, String<4>{"abba"}), Set<char, 2>{'a', 'b'}));

    volatile short runtime_value = 2;
    const auto runtime_source = Vector<short, 5>{short{1}, runtime_value, runtime_value, short{1}, runtime_value};
    CHECK(equal(into(destination, runtime_source), result));
    CHECK(fits_into(destination, runtime_source));
    CHECK(equal(destination, Set<long long, 2>{1LL}));
    CHECK(equal(runtime_source, source));
    const auto full = Set<int, 2>{1, 2};
    CHECK(fits_into(full, Vector<int, 3>{2, 1, 2}));
    CHECK(equal(into(full, Vector<int, 3>{2, 1, 2}), full));
    CHECK_FALSE(fits_into(full, Vector<int, 3>{2, 3, 1}));
    CHECK(equal(into(full, Vector<int, 3>{2, 3, 1}), full));
    CHECK(fits_into(full, Vector<int, 0>{}));
    CHECK_FALSE(fits_into(Set<int, 0>{}, Vector<int, 1>{1}));
}

TEST_CASE("Map materialization replaces keys in source order without skipping capacity failures", "[into][fits-into]") {
    using namespace cljonic;
    using Entry = MapEntry<int, int>;

    TRACE_ID("invariant.ProducerMaterialization.IntoMapReplacesMatchingKeysWithLastValue");
    TRACE_ID("invariant.ProducerMaterialization.IntoFoldsValuesInSourceTraversalOrder");
    TRACE_ID("invariant.ProducerMaterialization.IntoMapAcceptsExistingKeyEntriesAsReplacementsWhenFull");
    TRACE_ID("invariant.ProducerMaterialization.IntoStopsBeforeFirstCapacityFailureWithoutSkipping");
    TRACE_ID("invariant.ProducerMaterialization.FiniteSetAndMapPreflightAccountsForExistingAndPriorSourceValues");
    constexpr auto destination = Map<int, int, 2>{Entry{1, 10}};
    constexpr auto source = Vector<Entry, 4>{Entry{1, 11}, Entry{2, 20}, Entry{2, 22}, Entry{1, 12}};
    constexpr auto result = into(destination, source);
    STATIC_REQUIRE(result.count() == 2U);
    STATIC_REQUIRE(result(1) == 12);
    STATIC_REQUIRE(result(2) == 22);
    STATIC_REQUIRE(fits_into(destination, source));
    STATIC_REQUIRE(destination(1) == 10);
    STATIC_REQUIRE(destination.count() == 1U);
    STATIC_REQUIRE(noexcept(into(destination, source)));
    STATIC_REQUIRE(noexcept(fits_into(destination, source)));
    STATIC_REQUIRE(fits_into(Map<int, int, 0>{}, Vector<Entry, 0>{}));
    STATIC_REQUIRE_FALSE(fits_into(Map<int, int, 0>{}, Vector<Entry, 1>{Entry{1, 10}}));
    STATIC_REQUIRE(into(Map<int, int, 0>{}, source).count() == 0U);

    volatile int runtime_value = 11;
    const auto updates_then_failure = Vector<Entry, 3>{Entry{1, runtime_value}, Entry{3, 30}, Entry{2, 99}};
    const auto full = Map<int, int, 2>{Entry{1, 10}, Entry{2, 20}};
    const auto prefix = into(full, updates_then_failure);
    CHECK(prefix(1) == 11);
    CHECK(prefix(2) == 20);
    CHECK(prefix.count() == 2U);
    CHECK_FALSE(fits_into(full, updates_then_failure));
    CHECK(full(1) == 10);
    CHECK(full(2) == 20);
    CHECK(updates_then_failure(2U).value == 99);
    CHECK(equal(into(destination, source), result));
    CHECK(fits_into(destination, source));
    CHECK(fits_into(full, Vector<Entry, 0>{}));
    CHECK_FALSE(fits_into(Map<int, int, 0>{}, source));
    const auto map_source = Map<int, int, 2>{Entry{1, runtime_value}, Entry{2, 20}};
    CHECK(equal(into(destination, map_source), map_source));
    CHECK(fits_into(destination, map_source));
    CHECK(fits_into(full, repeat(3, Entry{1, 50})));
    CHECK(into(full, repeat(3, Entry{1, 50}))(1) == 50);
}

TEST_CASE("value-sensitive preflight and materialization independently evaluate finite producers",
          "[into][fits-into]") {
    using namespace cljonic;
    using Entry = MapEntry<int, int>;

    TRACE_ID("invariant.ProducerMaterialization.FiniteSetAndMapPreflightMayEvaluateCallbacks");
    TRACE_ID("invariant.ProducerMaterialization.FitsIntoAndIntoDoNotShareRealizationCache");
    TRACE_ID("invariant.ProducerMaterialization.FitsIntoPredictionRequiresEquivalentSourceValues");
    TRACE_ID("invariant.ProducerMaterialization.FitsIntoPreservesInputValues");
    int calls = 0;
    const auto source = repeatedly(4, [&calls, value = 0] mutable noexcept {
        ++calls;
        return ++value % 2;
    });
    const auto destination = Set<int, 2>{};
    CHECK(fits_into(destination, source));
    CHECK(calls == 4);
    CHECK(equal(into(destination, source), Set<int, 2>{0, 1}));
    CHECK(calls == 8);
    CHECK(destination.count() == 0U);
    CHECK(source.count() == 4U);

    calls = 0;
    const auto failing_source = repeatedly(4, [&calls] noexcept { return ++calls; });
    CHECK_FALSE(fits_into(Set<int, 1>{}, failing_source));
    CHECK(calls == 2);

    calls = 0;
    const auto changing_source = repeatedly(1, [&calls] noexcept { return ++calls; });
    const auto full = Set<int, 1>{1};
    CHECK(fits_into(full, changing_source));
    CHECK(equal(into(full, changing_source), full));
    CHECK(calls == 2);
    CHECK_FALSE(fits_into(full, changing_source));
    CHECK(calls == 3);

    calls = 0;
    const auto entries = repeatedly(3, [&calls] noexcept { return Entry{1, ++calls}; });
    CHECK(fits_into(Map<int, int, 1>{}, entries));
    CHECK(calls == 3);
    CHECK(into(Map<int, int, 1>{}, entries)(1) == 6);
    CHECK(calls == 6);
    CHECK_FALSE(fits_into(Map<int, int, 0>{}, entries));
    CHECK(calls == 7);
    CHECK(fits_into(Set<int, 0>{}, repeatedly(0, [&calls] noexcept { return ++calls; })));
    CHECK(calls == 7);
}

TEST_CASE("unbounded duplicate and replacement materialization stays traversal bounded", "[into][fits-into]") {
    using namespace cljonic;
    using Entry = MapEntry<int, int>;

    TRACE_ID("invariant.ProducerMaterialization.UnboundedMaterializationNeverExceedsSourceTraversalCap");
    TRACE_ID("invariant.ProducerMaterialization.UnboundedSourceDoesNotFitIntoDestination");
    int calls = 0;
    const auto duplicates = repeatedly([&calls] noexcept {
        ++calls;
        return 7;
    });
    const auto full_set = Set<int, 1>{7};
    CHECK_FALSE(fits_into(full_set, duplicates));
    CHECK(calls == 0);
    CHECK(equal(into(full_set, duplicates), full_set));
    CHECK(static_cast<std::size_t>(calls) == duplicates.count());

    calls = 0;
    const auto updates = repeatedly([&calls] noexcept { return Entry{1, ++calls}; });
    const auto full_map = Map<int, int, 1>{Entry{1, 0}};
    CHECK_FALSE(fits_into(full_map, updates));
    CHECK(calls == 0);
    const auto result = into(full_map, updates);
    CHECK(static_cast<std::size_t>(calls) == updates.count());
    CHECK(result(1) == calls);
    CHECK(full_map(1) == 0);
    CHECK_FALSE(fits_into(Set<int, 0>{}, duplicates));
    CHECK_FALSE(fits_into(Map<int, int, 0>{}, updates));
}
