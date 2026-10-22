#include <catch2/catch_test_macros.hpp>

#include <utility>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

namespace {

template <typename Collection, typename Index>
concept VectorIndexOperationsAcceptIndex = requires(const Collection& collection, Index index) {
    collection.contains(index);
    collection(index);
    collection.can_assoc(index, typename Collection::value_type{});
    collection.assoc(index, typename Collection::value_type{});
};

template <typename Collection, typename Index>
concept GetFreeFunctionAcceptsIndex = cljonic::concepts_detail::GetKeyAdmissible<Collection, Index>;

} // namespace

TEST_CASE("Vector construction establishes logical size", "[vector]") {
    using cljonic::Vector;

    TRACE_ID("entity-fields.Vector");
    TRACE_ID("invariant.Vector.CapacityIsNonNegative");
    TRACE_ID("invariant.Vector.LogicalSizeIsNonNegative");
    TRACE_ID("invariant.Vector.LogicalSizeDoesNotExceedCapacity");
    TRACE_ID("invariant.Vector.HeaderOnlyDistribution");
    TRACE_ID("invariant.Vector.StaticOrAutomaticStorageOnly");
    TRACE_ID("invariant.Vector.ContiguousStorage");
    TRACE_ID("invariant.Vector.CallableLookup");
    TRACE_ID("invariant.Vector.NoHeapAllocation");
    TRACE_ID("invariant.Vector.NoRtti");
    TRACE_ID("invariant.Vector.NoExceptions");
    TRACE_ID("invariant.Vector.SingleThreadedExecutionModel");
    TRACE_ID("invariant.Vector.ImmutablePublicApi");
    TRACE_ID("invariant.Vector.UpdateReturnsNewValue");
    TRACE_ID("invariant.Vector.DeepCopyUpdate");
    TRACE_ID("invariant.Vector.ReferentialTransparency");
    TRACE_ID("invariant.Vector.RequiresValueSemanticElements");
    TRACE_ID("invariant.Vector.RequiresNothrowDefaultConstruction");
    TRACE_ID("invariant.Vector.RequiresNothrowCopyConstruction");
    TRACE_ID("invariant.Vector.RequiresNothrowCopyAssignment");
    TRACE_ID("invariant.Vector.RequiresNothrowDestruction");
    TRACE_ID("invariant.Vector.SupportsLiteralDeducedConstruction");
    TRACE_ID("invariant.Vector.SupportsEmptyExplicitCapacityConstruction");
    TRACE_ID("invariant.Vector.SupportsExplicitCapacityConstruction");
    TRACE_ID("invariant.Vector.SupportsCapacityInferredLiteralEquivalentSemantics");
    TRACE_ID("invariant.Vector.OversizedInitializerIsCompileTimeFailure");
    TRACE_ID("invariant.Vector.OversizedInitializerDiagnosticIdentifiesCapacity");
    TRACE_ID("invariant.Vector.CapacityExceedsMaximumIsCompileTimeFailure");

    constexpr Vector<int, 4> empty{};
    constexpr Vector<int, 4> populated{1, 2};
    constexpr Vector<int, 2> full{1, 2};
    constexpr Vector<int, 0> zero_capacity{};
    constexpr Vector<int, 4> capacity_boundary{1, 2, 3, 4};
    constexpr auto inferred = Vector{1, 2, 3};
    constexpr Vector<int, 3> explicit_inferred_equivalent{1, 2, 3};

    struct CategoryArgument {};

    struct CategoryElement {
        int category = 0;

        constexpr CategoryElement() noexcept = default;
        constexpr explicit CategoryElement(int category_value) noexcept : category(category_value) {
        }
        constexpr explicit CategoryElement(const CategoryArgument& argument) noexcept : category(1) {
            (void)argument;
        }
        constexpr explicit CategoryElement(CategoryArgument&& argument) noexcept : category(2) {
            (void)argument;
        }
        constexpr CategoryElement(const CategoryElement&) noexcept = default;
        constexpr auto operator=(const CategoryElement&) noexcept -> CategoryElement& = default;
    };

    constexpr CategoryArgument category_argument{};
    STATIC_REQUIRE_FALSE(cljonic::concepts::NothrowElementConstruction<CategoryElement, const CategoryArgument&>);
    constexpr Vector<CategoryElement, 1> lvalue_category_vector{CategoryElement{category_argument}};
    constexpr Vector<CategoryElement, 1> rvalue_category_vector{CategoryElement{CategoryArgument{}}};

    STATIC_REQUIRE(empty.count() == 0U);
    STATIC_REQUIRE(populated.count() == 2U);
    STATIC_REQUIRE(full.count() == 2U);
    STATIC_REQUIRE(capacity_boundary.count() == capacity_boundary.capacity());
    STATIC_REQUIRE(Vector<int, 4>::capacity() == 4U);
    STATIC_REQUIRE(zero_capacity.capacity() == 0U);
    STATIC_REQUIRE(zero_capacity.count() == 0U);
    STATIC_REQUIRE(cljonic::is_empty(zero_capacity));
    STATIC_REQUIRE(zero_capacity(0U) == 0);
    STATIC_REQUIRE(zero_capacity(0U, 99) == 99);
    STATIC_REQUIRE_FALSE(zero_capacity.contains(0U));
    STATIC_REQUIRE(std::same_as<std::remove_cvref_t<decltype(inferred)>, Vector<int, 3>>);
    STATIC_REQUIRE(inferred.capacity() == explicit_inferred_equivalent.capacity());
    STATIC_REQUIRE(inferred.count() == explicit_inferred_equivalent.count());
    STATIC_REQUIRE(inferred(0U) == explicit_inferred_equivalent(0U));
    STATIC_REQUIRE(inferred(1U) == explicit_inferred_equivalent(1U));
    STATIC_REQUIRE(inferred(2U) == explicit_inferred_equivalent(2U));
    STATIC_REQUIRE(lvalue_category_vector(0U).category == 1);
    STATIC_REQUIRE(rvalue_category_vector(0U).category == 2);

    const auto runtime_values = Vector<int, 4>{1, 2};
    CHECK(runtime_values.count() == 2U);
    CHECK(runtime_values.capacity() == 4U);
    CHECK(cljonic::is_empty(zero_capacity));
    CHECK_FALSE(zero_capacity.contains(0U));
    CHECK(inferred.count() == explicit_inferred_equivalent.count());
    CHECK(inferred(2U) == explicit_inferred_equivalent(2U));
    CHECK(lvalue_category_vector(0U).category == 1);
    CHECK(rvalue_category_vector(0U).category == 2);
}

TEST_CASE("Vector canonical preflight predicates model index validity and emptiness", "[vector][preflight]") {
    using cljonic::Vector;

    TRACE_ID("entity-fields.Vector");
    TRACE_ID("invariant.Vector.CanonicalResultStatusModelIsDeclared");
    TRACE_ID("invariant.Vector.CompleteResultStatusDeclared");
    TRACE_ID("invariant.Vector.BoundedPrefixResultStatusDeclared");
    TRACE_ID("invariant.Vector.DefaultReturningResultStatusDeclared");
    TRACE_ID("invariant.Vector.CheckedFailureResultStatusDeclared");
    TRACE_ID("invariant.Vector.ProducerOnlyResultStatusDeclared");
    TRACE_ID("invariant.Vector.PreflightPredicatesAreNonThrowingNonAllocating");
    TRACE_ID("invariant.Vector.ContainsIsCanonicalIndexPredicate");
    TRACE_ID("invariant.Vector.FitsIntoIsCanonicalMaterializationPreflight");
    TRACE_ID("invariant.Vector.CompileTimeCapacityOverflowIsRejected");
    TRACE_ID("invariant.Vector.RuntimeCapacityFailuresHaveDocumentedPolicy");
    TRACE_ID("invariant.Vector.DefaultAccessHasPreflightPredicate");
    TRACE_ID("invariant.Vector.SupportsIndexedLookup");
    TRACE_ID("invariant.Vector.AcceptsNonbooleanNoncharacterIntegerIndexTypes");
    TRACE_ID("invariant.Vector.SupportsDefaultFallbackLookup");
    TRACE_ID("invariant.Vector.SupportsLookupCapability");
    TRACE_ID("invariant.Vector.SupportsAssociativeCapability");
    TRACE_ID("invariant.Vector.SeqableLifecycleIsDeferred");
    TRACE_ID("invariant.Vector.SupportsAssocUpdate");
    TRACE_ID("invariant.Vector.SupportsCanAssocPreflight");
    TRACE_ID("invariant.Vector.AssocReplacesExistingIndex");
    TRACE_ID("invariant.Vector.AssocAppendsAtLogicalCount");
    TRACE_ID("invariant.Vector.InvalidAssocIndexReturnsUnchangedVector");
    TRACE_ID("invariant.Vector.CanAssocTrueForExistingOrAppendIndex");
    TRACE_ID("invariant.Vector.CanAssocFalseForInvalidOrFullAppendIndex");
    TRACE_ID("invariant.Vector.InvalidIndexReturnsDefaultElement");
    TRACE_ID("invariant.Vector.InvalidIndexReturnsSuppliedFallback");

    constexpr Vector<int, 4> values{10, 20};
    constexpr Vector<int, 4> empty_values{};

    STATIC_REQUIRE(VectorIndexOperationsAcceptIndex<Vector<int, 4>, int>);
    STATIC_REQUIRE(VectorIndexOperationsAcceptIndex<Vector<int, 4>, unsigned long long>);
    STATIC_REQUIRE_FALSE(VectorIndexOperationsAcceptIndex<Vector<int, 4>, bool>);
    STATIC_REQUIRE_FALSE(VectorIndexOperationsAcceptIndex<Vector<int, 4>, char>);
    STATIC_REQUIRE_FALSE(VectorIndexOperationsAcceptIndex<Vector<int, 4>, signed char>);
    STATIC_REQUIRE_FALSE(VectorIndexOperationsAcceptIndex<Vector<int, 4>, unsigned char>);
    STATIC_REQUIRE_FALSE(VectorIndexOperationsAcceptIndex<Vector<int, 4>, wchar_t>);
    STATIC_REQUIRE_FALSE(VectorIndexOperationsAcceptIndex<Vector<int, 4>, char8_t>);
    STATIC_REQUIRE_FALSE(VectorIndexOperationsAcceptIndex<Vector<int, 4>, char16_t>);
    STATIC_REQUIRE_FALSE(VectorIndexOperationsAcceptIndex<Vector<int, 4>, char32_t>);
    STATIC_REQUIRE(GetFreeFunctionAcceptsIndex<Vector<int, 4>, int>);
    STATIC_REQUIRE_FALSE(GetFreeFunctionAcceptsIndex<Vector<int, 4>, bool>);
    STATIC_REQUIRE_FALSE(GetFreeFunctionAcceptsIndex<Vector<int, 4>, char>);
    STATIC_REQUIRE_FALSE(cljonic::concepts_detail::AssocPairAdmissible<Vector<int, 4>, char, int>);
    STATIC_REQUIRE_FALSE(cljonic::concepts_detail::AssocPairAdmissible<Vector<int, 4>, bool, int>);
    STATIC_REQUIRE_FALSE(cljonic::concepts_detail::CanAssocKeyAdmissible<Vector<int, 4>, char>);
    STATIC_REQUIRE_FALSE(cljonic::concepts_detail::CanAssocKeyAdmissible<Vector<int, 4>, bool>);

    STATIC_REQUIRE(values.contains(0U));
    STATIC_REQUIRE(values.contains(1U));
    STATIC_REQUIRE_FALSE(values.contains(2U));
    STATIC_REQUIRE(cljonic::is_empty(empty_values));
    STATIC_REQUIRE_FALSE(cljonic::is_empty(values));

    const auto runtime_values = Vector<int, 4>{10, 20};
    CHECK(runtime_values.contains(0U));
    CHECK(runtime_values.contains(1U));
    CHECK_FALSE(runtime_values.contains(2U));
    CHECK(cljonic::is_empty(Vector<int, 4>{}));
    CHECK_FALSE(cljonic::is_empty(runtime_values));
}

TEST_CASE("Vector indexed access handles valid and invalid indexes", "[vector][indexed-access]") {
    using cljonic::Vector;

    TRACE_ID("entity-fields.Vector");
    TRACE_ID("invariant.Vector.SupportsIndexedLookup");
    TRACE_ID("invariant.Vector.SupportsDefaultFallbackLookup");
    TRACE_ID("invariant.Vector.InvalidIndexReturnsDefaultElement");
    TRACE_ID("invariant.Vector.InvalidIndexReturnsSuppliedFallback");

    constexpr Vector<int, 4> values{10, 20};

    STATIC_REQUIRE(values(0U) == 10);
    STATIC_REQUIRE(values(1U) == 20);
    STATIC_REQUIRE(values(2U) == 0);
    STATIC_REQUIRE(values(2U, 99) == 99);

    const auto runtime_values = Vector<int, 4>{10, 20};
    CHECK(runtime_values(0U) == 10);
    CHECK(runtime_values(1U) == 20);
    CHECK(runtime_values(2U) == 0);

    // Runtime signed negative-index guard (defeats constexpr folding so gcov
    // records the branch).
    volatile int neg_raw = -1;
    const int neg = neg_raw;
    CHECK(runtime_values(neg) == 0);
    CHECK(runtime_values(neg, 99) == 99);
    CHECK_FALSE(runtime_values.contains(neg));
}

TEST_CASE("Vector imports from std::span without allocating or mutating the source", "[vector][interop]") {
    using cljonic::Vector;

    static constexpr int static_source_values[] = {11, 22, 33, 44, 55};
    constexpr std::span static_source_span{static_source_values};
    constexpr auto ctad_from_static_span = Vector{static_source_span};
    STATIC_REQUIRE(std::same_as<std::remove_cvref_t<decltype(ctad_from_static_span)>, Vector<int, 5>>);
    STATIC_REQUIRE(ctad_from_static_span.count() == 5U);
    STATIC_REQUIRE(ctad_from_static_span.capacity() == 5U);
    STATIC_REQUIRE(ctad_from_static_span(0) == 11);
    STATIC_REQUIRE(ctad_from_static_span(4) == 55);

    int mutable_source_values[] = {10, 20, 30};
    std::span mutable_source_span{mutable_source_values};
    auto ctad_from_mutable_span = Vector{mutable_source_span};
    STATIC_REQUIRE(std::same_as<std::remove_cvref_t<decltype(ctad_from_mutable_span)>, Vector<int, 3>>);
    CHECK(ctad_from_mutable_span.count() == 3U);
    CHECK(ctad_from_mutable_span(0) == 10);
    CHECK(ctad_from_mutable_span(2) == 30);

    const std::array<int, 5> source_values = {11, 22, 33, 44, 55};
    const std::span<const int> source{source_values.data(), 5U};

    static constexpr std::array<int, 5> constexpr_source_values = {11, 22, 33, 44, 55};
    constexpr auto from_span = Vector<int, 4>{std::span<const int>{constexpr_source_values.data(), 3U}};
    STATIC_REQUIRE(from_span.count() == 3U);
    STATIC_REQUIRE(from_span(0) == 11);
    STATIC_REQUIRE(from_span(2) == 33);

    const auto runtime_vector = Vector<int, 4>{source};
    CHECK(runtime_vector.count() == 4U);
    CHECK(runtime_vector(0) == 11);
    CHECK(runtime_vector(1) == 22);
    CHECK(runtime_vector(2) == 33);
    CHECK(runtime_vector(3) == 44);
    CHECK(runtime_vector(4) == 0);
    CHECK(source[0] == 11);
    CHECK(source[4] == 55);
}

TEST_CASE("Vector stack operations observe and remove the last element immutably", "[vector][stack]") {
    using cljonic::peek;
    using cljonic::pop;
    using cljonic::Vector;

    TRACE_ID("entity-fields.Vector");
    TRACE_ID("invariant.Vector.SupportsStackPeekMethod");
    TRACE_ID("invariant.Vector.PeekReturnsLastLogicalElementOrDefaultWhenEmpty");
    TRACE_ID("invariant.Vector.SupportsStackPopMethod");
    TRACE_ID("invariant.Vector.PopRemovesLastElementFromCopyAndPreservesEmptyValue");

    constexpr Vector<int, 4> values{10, 20};
    constexpr auto member_popped = values.pop();
    constexpr auto function_popped = pop(values);
    STATIC_REQUIRE(values.peek() == 20);
    STATIC_REQUIRE(peek(values) == 20);
    STATIC_REQUIRE(member_popped.count() == 1U);
    STATIC_REQUIRE(member_popped.peek() == 10);
    STATIC_REQUIRE(function_popped == member_popped);
    STATIC_REQUIRE(values.count() == 2U);
    STATIC_REQUIRE(noexcept(values.peek()));
    STATIC_REQUIRE(noexcept(values.pop()));
    STATIC_REQUIRE(noexcept(peek(values)));
    STATIC_REQUIRE(noexcept(pop(values)));

    constexpr Vector<int, 4> empty_values{};
    constexpr Vector<int, 0> zero_capacity_values{};
    STATIC_REQUIRE(empty_values.peek() == 0);
    STATIC_REQUIRE(peek(empty_values) == 0);
    STATIC_REQUIRE(empty_values.pop() == empty_values);
    STATIC_REQUIRE(pop(empty_values) == empty_values);
    STATIC_REQUIRE(zero_capacity_values.peek() == 0);
    STATIC_REQUIRE(zero_capacity_values.pop() == zero_capacity_values);

    volatile int first_raw = 31;
    volatile int last_raw = 62;
    const int first_value = first_raw;
    const int last_value = last_raw;
    const auto runtime_values = Vector<int, 4>{first_value, last_value};
    const auto runtime_member_popped = runtime_values.pop();
    const auto runtime_function_popped = pop(runtime_values);
    CHECK(runtime_values.peek() == 62);
    CHECK(peek(runtime_values) == 62);
    CHECK(runtime_member_popped.count() == 1U);
    CHECK(runtime_member_popped.peek() == 31);
    CHECK(runtime_function_popped == runtime_member_popped);
    CHECK(runtime_values.count() == 2U);
    CHECK(runtime_values.peek() == 62);
}

TEST_CASE("Vector factory creates typed bounded values from its arguments", "[vector][factory]") {
    using cljonic::Vector;
    using cljonic::vector;

    TRACE_ID("entity-fields.VectorFactory");
    TRACE_ID("invariant.VectorFactory.SupportsTypedEmptyFactory");
    TRACE_ID("invariant.VectorFactory.TypedEmptyFactoryHasZeroCapacity");
    TRACE_ID("invariant.VectorFactory.SupportsDeducedVariadicFactory");
    TRACE_ID("invariant.VectorFactory.DeducedElementTypeIsUnqualifiedFirstArgument");
    TRACE_ID("invariant.VectorFactory.SupportsExplicitElementTypeVariadicFactory");
    TRACE_ID("invariant.VectorFactory.ExplicitElementTypeDeterminesResultElementType");
    TRACE_ID("invariant.VectorFactory.VariadicCapacityEqualsArgumentCount");
    TRACE_ID("invariant.VectorFactory.VariadicFormsRequireAtLeastOneArgument");
    TRACE_ID("invariant.VectorFactory.SupportsExplicitCapacityFactory");
    TRACE_ID("invariant.VectorFactory.ExplicitCapacityDeterminesResultCapacity");
    TRACE_ID("invariant.VectorFactory.ExplicitCapacityFactorySupportsEmptyArguments");
    TRACE_ID("invariant.VectorFactory.ExplicitCapacityIsAtLeastArgumentCount");
    TRACE_ID("invariant.VectorFactory.ExplicitCapacityIsBoundedByConfiguredMaximum");
    TRACE_ID("invariant.VectorFactory.InvalidExplicitCapacityIsCompileTimeRejected");
    TRACE_ID("invariant.VectorFactory.ElementTypeSatisfiesNothrowCollectionElement");
    TRACE_ID("invariant.VectorFactory.EveryArgumentSatisfiesNothrowElementConstruction");
    TRACE_ID("invariant.VectorFactory.ArgumentCountIsBoundedByConfiguredMaximum");
    TRACE_ID("invariant.VectorFactory.OversizedArgumentPackIsCompileTimeRejected");
    TRACE_ID("invariant.VectorFactory.UntypedEmptyCallIsRejected");
    TRACE_ID("invariant.VectorFactory.ReturnsIndependentlyOwningVector");
    TRACE_ID("invariant.VectorFactory.PreservesLvalueArguments");
    TRACE_ID("invariant.VectorFactory.PermitsMovingFromRvalueArguments");
    TRACE_ID("invariant.VectorFactory.ConstexprNoexceptNonallocating");
    TRACE_ID("invariant.VectorFactory.NoRtti");
    TRACE_ID("invariant.VectorFactory.NoExceptions");
    TRACE_ID("invariant.VectorFactory.ReferentialTransparency");

    constexpr auto typed_empty = vector<int>();
    constexpr auto reserved_empty = vector<int, 4>();
    constexpr auto deduced = vector(1, short{2}, 3);
    constexpr auto explicitly_typed = vector<long>(short{4}, 5);
    constexpr auto explicitly_sized = vector<int, 5>(short{6}, 7);
    constexpr auto const_lvalue = 6;
    constexpr auto deduced_from_const_lvalue = vector(const_lvalue);

    STATIC_REQUIRE((std::same_as<std::remove_cvref_t<decltype(typed_empty)>, Vector<int, 0>>));
    STATIC_REQUIRE((std::same_as<std::remove_cvref_t<decltype(reserved_empty)>, Vector<int, 4>>));
    STATIC_REQUIRE((std::same_as<std::remove_cvref_t<decltype(deduced)>, Vector<int, 3>>));
    STATIC_REQUIRE((std::same_as<std::remove_cvref_t<decltype(explicitly_typed)>, Vector<long, 2>>));
    STATIC_REQUIRE((std::same_as<std::remove_cvref_t<decltype(explicitly_sized)>, Vector<int, 5>>));
    STATIC_REQUIRE((std::same_as<std::remove_cvref_t<decltype(deduced_from_const_lvalue)>, Vector<int, 1>>));
    STATIC_REQUIRE(typed_empty.count() == 0U);
    STATIC_REQUIRE(typed_empty.capacity() == 0U);
    STATIC_REQUIRE(reserved_empty.count() == 0U);
    STATIC_REQUIRE(reserved_empty.capacity() == 4U);
    STATIC_REQUIRE(deduced.count() == 3U);
    STATIC_REQUIRE(deduced.capacity() == 3U);
    STATIC_REQUIRE(deduced(0U) == 1);
    STATIC_REQUIRE(deduced(1U) == 2);
    STATIC_REQUIRE(deduced(2U) == 3);
    STATIC_REQUIRE(explicitly_typed(0U) == 4L);
    STATIC_REQUIRE(explicitly_typed(1U) == 5L);
    STATIC_REQUIRE(explicitly_sized.count() == 2U);
    STATIC_REQUIRE(explicitly_sized.capacity() == 5U);
    STATIC_REQUIRE(explicitly_sized(0U) == 6);
    STATIC_REQUIRE(explicitly_sized(1U) == 7);
    STATIC_REQUIRE(noexcept(vector<int>()));
    STATIC_REQUIRE(noexcept(vector(1, short{2})));
    STATIC_REQUIRE(noexcept(vector<long>(short{4}, 5)));
    STATIC_REQUIRE(noexcept(vector<int, 4>()));
    STATIC_REQUIRE(noexcept(vector<int, 5>(short{6}, 7)));

    struct MoveProbe {
        int value = 0;
        bool* move_observed = nullptr;

        constexpr MoveProbe() noexcept = default;
        constexpr explicit MoveProbe(int initial_value, bool* observer = nullptr) noexcept
            : value(initial_value), move_observed(observer) {
        }
        constexpr MoveProbe(const MoveProbe&) noexcept = default;
        constexpr MoveProbe(MoveProbe&& other) noexcept : value(other.value) {
            if (other.move_observed != nullptr) {
                *other.move_observed = true;
            }
        }
        constexpr auto operator=(const MoveProbe&) noexcept -> MoveProbe& = default;
    };

    MoveProbe lvalue{6};
    const auto from_lvalue = vector(lvalue);
    CHECK(lvalue.value == 6);
    CHECK(from_lvalue(0U).value == 6);
    lvalue.value = 8;
    CHECK(from_lvalue(0U).value == 6);

    bool move_observed = false;
    MoveProbe rvalue{7, &move_observed};
    const auto from_rvalue = vector(std::move(rvalue));
    CHECK(move_observed);
    CHECK(from_rvalue(0U).value == 7);
}
