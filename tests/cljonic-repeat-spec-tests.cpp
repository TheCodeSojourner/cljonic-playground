#include <catch2/catch_test_macros.hpp>
#include <concepts>
#include <cstddef>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

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
    TRACE_ID("invariant.Repeat.CountedFormUsesRuntimeCountForMaterialization");
    TRACE_ID("invariant.Repeat.UncountedFormMaterializesAtMostDestinationCapacity");
    TRACE_ID("invariant.Repeat.UncountedFormDoesNotFitIntoDestination");
    TRACE_ID("invariant.Repeat.UnboundedRepeatDoesNotSatisfyStableEquality");
    TRACE_ID("invariant.Repeat.UnboundedEqualityFailsAtCompileTime");
    TRACE_ID("invariant.Repeat.DoesNotSatisfyIndexedProducer");
    TRACE_ID("invariant.Repeat.DoesNotSatisfySequenceableProducer");
    TRACE_ID("invariant.Repeat.DoesNotExposeCallableOperator");
    TRACE_ID("invariant.Repeat.GetContainsAndLookupAreExcluded");

    constexpr Vector<int, 4> empty_destination{};
    constexpr auto finite_repeat = repeat(7, 3U);
    constexpr auto finite_result = into(empty_destination, finite_repeat);
    STATIC_REQUIRE(finite_result.count() == 3U);
    STATIC_REQUIRE(finite_result(0U) == 7);
    STATIC_REQUIRE(finite_result(2U) == 7);
    STATIC_REQUIRE(fits_into(empty_destination, finite_repeat));

    constexpr auto empty_repeat = repeat(7, 0U);
    constexpr auto empty_result = into(empty_destination, empty_repeat);
    STATIC_REQUIRE(empty_result.count() == 0U);
    STATIC_REQUIRE(fits_into(empty_destination, empty_repeat));

    constexpr auto unbounded_repeat = repeat(7);
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
    STATIC_REQUIRE_FALSE(SequenceableProducer<decltype(finite_repeat)>);
    STATIC_REQUIRE_FALSE(StableEqualityComparable<decltype(unbounded_repeat)>);
    STATIC_REQUIRE_FALSE(std::invocable<decltype(finite_repeat), std::size_t>);
    STATIC_REQUIRE_FALSE(requires { finite_repeat.count(); });
    STATIC_REQUIRE_FALSE(requires { finite_repeat.contains(0U); });
    STATIC_REQUIRE_FALSE(requires { finite_repeat(0U); });
    STATIC_REQUIRE_FALSE(requires { cljonic::get(finite_repeat, 0U); });

    auto mutable_value = 7;
    const auto owning_repeat = repeat(mutable_value, 2U);
    mutable_value = 9;
    const auto owning_result = into(Vector<int, 2>{}, owning_repeat);
    CHECK(owning_result(0U) == 7);
    CHECK(owning_result(1U) == 7);

    const auto runtime_finite_repeat = repeat(11, 2U);
    const auto runtime_finite_result = into(Vector<int, 4>{}, runtime_finite_repeat);
    CHECK(runtime_finite_result.count() == 2U);
    CHECK(runtime_finite_result(0U) == 11);
    CHECK(runtime_finite_result(1U) == 11);

    const auto runtime_unbounded_repeat = repeat(13);
    const auto runtime_prefix_result = into(Vector<int, 2>{}, runtime_unbounded_repeat);
    CHECK(runtime_prefix_result.count() == 2U);
    CHECK(runtime_prefix_result(0U) == 13);
    CHECK(runtime_prefix_result(1U) == 13);
    CHECK_FALSE(fits_into(Vector<int, 2>{}, runtime_unbounded_repeat));
}