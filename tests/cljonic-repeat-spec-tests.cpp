#include "cljonic-test-api.hpp"
#include <catch2/catch_test_macros.hpp>
#include <concepts>
#include <limits>

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("Existing Repeat factories match their constructors", "[repeat][factory]") {
    using namespace cljonic;
    TRACE_ID("invariant.Repeat.ExistingFactoriesPreserveEquivalentConstructorResultTypeAndBehavior");
    STATIC_REQUIRE(std::same_as<decltype(repeat(7)), Repeat<int>>);
    STATIC_REQUIRE(std::same_as<decltype(repeat(3, 7L)), Repeat<long>>);
    STATIC_REQUIRE(noexcept(repeat(7)));
    STATIC_REQUIRE(noexcept(repeat(3, 7)));
    STATIC_REQUIRE(repeat(7) == Repeat{7});
    STATIC_REQUIRE(repeat(3, 7) == Repeat{3, 7});
    STATIC_REQUIRE(repeat(0, 7) == Repeat{0, 7});
    STATIC_REQUIRE(repeat(-2, 7) == Repeat{-2, 7});
    auto value = 7;
    const auto unbounded = repeat(value);
    const auto finite = repeat(3, value);
    value = 9;
    REQUIRE(unbounded == Repeat{7});
    REQUIRE(finite == Repeat{3, 7});
    REQUIRE(equal(into(vector<int, 3>(), finite), vector(7, 7, 7)));
    REQUIRE(value == 9);
}

template <typename T>
concept HasContains = requires(const T& value) { value.contains(0U); };

template <typename T>
concept HasCallableLookup = requires(const T& value) { value(0U); };

template <typename T>
concept GetDomainAdmissible = cljonic::concepts_detail::GetKeyAdmissible<T, unsigned int>;

TEST_CASE("Repeat owns a value and materializes finite and unbounded forms", "[repeat]") {
    using cljonic::fits_into;
    using cljonic::into;
    using cljonic::Repeat;
    using cljonic::repeat;
    using cljonic::Set;
    using cljonic::Vector;
    using cljonic::concepts::CljonicCollection;
    using cljonic::concepts::CljonicProducer;
    using cljonic::concepts::CljonicSource;
    using cljonic::concepts::IndexedProducer;
    using cljonic::concepts::NothrowStableEqualityComparable;
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
    TRACE_ID("invariant.Repeat.OperatorEqualsUsesProducerParameterEquality");
    TRACE_ID("invariant.Repeat.EqualityComparesValueCountAndForm");
    TRACE_ID("invariant.Repeat.SatisfiesNothrowStableEqualityComparable");
    TRACE_ID("entity-fields.ProducerParameterEquality");
    TRACE_ID("invariant.ProducerParameterEquality.ComparesBoundedStoredParametersOnly");
    TRACE_ID("invariant.ProducerParameterEquality.OperatorEqualsUsesProducerParameterEquality");
    TRACE_ID("invariant.ProducerParameterEquality.DoesNotImplySequenceEquality");
    TRACE_ID("invariant.ProducerParameterEquality.DistinctParametersCompareUnequalEvenWhenSequencesCoincide");
    TRACE_ID("invariant.ProducerParameterEquality.ProducerAdmittedAsMapKeyAndSetElement");
    TRACE_ID("invariant.Repeat.DoesNotSatisfyIndexedProducer");
    TRACE_ID("invariant.Repeat.SatisfiesSequenceableProducer");
    TRACE_ID("invariant.Repeat.DoesNotExposeCallableOperator");
    TRACE_ID("invariant.Repeat.GetContainsAndLookupAreExcluded");

    constexpr Vector<int, 4> empty_destination{};
    constexpr auto finite_repeat = repeat(3U, 7);
    STATIC_REQUIRE(finite_repeat.count() == 3U);
    STATIC_REQUIRE(finite_repeat.is_finite());
    constexpr auto finite_result = into(empty_destination, finite_repeat);
    STATIC_REQUIRE(finite_result.count() == 3U);
    STATIC_REQUIRE(finite_result(0U) == 7);
    STATIC_REQUIRE(finite_result(2U) == 7);
    STATIC_REQUIRE(fits_into(empty_destination, finite_repeat));

    constexpr auto empty_repeat = repeat(0U, 7);
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

    // Repeat satisfies producer parameter equality: operator== compares the
    // stored value, count, and finite-form parameters, never the produced
    // sequence. The unbounded form, the zero-count finite form, and a counted
    // form are pairwise distinct even when their produced prefixes coincide.
    constexpr auto counted_a = repeat(3U, 7);
    constexpr auto counted_b = repeat(3U, 7);
    constexpr auto counted_c = repeat(2U, 7);
    STATIC_REQUIRE(counted_a == counted_b);
    STATIC_REQUIRE(!(counted_a == counted_c));
    STATIC_REQUIRE(!(counted_a == unbounded_repeat));
    STATIC_REQUIRE(!(counted_a == empty_repeat));
    STATIC_REQUIRE(StableEqualityComparable<std::remove_cvref_t<decltype(counted_a)>>);
    STATIC_REQUIRE(NothrowStableEqualityComparable<std::remove_cvref_t<decltype(counted_a)>>);
    STATIC_REQUIRE_FALSE(StableEqualityComparable<Repeat<double>>);

    // Repeat is admitted as a set element. The capability is type-level: the
    // unbounded and finite forms share the Repeat<int> type, so both admit
    // producer parameter equality even though their stored parameters differ.
    constexpr Set<Repeat<int>, 4> repeat_set{repeat(3U, 7), repeat(2U, 8)};
    STATIC_REQUIRE(repeat_set.count() == 2U);
    STATIC_REQUIRE(repeat_set.contains(repeat(3U, 7)));
    STATIC_REQUIRE(StableEqualityComparable<std::remove_cvref_t<decltype(unbounded_repeat)>>);

    STATIC_REQUIRE_FALSE(std::invocable<decltype(finite_repeat), std::size_t>);
    STATIC_REQUIRE_FALSE(HasContains<decltype(finite_repeat)>);
    STATIC_REQUIRE_FALSE(HasCallableLookup<decltype(finite_repeat)>);
    STATIC_REQUIRE_FALSE(GetDomainAdmissible<decltype(finite_repeat)>);

    auto mutable_value = 7;
    const auto owning_repeat = repeat(2U, mutable_value);
    mutable_value = 9;
    const auto owning_result = into(Vector<int, 2>{}, owning_repeat);
    CHECK(owning_result(0U) == 7);
    CHECK(owning_result(1U) == 7);

    const auto runtime_finite_repeat = repeat(2U, 11);
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

TEST_CASE("Repeat normalizes signed counts without changing count-first semantics", "[repeat]") {
    using namespace cljonic;
    TRACE_ID("invariant.Repeat.SignedCountFactoryConstructorAndDeductionAreSupported");
    TRACE_ID("invariant.Repeat.NegativeCountProducesEmptyFiniteResult");

    constexpr auto positive = repeat(2, 7);
    STATIC_REQUIRE(positive == Repeat{2, 7});
    STATIC_REQUIRE(positive == Repeat<int>{2, 7});
    STATIC_REQUIRE(std::same_as<decltype(repeat(2, 7)), Repeat<int>>);
    STATIC_REQUIRE(std::same_as<std::remove_cv_t<decltype(Repeat{2, 7})>, Repeat<int>>);
    STATIC_REQUIRE(positive.count() == 2);
    STATIC_REQUIRE(*positive.begin() == 7);
    STATIC_REQUIRE(noexcept(repeat(-2, 7)));
    STATIC_REQUIRE(noexcept(Repeat{-2, 7}));
    constexpr auto numeric = repeat(7, 3U);
    STATIC_REQUIRE(numeric.count() == 7);
    STATIC_REQUIRE(*numeric.begin() == 3U);
    STATIC_REQUIRE(std::same_as<std::remove_cv_t<decltype(numeric)>, Repeat<unsigned int>>);

    for (const auto count : {std::numeric_limits<int>::min(), -2, 0, 1, 3}) {
        const auto factory = repeat(count, 7);
        const auto deduced = Repeat{count, 7};
        const auto explicit_type = Repeat<int>{count, 7};
        const auto saved = factory;
        const auto expected = count <= 0 ? std::size_t{0} : static_cast<std::size_t>(count);
        CHECK(factory.count() == expected);
        CHECK(factory.is_finite());
        CHECK(factory == deduced);
        CHECK(factory == explicit_type);
        CHECK(into(Vector<int, 4>{}, factory).count() == expected);
        CHECK(fits_into(Vector<int, 4>{}, factory));
        CHECK(factory == saved);
    }
    constexpr auto minimum = Repeat{std::numeric_limits<int>::min(), 7};
    STATIC_REQUIRE(minimum.count() == 0);
    STATIC_REQUIRE(minimum.is_finite());
    STATIC_REQUIRE(minimum.begin() == minimum.end());
    STATIC_REQUIRE(minimum == Repeat{0, 7});
    STATIC_REQUIRE(minimum != repeat(7));
}

TEST_CASE("Repeat counted factory and construction use count-first ordering", "[repeat]") {
    using namespace cljonic;

    TRACE_ID("invariant.Repeat.CountedFactoryUsesCountThenValue");
    TRACE_ID("invariant.Repeat.CountedConstructorUsesCountThenValue");
    TRACE_ID("invariant.Repeat.CountedDeductionUsesSecondArgumentValueType");
    TRACE_ID("invariant.Repeat.CountedFormsExcludeValueFirstOverloadsAndOrderInference");

    constexpr auto factory = repeat(3U, 7U);
    constexpr auto deduced = Repeat{3U, 7U};
    constexpr auto explicit_type = Repeat<unsigned int>{3U, 7U};
    STATIC_REQUIRE(factory.count() == 3U);
    STATIC_REQUIRE(*factory.begin() == 7U);
    STATIC_REQUIRE(factory == deduced);
    STATIC_REQUIRE(factory == explicit_type);
    STATIC_REQUIRE(factory.is_finite());
    STATIC_REQUIRE(noexcept(repeat(3U, 7U)));
    STATIC_REQUIRE(noexcept(Repeat{3U, 7U}));

    constexpr auto signed_value = Repeat{std::size_t{2}, -7};
    STATIC_REQUIRE(std::same_as<std::remove_cv_t<decltype(signed_value)>, Repeat<int>>);
    STATIC_REQUIRE(std::same_as<decltype(repeat(std::size_t{2}, -7)), Repeat<int>>);
    STATIC_REQUIRE(signed_value.count() == 2U);
    STATIC_REQUIRE(*signed_value.begin() == -7);
    constexpr auto nested = Repeat{2U, Vector{10, 20}};
    STATIC_REQUIRE(std::same_as<std::remove_cv_t<decltype(nested)>, Repeat<Vector<int, 2>>>);
    STATIC_REQUIRE(nested.count() == 2U);
    STATIC_REQUIRE(*nested.begin() == Vector{10, 20});

    constexpr auto zero = Repeat{0U, 7};
    constexpr auto singleton = Repeat{1U, 7};
    STATIC_REQUIRE(zero.is_finite());
    STATIC_REQUIRE(zero.count() == 0U);
    STATIC_REQUIRE(zero.begin() == zero.end());
    STATIC_REQUIRE(singleton.count() == 1U);
    STATIC_REQUIRE(*singleton.begin() == 7);
    STATIC_REQUIRE(into(Vector<int, 3>{}, singleton).count() == 1U);

    volatile unsigned int runtime_count = 3U;
    volatile int runtime_value = -11;
    const auto count = runtime_count;
    const auto value = runtime_value;
    const auto from_factory = repeat(count, value);
    const auto from_constructor = Repeat{count, value};
    const auto saved = from_factory;
    const auto result = into(Vector<int, 4>{}, from_factory);
    CHECK(result.count() == 3U);
    CHECK(result(0U) == -11);
    CHECK(result(2U) == -11);
    CHECK(from_factory == from_constructor);
    CHECK(from_factory == saved);
    CHECK(fits_into(Vector<int, 4>{}, from_factory));
}