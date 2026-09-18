#include "cljonic-test-api.hpp"
#include <catch2/catch_test_macros.hpp>
#include <concepts>

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

template <typename T>
concept HasContains = requires(const T& value) { value.contains(0U); };

template <typename T>
concept HasCallableLookup = requires(const T& value) { value(0U); };

template <typename T>
concept HasGet = requires(const T& value) { cljonic::get(value, 0U); };

TEST_CASE("Repeat owns a value and materializes finite and unbounded forms", "[repeat]") {
    using cljonic::fits_into;
    using cljonic::into;
    using cljonic::repeat;
    using cljonic::Vector;
    using cljonic::concepts::CljonicCollection;
    using cljonic::concepts::CljonicProducer;
    using cljonic::concepts::CljonicSource;
    using cljonic::concepts::IndexedProducer;
    using cljonic::concepts::SequenceableProducer;
    using cljonic::concepts::StableEqualityComparable;

    TRACE_ID("entity-fields.Repeat");
    TRACE_ID("invariant.Repeat.HeaderOnlyDistribution");
    TRACE_ID("invariant.Repeat.NoHeapAllocation");
    TRACE_ID("invariant.Repeat.NoRtti");
    TRACE_ID("invariant.Repeat.NoExceptions");
    TRACE_ID("invariant.Repeat.SingleThreadedExecutionModel");
    TRACE_ID("invariant.Repeat.OwnsRepeatedValue");
    TRACE_ID("invariant.Repeat.DoesNotOwnResultStorage");
    TRACE_ID("invariant.Repeat.DoesNotBorrowSourceOrCallback");
    TRACE_ID("invariant.Repeat.ReferentialTransparency");
    TRACE_ID("invariant.Repeat.UncountedFormIsUnbounded");
    TRACE_ID("invariant.Repeat.CountedFormIsFinite");
    TRACE_ID("invariant.Repeat.ZeroCountProducesEmptyResult");
    TRACE_ID("invariant.Repeat.ExposesCountAndIsFinite");
    TRACE_ID("invariant.Repeat.CountedFormUsesRuntimeCountForMaterialization");
    TRACE_ID("invariant.Repeat.CountedFormCountIsExact");
    TRACE_ID("invariant.Repeat.UncountedFormCountIsObservableTraversalCap");
    TRACE_ID("invariant.Repeat.ConstTraversalTerminatesAtCount");
    TRACE_ID("invariant.Repeat.UncountedFormMaterializesAtMostDestinationCapacity");
    TRACE_ID("invariant.Repeat.UncountedFormDoesNotFitIntoDestination");
    TRACE_ID("invariant.Repeat.UnboundedRepeatDoesNotSatisfyStableEquality");
    TRACE_ID("invariant.Repeat.UnboundedEqualityFailsAtCompileTime");
    TRACE_ID("invariant.Repeat.DoesNotSatisfyIndexedProducer");
    TRACE_ID("invariant.Repeat.SatisfiesSequenceableProducer");
    TRACE_ID("invariant.Repeat.DoesNotExposeCallableOperator");
    TRACE_ID("invariant.Repeat.GetContainsAndLookupAreExcluded");

    constexpr Vector<int, 4> empty_destination{};
    constexpr auto finite_repeat = repeat(7, 3U);
    STATIC_REQUIRE(finite_repeat.count() == 3U);
    STATIC_REQUIRE(finite_repeat.is_finite());
    constexpr auto finite_result = into(empty_destination, finite_repeat);
    STATIC_REQUIRE(finite_result.count() == 3U);
    STATIC_REQUIRE(finite_result(0U) == 7);
    STATIC_REQUIRE(finite_result(2U) == 7);
    STATIC_REQUIRE(fits_into(empty_destination, finite_repeat));

    constexpr auto empty_repeat = repeat(7, 0U);
    STATIC_REQUIRE(empty_repeat.count() == 0U);
    STATIC_REQUIRE(empty_repeat.is_finite());
    constexpr auto empty_result = into(empty_destination, empty_repeat);
    STATIC_REQUIRE(empty_result.count() == 0U);
    STATIC_REQUIRE(fits_into(empty_destination, empty_repeat));

    constexpr auto unbounded_repeat = repeat(7);
    STATIC_REQUIRE(unbounded_repeat.count() == cljonic::CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE);
    STATIC_REQUIRE_FALSE(unbounded_repeat.is_finite());
    constexpr auto bounded_prefix_result = into(empty_destination, unbounded_repeat);
    STATIC_REQUIRE(bounded_prefix_result.count() == empty_destination.capacity());
    STATIC_REQUIRE(bounded_prefix_result(0U) == 7);
    STATIC_REQUIRE(bounded_prefix_result(3U) == 7);
    STATIC_REQUIRE_FALSE(fits_into(empty_destination, unbounded_repeat));

    constexpr Vector<int, 3> populated_destination{1, 2};
    constexpr auto appended_result = into(populated_destination, finite_repeat);
    STATIC_REQUIRE(appended_result.count() == 3U);
    STATIC_REQUIRE(appended_result(0U) == 1);
    STATIC_REQUIRE(appended_result(1U) == 2);
    STATIC_REQUIRE(appended_result(2U) == 7);
    STATIC_REQUIRE_FALSE(fits_into(populated_destination, finite_repeat));

    STATIC_REQUIRE(CljonicProducer<decltype(finite_repeat)>);
    STATIC_REQUIRE(CljonicSource<decltype(finite_repeat)>);
    STATIC_REQUIRE_FALSE(CljonicCollection<decltype(finite_repeat)>);
    STATIC_REQUIRE_FALSE(IndexedProducer<decltype(finite_repeat)>);
    STATIC_REQUIRE(SequenceableProducer<decltype(finite_repeat)>);
    STATIC_REQUIRE_FALSE(StableEqualityComparable<decltype(unbounded_repeat)>);
    STATIC_REQUIRE_FALSE(std::invocable<decltype(finite_repeat), std::size_t>);
    STATIC_REQUIRE_FALSE(HasContains<decltype(finite_repeat)>);
    STATIC_REQUIRE_FALSE(HasCallableLookup<decltype(finite_repeat)>);
    STATIC_REQUIRE_FALSE(HasGet<decltype(finite_repeat)>);

    auto mutable_value = 7;
    const auto owning_repeat = repeat(mutable_value, 2U);
    mutable_value = 9;
    const auto owning_result = into(Vector<int, 2>{}, owning_repeat);
    CHECK(owning_result(0U) == 7);
    CHECK(owning_result(1U) == 7);

    const auto runtime_finite_repeat = repeat(11, 2U);
    CHECK(runtime_finite_repeat.count() == 2U);
    CHECK(runtime_finite_repeat.is_finite());
    const auto runtime_finite_result = into(Vector<int, 4>{}, runtime_finite_repeat);
    CHECK(runtime_finite_result.count() == 2U);
    CHECK(runtime_finite_result(0U) == 11);
    CHECK(runtime_finite_result(1U) == 11);

    const auto runtime_unbounded_repeat = repeat(13);
    CHECK(runtime_unbounded_repeat.count() == cljonic::CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE);
    CHECK_FALSE(runtime_unbounded_repeat.is_finite());
    const auto runtime_prefix_result = into(Vector<int, 2>{}, runtime_unbounded_repeat);
    CHECK(runtime_prefix_result.count() == 2U);
    CHECK(runtime_prefix_result(0U) == 13);
    CHECK(runtime_prefix_result(1U) == 13);
    CHECK_FALSE(fits_into(Vector<int, 2>{}, runtime_unbounded_repeat));
}