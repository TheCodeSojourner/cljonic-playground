#include "cljonic-test-api.hpp"
#include <array>
#include <catch2/catch_test_macros.hpp>
#include <concepts>
#include <span>
#include <type_traits>
#include <variant>
#include <vector>

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

namespace {

constexpr int inc_fn(int v) noexcept {
    return v + 1;
}

constexpr int gen_fn() noexcept {
    return 1;
}

struct Pixel {
    int x;
    int y;

    [[nodiscard]] constexpr auto operator==(const Pixel& other) const noexcept -> bool {
        return x == other.x && y == other.y;
    }
};

} // namespace

TEST_CASE("equal implements general value equality over the supported domain", "[equal][core]") {
    using cljonic::cycle;
    using cljonic::equal;
    using cljonic::Iterate;
    using cljonic::Map;
    using cljonic::MapEntry;
    using cljonic::Queue;
    using cljonic::Range;
    using cljonic::Repeat;
    using cljonic::Repeatedly;
    using cljonic::Set;
    using cljonic::String;
    using cljonic::Vector;

    TRACE_ID("entity-fields.EqualFunction");
    TRACE_ID("invariant.EqualFunction.GeneralValueEqualityOperation");
    TRACE_ID("invariant.EqualFunction.NamedOperationDistinctFromOperatorEquals");
    TRACE_ID("invariant.EqualFunction.SupportedDomainScope");
    TRACE_ID("invariant.EqualFunction.ThreeClojureEqualArities");
    TRACE_ID("invariant.EqualFunction.UnaryFormAdmitsSingleSupportedDomainOperand");
    TRACE_ID("invariant.EqualFunction.VariadicFormConjoinsAdjacentPairEquality");
    TRACE_ID("invariant.EqualFunction.AdjacentPairsAndUnaryOperandCompileTimeGated");
    TRACE_ID("invariant.EqualFunction.VariadicEvaluationLeftToRightShortCircuit");
    TRACE_ID("invariant.EqualFunction.ScalarOperandsCompareViaOperatorEquals");
    TRACE_ID("invariant.EqualFunction.ScalarFallthroughDomainIsClosedValueDomain");
    TRACE_ID("invariant.EqualFunction.StandardRangeTypesRejectedAsInteropSurface");
    TRACE_ID("invariant.EqualFunction.StandardRangeComponentsRejectedAtAnyDepth");
    TRACE_ID("invariant.EqualFunction.UnscopedEnumsAndPointersExcludedFromFallthrough");
    TRACE_ID("invariant.EqualFunction.EqualityComparisonNonThrowingAtAnyDepth");
    TRACE_ID("invariant.EqualFunction.FloatingPointRejectedAtCompileTimeAtAnyDepth");
    TRACE_ID("invariant.EqualFunction.CljonicNonscalarPairsClassifiedByEqualityFamily");
    TRACE_ID("invariant.EqualFunction.SequentialFamilyMutuallyComparableByProducedSequence");
    TRACE_ID("invariant.EqualFunction.SequentialComparisonIsOrderSensitive");
    TRACE_ID("invariant.EqualFunction.SequentialElementsRequireIdenticalElementTypes");
    TRACE_ID("invariant.EqualFunction.MapComparableOnlyToMap");
    TRACE_ID("invariant.EqualFunction.SetComparableOnlyToSet");
    TRACE_ID("invariant.EqualFunction.StringComparableOnlyToString");
    TRACE_ID("invariant.EqualFunction.CrossFamilyAndMixedPairsRejectedAtCompileTime");
    TRACE_ID("invariant.EqualFunction.OutsideSupportedDomainRejectedAtCompileTime");
    TRACE_ID("invariant.EqualFunction.EqualityIsRecursiveOverNestedValues");
    TRACE_ID("invariant.EqualFunction.NestedProducerComponentsUseProducerParameterEquality");
    TRACE_ID("invariant.EqualFunction.LazyElementWiseComparisonNoEagerMaterialization");
    TRACE_ID("invariant.EqualFunction.TerminatesOnFirstDifferingElementPair");
    TRACE_ID("invariant.EqualFunction.UnboundedPairsCompareConfiguredTraversalCap");
    TRACE_ID("invariant.EqualFunction.EveryCallTerminates");
    TRACE_ID("invariant.EqualFunction.ConstexprNoexceptNonmutatingNonallocating");
    TRACE_ID("invariant.EqualFunction.RequiresStableEqualityComponents");
    TRACE_ID("invariant.EqualFunction.NoCrossTypeNumericUnification");

    // ------------------------------------------------------------------------
    // Scalar fallthrough within the closed value domain (compile time).
    // ------------------------------------------------------------------------
    STATIC_REQUIRE(equal(1, 1));
    STATIC_REQUIRE(!equal(1, 2));
    STATIC_REQUIRE(equal('a', 'a'));
    STATIC_REQUIRE(!equal('a', 'b'));
    enum class Color { Red, Green };
    STATIC_REQUIRE(equal(Color::Red, Color::Red));
    STATIC_REQUIRE(!equal(Color::Red, Color::Green));
    STATIC_REQUIRE(equal(Pixel{1, 2}, Pixel{1, 2}));
    STATIC_REQUIRE(!equal(Pixel{1, 2}, Pixel{1, 3}));
    STATIC_REQUIRE(equal(MapEntry<int, int>{1, 2}, MapEntry<int, int>{1, 2}));
    STATIC_REQUIRE(!equal(MapEntry<int, int>{1, 2}, MapEntry<int, int>{1, 3}));
    STATIC_REQUIRE(equal(cljonic::Variant<int, long>{1}, cljonic::Variant<int, long>{1}));
    STATIC_REQUIRE(!equal(cljonic::Variant<int, long>{1}, cljonic::Variant<int, long>{2}));

    // Tightened fallthrough domain: arithmetic scalars and scoped enums are in;
    // unscoped enums and pointers are out
    // (invariant UnscopedEnumsAndPointersExcludedFromFallthrough).
    enum UnscopedColor { UnscopedRed, UnscopedGreen };
    STATIC_REQUIRE(cljonic::concepts_detail::in_non_cljonic_fallthrough_domain_v<Color>);
    STATIC_REQUIRE_FALSE(cljonic::concepts_detail::in_non_cljonic_fallthrough_domain_v<UnscopedColor>);
    STATIC_REQUIRE_FALSE(cljonic::concepts_detail::in_non_cljonic_fallthrough_domain_v<int*>);

    // Non-throwing equality at any depth
    // (invariant EqualityComparisonNonThrowingAtAnyDepth): a comparison that
    // may throw is stable but not admissible to noexcept operations.
    struct ThrowingAggregate {
        int x;
        [[nodiscard]] auto operator==(const ThrowingAggregate& other) const -> bool {
            return x == other.x;
        }
    };
    STATIC_REQUIRE(cljonic::concepts::StableEqualityComparable<ThrowingAggregate>);
    STATIC_REQUIRE_FALSE(cljonic::concepts::NothrowEqualityComparable<ThrowingAggregate>);

    // Standard-range components are rejected at any depth
    // (invariant StandardRangeComponentsRejectedAtAnyDepth).
    STATIC_REQUIRE(cljonic::concepts_detail::contains_standard_range_v<cljonic::Variant<int, std::span<int>>>);
    STATIC_REQUIRE_FALSE(cljonic::concepts_detail::contains_standard_range_v<cljonic::Variant<int, long>>);

    // Floating-point rejection at compile time at any depth: the negative
    // compile-fail harness (scripts/check-equal-compile-failures.py) proves
    // equal(1.0, 1.0), float-bearing collections, and nested floats fail.
    // Here the concept-level gate is asserted for the same domain.
    STATIC_REQUIRE_FALSE(cljonic::concepts::StableEqualityComparable<double>);
    STATIC_REQUIRE_FALSE(cljonic::concepts::StableEqualityComparable<Vector<float, 2>>);
    STATIC_REQUIRE_FALSE(cljonic::concepts::StableEqualityComparable<cljonic::Variant<int, double>>);

    // ------------------------------------------------------------------------
    // Sequential family: mutual comparability by produced sequence.
    // ------------------------------------------------------------------------
    STATIC_REQUIRE(equal(Vector<int, 4>{1, 2, 3}, Range<int>{1, 4, 1}));
    STATIC_REQUIRE(equal(Vector<int, 4>{1, 2, 3}, Queue<int, 4>{1, 2, 3}));
    STATIC_REQUIRE(equal(Vector<int, 4>{1, 1, 1}, Repeat<int>{1, 3U}));
    STATIC_REQUIRE(equal(Queue<int, 4>{1, 2, 3}, Range<int>{1, 4, 1}));
    STATIC_REQUIRE(equal(Range<int>{5, 6, 1}, Repeat<int>{5, 1U})); // both produce [5]
    // A finite sequential value is not equal to an unbounded producer whose
    // prefix agrees: the finite operand exhausts before the unbounded operand
    // reaches its configured traversal cap (bounded-prefix rule).
    STATIC_REQUIRE(!equal(Vector<int, 4>{1, 2}, Iterate<int, int (*)(int) noexcept>{1, inc_fn}));
    STATIC_REQUIRE(
        equal(Iterate<int, int (*)(int) noexcept>{1, inc_fn}, Iterate<int, int (*)(int) noexcept>{1, inc_fn}));
    STATIC_REQUIRE(equal(Vector<int, 2>{1, 1}, Repeatedly<int, int (*)() noexcept>{gen_fn, 2U}));
    STATIC_REQUIRE(equal(cycle(Vector<int, 3>{1, 2}), cycle(Vector<int, 3>{1, 2})));
    STATIC_REQUIRE(!equal(cycle(Vector<int, 3>{1, 2}), Vector<int, 4>{1, 2, 3}));
    STATIC_REQUIRE(equal(Vector<int, 4>{}, Range<int>{5, 5, 1}));
    STATIC_REQUIRE(!equal(Vector<int, 4>{1, 2}, Vector<int, 4>{1, 3}));

    // Unbounded pairs compare the configured observable traversal cap.
    STATIC_REQUIRE(equal(Repeat<int>{7}, Repeat<int>{7}));
    STATIC_REQUIRE(!equal(Repeat<int>{7}, Repeat<int>{8}));
    STATIC_REQUIRE(equal(Range<int>{0, 5, 0}, Range<int>{0, 9, 0}));

    // Same-type sequential equality coincides with operator== (REQ-COLL-021).
    constexpr auto vector_a = Vector<int, 4>{1, 2, 3};
    constexpr auto vector_b = Vector<int, 4>{1, 2, 3};
    STATIC_REQUIRE(equal(vector_a, vector_b) == (vector_a == vector_b));

    // ------------------------------------------------------------------------
    // Map, Set, String families: same-kind only, order-insensitive.
    // ------------------------------------------------------------------------
    constexpr auto map_small = Map<int, int, 4>{MapEntry<int, int>{1, 10}, MapEntry<int, int>{2, 20}};
    constexpr auto map_large = Map<int, int, 8>{MapEntry<int, int>{2, 20}, MapEntry<int, int>{1, 10}};
    STATIC_REQUIRE(equal(map_small, map_large));
    STATIC_REQUIRE(!equal(map_small, Map<int, int, 4>{MapEntry<int, int>{1, 11}}));
    STATIC_REQUIRE(!equal(map_small, Map<int, int, 4>{MapEntry<int, int>{1, 10}}));
    constexpr auto set_small = Set<int, 4>{3, 1, 2};
    constexpr auto set_large = Set<int, 8>{1, 2, 3};
    STATIC_REQUIRE(equal(set_small, set_large));
    STATIC_REQUIRE(!equal(set_small, Set<int, 4>{1, 2, 4}));
    STATIC_REQUIRE(equal(String<8>{"abc"}, String<16>{"abc"}));
    STATIC_REQUIRE(!equal(String<8>{"abc"}, String<8>{"abd"}));
    STATIC_REQUIRE(equal(String<8>{"abc"}, String<4>{"abc"}));
    STATIC_REQUIRE(!equal(String<8>{"abc"}, String<8>{"ab"}));

    // ------------------------------------------------------------------------
    // Recursion into nested values: collections recurse via REQ-COLL-021;
    // nested producers compare by producer parameter equality (REQ-FN-014B).
    // ------------------------------------------------------------------------
    using NestedVector = Vector<Range<int>, 4>;
    STATIC_REQUIRE(equal(NestedVector{Range<int>{1, 3, 1}, Range<int>{1, 5, 2}},
                         NestedVector{Range<int>{1, 3, 1}, Range<int>{1, 5, 2}}));
    STATIC_REQUIRE(!equal(NestedVector{Range<int>{1, 3, 1}, Range<int>{1, 5, 2}},
                          NestedVector{Range<int>{1, 3, 1}, Range<int>{1, 5, 3}}));
    using VariantValue = cljonic::Variant<int, Vector<int, 4>>;
    using VariantVector = Vector<VariantValue, 4>;
    STATIC_REQUIRE(equal(VariantVector{VariantValue{1}, VariantValue{Vector<int, 4>{2, 3}}},
                         VariantVector{VariantValue{1}, VariantValue{Vector<int, 4>{2, 3}}}));
    STATIC_REQUIRE(!equal(VariantVector{VariantValue{1}, VariantValue{Vector<int, 4>{2, 3}}},
                          VariantVector{VariantValue{1}, VariantValue{Vector<int, 4>{2, 4}}}));
    using ProducerVector = Vector<Repeat<int>, 4>;
    STATIC_REQUIRE(equal(ProducerVector{Repeat<int>{7, 3U}}, ProducerVector{Repeat<int>{7, 3U}}));
    STATIC_REQUIRE(!equal(ProducerVector{Repeat<int>{7, 3U}}, ProducerVector{Repeat<int>{7}}));

    // ------------------------------------------------------------------------
    // Arities: unary admits a single domain operand; the variadic form
    // conjoins adjacent-pair equality left to right with short-circuit.
    // ------------------------------------------------------------------------
    STATIC_REQUIRE(equal(1));
    STATIC_REQUIRE(equal(Color::Red));
    STATIC_REQUIRE(equal(Pixel{1, 2}));
    STATIC_REQUIRE(equal(vector_a));
    STATIC_REQUIRE(equal(map_small));
    STATIC_REQUIRE(equal(set_small));
    STATIC_REQUIRE(equal(String<8>{"abc"}));
    STATIC_REQUIRE(equal(Repeat<int>{7}));
    STATIC_REQUIRE(equal(1, 1, 1));
    STATIC_REQUIRE(equal(1, 1, 1, 1, 1));
    STATIC_REQUIRE(!equal(1, 2, 1));
    STATIC_REQUIRE(!equal(1, 1, 2));
    STATIC_REQUIRE(equal(vector_a, Range<int>{1, 4, 1}, Queue<int, 4>{1, 2, 3}));
    STATIC_REQUIRE(equal(vector_a, vector_b, Range<int>{1, 4, 1}));
    STATIC_REQUIRE(!equal(vector_a, vector_b, Vector<int, 4>{1, 2, 4}));
    STATIC_REQUIRE(equal(map_small, map_large, map_small));
    STATIC_REQUIRE(!equal(map_small, map_large, Map<int, int, 4>{MapEntry<int, int>{1, 11}}));
    STATIC_REQUIRE(equal(set_small, set_large, set_small));
    STATIC_REQUIRE(equal(String<8>{"abc"}, String<16>{"abc"}, String<4>{"abc"}));
    STATIC_REQUIRE(!equal(String<8>{"abc"}, String<16>{"abc"}, String<4>{"abd"}));
    // Variadic recursion bottoms out in the binary overloads: mixed-family and
    // mixed cljonic-to-non-cljonic adjacent pairs fail compilation regardless
    // of position (proved by the compile-fail harness).

    // ------------------------------------------------------------------------
    // Runtime coverage: volatile-derived values defeat constexpr folding so
    // every overload and branch genuinely executes (coverage gate).
    // ------------------------------------------------------------------------
    volatile int runtime_raw = 1;
    const auto runtime_one = runtime_raw;
    volatile int runtime_other_raw = 2;
    const auto runtime_two = runtime_other_raw;

    CHECK(equal(runtime_one, runtime_one));
    CHECK_FALSE(equal(runtime_one, runtime_two));

    auto runtime_vector = Vector<int, 4>{runtime_one, runtime_two};
    auto runtime_vector_diff = Vector<int, 4>{runtime_one, runtime_one};
    CHECK(equal(runtime_vector, runtime_vector));
    CHECK_FALSE(equal(runtime_vector, runtime_vector_diff));
    CHECK(equal(runtime_vector, Range<int>{runtime_one, 3, 1}));
    CHECK_FALSE(equal(runtime_vector, Range<int>{runtime_one, 2, 1}));
    CHECK_FALSE(equal(Vector<int, 4>{runtime_one, runtime_two, runtime_two}, runtime_vector));

    auto runtime_queue = Queue<int, 4>{runtime_one, runtime_two};
    CHECK(equal(runtime_vector, runtime_queue));

    auto runtime_map = Map<int, int, 4>{MapEntry<int, int>{1, 10}, MapEntry<int, int>{2, 20}};
    auto runtime_map_reordered = Map<int, int, 8>{MapEntry<int, int>{2, 20}, MapEntry<int, int>{1, 10}};
    auto runtime_map_count = Map<int, int, 4>{MapEntry<int, int>{1, 10}};
    auto runtime_map_value = Map<int, int, 4>{MapEntry<int, int>{1, 11}, MapEntry<int, int>{2, 20}};
    auto runtime_map_key = Map<int, int, 4>{MapEntry<int, int>{1, 10}, MapEntry<int, int>{3, 20}};
    CHECK(equal(runtime_map, runtime_map_reordered));
    CHECK_FALSE(equal(runtime_map, runtime_map_count));
    CHECK_FALSE(equal(runtime_map, runtime_map_value));
    CHECK_FALSE(equal(runtime_map, runtime_map_key));

    auto runtime_set = Set<int, 4>{3, 1, 2};
    auto runtime_set_reordered = Set<int, 8>{1, 2, 3};
    auto runtime_set_count = Set<int, 4>{1, 2};
    auto runtime_set_member = Set<int, 4>{1, 2, 4};
    CHECK(equal(runtime_set, runtime_set_reordered));
    CHECK_FALSE(equal(runtime_set, runtime_set_count));
    CHECK_FALSE(equal(runtime_set, runtime_set_member));

    auto runtime_string = String<8>{"abc"};
    auto runtime_string_same = String<16>{"abc"};
    auto runtime_string_diff = String<8>{"abd"};
    auto runtime_string_short = String<8>{"ab"};
    CHECK(equal(runtime_string, runtime_string_same));
    CHECK_FALSE(equal(runtime_string, runtime_string_diff));
    CHECK_FALSE(equal(runtime_string, runtime_string_short));

    auto runtime_cycle = cycle(Vector<int, 3>{runtime_one, runtime_two});
    auto runtime_cycle_same = cycle(Vector<int, 3>{runtime_one, runtime_two});
    CHECK(equal(runtime_cycle, runtime_cycle_same));
    CHECK_FALSE(equal(runtime_cycle, Vector<int, 4>{runtime_one, runtime_two, runtime_two}));

    auto runtime_repeat_unbounded = Repeat<int>{runtime_one};
    CHECK(equal(runtime_repeat_unbounded, Repeat<int>{runtime_one}));

    auto runtime_producer_vector = ProducerVector{Repeat<int>{runtime_one, 3U}};
    CHECK(equal(runtime_producer_vector, ProducerVector{Repeat<int>{runtime_one, 3U}}));
    CHECK_FALSE(equal(runtime_producer_vector, ProducerVector{Repeat<int>{runtime_one}}));

    auto runtime_variant_vector = VariantVector{VariantValue{runtime_one}};
    CHECK(equal(runtime_variant_vector, VariantVector{VariantValue{runtime_one}}));
    CHECK_FALSE(equal(runtime_variant_vector, VariantVector{VariantValue{runtime_two}}));

    // noexcept and constexpr quality bar.
    STATIC_REQUIRE(noexcept(equal(1)));
    STATIC_REQUIRE(noexcept(equal(1, 1)));
    STATIC_REQUIRE(noexcept(equal(1, 1, 1)));
    STATIC_REQUIRE(noexcept(equal(vector_a, vector_b)));
    STATIC_REQUIRE(noexcept(equal(map_small, map_large)));
    STATIC_REQUIRE(noexcept(equal(set_small, set_large)));
    STATIC_REQUIRE(noexcept(equal(String<8>{"abc"}, String<8>{"abc"})));

    // Runtime arity coverage: unary and variadic overloads genuinely execute.
    CHECK(equal(runtime_one));
    CHECK(equal(runtime_vector));
    CHECK(equal(runtime_map));
    CHECK(equal(runtime_set));
    CHECK(equal(runtime_string));
    CHECK(equal(runtime_cycle));
    CHECK(equal(runtime_one, runtime_one, runtime_one));
    CHECK_FALSE(equal(runtime_one, runtime_two, runtime_one));
    CHECK_FALSE(equal(runtime_one, runtime_one, runtime_two));
    CHECK(equal(runtime_vector, runtime_queue, runtime_vector));
    CHECK_FALSE(equal(runtime_vector, runtime_vector, runtime_vector_diff));
    CHECK(equal(runtime_map, runtime_map_reordered, runtime_map));
    CHECK_FALSE(equal(runtime_map, runtime_map_reordered, runtime_map_value));
    CHECK(equal(runtime_set, runtime_set_reordered, runtime_set));
    CHECK(equal(runtime_string, runtime_string_same, runtime_string));
    CHECK_FALSE(equal(runtime_string, runtime_string_same, runtime_string_diff));
} // namespace