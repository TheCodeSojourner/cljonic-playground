#include "cljonic-test-api.hpp"
#include <catch2/catch_test_macros.hpp>
#include <concepts>
#include <type_traits>

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("parameters_equal is the named producer structural-comparison operation", "[parameters-equal][producer]") {
    using cljonic::cycle;
    using cljonic::parameters_equal;
    using cljonic::Range;
    using cljonic::Repeat;
    using cljonic::repeat;
    using cljonic::Vector;
    using cljonic::concepts::StableEqualityComparable;

    TRACE_ID("entity-fields.ParametersEqual");
    TRACE_ID("invariant.ParametersEqual.NamedStructuralComparisonOperation");
    TRACE_ID("invariant.ParametersEqual.SemanticIdentityWithProducerOperatorEquals");
    TRACE_ID("invariant.ParametersEqual.ComparesBoundedStoredParametersOnly");
    TRACE_ID("invariant.ParametersEqual.NeverTraversesProducedSequence");
    TRACE_ID("invariant.ParametersEqual.DoesNotImplySequenceEquality");
    TRACE_ID("invariant.ParametersEqual.DistinctParametersCompareUnequalEvenWhenSequencesCoincide");
    TRACE_ID("invariant.ParametersEqual.IsO1ConstexprNoexceptNonmutatingNonallocating");
    TRACE_ID("invariant.ParametersEqual.RequiresAllStoredComponentsAdmitStableEquality");
    TRACE_ID("invariant.ParametersEqual.CallableComponentsExcludeOperation");
    TRACE_ID("invariant.ParametersEqual.SupportsRangeRepeatAndCycle");
    TRACE_ID("invariant.ParametersEqual.RejectsFloatBearingAndCallableComponentsAtCompileTime");
    TRACE_ID("entity-fields.ProducerParameterEquality");
    TRACE_ID("invariant.ProducerParameterEquality.ComparesBoundedStoredParametersOnly");
    TRACE_ID("invariant.ProducerParameterEquality.OperatorEqualsUsesProducerParameterEquality");
    TRACE_ID("invariant.ProducerParameterEquality.ProvidesNamedParametersEqualOperation");
    TRACE_ID("invariant.ProducerParameterEquality.IsO1ConstexprNoexceptNonmutatingNonallocating");
    TRACE_ID("invariant.ProducerParameterEquality.NeverTraversesProducedSequence");
    TRACE_ID("invariant.ProducerParameterEquality.DoesNotImplySequenceEquality");
    TRACE_ID("invariant.ProducerParameterEquality.DistinctParametersCompareUnequalEvenWhenSequencesCoincide");
    TRACE_ID("invariant.ProducerParameterEquality.RequiresAllStoredComponentsAdmitStableEquality");
    TRACE_ID("invariant.ProducerParameterEquality.CallableComponentsExcludeParameterEquality");
    TRACE_ID("invariant.ProducerParameterEquality.ProducerAdmittedAsMapKeyAndSetElement");

    // Range: parameters_equal has identical semantics to operator==.
    constexpr Range<int> range_a{1, 5, 2};
    constexpr Range<int> range_b{1, 5, 2};
    constexpr Range<int> range_c{1, 5, 3};
    STATIC_REQUIRE(parameters_equal(range_a, range_b));
    STATIC_REQUIRE(!parameters_equal(range_a, range_c));
    STATIC_REQUIRE((range_a == range_b) == parameters_equal(range_a, range_b));
    STATIC_REQUIRE((range_a == range_c) == parameters_equal(range_a, range_c));

    // Distinct parameters compare unequal even when produced sequences
    // coincide: both zero-step ranges produce an infinite sequence of zeros.
    constexpr Range<int> infinite_zeros_a{0, 5, 0};
    constexpr Range<int> infinite_zeros_b{0, 7, 0};
    STATIC_REQUIRE(!parameters_equal(infinite_zeros_a, infinite_zeros_b));
    STATIC_REQUIRE(!(infinite_zeros_a == infinite_zeros_b));

    // Repeat: the unbounded form, a counted form, and the zero-count finite
    // form are pairwise distinct even when produced prefixes coincide.
    constexpr Repeat<int> unbounded_form{9};
    constexpr Repeat<int> counted_form{9, 3U};
    constexpr Repeat<int> zero_count_form{9, 0U};
    STATIC_REQUIRE(!parameters_equal(unbounded_form, counted_form));
    STATIC_REQUIRE(!parameters_equal(unbounded_form, zero_count_form));
    STATIC_REQUIRE(!parameters_equal(counted_form, zero_count_form));
    constexpr Repeat<int> counted_form_dup{9, 3U};
    STATIC_REQUIRE(parameters_equal(counted_form, counted_form_dup));

    // Cycle: equal owned sources yield equal parameters; distinct sources do
    // not, and a distinct source content is a distinct producer even though
    // every Cycle is unbounded.
    constexpr auto cycle_a = cycle(Vector<int, 3>{1, 2, 3});
    constexpr auto cycle_b = cycle(Vector<int, 3>{1, 2, 3});
    constexpr auto cycle_c = cycle(Vector<int, 3>{1, 2, 4});
    STATIC_REQUIRE(parameters_equal(cycle_a, cycle_b));
    STATIC_REQUIRE(!parameters_equal(cycle_a, cycle_c));

    // O(1) and constexpr: no traversal is required for any producer form.
    constexpr auto zero_step = Range<int>{0, 1000000000L * 0L, 0};
    (void)zero_step;
    STATIC_REQUIRE(parameters_equal(infinite_zeros_a, Range<int>{0, 5, 0}));

    // Mixed types are never comparable: parameters_equal requires the same
    // producer type on both sides (compile-time rejection), so here we only
    // verify the concept-level rejection for stable equality.
    STATIC_REQUIRE(StableEqualityComparable<Range<int>>);
    STATIC_REQUIRE(StableEqualityComparable<Repeat<int>>);
    STATIC_REQUIRE(StableEqualityComparable<std::remove_cvref_t<decltype(cycle_a)>>);

    // Callable-bearing producers are excluded at the type boundary: the
    // contains_callable walker rejects Iterate and Repeatedly, so
    // parameters_equal and operator== are never available for them.
    STATIC_REQUIRE_FALSE(StableEqualityComparable<cljonic::Iterate<int, int (*)(int) noexcept>>);
    STATIC_REQUIRE_FALSE(StableEqualityComparable<cljonic::Repeatedly<int, int (*)() noexcept>>);

    // Runtime demonstration of identical operator== / parameters_equal
    // semantics.
    auto runtime_a = Range<int>{2, 9, 2};
    auto runtime_b = Range<int>{2, 9, 2};
    auto runtime_c = Range<int>{2, 9, 3};
    CHECK(parameters_equal(runtime_a, runtime_b));
    CHECK_FALSE(parameters_equal(runtime_a, runtime_c));
    CHECK((runtime_a == runtime_b) == parameters_equal(runtime_a, runtime_b));
    CHECK((runtime_a == runtime_c) == parameters_equal(runtime_a, runtime_c));
}