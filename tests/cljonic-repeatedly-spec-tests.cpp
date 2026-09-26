#include "cljonic-test-api.hpp"
#include <catch2/catch_test_macros.hpp>
#include <concepts>
#include <utility>

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

template <typename T>
concept HasContains = requires(const T& value) { value.contains(0U); };

template <typename T>
concept HasCallableLookup = requires(const T& value) { value(0U); };

template <typename T>
concept HasGet = requires(const T& value) { cljonic::get(value, 0U); };

template <typename Step>
concept CanRepeatedly = requires(Step step) { cljonic::repeatedly(step); };

template <typename Element, typename Step>
concept CanRepeatedlyWith = requires(Step step, std::size_t count) { cljonic::repeatedly(step, count); };

constexpr auto five() noexcept -> int {
    return 5;
}

struct ThrowingElement {
    int value{};

    ThrowingElement() = default;

    ThrowingElement(const ThrowingElement& other) noexcept(false) : value(other.value) {
    }

    auto operator=(const ThrowingElement& other) noexcept(false) -> ThrowingElement& {
        value = other.value;
        return *this;
    }
};

struct ThrowingElementStep {
    auto operator()() const noexcept -> ThrowingElement {
        return ThrowingElement{};
    }
};

struct MoveOnlyStep {
    MoveOnlyStep() = default;
    MoveOnlyStep(const MoveOnlyStep&) = delete;
    MoveOnlyStep(MoveOnlyStep&&) = default;
    constexpr auto operator()() const noexcept -> int {
        return 1;
    }
};

struct ThrowingStep {
    constexpr auto operator()() const -> int {
        return 1;
    }
};

struct NonConstStep {
    constexpr auto operator()() noexcept -> int {
        return 1;
    }
};

TEST_CASE("Repeatedly owns a copyable zero-argument step and exposes counted uncounted traversal", "[repeatedly]") {
    using cljonic::fits_into;
    using cljonic::into;
    using cljonic::repeatedly;
    using cljonic::Vector;
    using cljonic::concepts::CljonicProducer;
    using cljonic::concepts::CljonicRepeatedly;
    using cljonic::concepts::CljonicSource;
    using cljonic::concepts::IndexedProducer;
    using cljonic::concepts::SequenceableProducer;
    using cljonic::concepts::StableEqualityComparable;

    TRACE_ID("entity-fields.Repeatedly");
    TRACE_ID("invariant.Repeatedly.HeaderOnlyDistribution");
    TRACE_ID("invariant.Repeatedly.NoHeapAllocation");
    TRACE_ID("invariant.Repeatedly.NoRtti");
    TRACE_ID("invariant.Repeatedly.NoExceptions");
    TRACE_ID("invariant.Repeatedly.SingleThreadedExecutionModel");
    TRACE_ID("invariant.Repeatedly.ElementSatisfiesNothrowCollectionElement");
    TRACE_ID("invariant.Repeatedly.OwnsStepCallback");
    TRACE_ID("invariant.Repeatedly.FactoryStoresDecayedStep");
    TRACE_ID("invariant.Repeatedly.AcceptsNamedFunctionLambdaAndFunctionObject");
    TRACE_ID("invariant.Repeatedly.StepIsCopyConstructible");
    TRACE_ID("invariant.Repeatedly.StepMayBeMoveConstructedFromRvalue");
    TRACE_ID("invariant.Repeatedly.StepConstInvocationProducesElementOfT");
    TRACE_ID("invariant.Repeatedly.StepInvocationIsNonThrowing");
    TRACE_ID("invariant.Repeatedly.DoesNotOwnResultStorage");
    TRACE_ID("invariant.Repeatedly.DoesNotBorrowStepCallback");
    TRACE_ID("invariant.Repeatedly.StepMayMaintainStateOrSideEffects");
    TRACE_ID("invariant.Repeatedly.UncountedFormIsUnbounded");
    TRACE_ID("invariant.Repeatedly.CountedFormIsFinite");
    TRACE_ID("invariant.Repeatedly.ZeroCountProducesEmptyResult");
    TRACE_ID("invariant.Repeatedly.ExposesCountAndIsFinite");
    TRACE_ID("invariant.Repeatedly.CountedFormUsesRuntimeCountForMaterialization");
    TRACE_ID("invariant.Repeatedly.CountedFormCountIsExact");
    TRACE_ID("invariant.Repeatedly.UncountedFormCountIsObservableTraversalCap");
    TRACE_ID("invariant.Repeatedly.ConstTraversalTerminatesAtCount");
    TRACE_ID("invariant.Repeatedly.UncountedFormMaterializesAtMostDestinationCapacity");
    TRACE_ID("invariant.Repeatedly.UncountedFormDoesNotFitIntoDestination");
    TRACE_ID("invariant.Repeatedly.CallbackInvokedOncePerProducedElement");
    TRACE_ID("invariant.Repeatedly.ConstructionDoesNotInvokeStep");
    TRACE_ID("invariant.Repeatedly.CallableStepExcludesParameterEquality");
    TRACE_ID("invariant.Repeatedly.UnboundedEqualityFailsAtCompileTime");
    TRACE_ID("invariant.Repeatedly.DoesNotSatisfyIndexedProducer");
    TRACE_ID("invariant.Repeatedly.SatisfiesSequenceableProducer");
    TRACE_ID("invariant.Repeatedly.DoesNotExposeCallableOperator");
    TRACE_ID("invariant.Repeatedly.GetContainsAndLookupAreExcluded");

    constexpr auto step = []() constexpr noexcept { return 5; };
    constexpr auto counted = repeatedly(3U, step);
    constexpr auto named_counted = repeatedly(2U, five);
    constexpr auto uncounted = repeatedly(step);
    STATIC_REQUIRE(counted.count() == 3U);
    STATIC_REQUIRE(counted.is_finite());
    STATIC_REQUIRE(uncounted.count() == cljonic::CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE);
    STATIC_REQUIRE_FALSE(uncounted.is_finite());

    constexpr auto counted_result = into(Vector<int, 4>{}, counted);
    STATIC_REQUIRE(counted_result.count() == 3U);
    STATIC_REQUIRE(counted_result(0U) == 5);
    STATIC_REQUIRE(counted_result(1U) == 5);
    STATIC_REQUIRE(counted_result(2U) == 5);

    constexpr auto oversized_result = into(Vector<int, 2>{}, repeatedly(3U, step));
    STATIC_REQUIRE(oversized_result.count() == 2U);
    STATIC_REQUIRE(oversized_result(0U) == 5);
    STATIC_REQUIRE(oversized_result(1U) == 5);

    constexpr auto empty_result = into(Vector<int, 4>{}, repeatedly(0U, step));
    STATIC_REQUIRE(empty_result.count() == 0U);
    STATIC_REQUIRE(fits_into(Vector<int, 4>{}, counted));
    STATIC_REQUIRE_FALSE(fits_into(Vector<int, 2>{}, repeatedly(3U, step)));
    STATIC_REQUIRE_FALSE(fits_into(Vector<int, 4>{}, uncounted));

    constexpr auto uncounted_result = into(Vector<int, 3>{}, uncounted);
    STATIC_REQUIRE(uncounted_result.count() == 3U);
    STATIC_REQUIRE(uncounted_result(0U) == 5);
    STATIC_REQUIRE(uncounted_result(1U) == 5);
    STATIC_REQUIRE(uncounted_result(2U) == 5);

    constexpr auto named_result = into(Vector<int, 2>{}, named_counted);
    STATIC_REQUIRE(named_result.count() == 2U);
    STATIC_REQUIRE(named_result(0U) == 5);
    STATIC_REQUIRE(named_result(1U) == 5);

    STATIC_REQUIRE(CljonicProducer<decltype(counted)>);
    STATIC_REQUIRE(CljonicRepeatedly<decltype(counted)>);
    STATIC_REQUIRE(CljonicSource<decltype(counted)>);
    STATIC_REQUIRE_FALSE(IndexedProducer<decltype(counted)>);
    STATIC_REQUIRE(SequenceableProducer<decltype(counted)>);
    STATIC_REQUIRE_FALSE(StableEqualityComparable<decltype(uncounted)>);
    STATIC_REQUIRE_FALSE(std::invocable<decltype(uncounted), std::size_t>);
    STATIC_REQUIRE_FALSE(HasContains<decltype(uncounted)>);
    STATIC_REQUIRE_FALSE(HasCallableLookup<decltype(uncounted)>);
    STATIC_REQUIRE_FALSE(HasGet<decltype(uncounted)>);
    STATIC_REQUIRE_FALSE(CanRepeatedly<MoveOnlyStep>);
    STATIC_REQUIRE_FALSE(CanRepeatedly<ThrowingStep>);
    STATIC_REQUIRE_FALSE(CanRepeatedly<NonConstStep>);
    STATIC_REQUIRE_FALSE(CanRepeatedlyWith<ThrowingElement, ThrowingElementStep>);

    constexpr auto copied_counted = counted;
    constexpr auto copied_result = into(Vector<int, 4>{}, copied_counted);
    STATIC_REQUIRE(copied_result.count() == 3U);
    STATIC_REQUIRE(copied_result(0U) == 5);
    STATIC_REQUIRE(copied_result(1U) == 5);
    STATIC_REQUIRE(copied_result(2U) == 5);

    int invocation_count = 0;
    const auto runtime_counted = repeatedly(4U, [&invocation_count]() noexcept {
        ++invocation_count;
        return invocation_count;
    });
    const auto runtime_result = into(Vector<int, 5>{}, runtime_counted);
    CHECK(runtime_result.count() == 4U);
    CHECK(runtime_result(0U) == 1);
    CHECK(runtime_result(1U) == 2);
    CHECK(runtime_result(2U) == 3);
    CHECK(runtime_result(3U) == 4);
    CHECK(invocation_count == 4);

    int empty_invocation_count = 0;
    const auto runtime_empty = repeatedly(0U, [&empty_invocation_count]() noexcept {
        ++empty_invocation_count;
        return 1;
    });
    const auto runtime_empty_result = into(Vector<int, 2>{}, runtime_empty);
    CHECK(runtime_empty_result.count() == 0U);
    CHECK(empty_invocation_count == 0);

    const auto runtime_uncounted = repeatedly([]() noexcept { return 9; });
    const auto runtime_unbounded_result = into(Vector<int, 3>{}, runtime_uncounted);
    CHECK(runtime_unbounded_result.count() == 3U);
    CHECK(runtime_unbounded_result(0U) == 9);
    CHECK(runtime_unbounded_result(1U) == 9);
    CHECK(runtime_unbounded_result(2U) == 9);
    CHECK_FALSE(fits_into(Vector<int, 3>{}, runtime_uncounted));
}