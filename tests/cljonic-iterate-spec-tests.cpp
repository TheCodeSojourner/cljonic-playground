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
concept CanIterate = requires(Step step) { cljonic::iterate(step, 0); };

template <typename Element, typename Step>
concept CanIterateWith = requires(Step step, Element value) { cljonic::iterate(step, value); };

constexpr auto add_one(int value) noexcept -> int {
    return value + 1;
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
    auto operator()(ThrowingElement value) const noexcept -> ThrowingElement {
        return value;
    }
};

struct MoveOnlyStep {
    MoveOnlyStep() = default;
    MoveOnlyStep(const MoveOnlyStep&) = delete;
    MoveOnlyStep(MoveOnlyStep&&) = default;
    constexpr auto operator()(int value) const noexcept -> int {
        return value + 1;
    }
};

struct ThrowingStep {
    constexpr auto operator()(int value) const -> int {
        return value + 1;
    }
};

struct NonConstStep {
    constexpr auto operator()(int value) noexcept -> int {
        return value + 1;
    }
};

struct WrongResultStep {
    constexpr auto operator()(int value) const noexcept -> long {
        return value + 1;
    }
};

TEST_CASE("Iterate owns a copyable step and exposes bounded unbounded traversal", "[iterate]") {
    using cljonic::fits_into;
    using cljonic::into;
    using cljonic::iterate;
    using cljonic::Vector;
    using cljonic::concepts::CljonicProducer;
    using cljonic::concepts::CljonicSource;
    using cljonic::concepts::IndexedProducer;
    using cljonic::concepts::SequenceableProducer;
    using cljonic::concepts::StableEqualityComparable;

    TRACE_ID("entity-fields.Iterate");
    TRACE_ID("invariant.Iterate.HeaderOnlyDistribution");
    TRACE_ID("invariant.Iterate.NoHeapAllocation");
    TRACE_ID("invariant.Iterate.NoRtti");
    TRACE_ID("invariant.Iterate.NoExceptions");
    TRACE_ID("invariant.Iterate.SingleThreadedExecutionModel");
    TRACE_ID("invariant.Iterate.OwnsInitialValue");
    TRACE_ID("invariant.Iterate.ElementSatisfiesNothrowCollectionElement");
    TRACE_ID("invariant.Iterate.OwnsStepCallback");
    TRACE_ID("invariant.Iterate.FactoryStoresDecayedStep");
    TRACE_ID("invariant.Iterate.AcceptsNamedFunctionLambdaAndFunctionObject");
    TRACE_ID("invariant.Iterate.StepIsCopyConstructible");
    TRACE_ID("invariant.Iterate.StepMayBeMoveConstructedFromRvalue");
    TRACE_ID("invariant.Iterate.StepConstInvocationAcceptsValueOfT");
    TRACE_ID("invariant.Iterate.StepConstInvocationReturnsExactT");
    TRACE_ID("invariant.Iterate.StepInvocationIsNonThrowing");
    TRACE_ID("invariant.Iterate.DoesNotOwnResultStorage");
    TRACE_ID("invariant.Iterate.DoesNotBorrowInitialOrStep");
    TRACE_ID("invariant.Iterate.ReferentialTransparency");
    TRACE_ID("invariant.Iterate.TwoArgumentFormOnly");
    TRACE_ID("invariant.Iterate.EmitsInitialFirst");
    TRACE_ID("invariant.Iterate.AppliesStepToPreviousValue");
    TRACE_ID("invariant.Iterate.IsUnbounded");
    TRACE_ID("invariant.Iterate.ExposesCountAndIsFinite");
    TRACE_ID("invariant.Iterate.CountIsObservableTraversalCap");
    TRACE_ID("invariant.Iterate.ConstTraversalTerminatesAtMaximumCap");
    TRACE_ID("invariant.Iterate.MaterializesAtMostDestinationCapacity");
    TRACE_ID("invariant.Iterate.DoesNotFitIntoDestination");
    TRACE_ID("invariant.Iterate.SatisfiesCljonicSource");
    TRACE_ID("invariant.Iterate.DirectSourceConsumptionIsSupported");
    TRACE_ID("invariant.Iterate.ConstexprCapableWhenArgumentsAre");
    TRACE_ID("invariant.Iterate.RuntimeUseIsSupported");
    TRACE_ID("invariant.Iterate.ConstructionDoesNotInvokeStep");
    TRACE_ID("invariant.Iterate.CallableStepExcludesParameterEquality");
    TRACE_ID("invariant.Iterate.UnboundedEqualityFailsAtCompileTime");
    TRACE_ID("invariant.Iterate.DoesNotSatisfyIndexedProducer");
    TRACE_ID("invariant.Iterate.SatisfiesSequenceableProducer");
    TRACE_ID("invariant.Iterate.DoesNotExposeCallableOperator");
    TRACE_ID("invariant.Iterate.GetContainsAndLookupAreExcluded");

    constexpr auto step = [](int value) constexpr noexcept { return value + 1; };
    constexpr auto sequence = iterate(step, 0);
    constexpr auto named_sequence = iterate(add_one, 0);
    STATIC_REQUIRE(sequence.count() == cljonic::CLJONIC_COLLECTION_MAXIMUM_ELEMENT_COUNT_VALUE);
    STATIC_REQUIRE_FALSE(sequence.is_finite());

    constexpr auto result = into(Vector<int, 4>{}, sequence);
    STATIC_REQUIRE(result.count() == 4U);
    STATIC_REQUIRE(result(0U) == 0);
    STATIC_REQUIRE(result(1U) == 1);
    STATIC_REQUIRE(result(2U) == 2);
    STATIC_REQUIRE(result(3U) == 3);
    STATIC_REQUIRE_FALSE(fits_into(Vector<int, 4>{}, sequence));
    constexpr auto named_result = into(Vector<int, 2>{}, named_sequence);
    STATIC_REQUIRE(named_result(0U) == 0);
    STATIC_REQUIRE(named_result(1U) == 1);

    STATIC_REQUIRE(CljonicProducer<decltype(sequence)>);
    STATIC_REQUIRE(CljonicSource<decltype(sequence)>);
    STATIC_REQUIRE_FALSE(IndexedProducer<decltype(sequence)>);
    STATIC_REQUIRE(SequenceableProducer<decltype(sequence)>);
    STATIC_REQUIRE_FALSE(StableEqualityComparable<decltype(sequence)>);
    STATIC_REQUIRE_FALSE(std::invocable<decltype(sequence), std::size_t>);
    STATIC_REQUIRE_FALSE(HasContains<decltype(sequence)>);
    STATIC_REQUIRE_FALSE(HasCallableLookup<decltype(sequence)>);
    STATIC_REQUIRE_FALSE(HasGet<decltype(sequence)>);
    STATIC_REQUIRE_FALSE(CanIterate<MoveOnlyStep>);
    STATIC_REQUIRE_FALSE(CanIterate<ThrowingStep>);
    STATIC_REQUIRE_FALSE(CanIterate<NonConstStep>);
    STATIC_REQUIRE_FALSE(CanIterate<WrongResultStep>);
    STATIC_REQUIRE_FALSE(CanIterateWith<ThrowingElement, ThrowingElementStep>);

    constexpr auto copied_sequence = sequence;
    constexpr auto copied_result = into(Vector<int, 3>{}, copied_sequence);
    STATIC_REQUIRE(copied_result(0U) == 0);
    STATIC_REQUIRE(copied_result(1U) == 1);
    STATIC_REQUIRE(copied_result(2U) == 2);

    const auto runtime_sequence = iterate([](int value) noexcept { return value * 2; }, 1);
    const auto runtime_result = into(Vector<int, 3>{}, runtime_sequence);
    CHECK(runtime_result.count() == 3U);
    CHECK(runtime_result(0U) == 1);
    CHECK(runtime_result(1U) == 2);
    CHECK(runtime_result(2U) == 4);
    CHECK_FALSE(fits_into(Vector<int, 3>{}, runtime_sequence));
}
